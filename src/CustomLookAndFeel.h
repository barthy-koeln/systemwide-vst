#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel :
  public juce::LookAndFeel_V4 {
public:
    char padding = 16;
    juce::Colour backdrop = juce::Colour(0xff181818);
    juce::Colour background = juce::Colour(0xff282828);
    juce::Colour textColour = juce::Colour(0xfff4f4f4);
    juce::Colour primaryDark = juce::Colour(0xff022c43);
    juce::Colour primary = juce::Colour(0xff115173);
    juce::Colour secondary = juce::Colour(0xffe4bc4e);
    juce::Colour tertiary = juce::Colour(0xffb33951);

    CustomLookAndFeel () {
        this->setColourScheme(
          juce::LookAndFeel_V4::ColourScheme(
            this->backdrop,     // windowBackground
            this->backdrop,     // widgetBackground
            this->background,   // menuBackground
            this->textColour,   // outline
            this->textColour,   // defaultText
            this->textColour,   // defaultFill
            this->textColour,   // highlightedText
            this->primary,      // highlightedFill
            this->textColour    // menuText
          )
        );

        this->setColour(juce::TextButton::buttonColourId, this->primary);
        this->setColour(juce::TextButton::buttonOnColourId, this->primaryDark);

        juce::LookAndFeel_V4::setDefaultLookAndFeel(this);
        this->setUsingNativeAlertWindows(true);

        juce::Font::setDefaultMinimumHorizontalScaleFactor(1.0f);
    }

    ~CustomLookAndFeel () override = default;

    void drawButtonBackground (
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

    juce::Font getPopupMenuFont () override {
        return juce::Font(16.0f);
    }

    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu (juce::ComboBox &box, juce::Label &label) override {
        auto boxBounds = box.getScreenBounds();

        auto boxBoundOffset = boxBounds
          .withX(boxBounds.getBottomLeft().x)
          .withY(boxBounds.getBottomLeft().y + 2)
          .withWidth(0)
          .withHeight(0);

        return juce::PopupMenu::Options()
          .withTargetComponent(&box)
          .withTargetScreenArea(boxBoundOffset)
          .withItemThatMustBeVisible(box.getSelectedId())
          .withMinimumWidth(box.getWidth())
          .withMaximumNumColumns(1)
          .withStandardItemHeight(label.getHeight())
          .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards);
    }

    void drawPopupMenuBackground (juce::Graphics &g, int width, int height) override {
        g.setColour(this->background);
        g.fillRect(0, 0, width, height);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};
