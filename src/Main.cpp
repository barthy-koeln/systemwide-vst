#include <JuceHeader.h>

#include <memory>
#include "ConfigurationWindow.h"
#include "SystemwideVSTProcess.h"
#include "TrayIconThread.h"

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

    this->trayIconThread = std::make_unique<TrayIconThread>();
    this->trayIconThread->startThread(juce::Thread::realtimeAudioPriority);
  }

  void shutdown() override {
    this->trayIconThread->stopThread(300);
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
  std::unique_ptr<TrayIconThread> trayIconThread;
};

START_JUCE_APPLICATION (systemwide_vstApplication)
