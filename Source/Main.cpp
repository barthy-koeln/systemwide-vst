#include <JuceHeader.h>

#include <memory>
#include "MainComponent.h"

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
    this->mainWindow = std::make_unique<MainWindow>(getApplicationName());
  }

  void shutdown() override {
    this->mainWindow = nullptr; // (deletes our window)
  }

  //==============================================================================
  void systemRequestedQuit() override {
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
  }

  void anotherInstanceStarted(const juce::String &commandLine) override {
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
  }

  class MainWindow : public juce::DocumentWindow {
   public:
    explicit MainWindow(const juce::String &name) : DocumentWindow(
        name,
        juce::Colour(0xff000000),
        DocumentWindow::closeButton
    ) {
      this->setUsingNativeTitleBar(true);
      this->setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
      setFullScreen (true);
#else
      this->setResizable(false, false);
      this->centreWithSize(this->getWidth(), this->getHeight());
#endif

      this->setVisible(true);
    }

    void closeButtonPressed() override {
      JUCEApplication::getInstance()->systemRequestedQuit();
    }

   private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
  };

 private:
  std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (systemwide_vstApplication)
