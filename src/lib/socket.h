#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>

#if defined SPTE_MSVC

#include <WS2tcpip.h>

typedef SOCKET socket_t;
typedef SSIZE_T ssize_t;

#elif defined SPTE_UNIX

#include <unistd.h>

typedef int socket_t;

#else
#error This library requires either a POSIX or Winsock API
#endif

socket_t socket_create_client(const char *address, const char *port, FILE *log_stream, FILE *error_stream);
socket_t socket_create_local_server_tcp(const char *port, FILE *log_stream, FILE *error_stream);
socket_t socket_accept(socket_t socket, FILE *log_stream, FILE *error_stream);
ssize_t  socket_recv(socket_t socket, char *message, size_t message_size, FILE *log_stream, FILE *error_stream);
ssize_t  socket_send(socket_t socket, const char *message, size_t message_size, FILE *log_stream, FILE *error_stream);

#endif
