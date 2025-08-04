#include "readwrite.h"
#include <iostream>
#include <sys/socket.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "error_handling.h"
#include <poll.h>
#include <fcntl.h>

const size_t max_msg = 4096; // can carry a max data of 4KB

// static int32_t len_den(const int &conn_fd){
//     //recieve a msg
//     char recv_buf[4 + max_msg];
    
//     errno = 0;

//     int32_t err = ReadWrite::readfull( conn_fd, recv_buf, 4);
//     if(err){
//         errors :: msg(errno == 0 ? "EOF" : "read() Error ");
//         return err;
//     }

//     uint32_t len = 0;
//     memcpy(&len, recv_buf, 4); // get len of msg

//     if(len > max_msg){
//         errors :: msg("too long message");
//         return -1;
//     }

//     // reads msg
//     err = ReadWrite::readfull(conn_fd, &recv_buf[4], len);
//     if(err < 0){
//         errors :: msg("read() error");
//         return err;
//     }

//     std::cout << std::endl<< "Client says: " << &recv_buf[4] << std::endl;

    
//     char reply[] = "World!";

//     len = (uint32_t)strlen(reply);
//     char send_buf[4 + sizeof(reply)];

//     memcpy(send_buf , &len, 4);
//     memcpy(&send_buf[4], reply, len);

//     err = ReadWrite :: writefull(conn_fd, send_buf, 4 + len);
//     if(err){
//         errors :: msg("write() error");
//         return err;
//     }
//     return 0;
// }



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
        errors :: die("bind()");
    }

    err_code = listen(fd, SOMAXCONN);
    if(err_code) {errors :: die("listen()");}
    else {
        socklen_t using_addrlen = sizeof(using_addr);
        getsockname(fd, (struct sockaddr *) &using_addr, &using_addrlen);
        std::clog << "listing on :" << inet_ntoa(using_addr.sin_addr) << ":" << ntohs(using_addr.sin_port) << std::endl;}

    // accepting connection
    
    std :: vector< Connection *> fdconns; // handling connections
    std :: vector<struct pollfd> candidates;
    
    set_fd_nb(fd);

    while(true){
        candidates.clear();
        // adding listening port in pollfd
        struct pollfd listening_fd =  {fd, POLLIN, 0};
        candidates.push_back(listening_fd);

        for(Connection *conn : fdconns){
            if(!conn) continue;

            struct pollfd temp = {conn -> fd, POLLERR, 0};
            if(conn -> want_read) temp.events |= POLLIN;
            if(conn -> want_write) temp.events |= POLLOUT;

            candidates.push_back(temp);
        }

        // checking readiness
        int err = poll(candidates.data(), (nfds_t)candidates.size(), -1);
        if(err < 0 && errno == EINTR) continue;
        if(err < 0) errors :: die("error : Poll()");

        if(candidates[0].revents){
            Connection *conn = ReadWrite :: Myaccept(fd);
            if(conn){
                if(fdconns.size() <= (size_t)conn -> fd){
                    fdconns.resize(conn -> fd + 1);
                }
                fdconns[conn -> fd] = conn;
            }
            
        }
        // if fd's are ready handle them for particular tasks
        for(size_t i = 1; i < candidates.size(); i++ ){
            uint32_t ready = candidates[i].revents;

            Connection *conn = fdconns[candidates[i].fd];
            if(ready & POLLIN) ReadWrite::Myread(conn);
            if(ready & POLLOUT) Mywrite(conn);
            if((ready & POLLERR) ||(conn -> want_close)){
                close(conn -> fd);
                fdconns[conn -> fd] = NULL;
                delete conn;
            }
        }

    }

    // while(true){
    //     struct sockaddr_in client_addr = {};
    //     socklen_t client_addrlen = sizeof(client_addr);
    //     int conn_fd = accept(fd, (struct sockaddr*) &client_addr, &client_addrlen);

    //     if(conn_fd < 0) continue;
    //     else std::clog << "accepted by client :" << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

    //     while(true){
    //         err_code = len_den(conn_fd); // just reads and writes for testing purposes
    //         if(err_code) {
    //             break;
    //         }
    //     }
    //     close(conn_fd);
    // }

    return 0;
}