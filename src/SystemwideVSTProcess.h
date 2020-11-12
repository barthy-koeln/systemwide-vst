#pragma once

#include <JuceHeader.h>
#include "PassthroughProcessor.h"
#include "SettingsConstants.h"
#include "MessagesConstants.h"

class SystemwideVSTProcess : public juce::ActionListener {
 public:
  std::unique_ptr<juce::PropertiesFile::Options> propertiesOptions;
  std::unique_ptr<juce::ApplicationProperties> applicationProperties;
  std::unique_ptr<juce::PropertiesFile> userSettings;
  std::unique_ptr<juce::AudioDeviceManager> deviceManager;
  std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
  std::unique_ptr<juce::AudioPluginInstance> loadedPlugin;

  SystemwideVSTProcess() {
    this->deviceManager = std::make_unique<juce::AudioDeviceManager>();
    this->propertiesOptions = std::make_unique<juce::PropertiesFile::Options>();
    this->propertiesOptions->applicationName = ProjectInfo::projectName;
    this->propertiesOptions->folderName = ProjectInfo::companyName;
    this->propertiesOptions->osxLibrarySubFolder = "Application Support";

    this->applicationProperties = std::make_unique<juce::ApplicationProperties>();
    this->applicationProperties->setStorageParameters(*this->propertiesOptions);
    this->userSettings = std::unique_ptr<juce::PropertiesFile>(this->applicationProperties->getUserSettings());

    std::unique_ptr<juce::XmlElement> savedDeviceManager = userSettings->getXmlValue(SETTING_DEVICE_MANAGER);

    SystemwideVSTProcess::doWithPermission(
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

    this->pluginFormatManager = std::make_unique<juce::AudioPluginFormatManager>();
    this->pluginFormatManager->addDefaultFormats();

    this->audioProcessorPlayer = std::make_unique<juce::AudioProcessorPlayer>();
    this->deviceManager->addAudioCallback(this->audioProcessorPlayer.get());

    auto currentDevice = this->deviceManager->getCurrentAudioDevice();
    this->passThrough = std::make_unique<PassthroughProcessor>(
        currentDevice->getActiveInputChannels().toInteger(),
        currentDevice->getActiveOutputChannels().toInteger()
    );
  }

  void loadSavedPlugin() {
    std::unique_ptr<juce::XmlElement> savedProcessor = this->userSettings->getXmlValue(SETTING_PROCESSOR);

    if (savedProcessor) {
      auto plugin = std::make_unique<juce::PluginDescription>();
      plugin->loadFromXml(*savedProcessor);
      this->loadPlugin(*plugin);

      juce::MemoryBlock state;
      state.fromBase64Encoding(this->userSettings->getValue(SETTING_PROCESSOR_STATE));
      this->loadedPlugin->setStateInformation(state.getData(), state.getSize());
      this->audioProcessorPlayer->setProcessor(this->loadedPlugin.get());
      return;
    }

    this->audioProcessorPlayer->setProcessor(this->passThrough.get());
  }

  [[nodiscard]] bool shouldShowConfig() const {
    bool shouldShowConfig = !this->userSettings->getBoolValue(SETTING_NO_CONFIG, false);

    if (shouldShowConfig) {
      this->userSettings->setValue(SETTING_NO_CONFIG, true);
      this->userSettings->saveIfNeeded();
    }

    return shouldShowConfig || DEBUG;
  }

  void loadPlugin(const juce::PluginDescription &plugin) {
    juce::AudioIODevice *currentDevice = this->deviceManager->getCurrentAudioDevice();
    juce::String error;

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
      this->userSettings->setValue(SETTING_PROCESSOR, processorToSave.get());
      this->userSettings->saveIfNeeded();
    }
  }

  void savePluginList(juce::XmlElement *value) {
    this->saveValue(SETTING_PLUGIN_LIST, value);
  }

  bool isDeviceManager(juce::ChangeBroadcaster *source) const {
    return source == this->deviceManager.get();
  }

  void saveDeviceManager() {
    std::unique_ptr<juce::XmlElement> deviceManagerToSave = this->deviceManager->createStateXml();
    if (deviceManagerToSave) {
      this->saveValue(SETTING_DEVICE_MANAGER, deviceManagerToSave.get());
    }
  }

  bool isLoadedPlugin(juce::PluginDescription &description) const {
    return description.fileOrIdentifier == this->loadedPlugin->getPluginDescription().fileOrIdentifier;
  }

  void actionListenerCallback(const juce::String &message) override {
    if (message == MESSAGE_CLOSE_PLUGIN) {
      this->deletePluginWindow();
      return;
    }

    if (message == MESSAGE_SHOW_PLUGIN) {
      this->createOrShowPluginWindow();
      return;
    }
  }
 private:
  std::unique_ptr<PassthroughProcessor> passThrough;
  std::unique_ptr<juce::AudioProcessorPlayer> audioProcessorPlayer;
  std::unique_ptr<PluginWindow> pluginWindow;

  void createOrShowPluginWindow() {
    if(this->pluginWindow){
      this->pluginWindow->setVisible(true);
      return;
    }

    this->pluginWindow = std::make_unique<PluginWindow>(*this->loadedPlugin, *this->userSettings, this);
  }

  void deletePluginWindow() {
    this->pluginWindow.reset();
  }

  void saveValue(const char *name, juce::XmlElement *value) const {
    this->userSettings->setValue(name, value);
    this->userSettings->saveIfNeeded();
  }

  static void doWithPermission(
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemwideVSTProcess)
};
