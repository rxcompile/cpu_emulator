cmake_minimum_required(VERSION 3.10)
project(CppPlayground VERSION 1.0.0)

add_library(CppPlayground
    include/cpp_playground/cpu.hpp
    cpu.cpp
)

target_include_directories(CppPlayground PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(CppPlayground PUBLIC cxx_std_17)

add_library(RxCompile::CppPlayground ALIAS CppPlayground)

install(
    TARGETS CppPlayground EXPORT CppPlaygroundTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    EXPORT CppPlaygroundTargets
    DESTINATION lib/cmake/CppPlayground
    FILE CppPlaygroundTargets.cmake
    NAMESPACE RxCompile::
)

install(
    DIRECTORY include/cpp_playground
    DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(CppPlaygroundConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)
install(FILES CppPlaygroundConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/CppPlaygroundConfigVersion.cmake
    DESTINATION lib/cmake/CppPlayground)