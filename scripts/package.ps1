param([string]$BuildDir = 'build', [string]$OutputDir = 'dist')
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force $OutputDir | Out-Null
Copy-Item "$BuildDir/SishhinAmp_artefacts/Release/VST3/SISHHIN AMP.vst3" $OutputDir -Recurse -Force
Copy-Item "$BuildDir/SishhinAmp_artefacts/Release/Standalone/SISHHIN AMP.exe" $OutputDir -Force
Copy-Item README.md,LICENSE,THIRD_PARTY.md,SISHHIN-*.png $OutputDir -Force
$notices = Join-Path $OutputDir 'licenses'
New-Item -ItemType Directory -Force $notices | Out-Null
$juce = "$BuildDir/_deps/juce-src"
$files = @{
    'JUCE.md'='LICENSE.md'
    'VST3.txt'='modules/juce_audio_processors/format_types/VST3_SDK/LICENSE.txt'
    'FLAC.txt'='modules/juce_audio_formats/codecs/flac/Flac Licence.txt'
    'Ogg-Vorbis.txt'='modules/juce_audio_formats/codecs/oggvorbis/Ogg Vorbis Licence.txt'
    'JPEG.txt'='modules/juce_graphics/image_formats/jpglib/README'
    'PNG.txt'='modules/juce_graphics/image_formats/pnglib/LICENSE'
    'ZLIB.txt'='modules/juce_core/zip/zlib/README'
    'HarfBuzz.txt'='modules/juce_graphics/fonts/harfbuzz/COPYING'
    'SheenBidi.txt'='modules/juce_graphics/unicode/sheenbidi/LICENSE'
}
foreach ($entry in $files.GetEnumerator()) { Copy-Item (Join-Path $juce $entry.Value) (Join-Path $notices $entry.Key) -Force }
$root = (Resolve-Path $OutputDir).Path
Get-ChildItem $root -Recurse -File | Where-Object Name -ne 'SHA256SUMS.txt' | Get-FileHash -Algorithm SHA256 | ForEach-Object { "$($_.Hash)  $([IO.Path]::GetRelativePath($root, $_.Path))" } | Set-Content (Join-Path $root 'SHA256SUMS.txt')
