#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

/**
    A desktop window containing a plugin's GUI.
*/
class PluginWindow : public juce::DocumentWindow {
 public:
  PluginWindow(
      juce::AudioProcessor &processor,
      LookAndFeel &lookAndFeel,
      juce::ApplicationProperties &applicationProperties
  ) :
      DocumentWindow(processor.getName(), lookAndFeel.background, DocumentWindow::closeButton),
      processor(processor),
      applicationProperties(applicationProperties) {

    if (auto *ui = PluginWindow::createProcessorEditor(this->processor)) {
      this->setContentOwned(ui, true);
    }

    this->setUsingNativeTitleBar(true);
    this->setAlwaysOnTop(true);
    this->centreWithSize(this->getWidth(), this->getHeight());
  }

  ~PluginWindow() override {
    this->clearContentComponent();
  }

  void closeButtonPressed() override {
    this->setVisible(false);

    juce::MemoryBlock state;
    this->processor.getStateInformation(state);
    this->applicationProperties.getUserSettings()->setValue("processorState", state.toBase64Encoding());
    this->applicationProperties.saveIfNeeded();
    state.reset();
  }

 private:
  juce::ApplicationProperties &applicationProperties;
  juce::AudioProcessor &processor;

  [[nodiscard]] float getDesktopScaleFactor() const override { return 1.0f; }

  static juce::AudioProcessorEditor *createProcessorEditor(juce::AudioProcessor &processor) {
    auto *ui = processor.createEditorIfNeeded();

    return nullptr != ui ? ui : new juce::GenericAudioProcessorEditor(processor);
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};