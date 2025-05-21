#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

// struct sockaddr_in my_addr;

const char* get_mime_type(const char* path) {
    if (strstr(path, ".html")) return "text/html";
    if (strstr(path, ".css")) return "text/css";
    if (strstr(path, ".js")) return "application/javascript";
    if (strstr(path, ".png")) return "image/png";
    if (strstr(path, ".jpg") || strstr(path, ".jpeg")) return "image/jpeg";
    return "text/plain";
}

void serve_file(int fd, const char *path) {
    char full_path[512];
    if (strcmp(path, "/") == 0)
        strcpy(full_path, "public/index.html");
    else
        snprintf(full_path, sizeof(full_path), "public%s", path);

    int file_fd = open(full_path, O_RDONLY);
    if (file_fd == -1) {
        const char *resp = "HTTP/1.0 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found\n";
        write(fd, resp, strlen(resp));
        return;
    }

    struct stat st;
    fstat(file_fd, &st);

    const char *mime = get_mime_type(full_path);

    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.0 200 OK\r\n"
        "Content-Length: %ld\r\n"
        "Content-Type: %s\r\n\r\n", st.st_size, mime);
    write(fd, header, strlen(header));

    char buf[4096];
    ssize_t n;
    while ((n = read(file_fd, buf, sizeof(buf))) > 0) {
        write(fd, buf, n);
    }

    close(file_fd);
}

void handle_request(int fd, char *request) {
    char method[16], path[256], version[32];

    char *line = strtok(request, "\r\n");
    if (sscanf(line, "%15s %255s %31s", method, path, version) != 3) {
        const char *resp = "HTTP/1.0 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        write(fd, resp, strlen(resp));
        return;
    }

    if (strcmp(method, "GET") != 0) {
        const char *resp = "HTTP/1.0 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
        write(fd, resp, strlen(resp));
        return;
    }

    // const char *resp =
    //     "HTTP/1.0 200 OK\r\n"
    //     "Content-Type: text/html\r\n"
    //     "Content-Length: 47\r\n"
    //     "\r\n"
    //     "<html><body><h1>Hello, World!</h1></body></html>";
    // write(fd, resp, strlen(resp));

    serve_file(fd, path);
}

void *thread_handler(void *arg) {
    int fd = *(int *)arg;
    free(arg);

    char buffer[1024] = {0};
    read(fd, buffer, sizeof(buffer) - 1);
    printf("Received:\n%s\n", buffer);

    handle_request(fd, buffer);
    close(fd);

    return NULL;
}


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

    while (1) {
        // int fd = accept(socketid, (struct sockaddr*)&their_addr, &addr_size);
        // if(fd == -1){
        //     perror("could not accept");
        //     return 0;
        // }

        // char buffer[1024] = {0};
        // read(fd, buffer, sizeof(buffer));
        // printf("Received: %s\n", buffer);

        // char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello world";
        // const char *msg =
        //     "HTTP/1.0 200 OK\r\n"
        //     "Content-Type: text/html\r\n"
        //     "Content-Length: 47\r\n"
        //     "\r\n"
        //     "<html><body><h1>Hello, World!</h1></body></html>";
        // write(fd, msg, strlen(msg));

        // close(fd);
        // close(socketid);

        // read(fd, buffer, sizeof(buffer) - 1);
        // printf("Received:\n%s\n", buffer);
        // handle_request(fd, buffer);
        // close(fd);
        int *fd = malloc(sizeof(int));
        *fd = accept(socketid, (struct sockaddr*)&their_addr, &addr_size);
        if (*fd == -1) {
            perror("accept failed");
            free(fd);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, thread_handler, fd) != 0) {
            perror("pthread_create failed");
            close(*fd);
            free(fd);
        } else {
            pthread_detach(tid);
        }
    }

}