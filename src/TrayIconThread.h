
#if defined (LINUX)
#define TRAY_APPINDICATOR 1
#define JUCE_LINUX 1

#include "tray.h"
#include <gtk/gtkx.h>
#include <JuceHeader.h>

class TrayIconThread : public juce::Thread, public juce::ActionBroadcaster {

 public:
  TrayIconThread() : Thread("TrayIconThread") {}

  ~TrayIconThread() override {
    this->removeAllActionListeners();
  }

  void run() override {
    struct tray_menu tray_menu[] = {
        {
            .text = "Show VST Editor",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [](struct tray_menu *item) {
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
            .cb = [](struct tray_menu *item) {
              auto thread = (TrayIconThread *) item->context;
              thread->sendActionMessage(MESSAGE_SHOW_CONFIG);
            },
            .context = this
        },
        {
            .text = "Quit",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [](struct tray_menu *item) {
              auto thread = (TrayIconThread *) item->context;
              thread->sendActionMessage(MESSAGE_QUIT);
            },
            .context = this
        },
        {nullptr, 0, 0, 0, nullptr, nullptr}
    };

    struct tray tray = {
        .icon = "/home/barthy/Documents/systemwide_vst/assets/images/logo.svg",
        .menu = tray_menu
    };

    tray_init(&tray);
    while (tray_loop(0) == 0 && !this->threadShouldExit()) {}
    tray_exit();
  }
};

#endif
