cmake_minimum_required(VERSION 4.0)
project(JUCE_Audio_Player)

# Adding a directory with the library/ application name as a subfolder
# of the include folder is good practice to help avoid name clashes later on
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/DelayPlugin")

# Adds a plugin target
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME abc
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3
    VERSION 0.0.1
    PRODUCT_NAME "Simple Delay"
)

# set source files of plugin project
set(SOURCE_FILES src/PluginEditor.cpp src/PluginProcessor.cpp)
# optional; includes header files in project files tree in Visual studio
set(HEADER_FILES ${INCLUDE_DIR}/PluginEditor.h ${INCLUDE_DIR}/PluginProcessor.h)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# set include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Links to all necessary dependencies
# If you use one of the additional modules, like the DSP module, it needs to be specified here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Enables strict c++ warnings and treats warning as errors.
# This needs to be set up only for your projects, not 3rd party.
set_source_files_properties(${SOURCE_FILE} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

# In visual studio this command provides a nice grouping of source files in "filters"
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)