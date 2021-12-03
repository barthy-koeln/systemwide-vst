#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "PluginSelectionTableModel.h"
#include "PassthroughProcessor.h"
#include "PluginWindow.h"
#include "SystemwideIOProcess.h"

class ConfigurationComponent :
  public juce::Component,
  public juce::ChangeListener,
  public juce::ActionListener,
  public juce::ActionBroadcaster {

public:
    explicit ConfigurationComponent (SystemwideVSTProcess &systemwideVSTProcess);

    ~ConfigurationComponent () override;

    void paint (juce::Graphics &g) override;

    void resized () override;

    void changeListenerCallback (juce::ChangeBroadcaster *) override;

    void actionListenerCallback (const juce::String &message) override;

private:
    SystemwideVSTProcess &systemwideVSTProcess;

    std::unique_ptr<CustomLookAndFeel> appLookAndFeel;
    std::unique_ptr<juce::KnownPluginList> knownPluginList;
    std::unique_ptr<juce::PluginListComponent> pluginListComponent;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioDeviceSelector;
    juce::HyperlinkButton sponsorButton;
    juce::Label versionLabel;

    PluginSelectionTableModel *getPluginSelectionModel ();

    void savePluginList ();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationComponent)
};
