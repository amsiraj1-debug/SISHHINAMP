#pragma once
#include "PluginProcessor.h"

class MetalLook final : public juce::LookAndFeel_V4 {
public:
    MetalLook();
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
    void drawLinearSlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
    void drawToggleButton(juce::Graphics&,juce::ToggleButton&,bool,bool) override;
};
class SishhinEditor final : public juce::AudioProcessorEditor,private juce::Timer {
public:
    explicit SishhinEditor(SishhinProcessor&);~SishhinEditor() override;
    void paint(juce::Graphics&) override;void resized() override;
private:
    SishhinProcessor& processor;MetalLook look;
    using SA=juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    struct Knob {juce::Slider slider;juce::Label label;juce::Rectangle<int> bounds;std::unique_ptr<SA> attachment;};
    struct Switch {juce::ToggleButton button;juce::Rectangle<int> bounds;std::unique_ptr<BA> attachment;};
    struct Choice {juce::ComboBox box;juce::Rectangle<int> bounds;std::unique_ptr<CA> attachment;};
    std::vector<std::unique_ptr<Knob>> knobs;std::vector<std::unique_ptr<Switch>> switches;std::vector<std::unique_ptr<Choice>> choices;
    juce::TextButton listen{"CAPTURE DI"},reference{"LOAD TARGET"},autoGain{"AUTO GAIN"},loadIR{"IMPORT IR"},save{"SAVE"},load{"OPEN"},about{"?"};
    juce::ComboBox presets;
    juce::Label status,tunerLabel;juce::TooltipWindow tips{this,600};
    std::unique_ptr<juce::FileChooser> chooser;
    float inputMeter=-60,outputMeter=-60;int autoTicks=0;
    float strobePhase=0;
    void knob(const char*,const char*,juce::Rectangle<int>,const juce::String&,juce::Slider::SliderStyle=juce::Slider::RotaryHorizontalVerticalDrag);
    void toggle(const char*,const char*,juce::Rectangle<int>);
    void choice(const char*,juce::StringArray,juce::Rectangle<int>);
    void timerCallback() override;
    void chooseFile(int);
    void report(const juce::Result&,const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SishhinEditor)
};
