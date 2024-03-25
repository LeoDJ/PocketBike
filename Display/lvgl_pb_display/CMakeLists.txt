cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(PocketBike-Display LANGUAGES C CXX)

# add_compile_options(-O0 -g3)

# Gather all source files from src directory
file(GLOB_RECURSE sources CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.c"
)

# Add them to the main executable
add_executable(pb_display ${sources})

# Specify path to own LVGL config header
set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
    CACHE STRING "" FORCE)

# Fetch LVGL from GitHub
FetchContent_Declare(lvgl        GIT_REPOSITORY https://github.com/lvgl/lvgl       GIT_TAG release/v8.2)
FetchContent_Declare(lv_drivers  GIT_REPOSITORY https://github.com/lvgl/lv_drivers GIT_TAG release/v8.2)
FetchContent_MakeAvailable(lvgl lv_drivers)

# The target "MyFirmware" depends on LVGL
target_link_libraries(pb_display  PRIVATE lvgl::lvgl lvgl::demos lv_drivers pthread)