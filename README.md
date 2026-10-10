# SISHHIN AMP

A free, open-source Windows x64 VST3 and standalone guitar amplifier for djent, thall and extended-range guitars. No accounts, activation, trial timer, licence keys or online checks.

The editor follows the supplied Slam Amp reference's control layout: distressed light metal panels, red indicators, three upper processing sections, a dark central amplifier, side level meters and a bottom cabinet strip. The artwork is drawn in code and the DSP is a new implementation. This is **not a bit-exact or sound-identical clone**.

## Download and install

Open **Actions → Build Windows VST3 → latest successful run → Artifacts → SISHHIN-AMP-Windows-x64**. GitHub may require sign-in to download Actions artifacts.

Extract the ZIP. Copy the **entire `SISHHIN AMP.vst3` folder**, including its Contents directory, into `C:\Program Files\Common Files\VST3\`. Rescan plugins in your DAW. Do not copy just the inner binary. The standalone executable can be run from the extracted folder; select your input/output device in its audio settings. Start with your interface output low and use headphones to avoid acoustic feedback.

## Signal path

Input → gate → tone match → whammy / dissonance / sub-octave → character → high-gain amp → four-band EQ → cabinet convolution → cuts / lo-fi → tempo stutter → output.

Parallel routes the sub voice through its own character, amplifier and EQ chain before cabinet mixing. Mono sums the input channels; Stereo preserves independent channels. The nonlinear character and amplifier paths run at 4× sample rate. The host receives the oversampling latency. Pitch shifting has additional variable time smear inherent to the granular algorithm, which is not a fixed delay suitable for host compensation.

## Controls

| Section | Included functionality |
| --- | --- |
| Input | ±24 dB input, three-second auto calibration to −18 dBFS RMS, peak meter, gate threshold (−80 dB disables gate) |
| Tone match | Load isolated reference WAV/AIFF (first 30 seconds), capture three seconds of live DI, ten logarithmic correction bands clamped to ±12 dB, Amount and Smooth, profile recall, input/tone lock on factory preset changes |
| Character | RAZOR cutting saturation, MIRE thick atan fuzz, RELIC rectified/gated octave fuzz, MAUL asymmetric growl; Distortion, Tone, Colour and makeup gain |
| Stutter | Host tempo/transport synchronization, eight straight/dotted/triplet divisions, Depth, Tighten and fallback BPM |
| Pitch | Continuous −24 to +12 semitone Whammy, +1 semitone Dissonance blend, −1/−2 octave Thicken, sub high cut, Cleanse and Parallel routing |
| Amplifier | Multistage high-gain saturation, Drive, Low, Mid, High, Presence, mono/stereo |
| Cabinet | Four original synthetic cabinet FIRs: Obsidian, Iron, Ruin and Monolith; custom mono/stereo WAV or AIFF IR up to two seconds; embedded IR recall |
| Output | Low/high cuts with endpoint-off settings, lo-fi band limiting, output trim and peak meter, global bypass |
| Tuner | Autocorrelation note/octave/cents/Hz display and moving strobe strip with optional mute; suitable for individual notes down to approximately 30 Hz |
| Presets | Six original factory sounds, save/open `.sishhin` files, host session recall; presets embed custom IR and match data |
| Host | Parameter automation, mono, stereo and mono-to-stereo buses; VST3 and standalone |

Double-click a knob to reset. Numeric values beneath controls can be typed directly. Hover controls for help. Red LEDs indicate enabled sections. A lit BYPASS indicator means the whole effect is bypassed.

### Tone matching

Load an **isolated guitar** target, then click **CAPTURE DI** and play representative material for three seconds. Enable TONE MATCH and adjust Amount/Smooth. Without a loaded reference the target is a flat ten-band spectrum. This matches broad input spectral balance; it does not learn an amplifier's nonlinear transfer function or reproduce a mixed master recording. A silent capture is ignored.

### Pitch and routing

The pitch engine processes chords using overlapping delay grains. Extreme shifts and complex chords can produce modulation, transient smear and grain coloration. It is not claimed to match the proprietary source plugin's polyphonic pitch quality. Cleanse suppresses Thicken whenever Whammy is away from zero. Thicken at 100% removes the unshifted main voice (Dissonance remains independently adjustable). Parallel gives the octave a separate distortion path.

### Presets and portability

Save/Open writes all parameters, the captured curve, target spectrum and custom IR into one file. DAW state uses the same representation. The custom IR therefore travels with the session and does not depend on the original path. Artist presets and original cabinet samples from the reference plugin are not included; factory sounds here are original designs with no artist endorsement.

## Build locally

Requirements: CMake 3.22+, Git, Visual Studio 2019 or newer with Desktop development with C++ and a Windows SDK. Configure needs internet access for the pinned JUCE revision.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Outputs:

* `build/SishhinAmp_artefacts/Release/VST3/SISHHIN AMP.vst3`
* `build/SishhinAmp_artefacts/Release/Standalone/SISHHIN AMP.exe`

The GitHub Actions workflow performs the same build, runs offline DSP regressions, renders the real editor, and uploads an installable artifact. It runs on main pushes, pull requests, version tags and manual dispatch. It does not require signing secrets or a paid JUCE account.

## Verification and limitations

Offline checks cover finite/bounded and non-silent audio across six presets, mono/stereo at 44.1/48/96 kHz, oversized host blocks, bypass, tuner mute, note detection, state recall, tone lock, tone capture, auto gain, embedded stereo IR recall after the source file is removed, sample-rate changes and octave pitch frequency. These checks do not constitute listening tests or validation in every DAW. Test the plugin in your preferred DAW before relying on it in a live or paid session.

Windows VST3 and standalone are the supported deliverables. AU, AAX, macOS binaries and original artist preset packs are not supplied. Stock IRs are synthetic resonant filters, not recovered recordings of physical cabinets. No binary patching, activation bypass, proprietary DSP extraction or copying of the supplied plugin's embedded assets is performed.

## Source licence

SISHHIN AMP is distributed under **AGPL-3.0-or-later**, with dependency notices in THIRD_PARTY.md and LICENSE. This is an open-source distribution licence, not a user activation system. The source is available at https://github.com/amsiraj1-debug/SISHHINAMP.
