# Signal design

Input trim → noise gate → transient emphasis → spectral correction → pitch matrix → bass focus → character → dynamic amp → EQ → cabinet → cuts / lo-fi → stutter → ambience → output.

* The amp and four character algorithms run at 4× sample rate using polyphase IIR oversampling. Saturation uses continuous tanh/atan curves, controlled asymmetry and DC removal.
* Low End Focus sets a 32–187 Hz pre-distortion high-pass. Pick Attack compares two envelope followers. Sag reduces drive as the signal envelope grows, with an approximately 40 ms response.
* Sub Weight blends an octave-down grain shifter. Parallel uses two additional oversampled channels with independent character, DC and amplifier/EQ states. Cleanse suppresses this voice while Whammy is not at zero.
* Matching uses 2048-point windowed FFT frames and ten octave-like power bands. The normalized target/source difference is clamped to ±12 dB and rendered as peaking filters.
* Cabinet presets are deterministic synthetic damped resonances and reflections with band limiting. Custom IRs use JUCE partitioned convolution and are transferred to the audio thread using a try-lock and ownership-moving buffer load.
* Delay uses a preallocated interpolated stereo ring, tempo-based timing, smoothed time/mix changes and low-pass-filtered cross-feedback. Reverb uses a diffuse stereo network. Wet-path filtering and an envelope-based ducker protect the dry attack.
* The tuner uses normalized autocorrelation and parabolic lag interpolation on the UI thread. Audio is handed over through a bounded lock-free FIFO.
* State stores all parameters, preset index, spectral data and encoded custom IR bytes. Recalled IRs are decoded and moved into the convolution engine; the source file is not needed.

Known tradeoffs: grain pitch shifting can smear complex transients; the amp is a designed saturation model; cabinet defaults are synthetic; abrupt routing/bypass changes may change phase and level. Parameter automation and native GUI rendering are tested separately from listening quality.
