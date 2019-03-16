cmake_minimum_required(VERSION 3.13)

project(raster VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

# general configs
# --------------------------------------- #
# set(YAML_CPP_BUILD_TESTS CACHE BOOL OFF)

# dependencies
# --------------------------------------- #
FetchContent_Declare(yamlcpp
    GIT_REPOSITORY  https://github.com/jbeder/yaml-cpp
    GIT_TAG         562aefc114938e388457e6a531ed7b54d9dc1b62 # 0.6.2
)
FetchContent_GetProperties(yamlcpp)
if (NOT yamlcpp_POPULATED)
    FetchContent_Populate(yamlcpp)
    add_subdirectory(${yamlcpp_SOURCE_DIR} ${yamlcpp_BINARY_DIR})
endif()

#FetchContent_Declare(googletest
#    GIT_REPOSITORY  https://github.com/google/googletest.git
#    GIT_TAG         2fe3bd994b3189899d93f1d5a881e725e046fdc2 # 1.8.1
#)
#FetchContent_GetProperties(googletest)
#if (NOT googletest_POPULATED)
#    FetchContent_Populate(googletest)
#    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
#endif()

# use c++17
# --------------------------------------- #
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic)
add_definitions(-D_LIBCPP_DISABLE_AVAILABILITY)

# executables
# --------------------------------------- #
file(GLOB cobra_src
    "src/*.cpp"
)
add_executable(cobra ${cobra_src})

target_include_directories(cobra PRIVATE include/)
target_include_directories(cobra PRIVATE external/glm)
target_include_directories(cobra PRIVATE external/stb)

target_include_directories(cobra PUBLIC ${yamlcpp_SOURCE_DIR}/include)
target_link_libraries(cobra PRIVATE yaml-cpp)

# test definitions
# use googletest framework
# --------------------------------------- #
#include(GoogleTest)
#
#set(test_sources ${test_sources}
#    tests/main.cpp
#    tests/yamlsd_reader_tests.cpp
#)

#add_executable(maintest ${test_sources})
#target_include_directories(maintest PRIVATE include)
#target_link_libraries(maintest PRIVATE gtest)

#gtest_discover_tests(maintest)
#
#enable_testing()
