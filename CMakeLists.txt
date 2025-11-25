cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
Project("hello world")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")

add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-O0;-g3;-ggdb")

# add_compile_options("-fsanitize=address" "-fsanitize=address" "-shared-libasan"  "-fno-omit-frame-pointer")
set(MY_TARGETS "hello-world" "create-process" "create-job")
foreach(MY_TARGET IN LISTS MY_TARGETS)
    add_executable(${MY_TARGET})
    target_compile_definitions(${MY_TARGET} PRIVATE UNICODE _UNICODE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MY_TARGET}.c")
        set(SOURCE_FILE "${MY_TARGET}.c")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MY_TARGET}.cpp")
        set(SOURCE_FILE "${MY_TARGET}.cpp")
    else()
        message(FATAL_ERROR "ERROR: ${CMAKE_CURRENT_SOURCE_DIR}/${MY_TARGET} .c or .cpp not found!")
    endif()
    target_sources(${MY_TARGET}
        PRIVATE
            "${MY_TARGET}.c"
    )
endforeach()

message("********************")
message(STATUS "CMAKE_C_LINK_FLAGS=${CMAKE_C_LINK_FLAGS}")
message("********************")
