#if defined (LINUX)

#define TRAY_APPINDICATOR 1

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
          {
            .text = "PulseAudio Volume Control",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [] (::tray_menu *item) {
                auto thread = (TrayIconThread *) item->context;
                thread->sendActionMessage(MESSAGE_SHOW_PAVUCONTROL);
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
          .icon = "/home/barthy/Documents/systemwide_vst/assets/images/logo.svg", // TODO don't forget this one
          .menu = tray_menu
        };

        tray_init(&tray);
        while (tray_loop(0) == 0 && !this->threadShouldExit()) {}
        tray_exit();
    }
};

#endif
