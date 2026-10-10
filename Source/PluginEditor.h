#pragma once
#include "PluginProcessor.h"

class ModernLook final : public juce::LookAndFeel_V4 {
public:
    ModernLook();
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
    void drawLinearSlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
    void drawToggleButton(juce::Graphics&,juce::ToggleButton&,bool,bool) override;
};
class SishhinEditor final : public juce::AudioProcessorEditor,private juce::Timer {
public:
    explicit SishhinEditor(SishhinProcessor&);~SishhinEditor() override;
    void paint(juce::Graphics&) override;void resized() override;
    void showPage(int);
private:
    SishhinProcessor& processor;ModernLook look;
    using SA=juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    struct Knob {juce::Slider slider;juce::Label label;juce::Rectangle<int> bounds;int page=-1;std::unique_ptr<SA> attachment;};
    struct Switch {juce::ToggleButton button;juce::Rectangle<int> bounds;int page=-1;std::unique_ptr<BA> attachment;};
    struct Choice {juce::ComboBox box;juce::Rectangle<int> bounds;int page=-1;std::unique_ptr<CA> attachment;};
    std::vector<std::unique_ptr<Knob>> knobs;std::vector<std::unique_ptr<Switch>> switches;std::vector<std::unique_ptr<Choice>> choices;
    juce::TextButton listen{"CAPTURE INPUT"},reference{"LOAD REFERENCE"},autoGain{"CALIBRATE INPUT"},loadIR{"IMPORT IR"},save{"SAVE"},load{"OPEN"},about{"?"},previous{"<"},next{">"};
    std::array<juce::TextButton,4> tabs;
    juce::ComboBox presets;
    juce::Label status,tunerLabel;juce::TooltipWindow tips{this,600};
    std::unique_ptr<juce::FileChooser> chooser;
    float inputMeter=-60,outputMeter=-60,strobePhase=0;int autoTicks=0,page=0,displayedPreset=0;
    void knob(const char*,const char*,juce::Rectangle<int>,const juce::String&,int=-1,juce::Slider::SliderStyle=juce::Slider::RotaryHorizontalVerticalDrag);
    void toggle(const char*,const char*,juce::Rectangle<int>,int=-1,const juce::String& = {});
    void choice(const char*,juce::StringArray,juce::Rectangle<int>,int=-1);
    void timerCallback() override;
    void chooseFile(int);
    void report(const juce::Result&,const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SishhinEditor)
};
