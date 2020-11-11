#pragma once

#include <JuceHeader.h>
#include "ConfigurationComponent.h"
#include "SystemwideVSTProcess.h"

class ConfigurationWindow : juce::DocumentWindow {
 public:
  explicit ConfigurationWindow(SystemwideVSTProcess &systemwideVSTProcess) : DocumentWindow(
      juce::String(ProjectInfo::projectName) + " Configuration",
      juce::Colour(0xff181818),
      DocumentWindow::closeButton
  ) {
    this->setVisible(false);
    this->setSize(1024, 512);
    this->setIcon(juce::ImageFileFormat::loadFrom(BinaryData::logo_svg, BinaryData::logo_svgSize));
    this->setUsingNativeTitleBar(true);
    this->setResizable(false, false);
    this->centreWithSize(this->getWidth(), this->getHeight());

    this->setContentOwned(new ConfigurationComponent(systemwideVSTProcess), true);

    this->setVisible(true);
  }

  void closeButtonPressed() override {
    delete this;
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConfigurationWindow)
};
