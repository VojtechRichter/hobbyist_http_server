#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    char buffer[BUFFER_SIZE];

    char res[] = "HTTP/2.0 200 OK\r\n"
        "Server: my-little-c-webserver\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><h1>Hello, World</h1></html>\r\n";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("webserver (socket)\n");
        return 1;
    }

    printf("socket created successfully\n");

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)\n");
        return 1;
    }

    printf("socket successfully bound to an address\n");

    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)\n");
        return 1;
    }

    printf("server listening for connections...\n");

    for (;;) {
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0) {
            perror("webserver (accept)\n");
            return 1;
        }

        printf("connection accepted\n");

        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0) {
            perror("webserver (getsockname)");
            return 1;
        }

        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("webserver (read)\n");
            continue;
        }

        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("client: [%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
                method, uri, version);

        int valwrite = write(newsockfd, res, strlen(res));
        if (valwrite < 0) {
            perror("webserver (write)\n");
            return 1;
        }

        close(newsockfd);
    }

    return 0;
}
