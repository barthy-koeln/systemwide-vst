// Copyright (c) 2012 Bg Porter.
#pragma once

#include <JuceHeader.h>

/**
 * @class PassthroughProcessor
 *
 * No-op audio processor that we're really only using for test. You can create an
 * instance with 1 or 2 input/output channels. Whatever comes in goes out unaffected.
 */
class PassthroughProcessor : public juce::AudioProcessor {

 public:
  /**
   *  @param inputChannelCount number of desired input channels
   *  @param outputChannelCount number of desired output channels (if ==0, will match the setting of the input channels.)
   */
  explicit PassthroughProcessor(int inputChannelCount = 1, int outputChannelCount = 0);

  ~PassthroughProcessor() override;

  /**
   * get the name of this processor.
   * @return String filled with the name.
   */
  const juce::String getName() const override;

  /**
   * Called before playback starts to get things ready.
   * @param sampleRate samples per second.
   * @param estimatedSamplesPerBlock
   */
  void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override;

  void releaseResources() override;

  void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &midiMessages) override;

  const juce::String getInputChannelName(int channelIndex) const override;

  const juce::String getOutputChannelName(int channelIndex) const override;

  bool isInputChannelStereoPair(int index) const override;

  bool isOutputChannelStereoPair(int index) const override;

  bool silenceInProducesSilenceOut() const override;

  bool acceptsMidi() const override;

  bool producesMidi() const override;

  juce::AudioProcessorEditor *createEditor() override;

  bool hasEditor() const override;

  int getNumParameters() override;

  const juce::String getParameterName(int parameterIndex) override;

  float getParameter(int parameterIndex) override;

  const juce::String getParameterText(int parameterIndex) override;

  void setParameter(int parameterIndex, float newValue) override;

  int getNumPrograms() override;

  int getCurrentProgram() override;

  void setCurrentProgram(int index) override;

  const juce::String getProgramName(int index) override;

  void changeProgramName(int index, const juce::String &newName) override;

  void getStateInformation(juce::MemoryBlock &destData) override;

  void setStateInformation(const void *data, int sizeInBytes) override;
  double getTailLengthSeconds() const override;

 protected:
  int inputChannelCount;
  int outputChannelCount;

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PassthroughProcessor);
};