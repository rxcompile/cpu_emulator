cmake_minimum_required(VERSION 3.10)
project(calculator VERSION 1.0.0)

add_library(calculator
    include/calculator/cpu.hpp
    cpu.cpp
)

target_include_directories(calculator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(calculator PUBLIC cxx_std_17)

add_library(RxCompile::calculator ALIAS calculator)

install(
    TARGETS calculator EXPORT calculator_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    EXPORT calculator_targets
    DESTINATION cmake
    FILE calculator_targets.cmake
    NAMESPACE RxCompile::
)

install(
    DIRECTORY include/calculator
    DESTINATION include
)
