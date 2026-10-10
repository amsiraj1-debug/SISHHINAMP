#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

namespace sishhin {
constexpr float pi = juce::MathConstants<float>::pi;
struct Biquad {
    float b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
    float tick(float x) noexcept { const auto y=b0*x+z1; z1=b1*x-a1*y+z2; z2=b2*x-a2*y; return y; }
    void reset() noexcept { z1=z2=0; }
    void set(double sr,float hz,float q,int type,float db=0) noexcept {
        hz=juce::jlimit(10.f,static_cast<float>(sr*.45),hz);
        const float w=2*pi*hz/static_cast<float>(sr), c=std::cos(w), s=std::sin(w), alpha=s/(2*q), A=std::pow(10.f,db/40.f);
        float a0=1;
        if(type==0){ b0=(1-c)*.5f;b1=1-c;b2=b0;a0=1+alpha;a1=-2*c;a2=1-alpha; }
        else if(type==1){ b0=(1+c)*.5f;b1=-(1+c);b2=b0;a0=1+alpha;a1=-2*c;a2=1-alpha; }
        else { b0=1+alpha*A;b1=-2*c;b2=1-alpha*A;a0=1+alpha/A;a1=-2*c;a2=1-alpha/A; }
        b0/=a0;b1/=a0;b2/=a0;a1/=a0;a2/=a0;
    }
};
// Two overlapping variable delay grains. Processes chords without note detection.
// Window length trades pitch stability against transient smearing.
struct Pitch {
    std::array<float,16384> ring{}; int write=0; float phase=0;
    void reset(){ring.fill(0);write=0;phase=0;}
    float tick(float x,float semitones,double sr) noexcept {
        ring[static_cast<size_t>(write)]=x;
        const float window=static_cast<float>(sr*.045);
        const float ratio=std::pow(2.f,semitones/12.f);
        if(std::abs(semitones)<.001f){write=(write+1)&16383;return x;}
        phase+=(1-ratio)/window;phase-=std::floor(phase);
        auto read=[&](float p){float pos=static_cast<float>(write)-32-p*window;while(pos<0)pos+=16384;
            int i=static_cast<int>(pos)&16383;float f=pos-std::floor(pos);return ring[static_cast<size_t>(i)]*(1-f)+ring[static_cast<size_t>((i+1)&16383)]*f;};
        float p2=phase+.5f;if(p2>=1)p2-=1;
        const float w=.5f-.5f*std::cos(2*pi*phase);
        float y=read(phase)*w+read(p2)*(1-w);write=(write+1)&16383;return y;
    }
};
constexpr int bands=10;
constexpr std::array<float,bands> centres{65,125,250,500,1000,2000,3500,5000,7500,11000};
struct Spectrum {
    juce::dsp::FFT fft{11}; std::array<float,4096> data{}; int fill=0,frames=0;
    std::array<double,bands> sums{};
    void reset(){fill=frames=0;sums.fill(0);data.fill(0);}
    bool push(float x,double sr){data[static_cast<size_t>(fill++)]=x;if(fill<2048)return false;
        for(int i=0;i<2048;++i)data[static_cast<size_t>(i)]*=.5f-.5f*std::cos(2*pi*static_cast<float>(i)/2047.f);
        std::fill(data.begin()+2048,data.end(),0.f);fft.performFrequencyOnlyForwardTransform(data.data());
        for(int b=0;b<bands;++b){double sum=0;int count=0;for(int k=1;k<1024;++k){const double hz=k*sr/2048.;
            if(hz>=centres[b]/1.4142 && hz<centres[b]*1.4142){sum+=data[k]*data[k];++count;}}
            sums[b]+=sum/juce::jmax(1,count);}
        ++frames;fill=0;return true;
    }
    std::array<float,bands> result() const { std::array<float,bands> out{};float mean=0;
        for(int b=0;b<bands;++b){out[b]=10*std::log10(static_cast<float>(sums[b]/juce::jmax(1,frames))+1.e-12f);mean+=out[b];}
        for(auto& v:out)v-=mean/bands;return out; }
};
}
