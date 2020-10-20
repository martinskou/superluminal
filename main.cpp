#include <iostream>
#include <thread>

#include "httpserver.hpp"



void handle(Request &req, Response &res)
{
    std::cout << "METHOD: " << req.method << std::endl;
    std::cout << "URL: " << req.url << std::endl;
    std::cout << "HOST: " << req.headers.Get("host") << std::endl;

if (req.url=="/file") {
    res.out.writefile("/Users/martin/Downloads/IMG_1794.JPG");
    res.headers.Add("content-type","image/jpeg");
} else {
    res.out.write("test...<br>");
    res.out << "URL : " << req.url << "<br>";
    res.headers.Add("content-type","text/html; charset=UTF-8");
}
    res.status=200;
}


int main() {
    Server s=Server();
    s.Start(handle,"8080");
    return 0;
}
