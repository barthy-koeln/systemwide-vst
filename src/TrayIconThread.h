#if defined (LINUX)

#define TRAY_APPINDICATOR 1
#define TRAY_APPINDICATOR_ID "SystemwideIO"

#include <tray.h>
#include <gtk/gtkx.h>
#include <JuceHeader.h>

class TrayIconThread :
  public juce::Thread,
  public juce::ActionBroadcaster {

public:
    TrayIconThread () :
      Thread("TrayIconThread") {}

    ~TrayIconThread () override {
        this->removeAllActionListeners();
    }

    void run () override {
        ::tray_menu tray_menu[] = {
          {
            .text = "Show VST Editor",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [] (::tray_menu *item) {
                auto thread = (TrayIconThread *) item->context;
                thread->sendActionMessage(MESSAGE_SHOW_PLUGIN);
            },
            .context = this
          },
          {
            .text = "Preferences",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [] (::tray_menu *item) {
                auto thread = (TrayIconThread *) item->context;
                thread->sendActionMessage(MESSAGE_SHOW_CONFIG);
            },
            .context = this
          },
          {"-",     0, 0, 0, nullptr, nullptr},
          {
            .text = "Quit",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [] (::tray_menu *item) {
                auto thread = (TrayIconThread *) item->context;
                thread->sendActionMessage(MESSAGE_QUIT);
            },
            .context = this
          },
          {nullptr, 0, 0, 0, nullptr, nullptr}
        };

        ::tray tray = {
          .icon = "/home/barthy/CLionProjects/systemwide-vst/assets/images/logo.svg", // TODO don't forget this one
          .menu = tray_menu
        };

        auto result = tray_init(&tray);

        if (result == -1) {
            std::cout << "error?" << std::endl;
        }

        while (tray_loop(0) == 0 && !this->threadShouldExit()) {}
        tray_exit();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrayIconThread)
};

#endif
