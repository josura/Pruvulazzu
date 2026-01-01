cmake_minimum_required(VERSION 3.24)

# Project metadata
project(Aeolus VERSION 0.1.0 LANGUAGES C CXX)

# Enable C++23 for latest features (std::expected, std::span improvements)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/develop # Or a specific stable version
)
FetchContent_MakeAvailable(juce)

# 2. Create the Plugin
juce_add_plugin(Aeolus
    COMPANY_NAME "josura"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    # Formats: VST3 and LV2 (Standard in JUCE 7+)
    FORMATS VST3 LV2 
    PRODUCT_NAME "Aeolus"
)

# 3. Source Files
target_sources(Aeolus PRIVATE
    src/PluginProcessor.cxx
    src/PluginEditor.cxx
)

# 4. Links and Dependencies
target_link_libraries(Aeolus PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_dsp
)

# 5. Optimization Flags for High Performance
if(MSVC)
    target_compile_options(Aeolus PRIVATE /O2 /arch:AVX2)
else()
    target_compile_options(Aeolus PRIVATE -O3 -march=native -ffast-math)
endif()