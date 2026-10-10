# SISHHIN AMP 1.1 validation

Windows x64 Release build, tested on 10 October 2026.

* Local MSVC 19.29 / Windows SDK 10.0.19041: VST3 and standalone compiled successfully.
* CTest AudioSmoke: passed. Tests all **28 presets** in mono/stereo at **44.1, 48 and 96 kHz**, oversized host blocks, non-silent/finite/bounded audio, bypass, tuner and mute, state recall, tone lock, spectral capture, calibration, embedded stereo IR recall after removing the source file, octave pitch frequency, and audible delay/reverb tails after input ends.
* Tracktion pluginval 1.0.4, strictness **5**, GUI tests skipped: **SUCCESS**, exit code **0**. The VST3 bundle was loaded and tested for programs, audio processing, state, automation and bus layouts.
* All four editor pages rendered from the actual JUCE components and visually inspected at 1200 × 830. Controls, labels, routing pages and the ambience rack are visible without overlap.
* Current source and product documentation checked for unrelated product branding.

Limitations: no DAW-specific interactive or listening session was performed. Automated tests confirm implemented behavior and host API checks; they do not establish subjective sound quality or compatibility with every host. Pitch-shifting artifacts and the synthetic nature of the stock cabinet FIRs are documented in README.md.

Reproduce:

```powershell
ctest --test-dir build -C Release --output-on-failure
./build/SishhinTests_artefacts/Release/SishhinTests.exe --render
pluginval --strictness-level 5 --skip-gui-tests --validate "full/path/to/SISHHIN AMP.vst3"
```
