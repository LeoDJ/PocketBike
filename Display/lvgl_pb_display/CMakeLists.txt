cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(PocketBike-Display LANGUAGES C CXX)


# Gather all source files from src directory
file(GLOB_RECURSE sources CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.c"
)

add_executable(pb_display ${sources})
set_property(TARGET pb_display PROPERTY CXX_STANDARD 17)

# add_compile_options(-O0 -g3)

if(DO_PROFILE)
    add_compile_options(-pg -g)
    add_link_options(-pg)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

if(BUILD_SIM) 
    # Find SDL2 package
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)

    # Add SDL2 include directories and link flags
    target_include_directories(pb_display PRIVATE ${SDL2_INCLUDE_DIRS})
    target_compile_options(pb_display PRIVATE ${SDL2_CFLAGS_OTHER})
    add_definitions(-DUSE_SDL=1)

    #target_link_libraries(pb_display PRIVATE ${SDL2_LIBRARIES}) # has to be done after lv_drivers?
endif()

# Specify path to own LVGL config header
set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
    CACHE STRING "" FORCE)

# Fetch LVGL from GitHub
FetchContent_Declare(lvgl        GIT_REPOSITORY https://github.com/lvgl/lvgl       GIT_TAG release/v8.2)
FetchContent_Declare(lv_drivers  GIT_REPOSITORY https://github.com/lvgl/lv_drivers GIT_TAG release/v8.2)
FetchContent_MakeAvailable(lvgl lv_drivers)


# Fetch ArduinoJson
FetchContent_Declare(ArduinoJson
    GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
    GIT_TAG        v7.0.4
)
FetchContent_MakeAvailable(ArduinoJson)


# Fetch Paho MQTT C++
# apparently needs to be set by FORCE, for whatever reason
set(PAHO_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(PAHO_WITH_SSL OFF CACHE BOOL "" FORCE)      
set(PAHO_WITH_MQTT_C ON CACHE BOOL "" FORCE)        # Instruct paho.mqtt.cpp to build paho.mqtt.c from internal submodule
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# for some reason CMake is too stupid to find this, even if directly pointed to by FindPahoMqttC.cmake:28. I give up.
# (otherwise it errors with "fatal error: mqtt/async_client.h: No such file or directory", sometimes building twice is needed)
set(PAHO_MQTT_C_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/paho_mqtt_cpp-src/src/externals/paho-mqtt-c/src 
    CACHE STRING "" FORCE)

FetchContent_Declare(
    paho_mqtt_cpp
    # GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
    # GIT_TAG        v1.5.3  
    # GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
    # GIT_TAG        v1.3.2   # 1.3.2 has problems when building the Paho C library statically (the newest master too :/)
    GIT_REPOSITORY https://github.com/LeoDJ/paho.mqtt.cpp.git
    GIT_TAG        f114ea8   # modified v1.5.3 with added get_buffered_messages_num()
)
FetchContent_MakeAvailable(paho_mqtt_cpp)


target_link_libraries(pb_display PRIVATE lvgl::lvgl lvgl::demos lv_drivers pthread ArduinoJson paho-mqttpp3-static)

if(BUILD_SIM) 
    target_link_libraries(pb_display PRIVATE ${SDL2_LIBRARIES})
endif()