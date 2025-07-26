#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


void die(const std::string &msg){
    int error = errno;
    std::cerr << "Error:" << error << strerror(errno) << ":"<< msg;
    abort();
}

void msg(const std::string &message){
    std::cerr << message;
}

int32_t len_den(const int &conn_fd){
    //recieve a msg
    char recv_buf[64] = {};
    ssize_t n = recv(conn_fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if(n <= 0){ return -1;}

    std::cout << n << std::endl<< "Client says: " << recv_buf << std::endl;
    
    char send_buf[] = "World!";
    send(conn_fd, send_buf, sizeof(send_buf) - 1, 0);
    return 0;
}

int main(){

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int val = 1;
    int err_code = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if(err_code < 0){
        perror("setsockopt failed");
        exit(1);
    }
    
    // setting addr + port
    struct sockaddr_in addr;
    struct sockaddr_in using_addr; //if an error occurs and os randomly binds 'fd' to another port

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    // binding the addr with sockect
    err_code = bind(fd,(const struct sockaddr*) &addr, sizeof(addr));
    
    if(err_code){
        die("bind()");
    }

    err_code = listen(fd, SOMAXCONN);
    if(err_code) {die("listen()");}
    else {
        socklen_t using_addrlen = sizeof(using_addr);
        getsockname(fd, (struct sockaddr *) &using_addr, &using_addrlen);
        std::clog << "listing on :" << inet_ntoa(using_addr.sin_addr) << ":" << ntohs(using_addr.sin_port) << std::endl;}

    // accepting connection
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t client_addrlen = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr*) &client_addr, &client_addrlen);

        if(conn_fd < 0) continue;
        else std::clog << "accepted by client :" << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

        while(true){
            err_code = len_den(conn_fd); // just reads and writes for testing purposes
            if(err_code) break;
        }
        close(conn_fd);
    }

    return 0;
}