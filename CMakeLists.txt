cmake_minimum_required(VERSION 3.22)
project(SishhinAmp VERSION 1.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 51a8a6d7aeae7326956d747737ccf1575e61e209 GIT_SUBMODULES "")
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(SishhinAmp
    COMPANY_NAME "SISHHIN Audio" BUNDLE_ID "audio.sishhin.amp"
    PLUGIN_MANUFACTURER_CODE Sshn PLUGIN_CODE ShA1
    FORMATS VST3 Standalone PRODUCT_NAME "SISHHIN AMP"
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE VST3_CATEGORIES Fx Distortion)
target_sources(SishhinAmp PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(SishhinAmp PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
if(MSVC)
    target_compile_options(SishhinAmp PUBLIC /utf-8)
endif()
target_link_libraries(SishhinAmp PRIVATE juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags)
option(SISHHIN_TESTS "Build offline DSP and editor checks" ON)
if(SISHHIN_TESTS)
    enable_testing()
    juce_add_console_app(SishhinTests PRODUCT_NAME "SISHHIN Tests")
    target_sources(SishhinTests PRIVATE Tests/Smoke.cpp)
    target_include_directories(SishhinTests PRIVATE Source)
    target_link_libraries(SishhinTests PRIVATE SishhinAmp juce::juce_audio_utils juce::juce_dsp)
    add_test(NAME AudioSmoke COMMAND SishhinTests)
endif()
