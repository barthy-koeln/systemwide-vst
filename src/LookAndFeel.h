#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
 public:
  char padding = 16;
  juce::Colour backdrop = juce::Colour(0xff181818);
  juce::Colour background = juce::Colour(0xff282828);
  juce::Colour textColour = juce::Colour(0xfff4f4f4);
  juce::Colour primaryDark = juce::Colour(0xff022c43);
  juce::Colour primary = juce::Colour(0xff115173);
  juce::Colour secondary = juce::Colour(0xffe4bc4e);
  juce::Colour tertiary = juce::Colour(0xffb33951);

  LookAndFeel () {
    this->setColour(juce::ResizableWindow::backgroundColourId, this->backdrop);
    this->setColour(juce::Label::backgroundColourId, this->backdrop);
    this->setColour(juce::ListBox::backgroundColourId, this->backdrop);

    this->setColour(juce::ComboBox::backgroundColourId, this->background);

    this->setColour(juce::TextButton::buttonColourId, this->primary);
    this->setColour(juce::TextButton::buttonOnColourId, this->primaryDark);

    this->setUsingNativeAlertWindows(true);

    juce::Font::setDefaultMinimumHorizontalScaleFactor(1.0f);
  }

  ~LookAndFeel () override = default;

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
    return juce::Font(14.0f);
  }

  juce::PopupMenu::Options getOptionsForComboBoxPopupMenu (juce::ComboBox &box, juce::Label &label) override {
    auto boxBounds = box.getScreenBounds();

    auto boxBoundOffset = boxBounds.withTop(boxBounds.getBottom() + this->padding).withHeight(boxBounds.getHeight());

    return juce::PopupMenu::Options()
      .withTargetScreenArea(boxBoundOffset)
      .withTargetComponent(&box)
      .withItemThatMustBeVisible(box.getSelectedId())
      .withMinimumWidth(box.getWidth())
      .withMaximumNumColumns(1)
      .withStandardItemHeight(label.getHeight());
  }

  void drawPopupMenuItem (
    juce::Graphics &g,
    const juce::Rectangle<int> &area,
    const bool isSeparator,
    const bool isActive,
    const bool isHighlighted,
    const bool isTicked,
    const bool hasSubMenu,
    const juce::String &text,
    const juce::String &shortcutKeyText,
    const juce::Drawable *icon,
    const juce::Colour *const textColourToUse
  ) override {
    if (isSeparator) {
      auto rect = area.reduced(this->padding, 0).withHeight(2);
      g.setColour(this->background);
      g.fillRect(rect);

      return;
    }

    auto finalTextColour = textColourToUse ?: &this->textColour;
    auto rect = area;

    if (isHighlighted || isTicked) {
      g.setColour(isTicked ? this->primary : this->primaryDark);
      g.fillRect(rect);
    }

    juce::Font font = getPopupMenuFont();
    auto maxFontHeight = (float) area.getHeight() / 1.3f;
    if (font.getHeight() > maxFontHeight) {
      font.setHeight(maxFontHeight);
    }

    g.setFont(font);

    if (icon != nullptr) {
      auto iconArea = rect.removeFromLeft((rect.getHeight() * 5) / 4).reduced(3).toFloat();
      icon->drawWithin(
        g,
        iconArea,
        (unsigned) juce::RectanglePlacement::centred | (unsigned) juce::RectanglePlacement::onlyReduceInSize,
        1.0f
      );
    }

    if (hasSubMenu) {
      auto arrowH = 0.6f * getPopupMenuFont().getAscent();

      auto x = (float) rect.removeFromRight((int) arrowH).getX();
      auto halfH = (float) rect.getCentreY();

      juce::Path p;
      p.addTriangle(
        x, halfH - arrowH * 0.5f,
        x, halfH + arrowH * 0.5f,
        x + arrowH * 0.6f, halfH
      );

      g.fillPath(p);
    }

    rect.reduce(this->padding, 0);
    g.setColour(isHighlighted ? findColour(juce::PopupMenu::highlightedTextColourId) : *finalTextColour);
    g.drawFittedText(text, rect, juce::Justification::centredLeft, 1);

    if (shortcutKeyText.isNotEmpty()) {
      juce::Font f2(font);
      f2.setHeight(f2.getHeight() * 0.75f);
      f2.setHorizontalScale(0.95f);
      g.setFont(f2);

      g.drawText(shortcutKeyText, rect, juce::Justification::centredRight, true);
    }
  }

  void drawPopupMenuBackground (juce::Graphics &g, int width, int height) override {
    g.setColour(this->background);
    g.fillRect(0, 0, width, height);
  }
};
