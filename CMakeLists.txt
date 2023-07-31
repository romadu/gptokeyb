# NOTE:  If you installed SDL2 at an unusual place and FindSDL2 fails,
#        you can define SDL2_INCLUDE_DIR on the cmdline.  For example:
#        cmake -DSDL2_INCLUDE_DIR=/opt/SDL2/include/SDL2 [other stuff]

cmake_minimum_required(VERSION 3.0.0)
project(gptokeyb
        VERSION 0.1.0
        LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake/modules"
)

find_package(LIBEVDEV REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
  ${LIBEVDEV_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  )


set(EXTRA_CXXFLAGS )
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} -Wall")
endif()

add_executable(gptokeyb
    src/analog.cpp
    src/config.cpp
    src/input.cpp
    src/xbox360.cpp
    src/keyboard.cpp
    src/util.cpp
    src/gptokeyb.cpp
    )

target_link_libraries(gptokeyb
    ${SDL2_LIBRARIES}
    ${LIBEVDEV_LIBRARIES}
    )
