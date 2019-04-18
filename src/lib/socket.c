#include "socket.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef SPTE_MSVC

#include <WS2tcpip.h>

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_SOCKET -1

#endif

static int
get_ip_str(const struct sockaddr *sa, char *buf, size_t size)
{
    const void *src, *ret;

    if (sa->sa_family == AF_INET) {
        src = &(((struct sockaddr_in *) sa)->sin_addr);
    } else if (sa->sa_family == AF_INET6) {
        src = &(((struct sockaddr_in6 *) sa)->sin6_addr);
    } else {
        return 0; // todo ?
    }

    ret = inet_ntop(sa->sa_family, src, buf, (socklen_t) size);

    return NULL != ret;
}

static const char *
get_family_name(int family)
{
    if (family == AF_INET) {
        return "Ipv4";
    } else if (family == AF_INET6) {
        return "Ipv6";
    } else if (family == AF_UNIX) {
        return "Unix domain socket";
    } else if (family == AF_UNSPEC) {
        return "Unspecified";
    }

    return "Unknown";
}

socket_t
socket_create_client(const char *addr, const char *port, FILE *logs, FILE *errs)
{
    struct addrinfo hints = {0};
    struct addrinfo *res, *rp;
    socket_t sfd = INVALID_SOCKET;
    int s;
    char ipstr[INET6_ADDRSTRLEN];

    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    fprintf(logs, "Getting address information for: %s:%s\n", addr, port);

    if (0 != (s = getaddrinfo(addr, port, &hints, &res))) {
        fprintf(errs, "getaddrinfo(): %s.\n", gai_strerror(s));
        return INVALID_SOCKET;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {

        if (!get_ip_str(rp->ai_addr, ipstr, sizeof(ipstr))) {
            fprintf(errs, "inet_ntop(): %s.\n", strerror(errno));
        }

        fprintf(logs, "Creating a socket for %s address: %s\n", get_family_name(rp->ai_family), ipstr);

        if (INVALID_SOCKET == (sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol))) {
            fprintf(errs, "socket(): %s.\n", strerror(errno));
            continue;
        }

        if (INVALID_SOCKET == (connect(sfd, rp->ai_addr, rp->ai_addrlen))) {
#ifdef SPTE_MSVC
            fprintf(errs, "connect(): %ld.\n", WSAGetLastError());
            shutdown(sfd, SD_BOTH);
            closesocket(sfd);
#else
            fprintf(errs, "connect(): %s.\n", strerror(errno));
            close(sfd);
#endif
            continue;
        }
        break;
    }

    if (NULL == rp) {
        fprintf(errs, "Failed to connect() the socket to any addrinfo results.\n");
        return INVALID_SOCKET;
    }

    fputs("Socket successfully created.\n", logs);

    freeaddrinfo(res);

    return sfd;
}

socket_t
socket_create_local_server_tcp(const char *port, FILE *logs, FILE *errs)
{
    struct addrinfo hints = {0};
    struct addrinfo *res, *rp;
    socket_t sfd = INVALID_SOCKET;
    int s;
    int yes = 1;
    char ipstr[INET6_ADDRSTRLEN];

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    fprintf(logs, "Getting local address information for port: %s\n", port);

    if (0 != (s = getaddrinfo(NULL, port, &hints, &res))) {
        fprintf(errs, "getaddrinfo(): %s.\n", gai_strerror(s));
        return INVALID_SOCKET;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {

        if (!get_ip_str(rp->ai_addr, ipstr, sizeof(ipstr))) {
            fprintf(errs, "inet_ntop(): %s.\n", strerror(errno));
        }

        fprintf(logs, "Creating a socket for local %s address: %s\n", get_family_name(rp->ai_family), ipstr);

        if (INVALID_SOCKET == (sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol))) {
            fprintf(errs, "socket(): %s.\n", strerror(errno));
            continue;
        }

#ifdef SPTE_MSVC
        if (-1 == setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes))) {
#else
        if (-1 == setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
#endif
            fprintf(errs, "setsockopt(): %s.\n", strerror(errno));
            return INVALID_SOCKET;
        }

#ifdef SPTE_MSVC
        if (-1 == bind(sfd, rp->ai_addr, (int) rp->ai_addrlen)) {
            shutdown(sfd, SD_BOTH);
            closesocket(sfd);
#else
        if (-1 == bind(sfd, rp->ai_addr, rp->ai_addrlen)) {
            close(sfd);
#endif
            fprintf(errs, "bind(): %s.\n", strerror(errno));
            continue;
        }
        break;
    }

    if (NULL == rp) {
        fprintf(errs, "Failed to bind() the socket to any addrinfo results.\n");
        return INVALID_SOCKET;
    }

    fputs("Socket successfully created.\n", logs);

    freeaddrinfo(res);

    if (-1 == listen(sfd, 5)) {
        fprintf(errs, "listen(): %s.\n", strerror(errno));
        return INVALID_SOCKET;
    }

    return (socket_t) sfd;
}

socket_t
socket_accept(socket_t sfd, FILE *logs, FILE *errs)
{
    socket_t remote_sfd;
    struct sockaddr_storage remote_addr_stor;
    struct sockaddr *address = (struct sockaddr *) &remote_addr_stor;
    socklen_t address_len = sizeof(remote_addr_stor);
    char ipstr[INET6_ADDRSTRLEN];

    fputs("Listening for incoming connection...\n", logs);

    if (-1 == (remote_sfd = accept((int) sfd, address, &address_len))) {
        fprintf(errs, "accept(): %s.\n", strerror(errno));
        return INVALID_SOCKET;
    }

    if (!get_ip_str(address, ipstr, sizeof(ipstr))) {
        fprintf(errs, "inet_ntop(): %s.\n", strerror(errno));
    }

    printf("Accepted a new connection from %s\n", ipstr);

    return remote_sfd;
}

ssize_t
socket_recv(socket_t sfd, char *msg, size_t msg_size, FILE *logs, FILE *errs)
{
    ssize_t r;

    fputs("Waiting to receive a message...\n", logs);

#ifdef SPTE_MSVC
    r = recv(sfd, msg, (int) msg_size, 0);
#else
    r = recv(sfd, msg, msg_size, 0);
#endif

    if (-1 == r) {
        fprintf(errs, "recv(): %s.\n", strerror(errno));
    } else if (0 == r) {
        fputs("Remote connection has been closed\n", logs);
    } else {
#ifdef SPTE_MSVC
        fprintf(logs, "Read %d bytes from socket.\n", r);
#else
        fprintf(logs, "Read %zd bytes from socket.\n", r);
#endif
    }

    return r;
}

ssize_t
socket_send(socket_t sfd, const char *msg, size_t msg_size, FILE *logs, FILE *errs)
{
    size_t total = 0;
    size_t bytesleft = msg_size;
    ssize_t n = INVALID_SOCKET;

    fputs("Sending the input buffer...\n", logs);

    while(total < msg_size) {
        n = send(sfd, msg+total, bytesleft, 0);

        if (-1 == n) {
            fprintf(errs, "send(): %s.\n", strerror(errno));
            return INVALID_SOCKET;
        }

        total += n;
        bytesleft -= n;
    }

    return total;
}
