// redir_client.h
// Reusable TCP client API for connecting to a USB redir guest or any TCP service.

#ifndef REDIR_CLIENT_H
#define REDIR_CLIENT_H

#include <stddef.h>
#include <sys/types.h> // for ssize_t

#ifdef __cplusplus
extern "C" {
#endif

#define REDIR_BUF_SIZE 1024

/**
 * Connect to a TCP server.
 *
 * @param host Hostname or IP string.
 * @param port Port string (e.g., "1337").
 * @return Socket file descriptor on success, -1 on failure.
 */
int connect_socket(const char *host, const char *port);

/**
 * Receive data from a connected socket.
 *
 * @param sock Socket file descriptor.
 * @param buf Buffer to store data.
 * @param bufsize Size of buffer.
 * @return Number of bytes received (>0), 0 if connection closed, -1 on error.
 */
ssize_t receive_data(int sock, unsigned char *buf, size_t bufsize);

/**
 * Print a binary buffer in a readable format (printable chars + hex escapes).
 *
 * @param buf Pointer to buffer.
 * @param len Number of bytes to print.
 */
void print_buffer(const unsigned char *buf, size_t len);

/**
 * Close a socket gracefully.
 *
 * @param sock Socket file descriptor.
 */
void close_socket(int sock);

#ifdef __cplusplus
}
#endif

#endif // REDIR_CLIENT_H
