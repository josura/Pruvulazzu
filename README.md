# Aeolus
Granular sampler plugin implemented in C++ and Juce. Support both LV2 and VST. Under production

## Project Structure
Aeolus/
├── CMakeLists.txt        # Main build script
├── src/               # All C++ code
│   ├── PluginProcessor.cpp
│   ├── PluginProcessor.h
│   ├── PluginEditor.cpp
│   ├── PluginEditor.h
│   └── DSP/              # Grain logic, buffers, etc.
├── tests/                # Unit tests (Catch2 or GTest)
├── modules/              # Git submodules (like JUCE)
└── resources/            # Icons, fonts, and assets