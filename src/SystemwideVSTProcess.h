#pragma once

#include <JuceHeader.h>
#include "PassthroughProcessor.h"
#include "SettingsConstants.h"
#include "MessagesConstants.h"

class SystemwideVSTProcess : public juce::ActionListener, public juce::ChangeListener
{
 public:

  SystemwideVSTProcess () {
    this->deviceManager = std::make_unique<juce::AudioDeviceManager>();
    this->propertiesOptions = std::make_unique<juce::PropertiesFile::Options>();
    this->propertiesOptions->applicationName = ProjectInfo::projectName;
    this->propertiesOptions->folderName = ProjectInfo::companyName;
    this->propertiesOptions->osxLibrarySubFolder = "Application Support";

    this->applicationProperties = std::make_unique<juce::ApplicationProperties>();
    this->applicationProperties->setStorageParameters(*this->propertiesOptions);

    std::unique_ptr<juce::XmlElement>
      savedDeviceManager = this->applicationProperties->getUserSettings()->getXmlValue(SETTING_DEVICE_MANAGER);

    SystemwideVSTProcess::doWithPermission(
      juce::RuntimePermissions::recordAudio,
      [&] (bool granted) {

        this->deviceManager->initialise(
          2,
          2,
          nullptr,
          true,
          "",
          new juce::AudioDeviceManager::AudioDeviceSetup(
            {
              .outputDeviceName = "",
              .inputDeviceName = "Playback/recording through the PulseAudio sound server",
              .sampleRate = 0,
              .bufferSize = 0,
              .inputChannels = 0,
              .useDefaultInputChannels = true,
              .outputChannels = 0,
              .useDefaultOutputChannels = true
            }
          ));
      }
    );

    this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
    this->pluginFormatManager->addDefaultFormats();

    this->audioProcessorPlayer = std::make_unique<juce::AudioProcessorPlayer>();
    this->deviceManager->addAudioCallback(this->audioProcessorPlayer.get());
    this->deviceManager->addChangeListener(this);

    auto currentDevice = this->deviceManager->getCurrentAudioDevice();
    if (nullptr == currentDevice) {
      return;
    }

    this->passThrough = std::make_unique<PassthroughProcessor>(
      currentDevice->getActiveInputChannels().toInteger(),
      currentDevice->getActiveOutputChannels().toInteger()
    );
  }

  ~SystemwideVSTProcess () override {
    this->deviceManager->closeAudioDevice();
    this->deviceManager->removeChangeListener(this);
    this->deviceManager->removeAudioCallback(this->audioProcessorPlayer.get());
  }

  void loadSavedPlugin () {
    std::unique_ptr<juce::XmlElement>
      savedProcessor = this->applicationProperties->getUserSettings()->getXmlValue(SETTING_PROCESSOR);

    if (savedProcessor) {
      auto plugin = std::make_unique<juce::PluginDescription>();
      plugin->loadFromXml(*savedProcessor);
      this->loadPlugin(*plugin);

      if (!this->loadedPlugin) {
        return;
      }

      juce::MemoryBlock state;
      state.fromBase64Encoding(this->applicationProperties->getUserSettings()->getValue(SETTING_PROCESSOR_STATE));
      this->loadedPlugin->setStateInformation(state.getData(), state.getSize());
      this->audioProcessorPlayer->setProcessor(this->loadedPlugin.get());
      return;
    }

    this->audioProcessorPlayer->setProcessor(this->passThrough.get());
  }

  [[nodiscard]] bool shouldShowConfig () const {
    bool shouldShowConfig = !this->applicationProperties->getUserSettings()->getBoolValue(SETTING_NO_CONFIG, false);

    if (shouldShowConfig) {
      this->applicationProperties->getUserSettings()->setValue(SETTING_NO_CONFIG, true);
      this->applicationProperties->saveIfNeeded();
    }

    return shouldShowConfig;
  }

  void loadPlugin (const juce::PluginDescription &plugin) {
    juce::AudioIODevice *currentDevice = this->deviceManager->getCurrentAudioDevice();
    juce::String error;

    if (nullptr == currentDevice) {
      return;
    }

    if (nullptr != this->pluginWindow) {
      this->deletePluginWindow();
    }

    if (this->loadedPlugin) {
      this->audioProcessorPlayer->setProcessor(this->passThrough.get());
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

    this->audioProcessorPlayer->setProcessor(this->loadedPlugin.get());

    std::unique_ptr<juce::XmlElement> processorToSave = plugin.createXml();
    if (processorToSave) {
      this->applicationProperties->getUserSettings()->setValue(SETTING_PROCESSOR, processorToSave.get());
      this->applicationProperties->saveIfNeeded();
    }
  }

  void savePluginList (juce::XmlElement *value) const {
    this->saveValue(SETTING_PLUGIN_LIST, value);
  }

  void saveDeviceManager () const {
    std::unique_ptr<juce::XmlElement> deviceManagerToSave = this->deviceManager->createStateXml();
    if (deviceManagerToSave) {
      this->saveValue(SETTING_DEVICE_MANAGER, deviceManagerToSave.get());
    }
  }

  void savePluginState () const {
    juce::MemoryBlock state;
    this->loadedPlugin->getStateInformation(state);
    this->applicationProperties->getUserSettings()->setValue(SETTING_PROCESSOR_STATE, state.toBase64Encoding());
    this->applicationProperties->saveIfNeeded();
    state.reset();
  }

  bool isLoadedPlugin (juce::PluginDescription &description) const {
    return description.fileOrIdentifier == this->loadedPlugin->getPluginDescription().fileOrIdentifier;
  }

  void actionListenerCallback (const juce::String &message) override {
    if (message == MESSAGE_CLOSE_PLUGIN) {
      this->savePluginState();
      this->deletePluginWindow();
      return;
    }

    if (message == MESSAGE_SHOW_PLUGIN) {
      this->createOrShowPluginWindow();
      return;
    }
  }

  void changeListenerCallback (juce::ChangeBroadcaster *source) override {
    if (source == this->deviceManager.get()) {
      this->saveDeviceManager();

      if (!this->loadedPlugin) {
        this->loadSavedPlugin();
      }
      return;
    }
  }

  [[nodiscard]] std::unique_ptr<juce::XmlElement> getXmlValue (const std::string &name) const {
    return this->applicationProperties->getUserSettings()->getXmlValue(name);
  }

  [[nodiscard]] juce::File getCrashFile () const {
    return this->applicationProperties->getUserSettings()->getFile().getSiblingFile("CrashedPlugins");
  }

  [[nodiscard]] bool hasLoadedPlugin () const {
    return (bool) this->loadedPlugin;
  }

  [[nodiscard]] juce::PluginDescription getLoadedPluginDescription () const {
    return this->loadedPlugin->getPluginDescription();
  }

  [[nodiscard]] juce::AudioDeviceManager &getDeviceManager () const {
    return *this->deviceManager;
  }

  [[nodiscard]] juce::AudioPluginFormatManager &getPluginFormatManager () const {
    return *this->pluginFormatManager;
  }
 private:
  std::unique_ptr<juce::PropertiesFile::Options> propertiesOptions;
  std::unique_ptr<juce::ApplicationProperties> applicationProperties;
  std::unique_ptr<juce::AudioDeviceManager> deviceManager;
  std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
  std::unique_ptr<juce::AudioPluginInstance> loadedPlugin;
  std::unique_ptr<PassthroughProcessor> passThrough;
  std::unique_ptr<juce::AudioProcessorPlayer> audioProcessorPlayer;
  std::unique_ptr<PluginWindow> pluginWindow;

  void createOrShowPluginWindow () {
    if (this->pluginWindow) {
      this->pluginWindow->setVisible(true);
      this->pluginWindow->toFront(true);
      return;
    }

    this->pluginWindow = std::make_unique<PluginWindow>(*this->loadedPlugin, this);
  }

  void deletePluginWindow () {
    this->pluginWindow.reset();
  }

  void saveValue (const char *name, juce::XmlElement *value) const {
    this->applicationProperties->getUserSettings()->setValue(name, value);
    this->applicationProperties->saveIfNeeded();
  }

  static void doWithPermission (
    juce::RuntimePermissions::PermissionID permission,
    const std::function<void (bool)> &callback
  ) {
    bool requiresPermission = juce::RuntimePermissions::isRequired(permission);
    bool hasPermission = juce::RuntimePermissions::isGranted(permission);

    if (requiresPermission && !hasPermission) {
      juce::RuntimePermissions::request(permission, callback);
      return;
    }

    callback(true);
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemwideVSTProcess)
};
