#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4 {
 public:
  LookAndFeel() {
    this->setColour(juce::ResizableWindow::backgroundColourId, this->background);
    this->setColour(juce::Label::backgroundColourId, this->background);
    this->setColour(juce::ListBox::backgroundColourId, this->background);

    this->setColour(juce::ComboBox::backgroundColourId, this->background);

    this->setColour(juce::TextButton::buttonColourId, this->primary);
    this->setColour(juce::TextButton::buttonOnColourId, this->primary_dark);

    this->setUsingNativeAlertWindows(true);

    juce::Font::setDefaultMinimumHorizontalScaleFactor(1.0f);
  }

  ~LookAndFeel() override = default;

  void drawButtonBackground(
      juce::Graphics &g,
      juce::Button &button,
      const juce::Colour &backgroundColour,
      bool shouldDrawButtonAsHighlighted,
      bool shouldDrawButtonAsDown
  ) override {
    auto bounds = button.getLocalBounds();

    auto baseColour = backgroundColour
        .withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 1.0f)
        .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted) {
      baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);
    }

    g.setColour(baseColour);
    g.fillRect(bounds);
  }

  char padding = 16;
  juce::Colour background = juce::Colour(0xff181818);
  juce::Colour primary_dark = juce::Colour(0xff022c43);
  juce::Colour primary = juce::Colour(0xff115173);
  juce::Colour secondary = juce::Colour(0xffe4bc4e);
  juce::Colour tertiary = juce::Colour(0xffb33951);

 private:
  juce::CustomTypeface typeface;

};
