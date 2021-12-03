#pragma once

#include <JuceHeader.h>

class PassthroughProcessor :
  public juce::AudioProcessor {
public:
    PassthroughProcessor () :
      juce::AudioProcessor(
        BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo())
          .withOutput("Output", juce::AudioChannelSet::stereo()
          )
      ) {}


    void prepareToPlay (double, int) override {}

    void releaseResources () override {}

    void processBlock (juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override {
        ignoreUnused(buffer, midiMessages);
    }

    void processBlock (juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiMessages) override {
        ignoreUnused(buffer, midiMessages);
    }

    juce::AudioProcessorEditor *createEditor () override { return nullptr; }

    bool hasEditor () const override { return false; }


    const juce::String getName () const override { return {}; } // NOLINT(readability-const-return-type)

    bool acceptsMidi () const override { return false; }

    bool producesMidi () const override { return false; }

    double getTailLengthSeconds () const override { return 0; }


    int getNumPrograms () override { return 0; }

    int getCurrentProgram () override { return 0; }

    void setCurrentProgram (int) override {}

    const juce::String getProgramName (int) override { return {}; } // NOLINT(readability-const-return-type)

    void changeProgramName (int, const juce::String &) override {}

    void getStateInformation (juce::MemoryBlock &) override {}

    void setStateInformation (const void *, int) override {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PassthroughProcessor)
};

