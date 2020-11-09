#include "MainComponent.h"
#include "PluginSelectionTableModel.h"
#include "PluginWindow.h"

MainComponent::MainComponent() {
  this->deviceManager = std::make_unique<juce::AudioDeviceManager>();

  this->audioDeviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
      *this->deviceManager,
      0,
      2,
      0,
      2,
      false,
      false,
      false,
      false
  );
  this->addAndMakeVisible(*this->audioDeviceSelector);

  this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
  this->pluginFormatManager->addDefaultFormats();

  this->propertiesOptions = std::make_unique<juce::PropertiesFile::Options>();
  this->propertiesOptions->applicationName = ProjectInfo::projectName;
  this->propertiesOptions->folderName = ProjectInfo::companyName;
  this->propertiesOptions->osxLibrarySubFolder = "Application Support";

  this->applicationProperties = std::make_unique<juce::ApplicationProperties>();
  this->applicationProperties->setStorageParameters(*this->propertiesOptions);
  juce::PropertiesFile *userSettings = this->applicationProperties->getUserSettings();

  this->appLookAndFeel = std::make_unique<LookAndFeel>();

  this->knownPluginList = std::make_unique<juce::KnownPluginList>();
  std::unique_ptr<juce::XmlElement> xmlPluginList(userSettings->getXmlValue("pluginList"));
  if (xmlPluginList) {
    this->knownPluginList->recreateFromXml(*xmlPluginList);
  }
  this->knownPluginList->sort(juce::KnownPluginList::sortAlphabetically, true);

  this->pluginListComponent = std::make_unique<juce::PluginListComponent>(
      *this->pluginFormatManager,
      *this->knownPluginList,
      userSettings->getFile().getSiblingFile("CrashedPlugins"),
      userSettings,
      true
  );

  this->pluginListComponent->setTableModel(
      new PluginSelectionTableModel(
          *this->pluginListComponent,
          *this->knownPluginList,
          *this->appLookAndFeel,
          *this
      ));

  this->addAndMakeVisible(*this->pluginListComponent);

  this->setLookAndFeel(this->appLookAndFeel.get());
  this->setSize(1024, 512);

  std::unique_ptr<juce::XmlElement> savedDeviceManager = userSettings->getXmlValue("deviceManager");
  MainComponent::doWithPermission(
      juce::RuntimePermissions::recordAudio,
      [&](bool granted) {
        char inputChannels = granted ? 2 : 0;

        if (savedDeviceManager) {
          this->deviceManager->initialise(inputChannels, 2, savedDeviceManager.get(), true);
          return;
        }

        this->deviceManager->initialiseWithDefaultDevices(inputChannels, 2);
      }
  );

  this->audioProcessorPlayer = std::make_unique<juce::AudioProcessorPlayer>();

  this->deviceManager->addAudioCallback(this->audioProcessorPlayer.get());

  auto currentDevice = this->deviceManager->getCurrentAudioDevice();
  this->passThrough = std::make_unique<PassthroughProcessor>(
      currentDevice->getActiveInputChannels().toInteger(),
      currentDevice->getActiveOutputChannels().toInteger()
  );

  this->audioProcessorPlayer->setProcessor(this->passThrough.get());

  std::unique_ptr<juce::XmlElement> savedProcessor = userSettings->getXmlValue("processor");
  if (savedProcessor) {
    auto plugin = std::make_unique<juce::PluginDescription>();
    plugin->loadFromXml(*savedProcessor);
    this->getPluginSelectionModel()->setSelected(*plugin);
    this->loadPlugin(*plugin, false);

    juce::MemoryBlock state;
    state.fromBase64Encoding(userSettings->getValue("processorState"));
    this->loadedPlugin->setStateInformation(state.getData(), state.getSize());
    state.reset();
  }

  this->knownPluginList->addChangeListener(this);
  this->deviceManager->addChangeListener(this);
}

MainComponent::~MainComponent() {
  this->deviceManager->removeChangeListener(this);
  this->knownPluginList->removeChangeListener(this);

  this->pluginListComponent->setTableModel(nullptr);
  this->pluginListComponent->getTableListBox();

  this->deviceManager->removeAudioCallback(this->audioProcessorPlayer.get());
  this->audioProcessorPlayer->setProcessor(nullptr);

  this->setLookAndFeel(nullptr);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
  if (source == this->knownPluginList.get()) {
    for (
      const juce::PluginDescription &plugin : this->knownPluginList->getTypes()) {
      if (plugin.isInstrument) {
        this->knownPluginList->removeType(plugin);
        this->knownPluginList->addToBlacklist(plugin.fileOrIdentifier);
      }
    }

    std::unique_ptr<juce::XmlElement> pluginListToSave = this->knownPluginList->createXml();
    if (pluginListToSave) {
      this->applicationProperties->getUserSettings()->setValue("pluginList", pluginListToSave.get());
      this->applicationProperties->saveIfNeeded();
    }

    return;
  }

  if (source == this->deviceManager.get()) {
    std::unique_ptr<juce::XmlElement> deviceManagerToSave = this->deviceManager->createStateXml();
    if (deviceManagerToSave) {
      this->applicationProperties->getUserSettings()->setValue("deviceManager", deviceManagerToSave.get());
      this->applicationProperties->saveIfNeeded();
    }

    return;
  }

  auto model = this->getPluginSelectionModel();
  if (source == model) {
    if (this->loadedPlugin->getPluginDescription().fileOrIdentifier == model->getSelected().fileOrIdentifier) {
      this->pluginWindow->setVisible(true);
      return;
    }

    this->loadPlugin(model->getSelected());

    return;
  }
}

void MainComponent::paint(juce::Graphics &g) {
  g.fillAll(this->getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
  // This is called when the MainContentComponent is resized.
  // If you add any child components, this is where you should
  // update their positions.
  auto rect = this->getLocalBounds();

  this->audioDeviceSelector->setBounds(rect.removeFromLeft(512).reduced(this->appLookAndFeel->padding));
  this->pluginListComponent->setBounds(rect.removeFromRight(512).reduced(this->appLookAndFeel->padding));
}

void MainComponent::doWithPermission(
    juce::RuntimePermissions::PermissionID permission,
    const std::function<void(bool)> &callback
) {
  bool requiresPermission = juce::RuntimePermissions::isRequired(permission);
  bool hasPermission = juce::RuntimePermissions::isGranted(permission);

  if (requiresPermission && !hasPermission) {
    juce::RuntimePermissions::request(permission, callback);
    return;
  }

  callback(true);
}

void MainComponent::loadPlugin(juce::PluginDescription &plugin, bool show) {
  juce::AudioIODevice *currentDevice = this->deviceManager->getCurrentAudioDevice();
  juce::String error;

  if (this->loadedPlugin) {
    this->audioProcessorPlayer->setProcessor(this->passThrough.get());
    this->pluginWindow.reset();
    this->loadedPlugin.reset();
  }

  this->loadedPlugin = this->pluginFormatManager->createPluginInstance(
      plugin,
      currentDevice->getCurrentSampleRate(),
      currentDevice->getCurrentBufferSizeSamples(),
      error
  );

  if (!this->loadedPlugin) {
    std::cerr << error << std::endl;
    return;
  }

  std::unique_ptr<juce::XmlElement> processorToSave = plugin.createXml();
  if (processorToSave) {
    this->applicationProperties->getUserSettings()->setValue("processor", processorToSave.get());
    this->applicationProperties->saveIfNeeded();
  }

  this->audioProcessorPlayer->setProcessor(this->loadedPlugin.get());

  this->pluginWindow = std::make_unique<PluginWindow>(
      *this->loadedPlugin,
      *this->appLookAndFeel,
      *this->applicationProperties
  );

  this->pluginWindow->setVisible(show);
}

PluginSelectionTableModel *MainComponent::getPluginSelectionModel() {
  return dynamic_cast<PluginSelectionTableModel *>(this->pluginListComponent->getTableListBox().getModel());
}
