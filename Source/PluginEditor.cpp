#include "PluginEditor.h"
using namespace juce;
namespace {
const Colour bg{0xff090d12},panel{0xff121920},border{0xff26323c},muted{0xff8496a4},white{0xffe6eef3},accent{0xff65e4cb},lime{0xffd3ed86};
void text(Graphics& g,const String& s,Rectangle<int> r,float size,Colour c,Justification j=Justification::centred,bool bold=false){g.setColour(c);Font font{FontOptions(size)};font.setBold(bold);g.setFont(font);g.drawText(s,r,j);}
void card(Graphics& g,Rectangle<float> r){g.setColour(Colour(0x66000000));g.fillRoundedRectangle(r.translated(0,3),13);g.setColour(panel);g.fillRoundedRectangle(r,12);g.setColour(border);g.drawRoundedRectangle(r,12,1);}
}
ModernLook::ModernLook(){
    setColour(ComboBox::backgroundColourId,Colour(0xff1b252e));setColour(ComboBox::textColourId,white);setColour(ComboBox::outlineColourId,border);setColour(ComboBox::arrowColourId,accent);
    setColour(PopupMenu::backgroundColourId,panel);setColour(PopupMenu::textColourId,white);setColour(PopupMenu::highlightedBackgroundColourId,Colour(0xff245044));setColour(PopupMenu::highlightedTextColourId,white);
    setColour(TextButton::textColourOffId,white);setColour(TextButton::textColourOnId,accent);setColour(TooltipWindow::backgroundColourId,panel);setColour(TooltipWindow::textColourId,white);
}
void ModernLook::drawRotarySlider(Graphics& g,int x,int y,int w,int h,float pos,float start,float end,Slider& s){
    const float size=static_cast<float>(jmin(w,h))-18,rad=size*.5f,cx=x+w*.5f,cy=y+h*.5f;const bool big=size>105;
    Path base;base.addCentredArc(cx,cy,rad+5,rad+5,0,start,end,true);g.setColour(border.brighter(.12f));g.strokePath(base,PathStrokeType(big?5.f:3.f,PathStrokeType::curved,PathStrokeType::rounded));
    Path arc;arc.addCentredArc(cx,cy,rad+5,rad+5,0,start,start+pos*(end-start),true);g.setColour(big?lime:accent);g.strokePath(arc,PathStrokeType(big?5.f:3.f,PathStrokeType::curved,PathStrokeType::rounded));
    g.setColour(Colour(0x99000000));g.fillEllipse(cx-rad,cy-rad+4,size,size);
    g.setGradientFill(ColourGradient(Colour(0xff2e3c47),cx-rad,cy-rad,Colour(0xff121a22),cx+rad,cy+rad,false));g.fillEllipse(cx-rad,cy-rad,size,size);
    g.setColour(Colour(0xff394b58));g.drawEllipse(cx-rad+.5f,cy-rad+.5f,size-1,size-1,1);
    if(big){g.setColour(Colour(0xff1e2a33));g.drawEllipse(cx-rad+12,cy-rad+12,size-24,size-24,1);text(g,"GAIN",{roundToInt(cx)-30,roundToInt(cy)-9,60,18},11,muted);}
    const float angle=start+pos*(end-start);g.setColour(white);g.drawLine(cx+std::sin(angle)*rad*.63f,cy-std::cos(angle)*rad*.63f,cx+std::sin(angle)*rad*.86f,cy-std::cos(angle)*rad*.86f,big?3.f:2.f);
    if(!s.isEnabled()){g.setColour(bg.withAlpha(.6f));g.fillEllipse(cx-rad-8,cy-rad-8,size+16,size+16);}
}
void ModernLook::drawLinearSlider(Graphics& g,int x,int y,int w,int h,float pos,float,float,Slider::SliderStyle style,Slider&){
    bool vertical=style==Slider::LinearVertical;float cx=x+w*.5f,cy=y+h*.5f;g.setColour(Colour(0xff070b10));
    if(vertical){g.fillRoundedRectangle(cx-3,static_cast<float>(y),6,static_cast<float>(h),3);g.setColour(accent.withAlpha(.3f));g.fillRoundedRectangle(cx-2,pos,4,y+h-pos,2);}
    else {g.fillRoundedRectangle(static_cast<float>(x),cy-2,static_cast<float>(w),4,2);g.setColour(accent.withAlpha(.5f));g.fillRoundedRectangle(static_cast<float>(x),cy-2,jmax(0.f,pos-x),4,2);}
    g.setColour(white);if(vertical){g.fillRoundedRectangle(cx-9,pos-7,18,14,3);g.setColour(bg);g.drawHorizontalLine(roundToInt(pos),cx-5,cx+5);}else g.fillEllipse(pos-5,cy-5,10,10);
}
void ModernLook::drawButtonBackground(Graphics& g,Button& b,const Colour&,bool hover,bool down){
    auto r=b.getLocalBounds().toFloat().reduced(.5f);const bool selected=b.getToggleState();g.setColour(selected?accent.withAlpha(.15f):hover?Colour(0xff263740):Colour(0xff1a242d));if(down)g.setColour(Colour(0xff315049));g.fillRoundedRectangle(r,6);g.setColour(selected?accent.withAlpha(.6f):border);g.drawRoundedRectangle(r,6,1);
}
void ModernLook::drawToggleButton(Graphics& g,ToggleButton& b,bool hover,bool){
    auto r=b.getLocalBounds();const bool on=b.getToggleState();g.setColour(on?accent.withAlpha(.07f):Colour(0xff171f27));g.fillRoundedRectangle(r.toFloat(),5);if(hover){g.setColour(border);g.drawRoundedRectangle(r.toFloat().reduced(.5f),5,1);}
    float cy=r.getHeight()*.5f;g.setColour(on?accent:Colour(0xff52626d));g.fillEllipse(10,cy-3,6,6);text(g,b.getButtonText(),r.withTrimmedLeft(24),11,on?white:muted,Justification::centredLeft,true);
}
void SishhinEditor::knob(const char* id,const char* name,Rectangle<int> bounds,const String& tip,int section,Slider::SliderStyle style){
    auto k=std::make_unique<Knob>();k->bounds=bounds;k->page=section;k->label.setText(name,dontSendNotification);k->label.setJustificationType(Justification::centred);k->label.setFont(Font(FontOptions(11.f)));k->label.setColour(Label::textColourId,muted);
    k->slider.setSliderStyle(style);k->slider.setTextBoxStyle(Slider::TextBoxBelow,false,jmin(bounds.getWidth()-8,100),20);k->slider.setTooltip(tip);k->slider.setRotaryParameters(MathConstants<float>::pi*1.22f,MathConstants<float>::pi*2.78f,true);
    k->slider.setColour(Slider::textBoxBackgroundColourId,Colour(0xff0e141a));k->slider.setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);k->slider.setColour(Slider::textBoxTextColourId,white);
    addAndMakeVisible(k->label);addAndMakeVisible(k->slider);k->attachment=std::make_unique<SA>(processor.state,id,k->slider);knobs.push_back(std::move(k));
}
void SishhinEditor::toggle(const char* id,const char* name,Rectangle<int> bounds,int section,const String& tip){auto s=std::make_unique<Switch>();s->bounds=bounds;s->page=section;s->button.setButtonText(name);s->button.setTooltip(tip);addAndMakeVisible(s->button);s->attachment=std::make_unique<BA>(processor.state,id,s->button);switches.push_back(std::move(s));}
void SishhinEditor::choice(const char* id,StringArray items,Rectangle<int> bounds,int section){auto c=std::make_unique<Choice>();c->bounds=bounds;c->page=section;c->box.addItemList(items,1);c->box.setJustificationType(Justification::centred);addAndMakeVisible(c->box);c->attachment=std::make_unique<CA>(processor.state,id,c->box);choices.push_back(std::move(c));}
SishhinEditor::SishhinEditor(SishhinProcessor& p):AudioProcessorEditor(p),processor(p){
    setLookAndFeel(&look);setSize(1200,830);
    toggle("bypass","BYPASS",{32,117,146,28},-1,"When lit, the full effect is bypassed.");
    knob("gate","NOISE GATE / dB",{52,160,105,113},"Threshold for the input noise gate. -80 dB disables gating.");
    knob("input","INPUT",{34,356,60,343},"Input trim in dB. Use Calibrate while playing a loud riff.",-1,Slider::LinearVertical);
    knob("output","OUTPUT",{116,356,60,343},"Final output trim in dB.",-1,Slider::LinearVertical);
    knob("tempo","FALLBACK BPM",{35,714,141,63},"Tempo used when your host does not provide one.",-1,Slider::LinearHorizontal);
    toggle("ampOn","AMPLIFIER",{229,116,144,28});toggle("stereo","STEREO",{814,116,114,28});
    knob("drive","DRIVE",{244,176,146,189},"Four-times oversampled, dynamically biased high-gain amplifier.");
    knob("low","LOW",{421,208,102,112},"Low-mid weight at 130 Hz.");knob("mid","MID",{548,208,102,112},"Midrange focus at 850 Hz.");knob("high","HIGH",{675,208,102,112},"Pick definition at 3 kHz.");knob("presence","PRESENCE",{802,208,102,112},"Upper harmonics at 6.2 kHz.");
    knob("tight","LOW END FOCUS",{427,337,136,67},"Controls the pre-distortion bass cutoff from 32 to 187 Hz for tighter extended-range riffs.",-1,Slider::LinearHorizontal);
    knob("attack","PICK ATTACK",{594,337,136,67},"Transient emphasis before saturation. Adds pick definition without a permanent treble boost.",-1,Slider::LinearHorizontal);
    knob("sag","POWER SAG",{761,337,136,67},"Envelope-dependent power-stage compression. Low values stay rigid; high values bloom.",-1,Slider::LinearHorizontal);
    toggle("characterOn","CHARACTER",{230,508,160,28},0);
    choice("engine",{"RAZOR / focused drive","MIRE / dense fuzz","RELIC / octave fracture","MAUL / asymmetric weight"},{410,508,280,28},0);
    knob("distortion","DISTORTION",{234,562,100,127},"Character-stage intensity.",0);knob("tone","TONE",{352,562,100,127},"Character-stage upper-frequency contour.",0);
    knob("colour","COLOUR",{470,562,100,127},"Character weight and asymmetry.",0);knob("makeup","MAKEUP / dB",{588,562,100,127},"Character-stage gain compensation.",0);
    toggle("stutterOn","STUTTER",{722,508,201,28},0);choice("speed",{"1/4","1/8","1/16","1/32","1/8 dotted","1/16 dotted","1/8 triplet","1/16 triplet"},{722,551,201,28},0);
    knob("depth","STUTTER DEPTH",{773,591,100,113},"Tempo-synced rhythmic gate depth. Delay and reverb tails continue after the gate.",0);toggle("tighten","SHORT GATE",{722,724,201,27},0,"Tighter rhythmic duty cycle for machine-like cuts.");
    toggle("pitchOn","PITCH MATRIX",{232,508,170,28},1);
    knob("whammy","WHAMMY / st",{241,565,115,138},"Continuous pitch shifting between -24 and +12 semitones.",1);knob("dissonance","DISSONANCE",{411,565,115,138},"Adds an independent semitone-up layer.",1);
    knob("thicken","SUB WEIGHT",{581,565,115,138},"Sub-octave blend. At 100%, the unshifted main voice is removed.",1);knob("subCut","SUB CUT / Hz",{751,565,115,138},"Sub-octave low-pass filter.",1);
    toggle("cleanse","CLEANSE",{241,725,133,28},1,"Suppress the sub layer while Whammy moves away from zero.");choice("octave",{"ONE OCTAVE DOWN","TWO OCTAVES DOWN"},{459,725,230,28},1);toggle("parallel","PARALLEL AMP",{753,725,156,28},1,"Run the sub voice through a separate character and amp chain.");
    toggle("toneOn","SPECTRAL MATCH",{232,508,178,28},2);toggle("toneLock","LOCK INPUT + MATCH",{430,508,212,28},2,"Protect input and matching settings when changing factory presets.");
    knob("amount","MATCH AMOUNT",{548,564,133,142},"Strength of the captured ten-band spectral correction.",2);knob("smooth","SPECTRAL SMOOTH",{740,564,133,142},"Smooth the correction across neighbouring frequency bands.",2);
    toggle("cabOn","CABINET",{232,508,144,28},3);choice("cab",{"Obsidian 4x12","Iron 4x12","Ruin 2x12","Monolith 4x12","Custom WAV / AIFF"},{410,508,310,28},3);
    knob("lowCut","LOW CUT / Hz",{249,577,262,88},"Post-cabinet low cut. 20 Hz disables this filter.",3,Slider::LinearHorizontal);knob("highCut","HIGH CUT / Hz",{620,577,262,88},"Post-cabinet high cut. 20000 Hz disables this filter.",3,Slider::LinearHorizontal);
    toggle("delayOn","TEMPO DELAY",{983,150,183,28});knob("delayMix","MIX",{981,195,91,111},"Tempo delay wet level.");knob("feedback","FEEDBACK",{1081,195,91,111},"Feedback amount; stereo mode cross-feeds the echoes.");choice("delayDivision",{"1/4 NOTE","1/8 NOTE","1/8 DOTTED","1/4 TRIPLET","1/2 NOTE"},{985,321,181,28});
    toggle("reverbOn","DIFFUSE REVERB",{983,383,183,28});knob("reverbMix","MIX",{981,426,91,111},"Reverb blend after the amp and cabinet.");knob("reverbSize","SIZE",{1081,426,91,111},"Reverb room size and decay character.");
    knob("duck","DUCKING",{981,587,91,111},"Reduce ambience under loud attacks so palm-muted riffs remain clear.");knob("spaceTone","TONE / Hz",{1081,587,91,111},"Low-pass filtering for delay feedback and ambience.");toggle("lofi","LO-FI FILTER",{989,741,173,28});
    toggle("tuner","TUNER",{1004,28,117,30});toggle("tunerMute","MUTE",{1075,73,87,22});
    presets.addItemList(SishhinProcessor::presetNames(),1);displayedPreset=processor.getCurrentProgram();presets.setSelectedId(displayedPreset+1,dontSendNotification);
    presets.onChange=[this]{displayedPreset=presets.getSelectedId()-1;processor.factoryPreset(displayedPreset);status.setText("Loaded "+SishhinProcessor::presetNames()[displayedPreset],dontSendNotification);};
    previous.onClick=[this]{presets.setSelectedId((presets.getSelectedId()+processor.getNumPrograms()-2)%processor.getNumPrograms()+1);};next.onClick=[this]{presets.setSelectedId(presets.getSelectedId()%processor.getNumPrograms()+1);};
    for(Component* c:std::initializer_list<Component*>{&listen,&reference,&autoGain,&loadIR,&save,&load,&about,&previous,&next,&presets,&status,&tunerLabel})addAndMakeVisible(c);
    const char* names[]{"01   CHARACTER","02   PITCH MATRIX","03   SPECTRAL MATCH","04   CABINET"};for(int i=0;i<4;++i){tabs[i].setButtonText(names[i]);tabs[i].onClick=[this,i]{showPage(i);};addAndMakeVisible(tabs[i]);}
    listen.onClick=[this]{processor.captureTone();processor.set("toneOn",1);status.setText("CAPTURE: play representative guitar input for three seconds.",dontSendNotification);};
    reference.onClick=[this]{chooseFile(0);};loadIR.onClick=[this]{chooseFile(1);};save.onClick=[this]{chooseFile(2);};load.onClick=[this]{chooseFile(3);};
    autoGain.onClick=[this]{processor.calibrate();autoTicks=95;status.setText("CALIBRATION: play your loudest riff for three seconds.",dontSendNotification);};
    about.onClick=[]{AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,"SISHHIN AMP 1.1 / THALL ENGINE","28 original presets. Four-times oversampled amplifier and character stages. Independent sub amplifier. Spectral input matching. Tempo delay and ducked reverb.\n\nUse the four lower tabs to access Character, Pitch, Match and Cabinet. All sound controls are host-automatable. Double-click controls to reset.\n\nSave/Open embeds the custom IR and spectral profile. Pitch uses dual-grain shifting; extreme shifts can smear attacks. Import cabinet WAV/AIFF files up to two seconds.\n\nFree and open source. No account, activation or online check.");};
    status.setColour(Label::textColourId,muted);status.setFont(Font(FontOptions(11.f)));status.setText("SISHHIN AUDIO   /   THALL ENGINE 1.1   /   28 ORIGINAL PRESETS",dontSendNotification);
    tunerLabel.setColour(Label::textColourId,accent);tunerLabel.setFont(Font(FontOptions(12.f)));tunerLabel.setJustificationType(Justification::centred);
    reference.setTooltip("Load an isolated reference guitar WAV or AIFF, then capture your input.");listen.setTooltip("Capture three seconds of input and compute a ten-band correction toward the reference.");loadIR.setTooltip("Import a mono or stereo cabinet IR. It is embedded in presets and sessions.");
    resized();showPage(0);startTimerHz(30);
}
SishhinEditor::~SishhinEditor(){stopTimer();setLookAndFeel(nullptr);}
void SishhinEditor::resized(){
    for(auto& k:knobs){auto r=k->bounds;k->label.setBounds(r.removeFromTop(20));k->slider.setBounds(r);}for(auto& s:switches)s->button.setBounds(s->bounds);for(auto& c:choices)c->box.setBounds(c->bounds);
    presets.setBounds(370,26,420,34);previous.setBounds(327,26,31,34);next.setBounds(800,26,31,34);save.setBounds(849,28,66,30);load.setBounds(923,28,66,30);about.setBounds(1142,28,34,30);
    for(int i=0;i<4;++i)tabs[i].setBounds(228+i*177,454,167,32);
    reference.setBounds(240,565,251,36);listen.setBounds(240,615,251,48);loadIR.setBounds(745,508,177,28);autoGain.setBounds(33,289,144,30);
    status.setBounds(24,802,1150,21);tunerLabel.setBounds(340,73,647,22);
}
void SishhinEditor::showPage(int index){page=jlimit(0,3,index);for(auto& k:knobs){bool show=k->page<0 || k->page==page;k->slider.setVisible(show);k->label.setVisible(show);}for(auto& s:switches)s->button.setVisible(s->page<0 || s->page==page);for(auto& c:choices)c->box.setVisible(c->page<0 || c->page==page);
    reference.setVisible(page==2);listen.setVisible(page==2);loadIR.setVisible(page==3);for(int i=0;i<4;++i)tabs[i].setToggleState(page==i,dontSendNotification);repaint();}
void SishhinEditor::paint(Graphics& g){
    g.fillAll(bg);g.setColour(Colour(0xff111a22));g.fillRect(0,0,1200,99);g.setColour(border);g.drawHorizontalLine(98,0,1200);
    // Original monogram: fractured obelisk with an offset core.
    Path logo;logo.startNewSubPath(25,22);logo.lineTo(51,22);logo.lineTo(42,40);logo.lineTo(58,40);logo.lineTo(35,69);logo.lineTo(42,47);logo.lineTo(23,47);logo.closeSubPath();g.setColour(accent);g.fillPath(logo);
    text(g,"SISHHIN",{72,17,230,40},31,white,Justification::centredLeft,true);text(g,"T H A L L   E N G I N E",{75,57,220,20},10,muted,Justification::centredLeft);
    card(g,{16,106,179,686});card(g,{210,106,740,315});card(g,{210,437,740,355});card(g,{966,106,218,686});
    text(g,"S P A C E",{985,116,180,24},12,muted,Justification::centredLeft,true);
    text(g,"4x",{384,119,38,21},12,accent,Justification::centredLeft,true);text(g,"DYNAMIC GAIN CORE",{435,119,290,21},11,muted,Justification::centredLeft);
    text(g,"PRECISION / PRESSURE",{239,377,160,20},9,muted);
    // A decorative contour gives the amplifier its own visual identity.
    const float gains[]{processor.value("low"),processor.value("mid"),processor.value("high"),processor.value("presence")};Path contour;
    for(int i=0;i<490;++i){float v=0;for(int band=0;band<4;++band){float d=(i-(45+band*126))/55.f;v+=gains[band]*std::exp(-d*d*.5f);}float x=422.f+i,y=178.f-v*.9f;if(i==0)contour.startNewSubPath(x,y);else contour.lineTo(x,y);}
    g.setColour(border);g.drawHorizontalLine(178,422,912);g.setColour(accent.withAlpha(.6f));g.strokePath(contour,PathStrokeType(1.5f));
    for(int x=422;x<=912;x+=49){g.setColour(Colour(0xff2b3a43));g.fillEllipse(static_cast<float>(x),192,2,2);}
    auto meter=[&](int x,float db){g.setColour(Colour(0xff070b10));g.fillRoundedRectangle(static_cast<float>(x),400,5,257,2);float fraction=jlimit(0.f,1.f,(db+60)/60);g.setColour(db>=0?Colour(0xffff6e73):accent);g.fillRoundedRectangle(static_cast<float>(x),657-257*fraction,5,257*fraction,2);};meter(24,inputMeter);meter(183,outputMeter);
    text(g,String(inputMeter,1)+" dB",{26,331,64,18},10,muted);text(g,String(outputMeter,1)+" dB",{111,331,70,18},10,muted);
    g.setColour(border);g.drawHorizontalLine(365,983,1167);g.drawHorizontalLine(564,983,1167);text(g,"ATTACK-SAFE AMBIENCE",{978,548,194,18},9,muted);
    if(page==0){g.setColour(border);g.drawVerticalLine(704,505,756);text(g,"FOUR VOICES. ONE HEAVY SIGNAL.",{242,730,432,22},11,muted);}
    if(page==1){text(g,"CHORD-CAPABLE GRAIN SHIFT / INDEPENDENT SUB AMPLIFIER",{425,511,493,22},10,muted,Justification::centredRight);}
    if(page==2){text(g,processor.profileReady.load()?"PROFILE READY":"NO CAPTURED PROFILE",{240,688,250,20},11,processor.profileReady.load()?accent:muted);
        g.setColour(border);g.drawHorizontalLine(737,240,491);for(int i=0;i<sishhin::bands;++i){float db=processor.matchDb[i].load();g.setColour(accent.withAlpha(.7f));g.fillRect(244+i*25,737-roundToInt(jmax(0.f,db)*2),13,jmax(2,roundToInt(std::abs(db)*2)));}
        text(g,"MATCH THE INPUT. KEEP THE ATTACK.",{534,729,359,22},11,muted);}
    if(page==3){text(g,processor.value("cab")==4?processor.irName():"FOUR ORIGINAL CABINET VOICES / EMBEDDED IR RECALL",{245,694,651,28},12,accent);text(g,"WAV / AIFF   |   MONO / STEREO   |   UP TO 2 SECONDS",{245,733,651,22},10,muted);}
    if(processor.value("tuner")>.5f){g.saveState();g.reduceClipRegion(404,94,518,4);g.setColour(std::abs(processor.tunerCents.load())<3?lime:accent);for(int i=-1;i<29;++i)g.fillRect(404+i*20+roundToInt(strobePhase),94,7,3);g.restoreState();}
}
void SishhinEditor::report(const Result& result,const String& ok){status.setText(result.wasOk()?ok:result.getErrorMessage(),dontSendNotification);}
void SishhinEditor::chooseFile(int mode){
    const bool saving=mode==2;chooser=std::make_unique<FileChooser>(mode==0?"Load tone reference":mode==1?"Load cabinet impulse response":saving?"Save SISHHIN preset":"Open SISHHIN preset",File{},mode<2?"*.wav;*.aif;*.aiff":"*.sishhin");
    auto safe=Component::SafePointer<SishhinEditor>(this);chooser->launchAsync((saving?FileBrowserComponent::saveMode|FileBrowserComponent::warnAboutOverwriting:FileBrowserComponent::openMode)|FileBrowserComponent::canSelectFiles,[safe,mode](const FileChooser& fc){if(!safe)return;auto file=fc.getResult();if(file==File{})return;
        if(mode==0)safe->report(safe->processor.loadReference(file),"Reference loaded. Capture your input while playing.");if(mode==1)safe->report(safe->processor.loadIR(file),"Custom cabinet loaded: "+file.getFileName());
        if(mode==2){MemoryBlock b;safe->processor.getStateInformation(b);file=file.withFileExtension("sishhin");safe->report(file.replaceWithData(b.getData(),b.getSize())?Result::ok():Result::fail("Could not save preset"),"Preset saved: "+file.getFileName());}
        if(mode==3){MemoryBlock b;file.loadFileAsData(b);auto xml=AudioProcessor::getXmlFromBinary(b.getData(),static_cast<int>(b.getSize()));if(xml && xml->hasTagName("SISHHIN")){safe->processor.setStateInformation(b.getData(),static_cast<int>(b.getSize()));safe->status.setText("Preset loaded: "+file.getFileName(),dontSendNotification);}else safe->status.setText("Invalid preset file",dontSendNotification);}
    });
}
void SishhinEditor::timerCallback(){
    inputMeter=jmax(Decibels::gainToDecibels(processor.inputPeak.load(),-60.f),inputMeter-1.8f);outputMeter=jmax(Decibels::gainToDecibels(processor.outputPeak.load(),-60.f),outputMeter-1.8f);
    listen.setButtonText(processor.captureRemaining.load()>0?"CAPTURING...":"CAPTURE INPUT");float calibration=0;
    if(processor.takeCalibration(calibration)){if(calibration<1000){processor.set("input",calibration);status.setText("Input calibrated to "+String(calibration,1)+" dB",dontSendNotification);}else status.setText("Calibration needs an audible guitar signal",dontSendNotification);autoTicks=0;}
    if(autoTicks>0)--autoTicks;autoGain.setButtonText(autoTicks>0?"PLAY YOUR RIFF...":"CALIBRATE INPUT");
    tunerLabel.setText(processor.value("tuner")>.5f?processor.tunerText():"LOW TUNINGS. HIGH INTENT.",dontSendNotification);strobePhase=std::fmod(strobePhase+processor.tunerCents.load()*.025f+20,20.f);
    if(displayedPreset!=processor.getCurrentProgram()){displayedPreset=processor.getCurrentProgram();presets.setSelectedId(displayedPreset+1,dontSendNotification);}repaint();
}
