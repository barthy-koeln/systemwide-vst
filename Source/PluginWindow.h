#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

/**
    A desktop window containing a plugin's GUI.
*/
class PluginWindow : public juce::DocumentWindow {
 public:
  PluginWindow(juce::AudioProcessor &processor, LookAndFeel &lookAndFeel) :
      DocumentWindow(processor.getName(), lookAndFeel.background, DocumentWindow::closeButton),
      processor(processor) {

    if (auto *ui = PluginWindow::createProcessorEditor(this->processor)) {
      this->setContentOwned(ui, true);
    }

    this->setUsingNativeTitleBar(true);
    this->setAlwaysOnTop(true);
    this->centreWithSize(this->getWidth(), this->getHeight());
    this->setVisible(true);
  }

  ~PluginWindow() override {
    clearContentComponent();
  }

  void closeButtonPressed() override {
    this->setVisible(false);
  }

  juce::AudioProcessor &processor;

 private:
  [[nodiscard]] float getDesktopScaleFactor() const override { return 1.0f; }

  static juce::AudioProcessorEditor *createProcessorEditor(juce::AudioProcessor &processor) {
    auto *ui = processor.createEditorIfNeeded();

    return nullptr != ui ? ui : new juce::GenericAudioProcessorEditor(processor);
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};