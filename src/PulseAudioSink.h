#pragma once

#define PULSE_AUDIO_SINK_ERROR "pa-sink-error"

#include <cstdio>

class PulseAudioSink
{

 public:
  PulseAudioSink () {
    this->handle = this->runCommand(
      "pactl load-module module-null-sink sink_properties=device.description=SystemwideIO sink_name=SystemwideIO"
    );
  }

  ~ PulseAudioSink () {
    this->runCommand(
      "pactl unload-module " + this->handle
    );
  }

 private:
  std::string handle{};

  std::string runCommand (const std::string input) {
    std::cout << "running command: [" << input << "]" << std::endl;

    std::unique_ptr<FILE, decltype(&pclose)> commandPipe(popen(input.c_str(), "r"), pclose);
    if (nullptr == commandPipe) {
      throw std::runtime_error(PULSE_AUDIO_SINK_ERROR);
    }

    std::array<char, 128> buffer;
    std::string output;

    while (fgets(buffer.data(), buffer.size(), commandPipe.get()) != nullptr) {
      output += buffer.data();
    }

    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());

    return output;
  }

};
