#include <JuceHeader.h>

#include <memory>
#include "ConfigurationWindow.h"
#include "SystemwideVSTProcess.h"

class systemwide_vstApplication : public juce::JUCEApplication {
 public:
  systemwide_vstApplication() = default;

  const juce::String getApplicationName() override { // NOLINT(readability-const-return-type)
    return ProjectInfo::projectName;
  }

  const juce::String getApplicationVersion() override { // NOLINT(readability-const-return-type)
    return ProjectInfo::versionString;
  }

  bool moreThanOneInstanceAllowed() override {
    return false;
  }

  void initialise(const juce::String &commandLine) override {
    this->systemwideVSTProcess = std::make_unique<SystemwideVSTProcess>();

    if (this->systemwideVSTProcess->shouldShowConfig()) {
      this->configurationWindow = std::make_unique<ConfigurationWindow>(*this->systemwideVSTProcess);
    }

    this->systemwideVSTProcess->loadSavedPlugin();
  }

  void shutdown() override {
    // do nothing
  }

  //==============================================================================
  void systemRequestedQuit() override {
    quit();
  }

  void anotherInstanceStarted(const juce::String &commandLine) override {}

 private:
  std::unique_ptr<ConfigurationWindow> configurationWindow;
  std::unique_ptr<SystemwideVSTProcess> systemwideVSTProcess;
};

START_JUCE_APPLICATION (systemwide_vstApplication)
