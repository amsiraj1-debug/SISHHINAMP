# SISHHIN AMP / Thall Engine 1.1

A free, open-source Windows x64 **VST3 and standalone** guitar amplifier for extended-range rhythm, thall, dissonant leads and atmospheric sound design. No accounts, activation, trial timer, keys or online checks.

## Interface

An original dark interface with mint and lime accents, a fractured-obelisk monogram, a large dynamic amplifier panel and a dedicated ambience rack. The four lower tabs provide **Character**, **Pitch Matrix**, **Spectral Match** and **Cabinet** controls. Input/output meters, calibration, tuner, preset navigation and the amp remain accessible throughout.

## Download and install

Open [Actions](https://github.com/amsiraj1-debug/SISHHINAMP/actions) → **Build Windows VST3** → latest successful run → **Artifacts** → **SISHHIN-AMP-Windows-x64**. GitHub may require sign-in for artifact downloads.

Extract the ZIP and copy the **entire `SISHHIN AMP.vst3` folder**, including Contents, into `C:\Program Files\Common Files\VST3\`. Rescan plugins in your DAW. Do not copy just the inner binary. Run `SISHHIN AMP.exe` for standalone use, then select your audio device and guitar input. Start with the interface output low.

## Tone and effects

* **Dynamic amplifier:** 4× oversampled multistage saturation, Drive, Low, Mid, High and Presence. Low End Focus controls bass entering the distortion; Pick Attack uses fast/slow envelopes to emphasize transients; Power Sag creates level-dependent compression.
* **Four character voices:** RAZOR focused drive, MIRE dense fuzz, RELIC gated octave fracture and MAUL asymmetric weight. Distortion, Tone, Colour and makeup gain shape each voice.
* **Pitch Matrix:** continuous −24 to +12 semitone Whammy, a +1 semitone Dissonance voice, one/two-octave Sub Weight, sub filtering and an optional independent sub amplifier. Cleanse removes the sub layer during whammy moves.
* **Spectral Match:** import an isolated reference WAV/AIFF, then capture three seconds of live input. Ten logarithmic bands provide up to ±12 dB of correction, with Amount and Smooth. A lock protects matching and input trim across factory preset changes.
* **Rhythmic stutter:** eight straight, dotted and triplet subdivisions, host tempo/transport synchronization, Depth and Short Gate. A fallback BPM works when no host tempo is available.
* **Cabinet:** four original synthetic FIR voices—Obsidian, Iron, Ruin and Monolith—plus custom mono/stereo WAV or AIFF IRs up to two seconds. Imported IR audio is embedded in presets and sessions.
* **Space rack:** tempo delay with five subdivisions and filtered cross-feedback; diffuse stereo reverb with Mix and Size. Ducking suppresses ambience under attacks, while Tone filters the wet effects. Stutter happens before ambience so tails can continue naturally.
* **Utility:** input calibration to −18 dBFS RMS, adjustable gate, input/output trim and peak meters, mono/stereo, low/high cuts, lo-fi, tuner with note/cents/Hz and moving strobe strip, optional tuner mute and global bypass.
* **Recall:** 28 host-visible factory programs, save/open `.sishhin` files, embedded match/IR data and host automation for all sound controls.

### Recommended starting points

Start with **Obsidian**, **Razor Grid** or **Eight String Focus** for articulation. Use **Null Cathedral**, **Monolith** or **Subzero** for low, heavy thall. **Event Horizon**, **Grey Bloom** and **Slow Collapse** explore longer delay/reverb fields. Increase Ducking when ambience obscures your palm mutes.

Low End Focus removes bass **before** distortion; Low Cut filters the sound **after** the cabinet. They have different effects on the tone. Keep Power Sag low for rigid rhythms and raise it for softer, blooming sustain.

### 28 original factory presets

| Category | Presets |
| --- | --- |
| Djent | Obsidian · Razor Grid · Glass Teeth · Polyrhythm · Carbon Crunch · Eight String Focus |
| Thall | Null Cathedral · Abyssal · Cold Sun · Monolith · Dislocated · Subzero |
| Space | Event Horizon · Grey Bloom · Halo · Slow Collapse · Arctic Memory · Night Glass |
| Lead | Signal Fire · Liquid Mercury · Screaming Wire · Pale Orbit |
| Fuzz | Mire · Ruin · Broken Relic · Rust Ritual |
| FX | Fracture · Blackout |

These are distinct parameter configurations, including different character voices, pitch routing, input dynamics, cabinet choices and ambience. The arrow buttons step through presets; the DAW also sees the 28 programs. Individual configurations remain fully editable.

### Spectral matching

Load an isolated guitar reference, click **CAPTURE INPUT**, and play representative material for three seconds. Adjust Match Amount and Spectral Smooth. Without an imported reference the target is a flat ten-band spectrum. Silent captures are ignored. This matches broad spectral balance, not nonlinear amplifier behavior or a full mixed song.

### Pitch, latency and tails

Pitch processing uses overlapping delay grains and can process chords, but extreme shifts may smear attacks or introduce modulation. Oversampling latency is reported to the host. Pitch time smear varies with the shift and is not a fixed delay suitable for host compensation. Host tail length is conservatively reported as 60 seconds for high-feedback delay and large reverb settings.

### Preset portability

Save/Open stores parameters, the captured curve, target spectrum and custom IR audio together. Host session recall uses the same representation, so moving the original IR file does not break the preset. Double-click a control to reset it; click its numeric value to type directly. Hover for help. A lit BYPASS means the entire effect is bypassed.

## Build

Requirements: CMake 3.22+, Git and Visual Studio 2019 or newer with C++ desktop tools and a Windows SDK. Initial configuration fetches a pinned JUCE revision.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Outputs:

* `build/SishhinAmp_artefacts/Release/VST3/SISHHIN AMP.vst3`
* `build/SishhinAmp_artefacts/Release/Standalone/SISHHIN AMP.exe`

The GitHub Actions workflow builds both formats, runs the DSP regression suite, renders all four editor pages and uploads the installable Windows artifact. It runs on main pushes, pull requests, version tags and manual dispatch. No signing secrets are required.

## Validation and scope

The offline suite checks all 28 factory presets in mono/stereo at 44.1/48/96 kHz, finite and non-silent output, oversized blocks, bypass, tuner, state, tone lock, capture, calibration, embedded stereo IR recall, pitch frequency and delay/reverb tails. The plugin is also checked with pluginval level 5. These automated checks are not a substitute for listening and testing in your preferred DAW.

Windows VST3 and standalone are the supported deliverables. Cabinet defaults are original synthetic resonant FIRs. AU, AAX and macOS binaries are not included.

## Source licence

AGPL-3.0-or-later, with dependency notices in LICENSE and THIRD_PARTY.md. This is an open-source distribution licence, not an activation system. Source: [amsiraj1-debug/SISHHINAMP](https://github.com/amsiraj1-debug/SISHHINAMP).
