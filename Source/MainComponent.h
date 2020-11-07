#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "PluginSelectionTableModel.h"

class MainComponent : public juce::Component, public juce::ChangeListener {
 public:
  MainComponent();
  ~MainComponent() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  void changeListenerCallback(juce::ChangeBroadcaster *) override;

 private:

  std::unique_ptr<LookAndFeel> appLookAndFeel;

  std::unique_ptr<juce::PropertiesFile::Options> propertiesOptions;
  std::unique_ptr<juce::ApplicationProperties> applicationProperties;

  std::unique_ptr<juce::AudioDeviceManager> deviceManager;

  std::unique_ptr<juce::KnownPluginList> knownPluginList;
  std::unique_ptr<juce::AudioPluginFormatManager> pluginFormatManager;
  std::unique_ptr<juce::AudioProcessorPlayer> audioProcessorPlayer;

  std::unique_ptr<juce::PluginListComponent> pluginListComponent;

  std::unique_ptr<juce::AudioDeviceSelectorComponent> audioDeviceSelector;
  std::unique_ptr<juce::AudioPluginInstance> loadedPlugin;

  static void doWithPermission(
      juce::RuntimePermissions::PermissionID permission,
      const std::function<void(bool)> &callback
  );

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
