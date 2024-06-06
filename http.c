#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <inttypes.h>

#define LISTEN_PORT 8080
#define BUFFER_SIZE 1024

int8_t receive_client_data(int32_t sock_fd, char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t recv_result = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
    if (recv_result == -1) {
        perror("recv() error");
        return -1;
    }

    buffer[recv_result] = '\0';

    printf("received: %s\n", buffer);
    printf("buffer size: %zd\n", strlen(buffer));

    return 0; 
}

int main()
{
    int32_t server_sock_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_sock_fd == -1) {
        perror("socket() error");
        return 1;
    }

    struct sockaddr_in host_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(LISTEN_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    size_t host_addr_size = sizeof(host_addr);

    if (bind(server_sock_fd, (struct sockaddr *)&host_addr, host_addr_size) != 0) {
        perror("bind() error");
        return 1;
    }

    if (listen(server_sock_fd, SOMAXCONN) != 0) {
        perror("listen() error");
        return 1;
    }

    printf("Listening on port %d...\n", (int32_t)LISTEN_PORT);

    struct sockaddr_in client_addr;
    size_t client_addr_size = sizeof(client_addr);

    char socket_buffer[BUFFER_SIZE];
    int i = 0;
    while(1) {
        int32_t accept_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
        if (accept_sock_fd < 0) {
            perror("accept() error");
            close(accept_sock_fd);
            continue;
        }

        int32_t sock_name_result = getsockname(accept_sock_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
        if (sock_name_result < 0) {
            perror("getsockname() error");
            close(accept_sock_fd);
            continue;
        }

        int8_t receive_result = receive_client_data(accept_sock_fd, socket_buffer);

        close(accept_sock_fd);

        printf("[Request number: %d]\n", i);
        i += 1;
    }
    
    close(server_sock_fd);

    return 0;
}
