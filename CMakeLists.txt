cmake_minimum_required(VERSION 3.8)

set(WARNINGS_AS_ERRORS_FOR_AUDIO OFF CACHE BOOL "ON iff you want to treat warnings as errors")

add_library(Audio)
add_library(Cool::Audio ALIAS Audio)
target_compile_features(Audio PUBLIC cxx_std_20)

# ---Add source files---
if(WARNINGS_AS_ERRORS_FOR_AUDIO)
    target_include_directories(Audio PUBLIC include)
else()
    target_include_directories(Audio SYSTEM PUBLIC include)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
target_sources(Audio PRIVATE ${SRC_FILES})

# ---Set warning level---
if(MSVC)
    target_compile_options(Audio PRIVATE /W4)
else()
    target_compile_options(Audio PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion -Wimplicit-fallthrough)
endif()

# ---Maybe enable warnings as errors---
if(WARNINGS_AS_ERRORS_FOR_AUDIO)
    if(MSVC)
        target_compile_options(Audio PRIVATE /WX)
    else()
        target_compile_options(Audio PRIVATE -Werror)
    endif()
endif()

# ---Add libnyquist---
set(LIBNYQUIST_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/libnyquist)
target_link_libraries(Audio PRIVATE libnyquist::libnyquist)
install(FILES "lib/libnyquist/LICENSE" DESTINATION "license/libnyquist")

# ---Add dj_fft---
target_include_directories(Audio SYSTEM PRIVATE lib/dj_fft)
install(FILES "lib/dj_fft/LICENSE.txt" DESTINATION "license/dj_fft")

# ---Add RtAudioWrapper---
add_subdirectory(lib/RtAudioWrapper)
target_link_libraries(Audio PUBLIC RtAudioWrapper::RtAudioWrapper)