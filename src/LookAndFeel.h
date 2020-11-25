#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
 public:
  char padding = 16;
  juce::Colour background = juce::Colour(0xff181818);
  juce::Colour textColour = juce::Colour(0xfff4f4f4);
  juce::Colour primaryDark = juce::Colour(0xff022c43);
  juce::Colour primary = juce::Colour(0xff115173);
  juce::Colour secondary = juce::Colour(0xffe4bc4e);
  juce::Colour tertiary = juce::Colour(0xffb33951);

  LookAndFeel () {
    this->setColour(juce::ResizableWindow::backgroundColourId, this->background);
    this->setColour(juce::Label::backgroundColourId, this->background);
    this->setColour(juce::ListBox::backgroundColourId, this->background);

    this->setColour(juce::ComboBox::backgroundColourId, this->background);
    this->setColour(juce::PopupMenu::backgroundColourId, this->background);

    this->setColour(juce::PopupMenu::highlightedBackgroundColourId, this->primaryDark);
    this->setColour(juce::PopupMenu::highlightedTextColourId, this->textColour);

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
      auto r = area.reduced(this->padding, 0);
      r.removeFromTop(r.getHeight() / 2 - 1);

      g.setColour(this->background.brighter());
      g.fillRect(r.removeFromTop(1));

      return;
    }

    auto finalTextColour = textColourToUse ?: &this->textColour;
    auto r = area.reduced(1);

    if (isHighlighted || isTicked) {
      g.setColour(isTicked ? this->primary : this->primaryDark);
      g.fillRect(r);
    }

    juce::Font font = getPopupMenuFont();
    auto maxFontHeight = (float) area.getHeight() / 1.3f;
    if (font.getHeight() > maxFontHeight) {
      font.setHeight(maxFontHeight);
    }

    g.setFont(font);

    auto iconArea = r.removeFromLeft((r.getHeight() * 5) / 4).reduced(3).toFloat();

    if (icon != nullptr) {
      icon->drawWithin(
        g,
        iconArea,
        (unsigned) juce::RectanglePlacement::centred | (unsigned) juce::RectanglePlacement::onlyReduceInSize,
        1.0f
      );
    }

    if (hasSubMenu) {
      auto arrowH = 0.6f * getPopupMenuFont().getAscent();

      auto x = (float) r.removeFromRight((int) arrowH).getX();
      auto halfH = (float) r.getCentreY();

      juce::Path p;
      p.addTriangle(
        x, halfH - arrowH * 0.5f,
        x, halfH + arrowH * 0.5f,
        x + arrowH * 0.6f, halfH
      );

      g.fillPath(p);
    }

    r.removeFromRight(3);

    g.setColour(isHighlighted ? findColour(juce::PopupMenu::highlightedTextColourId) : *finalTextColour);
    g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

    if (shortcutKeyText.isNotEmpty()) {
      juce::Font f2(font);
      f2.setHeight(f2.getHeight() * 0.75f);
      f2.setHorizontalScale(0.95f);
      g.setFont(f2);

      g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
    }
  }
};
