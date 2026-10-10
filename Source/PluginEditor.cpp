#include "PluginEditor.h"
using namespace juce;
namespace {const Colour paper{0xffb6b3a7},ink{0xff242323},red{0xffa32322},light{0xffe3dfd1};
void text(Graphics& g,const String& s,Rectangle<int> r,float size,Colour c,Justification j=Justification::centred){g.setColour(c);g.setFont(Font(FontOptions(size)));g.drawText(s,r,j);}
}
MetalLook::MetalLook(){setColour(Slider::textBoxTextColourId,light);setColour(Slider::textBoxBackgroundColourId,ink);setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);setColour(ComboBox::backgroundColourId,ink);setColour(ComboBox::textColourId,light);setColour(ComboBox::outlineColourId,Colour(0xff55514b));setColour(PopupMenu::backgroundColourId,ink);setColour(PopupMenu::textColourId,light);setColour(TextButton::textColourOffId,light);}
void MetalLook::drawRotarySlider(Graphics& g,int x,int y,int w,int h,float pos,float start,float end,Slider& slider){
    const float size=static_cast<float>(jmin(w,h))-12,rad=size*.5f,cx=x+w*.5f,cy=y+h*.5f;
    g.setColour(Colour(0x70000000));g.fillEllipse(cx-rad-2,cy-rad+3,size+4,size+4);
    g.setColour(ink);g.fillEllipse(cx-rad-1,cy-rad-1,size+2,size+2);
    Path arc;arc.addCentredArc(cx,cy,rad+4,rad+4,0,start,start+pos*(end-start),true);g.setColour(red);g.strokePath(arc,PathStrokeType(3));
    g.setGradientFill(ColourGradient(light,cx-rad,cy-rad,Colour(0xff66635a),cx+rad,cy+rad,false));g.fillEllipse(cx-rad,cy-rad,size,size);
    for(int i=0;i<24;++i){float a=i*MathConstants<float>::twoPi/24;g.setColour(i%2?Colour(0xffd4cfbd):Colour(0xff4d4a43));g.drawLine(cx+std::sin(a)*(rad-4),cy+std::cos(a)*(rad-4),cx+std::sin(a)*rad,cy+std::cos(a)*rad,2);}
    g.setColour(Colour(0xff5a584f));g.drawEllipse(cx-rad+6,cy-rad+6,size-12,size-12,1);
    float a=start+pos*(end-start);g.setColour(Colour(0xff252423));g.drawLine(cx+std::sin(a)*rad*.45f,cy-std::cos(a)*rad*.45f,cx+std::sin(a)*rad*.83f,cy-std::cos(a)*rad*.83f,3);
    if(!slider.isEnabled()){g.setColour(Colour(0x669a998f));g.fillEllipse(cx-rad,cy-rad,size,size);}
}
void MetalLook::drawButtonBackground(Graphics& g,Button& b,const Colour&,bool hover,bool down){auto r=b.getLocalBounds().toFloat().reduced(.5f);g.setGradientFill(ColourGradient(Colour(hover?0xff50443b:0xff393633),0,0,Colour(down?0xff6d1d1b:0xff121313),0,r.getHeight(),false));g.fillRoundedRectangle(r,3);g.setColour(Colour(0xff665e51));g.drawRoundedRectangle(r,3,1);}
void MetalLook::drawLinearSlider(Graphics& g,int x,int y,int w,int h,float pos,float,float,Slider::SliderStyle style,Slider&){
    const bool vertical=style==Slider::LinearVertical;float cx=x+w*.5f,cy=y+h*.5f;
    g.setColour(ink);if(vertical)g.fillRoundedRectangle(cx-3,static_cast<float>(y),6,static_cast<float>(h),2);else g.fillRoundedRectangle(static_cast<float>(x),cy-2,static_cast<float>(w),4,2);
    Rectangle<float> thumb=vertical?Rectangle<float>(cx-8,pos-15,16,30):Rectangle<float>(pos-13,cy-6,26,12);
    g.setColour(Colour(0x55000000));g.fillRect(thumb.translated(2,3));g.setGradientFill(ColourGradient(light,thumb.getX(),thumb.getY(),Colour(0xff4d4b45),thumb.getRight(),thumb.getBottom(),false));g.fillRoundedRectangle(thumb,2);g.setColour(ink);g.drawRoundedRectangle(thumb,2,1);
    if(vertical)g.drawHorizontalLine(roundToInt(pos),cx-6,cx+6);else g.drawVerticalLine(roundToInt(pos),cy-4,cy+4);
}
void MetalLook::drawToggleButton(Graphics& g,ToggleButton& b,bool hover,bool){auto r=b.getLocalBounds();const bool header=b.getProperties().getWithDefault("header",false);if(header)drawButtonBackground(g,b,ink,hover,false);
    const float cy=r.getHeight()*.5f;g.setColour(Colour(0xff302925));g.fillEllipse(7,cy-5,10,10);if(b.getToggleState()){g.setColour(Colour(0x559f1717));g.fillEllipse(3,cy-9,18,18);g.setColour(Colour(0xffff4c42));g.fillEllipse(9,cy-3,6,6);}else{g.setColour(Colour(0xff111111));g.fillEllipse(9,cy-3,6,6);}
    text(g,b.getButtonText(),r.withTrimmedLeft(23),header?14.f:12.f,header || b.getY()>730?light:ink,Justification::centredLeft);
}
void SishhinEditor::knob(const char* id,const char* name,Rectangle<int> bounds,const String& tip,Slider::SliderStyle style){
    auto k=std::make_unique<Knob>();k->bounds=bounds;k->label.setText(name,dontSendNotification);k->label.setJustificationType(Justification::centred);k->label.setFont(Font(FontOptions(13.f)));k->label.setColour(Label::textColourId,bounds.getY()>395 && bounds.getY()<610?light:ink);
    k->slider.setSliderStyle(style);k->slider.setTextBoxStyle(Slider::TextBoxBelow,false,bounds.getWidth()-8,17);k->slider.setTooltip(tip);k->slider.setRotaryParameters(MathConstants<float>::pi*1.22f,MathConstants<float>::pi*2.78f,true);
    k->slider.setColour(Slider::textBoxBackgroundColourId,ink);k->slider.setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);k->slider.setColour(Slider::textBoxTextColourId,light);
    k->slider.setPopupDisplayEnabled(false,false,this);addAndMakeVisible(k->label);addAndMakeVisible(k->slider);k->attachment=std::make_unique<SA>(processor.state,id,k->slider);knobs.push_back(std::move(k));
}
void SishhinEditor::toggle(const char* id,const char* name,Rectangle<int> bounds){auto s=std::make_unique<Switch>();s->bounds=bounds;s->button.setButtonText(name);s->button.getProperties().set("header",bounds.getHeight()>=27);addAndMakeVisible(s->button);s->attachment=std::make_unique<BA>(processor.state,id,s->button);switches.push_back(std::move(s));}
void SishhinEditor::choice(const char* id,StringArray items,Rectangle<int> bounds){auto c=std::make_unique<Choice>();c->bounds=bounds;c->box.addItemList(items,1);c->box.setJustificationType(Justification::centred);addAndMakeVisible(c->box);c->attachment=std::make_unique<CA>(processor.state,id,c->box);choices.push_back(std::move(c));}
SishhinEditor::SishhinEditor(SishhinProcessor& p):AudioProcessorEditor(p),processor(p){
    setLookAndFeel(&look);setSize(1060,780);
    knob("gate","GATE",{12,80,66,95},"Noise gate threshold in dB. -80 disables the gate.");
    knob("input","INPUT",{13,205,64,420},"Input trim in dB. Play hard for three seconds when using Auto Gain.",Slider::LinearVertical);
    knob("output","OUTPUT",{983,205,64,420},"Output trim in dB. The red meter warns at 0 dBFS.",Slider::LinearVertical);
    toggle("toneOn","TONE MATCH",{138,76,185,28});
    toggle("toneLock","LOCK",{251,347,92,22});
    knob("amount","AMOUNT",{120,223,93,108},"Amount of the captured ten-band spectral correction.");knob("smooth","SMOOTH",{252,223,93,108},"Smooths the correction across neighbouring frequency bands.");
    toggle("characterOn","CHARACTER",{438,76,185,28});
    knob("distortion","DISTORTION",{396,111,80,103},"Intensity of the selected character distortion.");knob("tone","TONE",{489,111,80,103},"Character low-pass tone filter.");knob("colour","COLOUR",{582,111,80,103},"Character weight and asymmetric saturation.");
    choice("engine",{"RAZOR / cutting","MIRE / thick fuzz","RELIC / octave fuzz","MAUL / growl"},{407,223,243,24});
    choice("speed",{"1/4","1/8","1/16","1/32","1/8 dotted","1/16 dotted","1/8 triplet","1/16 triplet"},{400,292,99,23});
    toggle("stutterOn","STUTTER",{400,258,98,22});toggle("tighten","TIGHTEN",{400,330,98,22});
    knob("depth","DEPTH",{574,255,80,100},"Rhythmic gate depth. Syncs to host tempo and transport.");
    knob("makeup","LEVEL dB",{502,255,70,100},"Character output makeup gain.");
    toggle("pitchOn","PITCH",{734,76,185,28});
    knob("whammy","WHAMMY",{697,112,89,106},"Pitch shift from -24 to +12 semitones. Automatable.");knob("dissonance","DISSONANCE",{871,112,89,106},"Blend a voice shifted one semitone upward.");
    toggle("cleanse","CLEANSE",{787,164,87,22});switches.back()->button.setTooltip("Suppress the sub-octave while Whammy moves away from zero.");
    knob("thicken","THICKEN",{697,253,89,102},"Sub-octave blend. Supports chords using overlapping delay grains.");knob("subCut","HI CUT Hz",{871,253,89,102},"Low-pass cutoff for the sub-octave voice.");
    choice("octave",{"OCT -1","OCT -2"},{790,261,77,23});toggle("parallel","PARALLEL",{787,308,90,22});switches.back()->button.setTooltip("Run the sub-octave through a separate oversampled character and amplifier chain.");
    toggle("ampOn","AMPLIFIER",{438,393,185,28});
    knob("low","LOW",{221,506,92,112},"Amplifier low band gain in dB.");knob("mid","MID",{340,458,90,112},"Amplifier mid band gain in dB.");
    knob("drive","DRIVE",{455,441,150,167},"Four-times oversampled multistage high-gain saturation.");
    knob("high","HIGH",{630,458,90,112},"Amplifier high band gain in dB.");knob("presence","PRESENCE",{749,506,92,112},"Amplifier presence band gain in dB.");
    toggle("stereo","STEREO",{483,618,95,28});
    toggle("cabOn","CAB IR",{449,660,165,28});choice("cab",{"Obsidian 4x12","Iron 4x12","Ruin 2x12","Monolith 4x12","Custom WAV"},{402,699,260,25});
    knob("lowCut","LOW CUT Hz",{110,653,200,75},"Post-cab low cut. 20 Hz disables it.",Slider::LinearHorizontal);
    knob("highCut","HIGH CUT Hz",{750,653,200,75},"Post-cab high cut. 20000 Hz disables it.",Slider::LinearHorizontal);
    toggle("lofi","LO-FI",{980,666,76,28});toggle("bypass","BYPASS",{12,28,85,28});toggle("tuner","TUNER",{866,20,96,28});
    toggle("tunerMute","MUTE",{888,736,77,22});knob("tempo","BPM",{979,82,75,95},"Fallback tempo when the host supplies none.");
    presets.addItemList({"01  Obsidian Djent","02  Serrated Rhythm","03  Abyssal Thall","04  Mire Fuzz","05  Fracture Machine","06  Singing Steel"},1);presets.setSelectedId(1,dontSendNotification);presets.onChange=[this]{processor.factoryPreset(presets.getSelectedId()-1);status.setText("Factory preset loaded",dontSendNotification);};
    for(Component* c:std::initializer_list<Component*>{&listen,&reference,&autoGain,&loadIR,&save,&load,&about,&presets,&status,&tunerLabel})addAndMakeVisible(c);
    listen.onClick=[this]{processor.captureTone();processor.set("toneOn",1);status.setText("Play your DI guitar for three seconds...",dontSendNotification);};
    reference.onClick=[this]{chooseFile(0);};loadIR.onClick=[this]{chooseFile(1);};save.onClick=[this]{chooseFile(2);};load.onClick=[this]{chooseFile(3);};
    autoGain.onClick=[this]{processor.calibrate();autoTicks=95;status.setText("Play your loudest riff for three seconds...",dontSendNotification);};
    about.onClick=[this]{AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,"SISHHIN AMP 1.0","Free djent amplifier. No activation.\n\nTone match: load an isolated reference, then capture three seconds of DI. Without a reference the target is a flat ten-band spectrum.\n\nPitch uses dual-grain shifting; large shifts smear transients. Cabinet presets are original synthetic FIRs. Import WAV IRs up to two seconds.\n\nDouble-click a knob to reset. All sound controls are host-automatable. Save/Open includes tone profile and custom IR.\n\nNo factory sounds or DSP were extracted from Slam Amp.");};
    status.setColour(Label::textColourId,light);status.setFont(Font(FontOptions(12.f)));status.setText("SISHHIN AUDIO  /  DJENT · THALL · EXTENDED RANGE",dontSendNotification);
    tunerLabel.setColour(Label::textColourId,light);tunerLabel.setFont(Font(FontOptions(12.f)));tunerLabel.setJustificationType(Justification::centredRight);
    listen.setTooltip("Capture the spectrum of three seconds of live DI, then compute the correction toward the target.");reference.setTooltip("Load an isolated guitar WAV/AIFF reference, analysed over up to 30 seconds.");loadIR.setTooltip("Import a WAV/AIFF cabinet IR; the audio is embedded in your session and presets.");
    resized();startTimerHz(30);
}
SishhinEditor::~SishhinEditor(){stopTimer();setLookAndFeel(nullptr);}
void SishhinEditor::resized(){
    for(auto& k:knobs){auto r=k->bounds;k->label.setBounds(r.removeFromTop(20));k->slider.setBounds(r);}
    for(auto& s:switches)s->button.setBounds(s->bounds);for(auto& c:choices)c->box.setBounds(c->bounds);
    presets.setBounds(339,19,324,28);save.setBounds(679,19,66,28);load.setBounds(753,19,66,28);about.setBounds(1014,20,28,28);
    reference.setBounds(124,124,220,27);listen.setBounds(124,163,220,49);autoGain.setBounds(7,663,77,30);loadIR.setBounds(674,700,65,25);
    status.setBounds(102,737,560,28);tunerLabel.setBounds(605,737,278,28);
}
void SishhinEditor::paint(Graphics& g){
    g.fillAll(ink);g.setGradientFill(ColourGradient(Colour(0xff2e3030),0,0,Colour(0xff111313),0,65,false));g.fillRect(0,0,1060,66);
    text(g,"SISHHIN AMP",{104,12,220,34},28,light,Justification::centredLeft);text(g,"FREE / DJENT ENGINE",{106,43,200,15},10,Colour(0xff9f9a8d),Justification::centredLeft);
    for(auto r:{Rectangle<int>(0,65,91,664),{97,65,286,322},{389,65,286,322},{681,65,286,322},{973,65,87,664},{97,651,870,78}}){
        g.setGradientFill(ColourGradient(paper.brighter(.18f),0,static_cast<float>(r.getY()),paper.darker(.16f),0,static_cast<float>(r.getBottom()),false));g.fillRect(r);
        g.setColour(Colour(0xff454039));g.drawRect(r,2);g.setColour(Colour(0xffddd7c5));g.drawRect(r.reduced(5),1);
    }
    Random rng(230819);for(int i=0;i<8500;++i){int x=rng.nextInt(1060),y=65+rng.nextInt(664);g.setColour(Colour(i%3==0?0x226a1410:0x18201b17));g.fillRect(x,y,rng.nextInt(3)+1,1);}
    for(int edge:{98,382,390,674,682,965})for(int i=0;i<65;++i){int y=66+rng.nextInt(320),x=edge+rng.nextInt(9)-4;g.setColour(Colour(0x8890231d));g.drawLine(static_cast<float>(x),static_cast<float>(y),static_cast<float>(x+rng.nextInt(5)-2),static_cast<float>(y+rng.nextInt(24)),.6f);}
    g.setGradientFill(ColourGradient(Colour(0xff383735),500,385,Colour(0xff0b0d0e),500,652,false));g.fillRect(98,387,868,264);
    for(int i=0;i<1100;++i){int x=98+rng.nextInt(868),y=389+rng.nextInt(258);g.setColour(Colour(0x163c3931));g.drawVerticalLine(x,static_cast<float>(y),static_cast<float>(y+rng.nextInt(32)));}
    // Forged ribs frame the amp, echoing the reference's mechanical arch.
    for(int side:{-1,1})for(int j=0;j<10;++j){float t=j/9.f;float cx=530+side*(225+175*t),cy=415+170*t;
        Path p;p.startNewSubPath(cx,cy);p.lineTo(cx+side*37,cy+14);p.lineTo(cx+side*18,cy+51);p.lineTo(cx-side*17,cy+27);p.closeSubPath();
        g.setGradientFill(ColourGradient(Colour(0xff686459),cx,cy,Colour(0xff151718),cx+side*25,cy+40,false));g.fillPath(p);g.setColour(Colour(0xff817567));g.strokePath(p,PathStrokeType(1));g.setColour(red.darker(.6f));g.drawLine(cx,cy+3,cx+side*20,cy+14,2);
        g.setColour(Colour(0xff99907e));g.fillEllipse(cx+side*13-2,cy+16,4,4);
    }
    text(g,"PRECISION HIGH GAIN",{375,427,310,18},11,Colour(0xff8f8879));
    text(g,processor.profileReady.load()?"PROFILE READY":"NO PROFILE",{117,346,131,20},11,ink);
    for(int i=0;i<sishhin::bands;++i){float db=processor.matchDb[i].load();g.setColour(red.withAlpha(.7f));g.fillRect(130+i*21,372-roundToInt(jmax(0.f,db)*.8f),12,jmax(1,roundToInt(std::abs(db)*.8f)));}
    auto meter=[&](int x,float db){g.setColour(Colour(0xff272827));g.fillRect(x,239,7,359);float fraction=jlimit(0.f,1.f,(db+60)/60);g.setColour(db>=0?Colour(0xffff362b):Colour(0xffa34d3b));g.fillRect(x,598-roundToInt(359*fraction),7,roundToInt(359*fraction));
        for(int d=0;d<=60;d+=12)text(g,String(-d),{x-14,231+d*6,20,12},9,ink);};meter(17,inputMeter);meter(1037,outputMeter);
    text(g,"INPUT",{3,630,83,18},12,ink);text(g,"OUTPUT",{974,630,83,18},12,ink);
    if(processor.value("tuner")>.5f){g.saveState();g.reduceClipRegion(609,766,272,8);g.setColour(std::abs(processor.tunerCents.load())<3?Colour(0xffaaa98c):red.brighter(.35f));
        for(int i=-1;i<16;++i)g.fillRect(609+i*20+roundToInt(strobePhase),767,7,6);g.setColour(light);g.fillRect(744,766,2,8);g.restoreState();}
}
void SishhinEditor::report(const Result& result,const String& ok){status.setText(result.wasOk()?ok:result.getErrorMessage(),dontSendNotification);}
void SishhinEditor::chooseFile(int mode){
    const bool saving=mode==2;chooser=std::make_unique<FileChooser>(mode==0?"Load tone reference":mode==1?"Load cabinet impulse response":saving?"Save SISHHIN preset":"Open SISHHIN preset",File{},mode<2?"*.wav;*.aif;*.aiff":"*.sishhin");
    auto safe=Component::SafePointer<SishhinEditor>(this);
    chooser->launchAsync((saving?FileBrowserComponent::saveMode|FileBrowserComponent::warnAboutOverwriting:FileBrowserComponent::openMode)|FileBrowserComponent::canSelectFiles,[safe,mode](const FileChooser& fc){if(!safe)return;auto f=fc.getResult();if(f==File{})return;
        if(mode==0)safe->report(safe->processor.loadReference(f),"Reference loaded. Now CAPTURE DI while playing.");
        if(mode==1)safe->report(safe->processor.loadIR(f),"Custom IR loaded: "+f.getFileName());
        if(mode==2){MemoryBlock b;safe->processor.getStateInformation(b);f=f.withFileExtension("sishhin");safe->report(f.replaceWithData(b.getData(),b.getSize())?Result::ok():Result::fail("Could not save preset"),"Preset saved: "+f.getFileName());}
        if(mode==3){MemoryBlock b;f.loadFileAsData(b);auto xml=AudioProcessor::getXmlFromBinary(b.getData(),static_cast<int>(b.getSize()));if(xml && xml->hasTagName("SISHHIN")){
            safe->processor.setStateInformation(b.getData(),static_cast<int>(b.getSize()));safe->status.setText("Preset loaded: "+f.getFileName(),dontSendNotification);
        }else safe->status.setText("Invalid preset file",dontSendNotification);}
    });
}
void SishhinEditor::timerCallback(){
    inputMeter=jmax(Decibels::gainToDecibels(processor.inputPeak.load(),-60.f),inputMeter-1.8f);outputMeter=jmax(Decibels::gainToDecibels(processor.outputPeak.load(),-60.f),outputMeter-1.8f);
    const int remaining=processor.captureRemaining.load();listen.setButtonText(remaining>0?"LISTENING...":"CAPTURE DI");
    float calibration=0;if(processor.takeCalibration(calibration)){if(calibration<1000){processor.set("input",calibration);status.setText("Input calibrated to "+String(calibration,1)+" dB",dontSendNotification);}else status.setText("Calibration needs an audible guitar signal",dontSendNotification);autoTicks=0;}
    if(autoTicks>0)--autoTicks;autoGain.setButtonText(autoTicks>0?"PLAY...":"AUTO GAIN");
    tunerLabel.setText(processor.value("tuner")>.5f?processor.tunerText():"",dontSendNotification);strobePhase+=processor.tunerCents.load()*.025f;strobePhase=std::fmod(strobePhase+20,20.f);repaint();
}
