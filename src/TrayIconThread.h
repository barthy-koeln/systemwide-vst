
#if defined (LINUX)
#define TRAY_APPINDICATOR 1
#define JUCE_LINUX 1

#include "tray.h"
#include <gtk/gtkx.h>
#include <JuceHeader.h>

class TrayIconThread : public juce::Thread {

 public:
  TrayIconThread() : Thread("TrayIconThread") {}

  void run() override {
    struct tray_menu tray_menu[] = {
        {
            .text = "Show VST Editor",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [](struct tray_menu *item) {
              auto thread = (TrayIconThread*) item->context;
            },
            .context = this
        },
        {
            .text = "-",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = nullptr,
            .context = this
        },
        {
            .text = "Quit",
            .disabled = 0,
            .checked = 0,
            .checkbox = 0,
            .cb = [](struct tray_menu *item) {
              auto thread = (TrayIconThread*) item->context;
            },
            .context = this
        },
        {nullptr, 0, 0, 0, nullptr, nullptr}
    };

    struct tray tray = {
        .icon = "/home/barthy/Documents/systemwide_vst/assets/images/logo_small.png",
        .menu = tray_menu
    };

    tray_init(&tray);
    while (!threadShouldExit() && tray_loop(1) == 0) {}
    tray_exit();
  }
};

#endif
