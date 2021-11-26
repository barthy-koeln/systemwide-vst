#pragma once

#include <JuceHeader.h>
#include "ConfigurationComponent.h"
#include "SystemwideIOProcess.h"

class ConfigurationWindow :
  public juce::DocumentWindow, public juce::ActionBroadcaster {
public:
    explicit ConfigurationWindow (SystemwideVSTProcess &systemwideVSTProcess, juce::ActionListener *listener) :
      juce::DocumentWindow(
        juce::String(ProjectInfo::projectName) + " Configuration",
        juce::Colour(0xff181818),
        DocumentWindow::closeButton
      ) {
        this->setUsingNativeTitleBar(true);
        this->addActionListener(listener);
        this->setResizable(false, false);
        this->setContentOwned(new ConfigurationComponent(systemwideVSTProcess), true);
        this->setVisible(true);
    }

    ~ConfigurationWindow () override {
        this->removeAllActionListeners();
    }

    void closeButtonPressed () override {
        this->sendActionMessage(MESSAGE_CLOSE_CONFIG);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationWindow)
};
