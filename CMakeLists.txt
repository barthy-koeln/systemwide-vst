cmake_minimum_required(VERSION 3.15)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

project(SYSTEMWIDE_IO VERSION 0.0.1)

add_subdirectory("/home/barthy/JUCE" JUCE)

find_package(PkgConfig REQUIRED)
pkg_search_module(ALSA REQUIRED alsa)
pkg_search_module(JACK REQUIRED jacl)
pkg_search_module(APPINDICATOR REQUIRED appindicator3-0.1)

add_definitions(
        -DDONT_SET_USING_JUCE_NAMESPACE=1
        -DJUCE_ALSA=0
        -DJUCE_JACK=1
        -DJUCE_PLUGINHOST_VST3=1
        -DJUCE_PLUGINHOST_LADSPA=1
        -DJUCE_CHECK_MEMORY_LEAKS=1
        -DJUCE_INCLUDE_ZLIB_CODE=1
        -DJUCE_USE_COREIMAGE_LOADER=1
        -DJUCE_USE_DIRECTWRITE=1
        -DJUCE_STANDALONE_APPLICATION=1
        -DDONT_SET_USING_JUCE_NAMESPACE=1
)

add_definitions(${APPINDICATOR_CFLAGS})
link_directories(${APPINDICATOR_LIBRARY_DIRS})

juce_add_gui_app(SystemwideIO
        # VERSION ...                       # Set this if the app version is different to the project version
        ICON_BIG ./assets/images/logo.svg   # ICON_* arguments specify a path to an image file to use as an icon
        ICON_SMALL ./assets/images/logo.svg
        COMPANY_NAME barthy.koeln           # Specify the name of the app's author
        PRODUCT_NAME "Systemwide IO")     # The name of the final executable, which can differ from the target name

set_target_properties(SystemwideIO PROPERTIES
        OUTPUT_NAME "SystemwideIO"
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        )

juce_generate_juce_header(SystemwideIO)

target_sources(SystemwideIO
        PRIVATE
        ./src/CommandRunner.h
        ./src/ConfigurationComponent.cpp
        ./src/ConfigurationComponent.h
        ./src/ConfigurationWindow.h
        ./src/CustomLookAndFeel.h
        ./src/Main.cpp
        ./src/MessagesConstants.h
        ./src/PassthroughProcessor.h
        ./src/PluginSelectionTableModel.h
        ./src/PluginWindow.h
        ./src/SettingsConstants.h
        ./src/SystemwideIOProcess.h
        ./src/TrayIconThread.h)

target_compile_definitions(SystemwideIO
        PRIVATE
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:SystemwideIO,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:SystemwideIO,JUCE_VERSION>"
        )

juce_add_binary_data(BinaryData SOURCES ./assets/images/logo.svg)


include_directories(
        ~/JUCE/modules/juce_audio_processors/format_types/VST3_SDK
        ../tray/
        ${ALSA_INCLUDE_DIRS}
        ${APPINDICATOR3-0.1_INCLUDE_DIRS}
)

target_link_libraries(SystemwideIO
        PRIVATE
        BinaryData
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        ${ALSA_LIBRARIES}
        ${APPINDICATOR_LIBRARIES}
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

if (CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(SystemwideIO PRIVATE
            NDEBUG=1
            )

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -pthread -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} ")
endif (CMAKE_BUILD_TYPE STREQUAL Release)