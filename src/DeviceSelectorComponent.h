# pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

class DeviceSelectorComponent : public juce::Component, public juce::ChangeListener
{

 public:

  DeviceSelectorComponent (juce::AudioDeviceManager &deviceManager, LookAndFeel &appLookAndFeel) :
    deviceManager(deviceManager),
    appLookAndFeel(appLookAndFeel) {
    this->deviceType = std::unique_ptr<juce::AudioIODeviceType>(this->deviceManager.getAvailableDeviceTypes()[0]);

    this->inputDeviceComboBox = std::make_unique<juce::ComboBox>();
    this->outputDeviceComboBox = std::make_unique<juce::ComboBox>();

    this->inputDeviceComboBox->setLookAndFeel(&this->appLookAndFeel);
    this->outputDeviceComboBox->setLookAndFeel(&this->appLookAndFeel);

    this->addAndMakeVisible(*this->inputDeviceComboBox);
    this->addAndMakeVisible(*this->outputDeviceComboBox);

    this->setDeviceOptions();
  }

  ~DeviceSelectorComponent () override = default;

  void changeListenerCallback (juce::ChangeBroadcaster *source) override {
    if (source == &this->deviceManager) {
      this->setDeviceOptions();
    }
  }

  void resized () override {
    int comboBoxHeight = 32;
    auto rect = this->getLocalBounds();

    if (this->inputDeviceComboBox != nullptr) {
      this->inputDeviceComboBox->setBounds(rect.removeFromTop(comboBoxHeight));
      rect.removeFromTop(this->appLookAndFeel.padding);
    }

    if (this->outputDeviceComboBox != nullptr) {
      this->outputDeviceComboBox->setBounds(rect.removeFromTop(comboBoxHeight));
      rect.removeFromTop(this->appLookAndFeel.padding);
    }
  }

 private:
  juce::AudioDeviceManager &deviceManager;
  LookAndFeel &appLookAndFeel;
  std::unique_ptr<juce::AudioIODeviceType> deviceType;
  std::unique_ptr<juce::ComboBox> inputDeviceComboBox;
  std::unique_ptr<juce::ComboBox> outputDeviceComboBox;

  void setDeviceOptions () {
    this->setComboBoxOptions(true);
    this->setComboBoxOptions(false);
  }

  void setComboBoxOptions (bool input) {
    const juce::StringArray deviceNames = this->deviceType->getDeviceNames(input);
    auto *comboBox = input ? this->outputDeviceComboBox.get() : this->inputDeviceComboBox.get();

    comboBox->clear(juce::NotificationType::dontSendNotification);

    int id = 1;
    for (const juce::String &name : deviceNames) {
      comboBox->addItem(name, id++);
    }

    comboBox->addItem(DeviceSelectorComponent::getNoDeviceString(), -1);
    comboBox->setSelectedId(-1, juce::NotificationType::dontSendNotification);
  }

  static juce::String getNoDeviceString () { return "[" + TRANS("none") + "]"; }
};
