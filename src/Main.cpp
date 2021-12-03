#include <memory>
#include <JuceHeader.h>
#include "ConfigurationWindow.h"
#include "SystemwideIOProcess.h"
#include "TrayIconThread.h"
#include "CommandRunner.h"

class systemwide_vstApplication :
  public juce::JUCEApplication,
  public juce::ActionListener {
public:
    systemwide_vstApplication () = default;

    const juce::String getApplicationName () override { // NOLINT(readability-const-return-type)
        return ProjectInfo::projectName;
    }

    const juce::String getApplicationVersion () override { // NOLINT(readability-const-return-type)
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed () override {
        return false;
    }

    void initialise (const juce::String &/*commandLine*/) override {
        this->commandRunner = std::make_unique<CommandRunner>();
        this->systemwideVSTProcess = std::make_unique<SystemwideVSTProcess>();
        this->systemwideVSTProcess->loadSavedPlugin();

        this->commandRunner->createPulseSink();

        if (this->systemwideVSTProcess->shouldShowConfig()) {
            this->createOrShowConfigWindow();
        }

        this->trayIconThread = std::make_unique<TrayIconThread>();
        this->trayIconThread->startThread(0);
        this->trayIconThread->addActionListener(this->systemwideVSTProcess.get());

        this->systemwideVSTProcess->addActionListener(this);
        this->trayIconThread->addActionListener(this);

#if !NDEBUG
        this->createOrShowConfigWindow();
#endif
    }

    void shutdown () override {
        this->commandRunner->removePulseSink();
        this->commandRunner.reset();
        this->trayIconThread->stopThread(3000);
    }

    void systemRequestedQuit () override {
        JUCEApplicationBase::quit();
    }

    void anotherInstanceStarted (const juce::String &/*commandLine*/) override {}

    void actionListenerCallback (const juce::String &message) override {
        if (message == MESSAGE_CLOSE_CONFIG) {
            this->deleteConfigWindow();
            return;
        }

        if (message == MESSAGE_SHOW_CONFIG) {
            this->createOrShowConfigWindow();
            return;
        }

        if (message == MESSAGE_QUIT) {
            JUCEApplicationBase::quit();
            return;
        }
    }

    void createOrShowConfigWindow () {
        if (this->configurationWindow) {
            this->configurationWindow->setVisible(true);
            this->configurationWindow->toFront(true);
            return;
        }

        this->configurationWindow = std::make_unique<ConfigurationWindow>(*this->systemwideVSTProcess, this);
    }

    void deleteConfigWindow () {
        this->configurationWindow.reset();
    }

private:
    std::unique_ptr<CommandRunner> commandRunner;
    std::unique_ptr<ConfigurationWindow> configurationWindow;
    std::unique_ptr<SystemwideVSTProcess> systemwideVSTProcess;
    std::unique_ptr<TrayIconThread> trayIconThread;
};

START_JUCE_APPLICATION (systemwide_vstApplication)
