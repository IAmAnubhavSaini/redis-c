#include <stdio.h>
/* sys/socket.h <- bits/socket.h <- bits/socket_type.h */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define ERROR_SOCKET_CREATION_FAILED 1001
#define ERROR_BIND_FAILED 1002
#define ERROR_LISTEN_FAILED 1003

static void die(const char *msg, int exit_code) {
    perror(msg);
    exit(exit_code);
}

int main() {
    /* From socket.h
     * Create a new socket of type TYPE in domain DOMAIN, using
     * protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
     * Returns a file descriptor for the new socket, or -1 for errors.
     * extern int socket (int __domain, int __type, int __protocol) __THROW;
     * From bits/socket.h
     * #define AF_INET		PF_INET
     * #define PF_INET		2	/.* IP protocol family.   *./
     * From bits/socket_type.h
     * enum __socket_type   {   SOCK_STREAM = 1, /.* Sequenced, reliable, connection-based byte streams.  *./
     * */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket creation failed", ERROR_SOCKET_CREATION_FAILED);
    }

    int val = 1;
    /* sys/socket.h
     * Set socket FD's option OPTNAME at protocol level LEVEL to *OPTVAL (which is OPTLEN bytes long).
     * Returns 0 on success, -1 for errors.
     * extern int setsockopt (int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen) __THROW;
     * */
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    /* From netinet/in.h
     * Structure describing an Internet socket address.
     * struct sockaddr_in
     * */
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(8765);
    addr.sin_addr.s_addr = ntohl(0); // wild card address 0.0.0.0

    /* From sys.socket.h
     * Give the socket FD the local address ADDR (which is LEN bytes long).
     * extern int bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len) __THROW;
     * */
    int rv = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (rv) {
        die("bind failed", ERROR_BIND_FAILED);
    }

    rv = listen(fd, SOMAXCONN);
    if(rv) {
        die("listen failed", ERROR_LISTEN_FAILED);
    }

    while(1) {
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
        if (connfd < 0) {
            perror("accept failed");
            continue;
        }

        // do something
        // close

    }

    return EXIT_SUCCESS
}
