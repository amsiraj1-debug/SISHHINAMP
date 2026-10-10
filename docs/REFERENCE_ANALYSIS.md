# Supplied reference analysis

Input: Windows x64 `Slam Amp.vst3`, 62,319,616 bytes.
SHA-256: `C9E87312DA6A44E205C9EBEB43A5C727AD461221552F5BF514517F0A3E0E3884`.

Read-only static inspection found a VST3 `GetPluginFactory` export string, JUCE 7 version text, and readable parameter/tooltip strings consistent with the attached screenshot and feature inventory. This is metadata inspection, not recovery of original source code or an audio transfer-function measurement.

Observations used to refine behavior:

* Whammy has continuous pitch adjustment.
* Cleanse disables the thicken layer while Whammy is away from zero.
* Thicken offers one/two octaves below, high-cut filtering and wet/dry mixing.
* Parallel means a separate amplifier path for the octave voice.
* Dissonance adds a semitone-up voice.
* Tone lock protects tone-match and input settings during preset selection.
* The source capture description specifies five seconds; the new implementation uses a documented three-second capture.

The screenshot establishes panel arrangement and relative hierarchy. New procedural metal/grunge artwork follows that arrangement without copying the source plugin's embedded art, logo or signature.

Not recovered: proprietary amp transfer functions, exact pitch algorithm, cabinet recordings, factory/artist preset values, full tone-matching algorithm. The implementation substitutes documented original DSP. The reference plugin was not modified.
