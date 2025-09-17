cmake_minimum_required(VERSION 4.0)
project(JUCE_Audio_Player)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME abc
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3
    PRODUCT_NAME "Simple Delay"
)

add_subdirectory(plugin)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/main.cpp
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)