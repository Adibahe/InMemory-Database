#ifndef READWRITE_H
#define READWRITE_H

#include <iostream>
#include <unistd.h>
#include <assert.h>
#include "structures.h"
#include <fcntl.h>



static void set_fd_nb(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

static void Myread(Connection *conn){
    uint8_t r_buf[64 * 1024]; // a buffer of 64kb
    ssize_t size = read(conn -> fd, r_buf, sizeof(r_buf));
    if(size <= 0){
        conn -> want_close = true;
        return;
    }
    // insert the r_buf in incoming_buf
    conn->incoming_buffer.insert(conn -> incoming_buffer.end(), r_buf, r_buf + size);

}

class ReadWrite{

    public:

    static int32_t readfull(const int &conn_fd, char * buf, size_t buf_size){
        
        // used to read byte stream with expected size
        while(buf_size > 0){
            ssize_t n = read(conn_fd, buf, buf_size);
            if(n <= 0) return -1; //error in reading

            assert((size_t)n <= buf_size);
            buf_size -= (size_t)n;
            buf += (size_t)n;
        }
        return 0;
    }

    static int32_t writefull(const int &conn_fd, char * buf, size_t buf_size){
    
        // used to write byte stream with expected size
        while(buf_size > 0){
            ssize_t n = write(conn_fd, buf, buf_size);
            if(n <= 0) return -1; //error in reading

            assert((size_t)n <= buf_size);
            buf_size -= (size_t)n;
            buf += (size_t)n;
        }
        return 0;
    }

    static Connection* Myaccept(int fd){
        struct sockaddr_in client_addr = {};
        socklen_t addr_len = sizeof(client_addr);

        int connfd = accept(fd, (struct sockaddr*) &client_addr, &addr_len);
        if(connfd < 0){
            errors::die("error : accept()");
            return NULL;
        }

        set_fd_nb(connfd);
        struct Connection conn = {};
        conn.fd = connfd;
        conn.want_read = true;
        return &conn;
    }

};

#endif
