/*
  ==============================================================================

    TableListBox.h
    Created: 7 Nov 2020 4:04:36pm
    Author:  barthy

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

class PluginSelectionTableModel : public juce::TableListBoxModel, public juce::ChangeBroadcaster {
 public:
  PluginSelectionTableModel(
      juce::PluginListComponent &pluginListComponent,
      juce::KnownPluginList &list,
      LookAndFeel &lookAndFeel,
      juce::ChangeListener &listener
  ) : pluginListComponent(pluginListComponent), list(list), lookAndFeel(lookAndFeel) {
    this->addChangeListener(&listener);
  }

  ~PluginSelectionTableModel() override {
    this->removeAllChangeListeners();
  }

  int getNumRows() override {
    return this->list.getNumTypes() + this->list.getBlacklistedFiles().size();
  }

  void paintRowBackground(
      juce::Graphics &g,
      int /*rowNumber*/,
      int /*width*/,
      int /*height*/,
      bool rowIsSelected
  ) override {
    g.fillAll(rowIsSelected ? this->lookAndFeel.primary_dark : this->lookAndFeel.background);
  }

  enum {
    nameCol = 1,
    typeCol = 2,
    categoryCol = 3,
    manufacturerCol = 4,
    descCol = 5
  };

  [[nodiscard]] juce::String getCellText(bool isBlacklisted, int row, int columnId) const {
    if (isBlacklisted) {
      if (columnId == nameCol) {
        return this->list.getBlacklistedFiles()[row - this->list.getNumTypes()];
      }

      if (columnId == descCol) {
        return TRANS("Deactivated after failing to initialise correctly");
      }

      return "";
    }

    auto desc = this->list.getTypes()[row];

    switch (columnId) {
      case nameCol:return desc.name;
      case typeCol:return desc.pluginFormatName;
      case categoryCol:return desc.category.isNotEmpty() ? desc.category : "-";
      case manufacturerCol:return desc.manufacturerName;
      case descCol:return getPluginDescription(desc);
      default: jassertfalse;
        break;
    }

    return "";
  }

  void paintCell(juce::Graphics &g, int row, int column, int width, int height, bool /*rowIsSelected*/) override {
    bool isBlacklisted = row >= this->list.getNumTypes();
    juce::String text = this->getCellText(isBlacklisted, row, column);

    if (text.isEmpty()) {
      return;
    }

    const auto defaultTextColour = this->pluginListComponent.findColour(juce::ListBox::textColourId);
    g.setColour(isBlacklisted ? juce::Colours::red : defaultTextColour);
    g.setFont(juce::Font((float) height * 0.7f, nameCol == column ? juce::Font::bold : juce::Font::plain));
    g.drawFittedText(text, 4, 0, width - 6, height, juce::Justification::centredLeft, 1, 0.9f);
  }

  void cellClicked(int row, int column, const juce::MouseEvent &e) override {
    juce::TableListBoxModel::cellClicked(row, column, e);

    if (row >= 0 && row < getNumRows() && e.mods.isPopupMenu()) {
      auto menu = this->pluginListComponent.createMenuForRow(row);
      menu.showMenuAsync(juce::PopupMenu::Options().withDeletionCheck(this->pluginListComponent));
    }
  }

  void cellDoubleClicked(int row, int column, const juce::MouseEvent &e) override {
    juce::TableListBoxModel::cellDoubleClicked(row, column, e);

    if (row >= 0 && row < this->list.getNumTypes()) {
      this->selected = this->list.getTypes()[row];
      this->sendChangeMessage();
    }
  }

  void deleteKeyPressed(int) override {
    this->pluginListComponent.removeSelectedPlugins();
  }

  void sortOrderChanged(int newSortColumnId, bool isForwards) override {
    switch (newSortColumnId) {
      case nameCol:this->list.sort(juce::KnownPluginList::sortAlphabetically, isForwards);
        break;
      case typeCol:this->list.sort(juce::KnownPluginList::sortByFormat, isForwards);
        break;
      case categoryCol:this->list.sort(juce::KnownPluginList::sortByCategory, isForwards);
        break;
      case manufacturerCol:this->list.sort(juce::KnownPluginList::sortByManufacturer, isForwards);
        break;
      case descCol: break;

      default: jassertfalse;
        break;
    }
  }

  static juce::String getPluginDescription(const juce::PluginDescription &desc) {
    juce::StringArray items;

    if (desc.descriptiveName != desc.name) {
      items.add(desc.descriptiveName);
    }

    items.add("(Version " + desc.version + ")");

    items.removeEmptyStrings();
    return items.joinIntoString(" ");
  }

  juce::PluginListComponent &pluginListComponent;
  juce::KnownPluginList &list;
  LookAndFeel &lookAndFeel;

  juce::PluginDescription selected;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginSelectionTableModel)
};