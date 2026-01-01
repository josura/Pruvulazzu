#include <catch2/catch_test_macros.hpp>
#include "../src/DSP/GrainEngine.h"

TEST_CASE("Grain Engine Initialization", "[dsp]") {
    GrainEngine engine;
    engine.prepare(44100.0, 128); // Sample rate, max grains
    
    SECTION("Initial state is silent") {
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        engine.process(buffer);
        
        REQUIRE(buffer.getMagnitude(0, 512) == 0.0f);
    }
}