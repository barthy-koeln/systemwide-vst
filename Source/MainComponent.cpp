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
  if (nullptr != xmlPluginList) {
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

  this->pluginListComponent->setTableModel(new PluginSelectionTableModel(
      *this->pluginListComponent,
      *this->knownPluginList,
      *this->appLookAndFeel,
      *this
  ));

  this->addAndMakeVisible(*this->pluginListComponent);

  this->setLookAndFeel(this->appLookAndFeel.get());
  this->setSize(1024, 512);

  std::unique_ptr<juce::XmlElement> savedDeviceManager(userSettings->getXmlValue("deviceManager"));
  MainComponent::doWithPermission(
      juce::RuntimePermissions::recordAudio,
      [&](bool granted) {
        char inputChannels = granted ? 2 : 0;

        if (nullptr != savedDeviceManager) {
          this->deviceManager->initialise(inputChannels, 2, savedDeviceManager.get(), true);
          return;
        }

        this->deviceManager->initialiseWithDefaultDevices(inputChannels, 2);
      }
  );

  this->audioProcessorPlayer = std::make_unique<juce::AudioProcessorPlayer>();

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

    std::unique_ptr<juce::XmlElement> savedPluginList(this->knownPluginList->createXml());
    if (nullptr != savedPluginList) {
      this->applicationProperties->getUserSettings()->setValue("pluginList", savedPluginList.get());
      this->applicationProperties->saveIfNeeded();
    }

    return;
  }

  if (source == this->deviceManager.get()) {
    std::unique_ptr<juce::XmlElement> savedDeviceManager(this->deviceManager->createStateXml());

    if (nullptr != savedDeviceManager) {
      this->applicationProperties->getUserSettings()->setValue("deviceManager", savedDeviceManager.get());
      this->applicationProperties->saveIfNeeded();
    }

    return;
  }

  if (source == dynamic_cast<PluginSelectionTableModel*>(this->pluginListComponent->getTableListBox().getModel())) {
    auto model = dynamic_cast<PluginSelectionTableModel*>(this->pluginListComponent->getTableListBox().getModel());
    juce::String error;
    juce::AudioIODevice *currentDevice = this->deviceManager->getCurrentAudioDevice();

    if(this->loadedPlugin){
      this->deviceManager->removeAudioCallback(this->audioProcessorPlayer.get());
      this->audioProcessorPlayer->setProcessor(nullptr);
      this->loadedPlugin.reset();
    }

    this->loadedPlugin = this->pluginFormatManager->createPluginInstance(
        model->selected,
        currentDevice->getCurrentSampleRate(),
        currentDevice->getCurrentBufferSizeSamples(),
        error
    );

    if (!this->loadedPlugin) {
      std::cerr << error << std::endl;
      return;
    }

    this->audioProcessorPlayer->setProcessor(this->loadedPlugin.get());
    this->deviceManager->addAudioCallback(this->audioProcessorPlayer.get());

    auto window = new PluginWindow(*this->loadedPlugin, *this->appLookAndFeel);

    window->setVisible(true);

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
