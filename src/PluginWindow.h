#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "SettingsConstants.h"
#include "MessagesConstants.h"

/**
    A desktop window containing a plugin's GUI.
*/
class PluginWindow : public juce::DocumentWindow, juce::ActionBroadcaster {
 public:
  PluginWindow(
      juce::AudioProcessor &processor,
      juce::ActionListener *actionListener
  ) :
      DocumentWindow(processor.getName(), juce::Colour(0xff181818), DocumentWindow::closeButton),
      processor(processor) {

    this->setUsingNativeTitleBar(true);
    this->addActionListener(actionListener);

    if (auto *ui = PluginWindow::createProcessorEditor(this->processor)) {
      this->setContentOwned(ui, true);
    }
  }

  ~PluginWindow() override {
    this->clearContentComponent();
  }

  void closeButtonPressed() override {
    this->sendActionMessage(MESSAGE_CLOSE_PLUGIN);
  }

 private:
  juce::AudioProcessor &processor;

  [[nodiscard]] float getDesktopScaleFactor() const override { return 1.0f; }

  static juce::AudioProcessorEditor *createProcessorEditor(juce::AudioProcessor &processor) {
    auto *ui = processor.createEditorIfNeeded();

    return nullptr != ui ? ui : new juce::GenericAudioProcessorEditor(processor);
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};