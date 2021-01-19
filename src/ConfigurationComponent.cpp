#include "ConfigurationComponent.h"
#include "PluginSelectionTableModel.h"
#include "SettingsConstants.h"

ConfigurationComponent::ConfigurationComponent (SystemwideVSTProcess &systemwideVSTProcess) :
  systemwideVSTProcess(systemwideVSTProcess) {
  this->appLookAndFeel = std::make_unique<LookAndFeel>();

  this->addActionListener(&this->systemwideVSTProcess);

  this->audioDeviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
    this->systemwideVSTProcess.getDeviceManager(),
    0,
    2,
    0,
    2,
    false,
    false,
    true,
    false
  );

  this->knownPluginList = std::make_unique<juce::KnownPluginList>();
  std::unique_ptr<juce::XmlElement> xmlPluginList = this->systemwideVSTProcess.getXmlValue(SETTING_PLUGIN_LIST);
  if (xmlPluginList) {
    this->knownPluginList->recreateFromXml(*xmlPluginList);
  }
  this->knownPluginList->sort(juce::KnownPluginList::sortAlphabetically, true);

  this->pluginListComponent = std::make_unique<juce::PluginListComponent>(
    this->systemwideVSTProcess.getPluginFormatManager(),
    *this->knownPluginList,
    this->systemwideVSTProcess.getCrashFile(),
    nullptr
  );

  this->pluginListComponent->setTableModel(
    new PluginSelectionTableModel(
      *this->pluginListComponent,
      *this->knownPluginList,
      *this->appLookAndFeel,
      *this
    )
  );

  this->knownPluginList->addChangeListener(this);

  if (this->systemwideVSTProcess.hasLoadedPlugin()) {
    this->getPluginSelectionModel()->setSelected(this->systemwideVSTProcess.getLoadedPluginDescription());
  }

  this->addAndMakeVisible(*this->audioDeviceSelector);
  this->addAndMakeVisible(*this->pluginListComponent);
  this->setSize(1024, 512);
}

ConfigurationComponent::~ConfigurationComponent () {
  this->knownPluginList->removeChangeListener(this);
  this->removeAllActionListeners();

  this->pluginListComponent->setTableModel(nullptr);
  this->setLookAndFeel(nullptr);
}

void ConfigurationComponent::changeListenerCallback (juce::ChangeBroadcaster *source) {
  if (source == this->knownPluginList.get()) {
    this->savePluginList();
    return;
  }
}

void ConfigurationComponent::actionListenerCallback (const juce::String &message) {
  if (message == MESSAGE_SELECTION_CHANGED) {
    juce::PluginDescription description = this->getPluginSelectionModel()->getSelected();

    if (!this->systemwideVSTProcess.isLoadedPlugin(description)) {
      this->systemwideVSTProcess.loadPlugin(description);
    }

    this->sendActionMessage(MESSAGE_SHOW_PLUGIN);
    return;
  }
}

void ConfigurationComponent::paint (juce::Graphics &g) {
  g.fillAll(this->getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ConfigurationComponent::resized () {
  if (this->audioDeviceSelector && this->pluginListComponent) {
    auto rect = this->getLocalBounds();
    this->audioDeviceSelector->setBounds(rect.removeFromLeft(512).reduced(this->appLookAndFeel->padding));
    this->pluginListComponent->setBounds(rect.removeFromRight(512).reduced(this->appLookAndFeel->padding));
    return;
  }
}

PluginSelectionTableModel *ConfigurationComponent::getPluginSelectionModel () {
  return dynamic_cast<PluginSelectionTableModel *>(this->pluginListComponent->getTableListBox().getModel());
}

void ConfigurationComponent::savePluginList () {
  for (
    const juce::PluginDescription &plugin : this->knownPluginList->getTypes()) {
    if (plugin.isInstrument) {
      this->knownPluginList->removeType(plugin);
      this->knownPluginList->addToBlacklist(plugin.fileOrIdentifier);
    }
  }

  std::unique_ptr<juce::XmlElement> pluginListToSave = this->knownPluginList->createXml();
  if (pluginListToSave) {
    this->systemwideVSTProcess.savePluginList(pluginListToSave.get());
  }
}
