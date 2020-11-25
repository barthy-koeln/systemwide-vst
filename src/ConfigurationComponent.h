#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "PluginSelectionTableModel.h"
#include "PassthroughProcessor.h"
#include "PluginWindow.h"
#include "SystemwideVSTProcess.h"
#include "DeviceSelectorComponent.h"

class ConfigurationComponent :
    public juce::Component,
    public juce::ChangeListener,
    public juce::ActionListener,
    public juce::ActionBroadcaster {

 public:
  explicit ConfigurationComponent(SystemwideVSTProcess &systemwideVSTProcess);
  ~ConfigurationComponent() override;

  void paint(juce::Graphics &g) override;
  void resized() override;

  void changeListenerCallback(juce::ChangeBroadcaster *) override;
  void actionListenerCallback(const juce::String &message) override;

 private:
  SystemwideVSTProcess &systemwideVSTProcess;

  std::unique_ptr<LookAndFeel> appLookAndFeel;
  std::unique_ptr<juce::KnownPluginList> knownPluginList;
  std::unique_ptr<juce::PluginListComponent> pluginListComponent;
  std::unique_ptr<DeviceSelectorComponent> audioDeviceSelector;

  PluginSelectionTableModel *getPluginSelectionModel();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationComponent)
  void savePluginList();
};
