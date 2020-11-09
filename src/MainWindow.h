#pragma once

#include <JuceHeader.h>
#include "MainComponent.h"

class MainWindow : public juce::DocumentWindow {
 public:
  explicit MainWindow(const juce::String &name) : DocumentWindow(
      name,
      juce::Colour(0xff000000),
      DocumentWindow::closeButton
  ) {

    this->setUsingNativeTitleBar(true);
    this->setContentOwned(new MainComponent(), true);

    this->setResizable(false, false);
    this->centreWithSize(this->getWidth(), this->getHeight());

    this->setVisible(true);
  }

  void closeButtonPressed() override {
    this->setVisible(false);
    // JUCEApplication::getInstance()->systemRequestedQuit();
  }

 private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
