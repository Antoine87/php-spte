if(MSVC)
    add_definitions(-DSPTE_MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    add_compile_options(/W4)
elseif(UNIX)
    add_definitions(-DSPTE_UNIX)

    add_compile_options(-Wall -Wextra -pedantic)
endif()


# Libraries in "lib" dir
add_library(socket "lib/socket.c")


# Make available #include "lib/*" in every source file
include_directories(".")


# Executables in "bin" dir
add_executable(hello "bin/hello.c")

add_executable(socket-recv-file "bin/socket-recv-file.c")
target_link_libraries(socket-recv-file socket)

add_executable(socket-send-stdin "bin/socket-send-stdin.c")
target_link_libraries(socket-send-stdin socket)

add_executable(stdin-to-stdout "bin/stdin-to-stdout.c")

add_executable(stdout-and-stderr "bin/stdout-and-stderr.c")

add_executable(wait "bin/wait.c")


# Include windows socket dll for executables requiring it
if(MSVC)
    target_link_libraries(socket-recv-file ws2_32)
    target_link_libraries(socket-send-stdin ws2_32)
endif()
