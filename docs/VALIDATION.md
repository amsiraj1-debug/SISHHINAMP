# Validation record

Windows x64 Release build, tested on 10 October 2026.

* Local MSVC 19.29 / Windows SDK 10.0.19041 build: VST3 and standalone compiled successfully.
* CTest AudioSmoke: passed. Includes six factory presets, mono/stereo, 44.1/48/96 kHz, oversized blocks, finite/bounded audio, bypass, tuner note and mute, state, tone lock, tone capture, calibration, embedded stereo IR recall after deleting its source and octave pitch frequency.
* Tracktion pluginval 1.0.4, strictness level 5, `--skip-gui-tests`: **SUCCESS**, exit code 0. The actual VST3 bundle was loaded and tested. Log includes parameter information, programs, state restoration, audio processing, automation, and bus-layout checks.
* Actual editor rendered via JUCE `createComponentSnapshot` and visually inspected. Automated GUI interaction and DAW-specific testing have not been performed.
* No listening comparison or null test against the supplied Slam Amp has been performed. The project does not claim sonic equivalence.

Reproduce offline regressions with `ctest --test-dir build -C Release --output-on-failure`. Render the editor with `build/SishhinTests_artefacts/Release/SishhinTests.exe --render`. For optional host validation, obtain pluginval from https://github.com/Tracktion/pluginval/releases/tag/v1.0.4 and run it with `--strictness-level 5 --skip-gui-tests --validate` followed by the full path to the built `.vst3` bundle.
