#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Dsp.h"

class SishhinProcessor final : public juce::AudioProcessor {
public:
    SishhinProcessor(); ~SishhinProcessor() override = default;
    void prepareToPlay(double,int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override {return true;}
    const juce::String getName() const override {return "SISHHIN AMP";}
    bool acceptsMidi() const override {return false;} bool producesMidi() const override {return false;}
    double getTailLengthSeconds() const override {return 60.;}
    static const juce::StringArray& presetNames();
    int getNumPrograms() override {return presetNames().size();} int getCurrentProgram() override {return currentPreset.load();}
    void setCurrentProgram(int index) override {factoryPreset(index);} const juce::String getProgramName(int index) override {return presetNames()[juce::jlimit(0,presetNames().size()-1,index)];}
    void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;
    juce::AudioProcessorValueTreeState state;
    float value(const char* id) const {return state.getRawParameterValue(id)->load();}
    void set(const juce::String& id,float v);
    void factoryPreset(int);
    juce::Result loadIR(const juce::File&);
    juce::Result loadReference(const juce::File&);
    void captureTone(){captureRequest.store(true);}
    void calibrate(){calibrateRequest.store(true);}
    bool takeCalibration(float&);
    juce::String tunerText();
    juce::String irName() const;
    std::atomic<float> inputPeak{0},outputPeak{0};
    std::atomic<float> tunerCents{0};
    std::atomic<int> captureRemaining{0};
    std::atomic<bool> profileReady{false};
    std::array<std::atomic<float>,sishhin::bands> matchDb;
    juce::AudioProcessorParameter* getBypassParameter() const override {return state.getParameter("bypass");}
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout parameters();
    double rate=48000; int maxBlock=512;
    struct Channel {
        sishhin::Biquad pre,colour,dc,low,mid,high,presence,loCut,hiCut,subCut,cleanse;
        std::array<sishhin::Biquad,sishhin::bands> match;
        sishhin::Pitch whammy,sub,dissonance;
        float envelope=0,gate=0,previous=0,attackFast=0,attackSlow=0,sagEnvelope=0;
    };
    std::array<Channel,4> channels;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    std::array<std::unique_ptr<juce::dsp::Convolution>,5> cabinets;
    juce::SmoothedValue<float> inGain,outGain,whammyShift,driveGain;
    juce::AudioBuffer<float> parallelSub,ampWork;
    juce::AudioBuffer<float> delayBuffer,reverbWork;
    juce::Reverb reverb;
    std::array<sishhin::Biquad,2> delayTone,spaceTone;
    juce::SmoothedValue<float> delaySamples,delayWet,reverbWet;
    int delayWrite=0;float duckEnvelope=0;
    std::atomic<int> currentPreset{0};
    double stutterPhase=0; float stutterGain=1;
    juce::AudioFormatManager formats;
    mutable juce::CriticalSection assetsLock;
    juce::AudioBuffer<float> pendingIR;
    double pendingRate=48000; bool hasPendingIR=false;
    juce::MemoryBlock irBytes; juce::String customIRName="No custom IR";
    std::atomic<bool> customReady{false},captureRequest{false},calibrateRequest{false};
    sishhin::Spectrum capture;
    std::array<float,sishhin::bands> target{};
    std::array<std::atomic<float>,sishhin::bands> targetPublished;
    int calibrationSamples=0;double calibrationEnergy=0;int calibrationCount=0;
    std::atomic<float> calibrationResult{1000.f};
    juce::AbstractFifo tunerFifo{32768};std::array<float,32768> tunerQueue{};
    std::array<float,8192> tunerWindow{};int tunerFill=0;
    juce::String lastTuner="Play a single note";
    void updateFilters();
    void processChunk(juce::AudioBuffer<float>&,double);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SishhinProcessor)
};
