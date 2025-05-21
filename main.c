#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// struct sockaddr_in my_addr;

int main(){
    printf("hello world\n");
    // if(socket(AF_INET, SOCK_STREAM, 0)!=-1){
    //     printf("socket created!");
    // }
    int socketid = socket(AF_INET, SOCK_STREAM, 0);
    if(socketid == -1){
        perror("error occured");
        return 0;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(8080);

    if(bind(socketid, (struct sockaddr *)& my_addr, sizeof(my_addr)) < 0){
        perror("bind failure");
        return 0;
    }

    // socklen_t *addrlen;
    struct sockaddr *addr;

    if(listen(socketid, 20) == -1){
        perror("listening failure");
        return 0;
    }

    printf("listening on port %d\n", ntohs(my_addr.sin_port));
    
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);

    int fd = accept(socketid, (struct sockaddr*)&their_addr, &addr_size);
    if(fd == -1){
        perror("could not accept");
        return 0;
    }

    char buffer[1024] = {0};
    read(fd, buffer, sizeof(buffer));
    printf("Received: %s\n", buffer);

    // char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world";
    const char *msg =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 47\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";
    write(fd, msg, strlen(msg));

    close(fd);
    close(socketid);

}