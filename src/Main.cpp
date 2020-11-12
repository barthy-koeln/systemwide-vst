#include <JuceHeader.h>

#include <memory>
#include "ConfigurationWindow.h"
#include "SystemwideVSTProcess.h"
#include "TrayIconThread.h"

class systemwide_vstApplication : public juce::JUCEApplication, public juce::ActionListener {
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
      this->createOrShowConfigWindow();
    }

    this->systemwideVSTProcess->loadSavedPlugin();

    /*
    this->trayIconThread = std::make_unique<TrayIconThread>();
    this->trayIconThread->startThread(juce::Thread::realtimeAudioPriority);
    this->trayIconThread->addActionListener(this->systemwideVSTProcess.get());
    this->trayIconThread->addActionListener(this);
     */
  }

  void shutdown() override {
    this->trayIconThread->stopThread(3000);
  }

  void systemRequestedQuit() override {
    juce::JUCEApplicationBase::quit();
  }

  void anotherInstanceStarted(const juce::String &commandLine) override {}

  void actionListenerCallback(const juce::String &message) override {
    if (message == MESSAGE_CLOSE_CONFIG) {
      this->deleteConfigWindow();
      return;
    }

    if (message == MESSAGE_SHOW_CONFIG) {
      this->createOrShowConfigWindow();
      return;
    }

    if (message == MESSAGE_QUIT) {
      juce::JUCEApplicationBase::quit();
      return;
    }
  }

  void createOrShowConfigWindow() {
    if (this->configurationWindow) {
      this->configurationWindow->setVisible(true);
      return;
    }

    this->configurationWindow = std::make_unique<ConfigurationWindow>(*this->systemwideVSTProcess, this);
  }

  void deleteConfigWindow() {
    this->configurationWindow.reset();
  }

 private:
  std::unique_ptr<ConfigurationWindow> configurationWindow;
  std::unique_ptr<SystemwideVSTProcess> systemwideVSTProcess;
  std::unique_ptr<TrayIconThread> trayIconThread;
};

START_JUCE_APPLICATION (systemwide_vstApplication)
