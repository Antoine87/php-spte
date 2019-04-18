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

#define MESSAGE_BUFFER 4096

static void
usage()
{
    puts("Usage: socket-recv-file <port> <file> [--stop-first-msg]\n"
         "   <port>            The port to listen for an incoming connection.\n"
         "   <file>            The file to which all incoming message will be written.\n"
         "   --stop-first-msg  Terminate once the first message is received.\n");
}

int
main(int argc, char *argv[])
{
#ifdef SPTE_MSVC
    WSADATA wsaData = {0};
    int iResult = 0;
#endif
    socket_t s, rs;
    ssize_t r;
    char msg[MESSAGE_BUFFER];
    FILE *f;
    size_t w, bytes;
    int stop = 0;

    if (argc < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (argc >= 4) {
        stop = (0 == strcmp(argv[3], "--stop-first-msg"));
    }

    // Open the output file given for writting
    if (NULL == (f = fopen(argv[2], "w+b"))) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

#ifdef SPTE_MSVC
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (0 != iResult) {
        printf("WSAStartup failed: %d\n", iResult);
        exit(EXIT_FAILURE);
    }
#endif

    // Listen to the port specified
    if (-1 == (s = socket_create_local_server_tcp(argv[1], stdout, stderr))) {
        fprintf(stderr, "Failed to create the tcp server socket.\n");
        exit(EXIT_FAILURE);
    }

    // Accept the first remote connection
    if (-1 == (rs = socket_accept(s, stdout, stderr))) {
        fprintf(stderr, "An error occurred accepting the first connection.\n");
        exit(EXIT_FAILURE);
    }

#ifdef SPTE_MSVC
    closesocket(s);
#else
    close(s);
#endif

    // Read incoming messages until remote connection is closed or an error occur
    do {
        r = socket_recv(rs, msg, sizeof(msg), stdout, stderr);
        bytes = (size_t) r;

        // Write MESSAGE_BUFFER number of bytes to the file
        if (bytes != (w = fwrite(msg, sizeof(*msg), bytes, f))) {
            fprintf(stderr, "An error occurred writing to the file.\n");
            exit(EXIT_FAILURE);
        }

        // Ensure written data can be read even if the program is still running
        if (0 != fflush(f)) {
            fprintf(stderr, "An error occurred flushing the file buffer.\n");
            exit(EXIT_FAILURE);
        }

        printf("%zu bytes written (flushed).\n", w);

    } while (r > 0 && !stop);

    printf("Closing connections.\n");

#ifdef SPTE_MSVC
    shutdown(rs, SD_SEND);
    closesocket(rs);
    WSACleanup();
#else
    close(rs);
#endif

    return EXIT_SUCCESS;
}
