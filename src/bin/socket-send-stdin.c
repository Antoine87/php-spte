#include "lib/socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined SPTE_MSVC

#include <WS2tcpip.h>

#elif defined SPTE_UNIX

#include <unistd.h>

#else
#error This program requires either a POSIX or Winsock API
#endif

#define INPUT_BUFFER_LENGTH 1000

static void
usage()
{
    printf("Usage: <input> | socket-send-stdin <address> <port>\n"
         "   <address>   The IP (v4 or v6) address with which to establish the connection.\n"
         "   <port>      The port to connect.\n"
         "\n"
         "Examples:\n"
         "   - cat file.txt | socket-send-stdin www.example.com 80\n"
         "   - echo foobar | socket-send-stdin localhost 3490\n"
         "\n"
         "Note: The input buffer is currently limited to %d 'char' types on this machine.\n", INPUT_BUFFER_LENGTH);
}

int
main(int argc, char *argv[])
{
#ifdef SPTE_MSVC
    WSADATA wsaData = {0};
    int iResult = 0;
#endif
    socket_t s;
    ssize_t r;
    char input[INPUT_BUFFER_LENGTH];

    if (argc < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    fgets(input, sizeof(input), stdin);

#ifdef SPTE_MSVC
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (0 != iResult) {
        printf("WSAStartup failed: %d\n", iResult);
        exit(EXIT_FAILURE);
    }
#endif

    if (-1 == (s = socket_create_client(argv[1], argv[2], stdout, stderr))) {
        fprintf(stderr, "Failed to establish a connection to %s:%s.\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    if (-1 == (r = socket_send(s, input, strlen(input), stdout, stderr))) {
        fprintf(stderr, "An error occured sending the input message.\n");
        exit(EXIT_FAILURE);
    }
/*
#ifdef SPTE_MSVC
    shutdown(rs, SD_SEND);
    closesocket(rs);
    WSACleanup();
#else
    close(rs);
#endif
*/
    return EXIT_SUCCESS;
}
