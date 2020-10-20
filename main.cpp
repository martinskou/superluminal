#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memset() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <errno.h>
#include <chrono>

#include "httpserver.hpp"



void handle(Request &req, Response &res)
{
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "URL: " << req.url << std::endl;
    std::cout << "HOST: " << req.headers.Get("host") << std::endl;
    res.out << "Hello World <br> " << req.url << "<br>";
    res.headers.Add("content-type","text/html; charset=UTF-8");
}


int main() {
    Server s=Server();
    s.Start(handle,"8080");
    return 0;
}
