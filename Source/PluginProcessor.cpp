#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;
AudioProcessorValueTreeState::ParameterLayout SishhinProcessor::parameters(){
    AudioProcessorValueTreeState::ParameterLayout p;
    auto f=[&](const char* id,const char* name,float lo,float hi,float def,float step=0.01f){p.add(std::make_unique<AudioParameterFloat>(ParameterID{id,1},name,NormalisableRange<float>(lo,hi,step),def));};
    auto b=[&](const char* id,const char* name,bool def){p.add(std::make_unique<AudioParameterBool>(ParameterID{id,1},name,def));};
    auto c=[&](const char* id,const char* name,StringArray choices,int def){p.add(std::make_unique<AudioParameterChoice>(ParameterID{id,1},name,choices,def));};
    f("input","Input dB",-24,24,0);f("output","Output dB",-36,12,-6);f("gate","Gate dB",-80,0,-55);
    b("bypass","Bypass",false);b("toneOn","Tone match",false);f("amount","Match amount %",0,100,50);f("smooth","Match smooth %",0,100,80);
    b("toneLock","Lock tone and input on preset change",false);
    b("characterOn","Character",true);c("engine","Character engine",{"RAZOR","MIRE","RELIC","MAUL"},0);
    f("distortion","Distortion %",0,100,30);f("tone","Character tone %",0,100,55);f("colour","Character colour %",0,100,50);
    f("makeup","Character makeup dB",-18,18,0);
    b("stutterOn","Stutter",false);c("speed","Stutter division",{"1/4","1/8","1/16","1/32","1/8 D","1/16 D","1/8 T","1/16 T"},2);
    f("depth","Stutter depth %",0,100,100);b("tighten","Tighten",true);f("tempo","Free tempo BPM",40,240,120,1);
    b("pitchOn","Pitch",true);f("whammy","Whammy semitones",-24,12,0);f("dissonance","Dissonance %",0,100,0);
    f("thicken","Thicken %",0,100,0);c("octave","Sub octave",{"-1","-2"},0);f("subCut","Sub high cut Hz",80,12000,6000,1);
    b("cleanse","Pitch cleanse",true);b("parallel","Parallel sub routing",false);
    b("ampOn","Amplifier",true);f("drive","Drive %",0,100,64);f("low","Low dB",-12,12,-2);f("mid","Mid dB",-12,12,1);
    f("high","High dB",-12,12,1);f("presence","Presence dB",-12,12,1);b("stereo","Stereo",true);
    b("cabOn","Cabinet",true);c("cab","Cabinet IR",{"Obsidian 4x12","Iron 4x12","Ruin 2x12","Monolith 4x12","Custom WAV"},0);
    f("lowCut","Low cut Hz (20 = off)",20,500,70,1);f("highCut","High cut Hz (20000 = off)",2000,20000,10000,1);
    b("lofi","Lo-fi",false);b("tuner","Tuner",false);b("tunerMute","Mute while tuning",true);
    return p;
}
SishhinProcessor::SishhinProcessor():AudioProcessor(BusesProperties().withInput("Input",AudioChannelSet::stereo(),true).withOutput("Output",AudioChannelSet::stereo(),true)),state(*this,nullptr,"SISHHIN",parameters()){
    formats.registerBasicFormats();for(int i=0;i<sishhin::bands;++i){matchDb[i]=0;targetPublished[i]=0;}
    for(auto& c:cabinets)c=std::make_unique<dsp::Convolution>();
}
bool SishhinProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
    auto in=l.getMainInputChannelSet(),out=l.getMainOutputChannelSet();
    return (in==AudioChannelSet::mono() || in==AudioChannelSet::stereo()) && (out==in || out==AudioChannelSet::stereo());
}
void SishhinProcessor::set(const String& id,float v){if(auto* p=state.getParameter(id)){p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(v));p->endChangeGesture();}}
void SishhinProcessor::prepareToPlay(double sr,int block){
    rate=sr;maxBlock=jmax(32,block);for(auto& channel:channels)channel=Channel{};capture.reset();captureRemaining=0;calibrationSamples=0;stutterPhase=0;stutterGain=1;
    oversampler=std::make_unique<dsp::Oversampling<float>>(4,2,dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true);
    oversampler->initProcessing(static_cast<size_t>(maxBlock));oversampler->reset();setLatencySamples(static_cast<int>(oversampler->getLatencyInSamples()));
    parallelSub.setSize(2,maxBlock);
    ampWork.setSize(4,maxBlock);
    for(auto* s:{&inGain,&outGain,&whammyShift,&driveGain})s->reset(sr,.025);
    driveGain.reset(sr*4,.025);
    inGain.setCurrentAndTargetValue(Decibels::decibelsToGain(value("input")));outGain.setCurrentAndTargetValue(Decibels::decibelsToGain(value("output")));
    whammyShift.setCurrentAndTargetValue(value("whammy"));driveGain.setCurrentAndTargetValue(1+value("drive")*.32f);
    // Original synthetic cabinet FIRs: damped resonances, early reflections and a rolled-off cone response.
    for(int n=0;n<4;++n){
        AudioBuffer<float> ir(1,2048);Random rng(0x51aa+n);sishhin::Biquad lp,hp;lp.set(48000,4700.f+n*750.f,.707f,0);hp.set(48000,65.f+n*14.f,.707f,1);
        for(int i=0;i<2048;++i){float t=i/48000.f;float x=(i==0?.8f:0.f)+.22f*std::exp(-t*(220.f+n*25.f))*rng.nextFloat()*std::cos(2*sishhin::pi*(115.f+n*37.f)*t);
            x+=.12f*std::sin(2*sishhin::pi*(1800.f+n*320.f)*t)*std::exp(-t*520.f);if(i==67+n*11)x+=.2f;if(i==133+n*19)x-=.13f;
            ir.setSample(0,i,hp.tick(lp.tick(x)));}
        cabinets[n]->loadImpulseResponse(std::move(ir),48000,dsp::Convolution::Stereo::no,dsp::Convolution::Trim::no,dsp::Convolution::Normalise::yes);
    }
    {const ScopedLock lock(assetsLock);if(irBytes.getSize()>0){
        auto stream=std::make_unique<MemoryInputStream>(irBytes,false);std::unique_ptr<AudioFormatReader> reader(formats.createReaderFor(std::move(stream)));
        if(reader && reader->lengthInSamples>0 && reader->lengthInSamples<=reader->sampleRate*2){AudioBuffer<float> decoded(jmin(2,static_cast<int>(reader->numChannels)),static_cast<int>(reader->lengthInSamples));reader->read(&decoded,0,decoded.getNumSamples(),0,true,true);
            cabinets[4]->loadImpulseResponse(std::move(decoded),reader->sampleRate,dsp::Convolution::Stereo::yes,dsp::Convolution::Trim::yes,dsp::Convolution::Normalise::yes);customReady=true;hasPendingIR=false;}
    }}
    for(auto& c:cabinets){c->prepare({sr,static_cast<uint32>(maxBlock),2});c->reset();}
    tunerFifo.reset();tunerFill=0;updateFilters();
}
void SishhinProcessor::updateFilters(){
    const float sm=value("smooth")*.01f,amt=value("amount")*.01f;
    for(auto& c:channels){
        c.pre.set(rate,value("tighten")>0.5f?100.f:38.f,.707f,1);
        c.cleanse.set(rate,65,.707f,1);c.subCut.set(rate,value("subCut"),.707f,0);
        c.colour.set(rate*4,700+value("tone")*100,.707f,0);c.dc.set(rate*4,25,.707f,1);
        c.low.set(rate,130,.65f,2,value("low"));c.mid.set(rate,850,.8f,2,value("mid"));c.high.set(rate,3000,.7f,2,value("high"));c.presence.set(rate,6200,.65f,2,value("presence"));
        c.loCut.set(rate,value("lofi")>.5f?450:value("lowCut"),.707f,1);c.hiCut.set(rate,value("lofi")>.5f?2600:value("highCut"),.707f,0);
        for(int b=0;b<sishhin::bands;++b){float db=matchDb[b].load();float near=(matchDb[jmax(0,b-1)].load()+db+matchDb[jmin(sishhin::bands-1,b+1)].load())/3;
            c.match[b].set(rate,sishhin::centres[b],.9f,2,amt*((1-sm)*db+sm*near));}
    }
}
void SishhinProcessor::processBlock(AudioBuffer<float>& buffer,MidiBuffer& midi){
    ScopedNoDenormals noDenormals;midi.clear();
    if(getTotalNumInputChannels()==1 && buffer.getNumChannels()>1)buffer.copyFrom(1,0,buffer,0,0,buffer.getNumSamples());
    for(int c=getTotalNumOutputChannels();c<buffer.getNumChannels();++c)buffer.clear(c,0,buffer.getNumSamples());
    double bpm=value("tempo");if(auto* play=getPlayHead())if(auto pos=play->getPosition()){if(auto tempo=pos->getBpm())bpm=*tempo;
        if(value("stutterOn")>.5f && pos->getIsPlaying())if(auto ppq=pos->getPpqPosition()){
            constexpr double beats[]{1,.5,.25,.125,.75,.375,1./3,1./6};stutterPhase=std::fmod(*ppq/beats[static_cast<int>(value("speed"))],1.);if(stutterPhase<0)stutterPhase+=1;}}
    {const ScopedTryLock lock(assetsLock);if(lock.isLocked() && hasPendingIR){cabinets[4]->loadImpulseResponse(std::move(pendingIR),pendingRate,dsp::Convolution::Stereo::yes,dsp::Convolution::Trim::yes,dsp::Convolution::Normalise::yes);hasPendingIR=false;customReady=true;}}
    if(captureRequest.exchange(false)){capture.reset();captureRemaining=static_cast<int>(rate*3);for(int i=0;i<sishhin::bands;++i)target[i]=targetPublished[i].load();}
    if(calibrateRequest.exchange(false)){calibrationSamples=static_cast<int>(rate*3);calibrationEnergy=0;calibrationCount=0;}
    inGain.setTargetValue(Decibels::decibelsToGain(value("input")));outGain.setTargetValue(Decibels::decibelsToGain(value("output")));
    whammyShift.setTargetValue(value("whammy"));driveGain.setTargetValue(1+value("drive")*.32f);updateFilters();
    float inPeak=0,outPeak=0;
    for(int start=0;start<buffer.getNumSamples();start+=maxBlock){const int count=jmin(maxBlock,buffer.getNumSamples()-start);
        float* ptrs[2]{buffer.getWritePointer(0,start),buffer.getWritePointer(jmin(1,buffer.getNumChannels()-1),start)};
        AudioBuffer<float> chunk(ptrs,jmin(2,buffer.getNumChannels()),count);
        for(int c=0;c<chunk.getNumChannels();++c)inPeak=jmax(inPeak,chunk.getMagnitude(c,0,count));
        processChunk(chunk,bpm);
        for(int c=0;c<chunk.getNumChannels();++c)outPeak=jmax(outPeak,chunk.getMagnitude(c,0,count));
    }
    inputPeak=inPeak;outputPeak=outPeak;
}
void SishhinProcessor::processChunk(AudioBuffer<float>& buffer,double bpm){
    const int n=buffer.getNumSamples(),nc=buffer.getNumChannels();
    const bool bypass=value("bypass")>.5f,pitch=value("pitchOn")>.5f,amp=value("ampOn")>.5f,character=value("characterOn")>.5f;
    const bool stereo=value("stereo")>.5f,parallel=value("parallel")>.5f;
    const float threshold=Decibels::decibelsToGain(value("gate")),thicken=value("thicken")*.01f,dissonance=value("dissonance")*.01f,octave=value("octave")>0?-24.f:-12.f;
    const float envDecay=std::exp(-1.f/static_cast<float>(rate*.015));
    for(int i=0;i<n;++i){
        float mono=buffer.getSample(0,i);if(nc==2)mono=.5f*(mono+buffer.getSample(1,i));
        if(value("tuner")>.5f){int s1,z1,s2,z2;tunerFifo.prepareToWrite(1,s1,z1,s2,z2);if(z1){tunerQueue[s1]=mono;tunerFifo.finishedWrite(1);}}
        if(calibrationSamples>0){calibrationEnergy+=mono*mono;++calibrationCount;if(--calibrationSamples==0){float rms=std::sqrt(static_cast<float>(calibrationEnergy/jmax(1,calibrationCount)));calibrationResult=rms>1.e-5f?jlimit(-24.f,24.f,-18-Decibels::gainToDecibels(rms)):1001.f;}}
        if(captureRemaining.load()>0){capture.push(mono,rate);if(captureRemaining.fetch_sub(1)==1){const auto source=capture.result();
            if(*std::max_element(capture.sums.begin(),capture.sums.end())>1.e-8){for(int b=0;b<sishhin::bands;++b)matchDb[b]=jlimit(-12.f,12.f,target[b]-source[b]);profileReady=true;}}}
        if(bypass)continue;
        const float input=inGain.getNextValue(),shift=whammyShift.getNextValue();
        for(int c=0;c<nc;++c){auto& ch=channels[c];float x=(stereo?buffer.getSample(c,i):mono)*input;
            ch.envelope=jmax(std::abs(x),ch.envelope*envDecay);float open=(ch.envelope>threshold || value("gate")<=-79.9f)?1.f:0.f;
            ch.gate+=(open-ch.gate)*(open>.5f?.08f:.003f);x*=ch.gate;
            if(value("toneOn")>.5f)for(auto& eq:ch.match)x=eq.tick(x);
            if(pitch){x=ch.whammy.tick(x,shift,rate);float sub=ch.subCut.tick(ch.sub.tick(x,octave,rate));float dis=ch.dissonance.tick(x,1.f,rate);
                const float mix=value("cleanse")>.5f && std::abs(shift)>.01f?0:thicken;
                ch.previous=parallel?sub*mix:0.f;x=x*(1-mix)+dis*dissonance+(parallel?0:sub*mix);
            }else ch.previous=0;
            parallelSub.setSample(c,i,ch.previous);
            buffer.setSample(c,i,ch.pre.tick(x));
        }
    }
    if(bypass)return;
    dsp::AudioBlock<float> block(buffer);
    ampWork.clear();for(int c=0;c<nc;++c){ampWork.copyFrom(c,0,buffer,c,0,n);ampWork.copyFrom(c+2,0,parallelSub,c,0,n);}
    auto workBlock=dsp::AudioBlock<float>(ampWork).getSubBlock(0,static_cast<size_t>(n));
    auto up=oversampler->processSamplesUp(workBlock);
    const int engine=static_cast<int>(value("engine"));const float dist=value("distortion")*.01f,colour=value("colour")*.01f;
    const float makeup=Decibels::decibelsToGain(value("makeup"));
    for(size_t i=0;i<up.getNumSamples();++i){float drive=driveGain.getNextValue(); // smoothing runs at the oversampled rate
        for(size_t c=0;c<up.getNumChannels();++c){auto& ch=channels[c];float x=up.getSample(c,i);
            if(character){float wet=x;
                switch(engine){case 0:wet=std::tanh(x*(2+dist*22))*.6f;break;
                    case 1:wet=std::atan(x*(4+dist*38))*.52f;break;
                    case 2:wet=(std::abs(std::tanh(x*(3+dist*25)))-.25f)*1.2f;wet*=jmin(1.f,std::abs(x)*90);break;
                    default:wet=std::tanh(x*(2+dist*25)+.15f*colour)-std::tanh(.15f*colour);break;}
                x=((1-dist*.75f)*x+dist*.75f*wet)*makeup;x=ch.colour.tick(x)*(0.7f+colour*.6f);}
            if(amp){x=std::tanh(x*drive);x=std::tanh((x+.08f*x*x)*2.8f)*.45f;}
            up.setSample(c,i,ch.dc.tick(x));
        }
    }
    oversampler->processSamplesDown(workBlock);
    for(int c=0;c<4;++c){auto& ch=channels[c];for(int i=0;i<n;++i){float x=ampWork.getSample(c,i);if(amp)x=ch.presence.tick(ch.high.tick(ch.mid.tick(ch.low.tick(x))));ampWork.setSample(c,i,x);}}
    for(int c=0;c<nc;++c)for(int i=0;i<n;++i)buffer.setSample(c,i,ampWork.getSample(c,i)+ampWork.getSample(c+2,i));
    if(value("cabOn")>.5f){int choice=static_cast<int>(value("cab"));if(choice==4 && !customReady.load())choice=0;dsp::ProcessContextReplacing<float> ctx(block);cabinets[choice]->process(ctx);}
    constexpr double beats[]{1,.5,.25,.125,.75,.375,1./3,1./6};
    const double step=bpm/(60.*rate*beats[static_cast<int>(value("speed"))]);
    for(int i=0;i<n;++i){stutterPhase+=step;stutterPhase-=std::floor(stutterPhase);
        const float targetGain=value("stutterOn")>.5f && stutterPhase>(value("tighten")>.5f?.42:.55)?1-value("depth")*.01f:1;
        stutterGain+=(targetGain-stutterGain)*.08f;float gain=outGain.getNextValue()*stutterGain;
        if(value("tuner")>.5f && value("tunerMute")>.5f)gain=0;
        for(int c=0;c<nc;++c){auto& ch=channels[c];float x=buffer.getSample(c,i);
            if(value("lowCut")>20 || value("lofi")>.5f)x=ch.loCut.tick(x);if(value("highCut")<20000 || value("lofi")>.5f)x=ch.hiCut.tick(x);
            x*=gain;if(!std::isfinite(x))x=0;buffer.setSample(c,i,jlimit(-4.f,4.f,x));}
    }
}
bool SishhinProcessor::takeCalibration(float& db){float v=calibrationResult.exchange(1000.f);if(v==1000.f)return false;db=v;return true;}
Result SishhinProcessor::loadIR(const File& file){
    std::unique_ptr<AudioFormatReader> reader(formats.createReaderFor(file));if(!reader)return Result::fail("Choose a readable WAV or AIFF impulse response.");
    if(reader->lengthInSamples<=0 || reader->lengthInSamples>reader->sampleRate*2)return Result::fail("IR must be between one sample and two seconds.");
    AudioBuffer<float> data(jmin(2,static_cast<int>(reader->numChannels)),static_cast<int>(reader->lengthInSamples));
    if(!reader->read(&data,0,data.getNumSamples(),0,true,true))return Result::fail("Could not read IR samples.");
    MemoryBlock bytes;if(!file.loadFileAsData(bytes))return Result::fail("Could not read IR file.");
    {const ScopedLock lock(assetsLock);pendingIR=std::move(data);pendingRate=reader->sampleRate;hasPendingIR=true;irBytes=std::move(bytes);customIRName=file.getFileName();}
    set("cab",4);return Result::ok();
}
Result SishhinProcessor::loadReference(const File& file){
    std::unique_ptr<AudioFormatReader> reader(formats.createReaderFor(file));if(!reader)return Result::fail("Choose a readable guitar reference WAV or AIFF.");
    sishhin::Spectrum analyser;AudioBuffer<float> b(2,2048);const int64 length=jmin(reader->lengthInSamples,static_cast<int64>(reader->sampleRate*30));
    for(int64 pos=0;pos<length;pos+=2048){b.clear();int count=static_cast<int>(jmin(int64(2048),length-pos));reader->read(&b,0,count,pos,true,true);for(int i=0;i<count;++i)analyser.push((b.getSample(0,i)+b.getSample(1,i))*.5f,reader->sampleRate);}
    if(analyser.frames==0 || *std::max_element(analyser.sums.begin(),analyser.sums.end())<1.e-8)return Result::fail("Reference is too short or silent.");
    const auto result=analyser.result();for(int i=0;i<sishhin::bands;++i)targetPublished[i]=result[i];return Result::ok();
}
String SishhinProcessor::irName() const {const ScopedLock lock(assetsLock);return customIRName;}
String SishhinProcessor::tunerText(){
    int a,b,c,d;tunerFifo.prepareToRead(tunerFifo.getNumReady(),a,b,c,d);
    auto take=[&](int start,int size){for(int i=0;i<size;++i){tunerWindow[tunerFill++]=tunerQueue[start+i];if(tunerFill==8192){
        double energy=0;for(float x:tunerWindow)energy+=x*x;
        if(energy/8192<1.e-7){lastTuner="Play a single note";tunerCents=0;}else{
            const int minLag=static_cast<int>(rate/1200),maxLag=jmin(4095,static_cast<int>(rate/30));float best=0;int lagBest=0;
            std::array<float,4096> corr{};
            for(int lag=minLag;lag<=maxLag;++lag){double xy=0,xx=0,yy=0;for(int j=0;j<4096;j+=2){float x=tunerWindow[j],y=tunerWindow[j+lag];xy+=x*y;xx+=x*x;yy+=y*y;}corr[lag]=static_cast<float>(xy/std::sqrt(xx*yy+1.e-20));best=jmax(best,corr[lag]);}
            for(int lag=minLag+1;lag<maxLag;++lag)if(corr[lag]>.85f && corr[lag]>best*.98f && corr[lag]>=corr[lag-1] && corr[lag]>corr[lag+1]){lagBest=lag;break;}
            if(lagBest){float denom=corr[lagBest-1]-2*corr[lagBest]+corr[lagBest+1];float delta=std::abs(denom)>1.e-9f?.5f*(corr[lagBest-1]-corr[lagBest+1])/denom:0;
                double hz=rate/(lagBest+delta),note=69+12*std::log2(hz/440.);int nearest=roundToInt(note);const char* names[]{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
                tunerCents=static_cast<float>((note-nearest)*100);lastTuner=String(names[(nearest%12+12)%12])+String(nearest/12-1)+"  "+String((note-nearest)*100,1)+" ct  |  "+String(hz,1)+" Hz";
            }else {lastTuner="Unstable / play one note";tunerCents=0;}
        }tunerFill=0;}}};take(a,b);take(c,d);tunerFifo.finishedRead(b+d);return lastTuner;
}
void SishhinProcessor::getStateInformation(MemoryBlock& dest){
    auto tree=state.copyState();for(int i=0;i<sishhin::bands;++i){tree.setProperty("match"+String(i),matchDb[i].load(),nullptr);tree.setProperty("target"+String(i),targetPublished[i].load(),nullptr);}
    tree.setProperty("profileReady",profileReady.load(),nullptr);
    {const ScopedLock lock(assetsLock);tree.setProperty("irData",irBytes.toBase64Encoding(),nullptr);tree.setProperty("irName",customIRName,nullptr);}
    if(auto xml=tree.createXml())copyXmlToBinary(*xml,dest);
}
void SishhinProcessor::setStateInformation(const void* data,int size){
    auto xml=getXmlFromBinary(data,size);if(!xml || !xml->hasTagName("SISHHIN"))return;auto tree=ValueTree::fromXml(*xml);
    for(int i=0;i<sishhin::bands;++i){matchDb[i]=jlimit(-12.f,12.f,static_cast<float>(tree.getProperty("match"+String(i),0)));targetPublished[i]=static_cast<float>(tree.getProperty("target"+String(i),0));}
    profileReady=static_cast<bool>(tree.getProperty("profileReady",false));
    MemoryBlock bytes;bytes.fromBase64Encoding(tree.getProperty("irData").toString());
    AudioBuffer<float> decoded;double sr=48000;
    if(bytes.getSize()>0 && bytes.getSize()<16000000){auto stream=std::make_unique<MemoryInputStream>(bytes,false);std::unique_ptr<AudioFormatReader> reader(formats.createReaderFor(std::move(stream)));
        if(reader && reader->lengthInSamples>0 && reader->lengthInSamples<=reader->sampleRate*2){decoded.setSize(jmin(2,static_cast<int>(reader->numChannels)),static_cast<int>(reader->lengthInSamples));reader->read(&decoded,0,decoded.getNumSamples(),0,true,true);sr=reader->sampleRate;}}
    {const ScopedLock lock(assetsLock);irBytes=std::move(bytes);customIRName=tree.getProperty("irName","No custom IR").toString();pendingIR=std::move(decoded);pendingRate=sr;hasPendingIR=pendingIR.getNumSamples()>0;customReady=hasPendingIR;}
    state.replaceState(tree);
}
void SishhinProcessor::factoryPreset(int index){
    const bool locked=value("toneLock")>.5f;
    for(auto* base:getParameters())if(auto* p=dynamic_cast<RangedAudioParameter*>(base)){
        if(p->paramID=="toneLock" || (locked && (p->paramID=="input" || p->paramID=="toneOn" || p->paramID=="amount" || p->paramID=="smooth")))continue;
        p->beginChangeGesture();p->setValueNotifyingHost(p->getDefaultValue());p->endChangeGesture();}
    switch(index){case 1:set("drive",75);set("low",-4);set("mid",3);set("gate",-44);set("cab",1);break;
        case 2:set("engine",3);set("drive",80);set("thicken",22);set("whammy",-5);set("cab",3);set("highCut",7200);break;
        case 3:set("engine",1);set("distortion",72);set("low",4);set("cab",2);set("tighten",0);break;
        case 4:set("engine",2);set("dissonance",30);set("stutterOn",1);set("speed",7);set("depth",90);break;
        case 5:set("drive",47);set("mid",4);set("presence",-1);set("gate",-65);break;
        default:break;}
}
AudioProcessorEditor* SishhinProcessor::createEditor(){return new SishhinEditor(*this);}
AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new SishhinProcessor();}
