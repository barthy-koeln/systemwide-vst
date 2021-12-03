#include <JuceHeader.h>
#include "ConfigurationComponent.h"
#include "PluginSelectionTableModel.h"
#include "SettingsConstants.h"
#include "CustomLookAndFeel.h"

#define  PANEL_SIZE 512

ConfigurationComponent::ConfigurationComponent (SystemwideVSTProcess &systemwideVSTProcess) :
  systemwideVSTProcess(systemwideVSTProcess) {
    this->appLookAndFeel = std::make_unique<CustomLookAndFeel>();

    this->addActionListener(&this->systemwideVSTProcess);

    this->audioDeviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
      this->systemwideVSTProcess.getDeviceManager(),
      0,
      2,
      0,
      2,
      false,
      false,
      true,
      true
    );

    this->knownPluginList = std::make_unique<juce::KnownPluginList>();
    std::unique_ptr<juce::XmlElement> xmlPluginList = this->systemwideVSTProcess.getXmlValue(SETTING_PLUGIN_LIST);
    if (xmlPluginList) {
        this->knownPluginList->recreateFromXml(*xmlPluginList);
    }
    this->knownPluginList->sort(juce::KnownPluginList::sortAlphabetically, true);

    this->pluginListComponent = std::make_unique<juce::PluginListComponent>(
      this->systemwideVSTProcess.getPluginFormatManager(),
      *this->knownPluginList,
      this->systemwideVSTProcess.getCrashFile(),
      nullptr
    );

    this->pluginListComponent->setTableModel(
      new PluginSelectionTableModel(
        *this->pluginListComponent,
        *this->knownPluginList,
        *this->appLookAndFeel,
        *this
      )
    );

    this->knownPluginList->addChangeListener(this);

    if (this->systemwideVSTProcess.hasLoadedPlugin()) {
        this->getPluginSelectionModel()->setSelected(this->systemwideVSTProcess.getLoadedPluginDescription());
    }

    juce::String urlPath("https://github.com/sponsors/barthy-koeln");
    juce::String buttonText("Sponsor on GitHub");
    juce::URL url(urlPath);
    this->sponsorButton.setButtonText(buttonText);
    this->sponsorButton.setURL(url);

    this->versionLabel.setText(
      ProjectInfo::projectName + juce::String(" v") + ProjectInfo::versionString,
      juce::NotificationType::dontSendNotification
    );

    this->sponsorButton.setFont(
      this->versionLabel.getFont(),
      false,
      juce::Justification::right
    );

    this->addAndMakeVisible(*this->audioDeviceSelector);
    this->addAndMakeVisible(*this->pluginListComponent);
    this->addAndMakeVisible(this->versionLabel);
    this->addAndMakeVisible(this->sponsorButton);
    this->setSize(2 * PANEL_SIZE, PANEL_SIZE);
}

ConfigurationComponent::~ConfigurationComponent () {
    this->knownPluginList->removeChangeListener(this);
    this->removeAllActionListeners();

    this->pluginListComponent->setTableModel(nullptr);
    this->setLookAndFeel(nullptr);
}

void ConfigurationComponent::changeListenerCallback (juce::ChangeBroadcaster *source) {
    if (source == this->knownPluginList.get()) {
        this->savePluginList();
        return;
    }
}

void ConfigurationComponent::actionListenerCallback (const juce::String &message) {
    if (message == MESSAGE_SELECTION_CHANGED) {
        juce::PluginDescription description = this->getPluginSelectionModel()->getSelected();

        if (!this->systemwideVSTProcess.isLoadedPlugin(description)) {
            this->systemwideVSTProcess.loadPlugin(description);
        }

        this->sendActionMessage(MESSAGE_SHOW_PLUGIN);
        return;
    }
}

void ConfigurationComponent::paint (juce::Graphics &g) {
    g.fillAll(this->getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ConfigurationComponent::resized () {
    juce::FlexBox windowBox;
    juce::FlexBox panelBox;
    juce::FlexBox finePrintBox;

    panelBox.flexDirection = juce::FlexBox::Direction::row;
    panelBox.alignItems = juce::FlexBox::AlignItems::center;
    panelBox.flexWrap = juce::FlexBox::Wrap::wrap;

    if (this->audioDeviceSelector && this->pluginListComponent) {
        panelBox.items.add(
          juce::FlexItem(*this->audioDeviceSelector)
            .withWidth(PANEL_SIZE - (2 * this->appLookAndFeel->padding))
            .withHeight(.5 * PANEL_SIZE)
        );

        panelBox.items.add(
          juce::FlexItem(*this->pluginListComponent)
            .withMinWidth(PANEL_SIZE)
            .withHeight(.5 * PANEL_SIZE)
            .withMargin(juce::FlexItem::Margin(12, 0, 0, 0))
        );
    }

    finePrintBox.flexDirection = juce::FlexBox::Direction::row;
    finePrintBox.alignItems = juce::FlexBox::AlignItems::flexEnd;
    finePrintBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    finePrintBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    finePrintBox.items.add(
      juce::FlexItem(this->versionLabel)
        .withHeight(32.0f)
        .withFlex(1, 0, juce::FlexItem::autoValue)
        .withAlignSelf(juce::FlexItem::AlignSelf::flexEnd)
    );
    finePrintBox.items.add(
      juce::FlexItem(this->sponsorButton)
        .withHeight(32.0f)
        .withWidth((float) this->sponsorButton.getWidth())
        .withFlex(1, 0, juce::FlexItem::autoValue)
        .withAlignSelf(juce::FlexItem::AlignSelf::flexEnd)
    );

    windowBox.flexDirection = juce::FlexBox::Direction::column;
    windowBox.alignItems = juce::FlexBox::AlignItems::flexStart;
    windowBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    windowBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    windowBox.items.add(
      juce::FlexItem(panelBox)
        .withFlex(1, 0, juce::FlexItem::autoValue)
        .withAlignSelf(juce::FlexItem::AlignSelf::stretch)
    );
    windowBox.items.add(
      juce::FlexItem(finePrintBox)
        .withFlex(1, 0, juce::FlexItem::autoValue)
        .withMargin(juce::FlexItem::Margin(this->appLookAndFeel->padding, 0, 0, 0))
        .withAlignSelf(juce::FlexItem::AlignSelf::stretch)
    );

    windowBox.performLayout(this->getLocalBounds().reduced(this->appLookAndFeel->padding).toFloat());
}

PluginSelectionTableModel *ConfigurationComponent::getPluginSelectionModel () {
    return dynamic_cast<PluginSelectionTableModel *>(this->pluginListComponent->getTableListBox().getModel());
}

void ConfigurationComponent::savePluginList () {
    for (
        const juce::PluginDescription &plugin : this->knownPluginList->getTypes()) {
        if (plugin.isInstrument) {
            this->knownPluginList->removeType(plugin);
            this->knownPluginList->addToBlacklist(plugin.fileOrIdentifier);
        }
    }

    std::unique_ptr<juce::XmlElement> pluginListToSave = this->knownPluginList->createXml();
    if (pluginListToSave) {
        this->systemwideVSTProcess.savePluginList(pluginListToSave.get());
    }
}
