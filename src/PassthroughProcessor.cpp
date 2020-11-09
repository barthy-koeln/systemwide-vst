// Copyright (c) 2012 Bg Porter


#include "PassthroughProcessor.h"

PassthroughProcessor::PassthroughProcessor(int inputChannelCount, int outputChannelCount) {
  jassert(inputChannelCount >= 1);
  jassert(inputChannelCount <= 2);
  jassert(outputChannelCount >= 0);
  jassert(outputChannelCount <= 2);

  this->inputChannelCount = inputChannelCount;
  this->outputChannelCount = outputChannelCount;

  if (0 == this->outputChannelCount) {
    this->outputChannelCount = this->inputChannelCount;
  }

  this->setLatencySamples(0);
  this->setPlayConfigDetails(this->inputChannelCount, this->outputChannelCount, 0, 0);
}

PassthroughProcessor::~PassthroughProcessor() = default;

const juce::String PassthroughProcessor::getName() const { // NOLINT(readability-const-return-type)
  return "PassthroughProcessor";
}

void PassthroughProcessor::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) {
  this->setPlayConfigDetails(this->inputChannelCount, this->outputChannelCount, sampleRate,estimatedSamplesPerBlock);
}

void PassthroughProcessor::releaseResources() {

}

void PassthroughProcessor::processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &midiMessages) {

}

const juce::String PassthroughProcessor::getInputChannelName(int channelIndex) const { // NOLINT(readability-const-return-type)
  return "channel " + juce::String(channelIndex);
}

const juce::String PassthroughProcessor::getOutputChannelName(int channelIndex) const { // NOLINT(readability-const-return-type)
  return "channel " + juce::String(channelIndex);
}

bool PassthroughProcessor::isInputChannelStereoPair(int index) const {
  return (2 == inputChannelCount);
}

bool PassthroughProcessor::isOutputChannelStereoPair(int index) const {
  return (2 == outputChannelCount);
}

bool PassthroughProcessor::silenceInProducesSilenceOut() const {
  return true;
}

bool PassthroughProcessor::acceptsMidi() const {
  return false;
}

bool PassthroughProcessor::producesMidi() const {
  return false;
}

juce::AudioProcessorEditor *PassthroughProcessor::createEditor() {
  return nullptr;
}

bool PassthroughProcessor::hasEditor() const {
  return false;
}

int PassthroughProcessor::getNumParameters() {
  return 0;
}

const juce::String PassthroughProcessor::getParameterName(int parameterIndex) { // NOLINT(readability-const-return-type)
  return "parameter " + juce::String(parameterIndex);
}

float PassthroughProcessor::getParameter(int parameterIndex) {
  return 0.f;
}

const juce::String PassthroughProcessor::getParameterText(int parameterIndex) { // NOLINT(readability-const-return-type)
  return "0.0";
}

void PassthroughProcessor::setParameter(int parameterIndex, float newValue) {

}

int PassthroughProcessor::getNumPrograms() {
  return 0;
}

int PassthroughProcessor::getCurrentProgram() {
  return 0;
}

void PassthroughProcessor::setCurrentProgram(int index) {

}

const juce::String PassthroughProcessor::getProgramName(int index) { // NOLINT(readability-const-return-type)
  return "program #" + juce::String(index);
}

void PassthroughProcessor::changeProgramName(int index, const juce::String &newName) {

}

void PassthroughProcessor::getStateInformation(juce::MemoryBlock &destData) {

}

void PassthroughProcessor::setStateInformation(const void *data, int sizeInBytes) {

}
double PassthroughProcessor::getTailLengthSeconds() const {
  return 0;
}

