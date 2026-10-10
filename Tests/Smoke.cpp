#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <stdexcept>
using namespace juce;
void check(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
void signal(AudioBuffer<float>& b,double sr,int offset=0){for(int c=0;c<b.getNumChannels();++c)for(int i=0;i<b.getNumSamples();++i)b.setSample(c,i,.13f*std::sin(static_cast<float>((i+offset)*MathConstants<double>::twoPi*110/sr)));}
int main(int argc,char** argv){ScopedJuceInitialiser_GUI init;
try{
    if(argc>1 && String(argv[1])=="--render"){auto p=std::make_unique<SishhinProcessor>();std::unique_ptr<AudioProcessorEditor> e(p->createEditor());auto image=e->createComponentSnapshot(e->getLocalBounds());File file=File::getCurrentWorkingDirectory().getChildFile("SISHHIN-AMP.png");auto stream=file.createOutputStream();check(stream!=nullptr,"PNG stream");stream->setPosition(0);stream->truncate();check(PNGImageFormat().writeImageToStream(image,*stream),"PNG render");std::cout<<file.getFullPathName()<<std::endl;return 0;}
    for(double sr:{44100.,48000.,96000.})for(int chans:{1,2}){
        auto p=std::make_unique<SishhinProcessor>();p->setPlayConfigDetails(chans,chans,sr,256);p->prepareToPlay(sr,256);MidiBuffer midi;AudioBuffer<float> b(chans,777);
        for(int preset=0;preset<6;++preset){p->factoryPreset(preset);double energy=0;
            for(int pass=0;pass<16;++pass){signal(b,sr,pass*b.getNumSamples());p->processBlock(b,midi);for(int c=0;c<chans;++c)for(int i=0;i<b.getNumSamples();++i){float v=b.getSample(c,i);check(std::isfinite(v),"Non-finite audio");check(std::abs(v)<=4,"Unbounded output");energy+=v*v;}}
            check(energy>1.e-5,"Silent factory preset");}
        p->set("bypass",1);signal(b,sr);float original=b.getSample(0,20);p->processBlock(b,midi);check(b.getSample(0,20)==original,"Bypass changes audio");p->set("bypass",0);
        p->set("tuner",1);p->set("tunerMute",1);signal(b,sr);p->processBlock(b,midi);check(b.getMagnitude(0,b.getNumSamples())==0,"Tuner mute");
        p->set("drive",83);MemoryBlock saved;p->getStateInformation(saved);p->set("drive",1);p->setStateInformation(saved.getData(),static_cast<int>(saved.getSize()));check(std::abs(p->value("drive")-83)<.02f,"State roundtrip");
        p->set("input",7);p->set("toneLock",1);p->factoryPreset(2);check(p->value("input")==7,"Tone lock");
    }
    auto p=std::make_unique<SishhinProcessor>();p->prepareToPlay(48000,512);MidiBuffer midi;AudioBuffer<float> b(2,512);
    p->set("tuner",1);p->set("tunerMute",0);String note;
    for(int pass=0;pass<40;++pass){signal(b,48000,pass*512);p->processBlock(b,midi);note=p->tunerText();}check(note.startsWith("A2"),"Tuner failed 110 Hz");
    p->captureTone();for(int pass=0;pass<290;++pass){signal(b,48000,pass*512);p->processBlock(b,midi);}check(p->profileReady.load(),"Tone capture did not complete");
    p->calibrate();for(int pass=0;pass<290;++pass){signal(b,48000,pass*512);p->processBlock(b,midi);}float gain;check(p->takeCalibration(gain)&&gain<1000,"Auto gain failed");
    // A custom IR must survive recall without the original file and sample-rate changes.
    TemporaryFile temp(".wav");WavAudioFormat wav;{
        auto stream=temp.getFile().createOutputStream();check(stream!=nullptr,"IR file stream");
        std::unique_ptr<AudioFormatWriter> writer(wav.createWriterFor(stream.get(),48000,2,24,{},0));check(writer!=nullptr,"IR writer");stream.release();
        AudioBuffer<float> ir(2,1024);ir.clear();ir.setSample(0,0,.9f);ir.setSample(1,3,.8f);ir.setSample(0,43,-.3f);check(writer->writeFromAudioSampleBuffer(ir,0,1024),"IR write");
    }
    check(p->loadIR(temp.getFile()).wasOk(),"IR import");MemoryBlock irState;p->getStateInformation(irState);check(irState.getSize()>6000,"IR not embedded");
    auto recalled=std::make_unique<SishhinProcessor>();recalled->setStateInformation(irState.getData(),static_cast<int>(irState.getSize()));check(recalled->value("cab")==4,"Custom cabinet not recalled");
    temp.getFile().deleteFile();recalled->prepareToPlay(96000,256);signal(b,96000);recalled->processBlock(b,midi);check(std::isfinite(b.getMagnitude(0,b.getNumSamples())),"Recalled IR output");
    check(p->loadIR(temp.getFile()).failed(),"Missing IR accepted");
    // A +12 semitone voice should double a sustained test tone's frequency.
    sishhin::Pitch shifter;int crossings=0;float previous=0;
    for(int i=0;i<96000;++i){float v=shifter.tick(.2f*std::sin(static_cast<float>(i*MathConstants<double>::twoPi*220/48000)),12,48000);if(i>=48000 && previous<=0 && v>0)++crossings;previous=v;}
    check(std::abs(crossings-440)<15,"Octave pitch frequency");
    std::cout<<"PASS: presets, finite output, mono/stereo, 44.1/48/96k, oversized blocks, bypass, tuner/mute, state, tone lock, capture, calibration, embedded stereo IR recall and octave pitch\n";
    return 0;
}catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<std::endl;return 1;}}
