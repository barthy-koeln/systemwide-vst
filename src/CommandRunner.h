#pragma once

#define PULSE_AUDIO_SINK_ERROR "pa-sink-error"
#define PULSE_AUDIO_CONTROL_ERROR "pa-control-error"

#include <cstdio>

class CommandRunner
{

 public:
  CommandRunner () = default;

  ~ CommandRunner () = default;

  void createPulseSink () {
    this->handle = CommandRunner::run(
      "pactl load-module module-null-sink sink_properties=device.description=SystemwideIO sink_name=SystemwideIO",
      PULSE_AUDIO_SINK_ERROR
    );
  }

  void removePulseSink () {
    CommandRunner::run(
      "pactl unload-module " + this->handle,
      PULSE_AUDIO_SINK_ERROR
    );

    this->handle.clear();
  }

  void openPulseAudioVolumeControl () {
    CommandRunner::run(
      "pavucontrol &",
      PULSE_AUDIO_CONTROL_ERROR
    );
  }

  void openSponsorshipPage () {
    CommandRunner::run(
      "xdg-open https://github.com/sponsors/barthy-koeln",
      PULSE_AUDIO_CONTROL_ERROR
    );
  }

 private:
  std::string handle{};

  static std::string run (const std::string &input, const std::string &errorId) {
    std::cout << "running command: [" << input << "]" << std::endl;

    std::unique_ptr<FILE, decltype(&pclose)> commandPipe(popen(input.c_str(), "r"), pclose);
    if (nullptr == commandPipe) {
      throw std::runtime_error(errorId);
    }

    std::array<char, 128> buffer{};
    std::string output;

    while (fgets(buffer.data(), buffer.size(), commandPipe.get()) != nullptr) {
      output += buffer.data();
    }

    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());

    return output;
  }

};
