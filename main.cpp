#include <iostream>
#include <thread>

#include "httpserver.hpp"



void handle(Request &req, Response &res)
{
    std::cout << req.method << " [" << req.url << "] " << std::this_thread::get_id() << std::endl;
//    std::cout << "HOST: " << req.headers.Get("host") << std::endl;

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
    s.Start(Logger(
            Assets(handle,"..", "/assets")),"8080",50,true,16);
    s.Wait();


    /*
    ThreadPool pool(4);

// enqueue and store future
    auto result = pool.enqueue([](int answer) { return answer; }, 42);
    auto result2 = pool.enqueue([](int answer) { std::cout << "x" << std::endl; return answer; }, 13);

// get result from future
    std::cout << result.get() << std::endl;

//    result2.wait();
*/
    return 0;
}
