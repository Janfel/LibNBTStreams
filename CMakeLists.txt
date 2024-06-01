cmake_minimum_required(VERSION 3.26)
project(
    NBTStreams
    VERSION 0.1.0
    DESCRIPTION "Modular Zero-Allocation Streaming NBT Parser for C"
    LANGUAGES C
)

include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_C_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(NBTStreams_BUILD_EXECUTABLES "Build executable binaries" ${PROJECT_IS_TOP_LEVEL})
option(NBTStreams_BUILD_WITH_SANITIZERS "Build with sanitizers" OFF)
cmake_dependent_option(NBTStreams_BUILD_WITH_LIBFUZZER "Build fuzz test binaries" OFF [[CMAKE_C_COMPILER_ID STREQUAL "Clang"]] OFF)

add_library(NBTStreams_Options INTERFACE)
if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    target_compile_options(NBTStreams_Options INTERFACE -Wall -Wextra -Werror=switch -fno-semantic-interposition)
    if(NBTStreams_BUILD_WITH_SANITIZERS)
        target_compile_options(NBTStreams_Options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(NBTStreams_Options INTERFACE -fsanitize=address,undefined)
    endif()
    if(NBTStreams_BUILD_WITH_LIBFUZZER)
        target_compile_definitions(NBTStreams_Options INTERFACE FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
        target_compile_options(NBTStreams_Options INTERFACE -fsanitize=fuzzer-no-link)
    endif()
elseif(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(NBTStreams_Options INTERFACE /W4 /utf-8)
    if(NBTStreams_BUILD_WITH_SANITIZERS)
        target_compile_options(NBTStreams_Options INTERFACE /fsanitize=address)
        target_link_options(NBTStreams_Options INTERFACE /fsanitize=address)
    endif()
    if(NBTStreams_BUILD_WITH_LIBFUZZER)
        target_compile_definitions(NBTStreams_Options INTERFACE FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
        target_compile_options(NBTStreams_Options INTERFACE /fsanitize=fuzzer-no-link)
    endif()
else()
    message(WARNING "This compiler frontend is not supported")
    if(NBTStreams_BUILD_WITH_SANITIZERS)
        message(SEND_ERROR "Cannot enable sanitizers for this compiler frontend")
    endif()
    if(NBTStreams_BUILD_WITH_LIBFUZZER)
        message(SEND_ERROR "Cannot enable libfuzzer for this compiler frontend")
    endif()
endif()

add_library(NBTStreams)
add_library(NBTStreams::NBTStreams ALIAS NBTStreams)
target_sources(NBTStreams PRIVATE nbts/nbts.c nbts/print.c)
target_sources(NBTStreams PUBLIC FILE_SET HEADERS FILES nbts/nbts.h nbts/print.h)
target_compile_features(NBTStreams PUBLIC c_std_23)
target_link_libraries(NBTStreams PRIVATE $<BUILD_LOCAL_INTERFACE:NBTStreams_Options>)
set_target_properties(NBTStreams PROPERTIES
    OUTPUT_NAME "nbts" C_EXTENSIONS ON
    # VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION_MAJOR}"
)

install(TARGETS NBTStreams EXPORT NBTStreamsTargets FILE_SET HEADERS)
install(EXPORT NBTStreamsTargets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NBTStreams" NAMESPACE NBTStreams::)
export(EXPORT NBTStreamsTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/NBTStreamsTargets.cmake" NAMESPACE NBTStreams::)

# configure_package_config_file(Config.cmake.in
#     "${CMAKE_CURRENT_BINARY_DIR}/cmake/NBTStreamsConfig.cmake"
#     INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NBTStreams"
# )
# write_basic_package_version_file(
#     "${CMAKE_CURRENT_BINARY_DIR}/cmake/NBTStreamsConfigVersion.cmake"
#     VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMajorVersion
# )
# install(FILES
#     "${CMAKE_CURRENT_BINARY_DIR}/cmake/NBTStreamsConfig.cmake"
#     "${CMAKE_CURRENT_BINARY_DIR}/cmake/NBTStreamsConfigVersion.cmake"
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/NBTStreams"
# )

if(NBTStreams_BUILD_EXECUTABLES)
    add_executable(nbts_print nbts/print.main.c)
    target_link_libraries(nbts_print PRIVATE NBTStreams NBTStreams_Options)

    if(NBTStreams_BUILD_WITH_LIBFUZZER)
        add_library(NBTStreams_Fuzzer INTERFACE)
        if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
            target_link_options(NBTStreams_Fuzzer INTERFACE -fsanitize=fuzzer)
        elseif(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
            target_link_options(NBTStreams_Fuzzer INTERFACE /fsanitize=fuzzer)
        else()
            message(SEND_ERROR "Cannot enable libfuzzer for this compiler frontend")
        endif()
    
        add_executable(nbts_fuzz_print tests/print.fuzz.c)
        target_link_libraries(nbts_fuzz_print PRIVATE NBTStreams NBTStreams_Options NBTStreams_Fuzzer)
        set_target_properties(nbts_fuzz_print PROPERTIES C_EXTENSIONS ON)
    endif()
endif()
