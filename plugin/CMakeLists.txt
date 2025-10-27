cmake_minimum_required(VERSION 4.0)
project(Simple_Delay VERSION 1.00)

# Adding a directory with the library/ application name as a subfolder
# of the include folder is good practice to help avoid name clashes later on
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/DelayPlugin")

# Adds a plugin target
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME abc
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    FORMATS VST3 Standalone
    VERSION 0.1.0
    COPY_PLUGIN_AFTER_BUILD TRUE # this will install the plugin into system VST folder
    PRODUCT_NAME "Simple Delay"
    PLUGIN_MANUFACTURER_CODE Mcmp
    PLUGIN_CODE Expl
)

# convert images into binary data
# this will auto generate the binarydata.h juce header file in the build folder
juce_add_binary_data(DelayBinaryData
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/assets/background.jpg
)

# set source files of plugin project
set(SOURCE_FILES src/PluginEditor.cpp src/PluginProcessor.cpp)
# optional; includes header files in project files tree in Visual studio
set(HEADER_FILES ${INCLUDE_DIR}/PluginEditor.h ${INCLUDE_DIR}/PluginProcessor.h ${INCLUDE_DIR}/CircularBuffer.h ${INCLUDE_DIR}/LowPass1P.h)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})

# set include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Links to all necessary dependencies
# If you use one of the additional modules, like the DSP module, it needs to be specified here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        DelayBinaryData
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


# In visual studio this command provides a nice grouping of source files in "filters"
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
