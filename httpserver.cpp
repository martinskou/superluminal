//
// Created by Martin Drewes on 18/10/2020.
//

#include "httpserver.hpp"

void Server::SendResponse(int *s, Response &res) {
    ByteBuffer out;

    out << "HTTP/1.0 " << res.status << " OK\n";
    out << "Server: superluminal\n";
    for (auto x : res.headers.GetRaw()) {
        out << x << "\n";
    }
    out << "Content-Length: " << res.out.size() << "\n\n";
    out << res.out;

//    std::cout << out.data() << std::endl;

    send(*s, out.data(), out.size(), 0);

}


void Server::Handle(int *pnewsock) {
    int *s = (int *)pnewsock;

//    std::cout << "Thread ID " << std::this_thread::get_id() << std::endl;

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
   // setsockopt(*s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char buffer[10240]; // 10 kb, just in case
    int receivedBCount = recv(*s,&buffer,10240,0); // Receive the request
    if (receivedBCount <= 0) {
        std::cout << "Error (" << receivedBCount << ") : " << errno << std::endl;
    } else {

        auto req = Request(std::string(buffer));
        // std::this_thread::sleep_for (std::chrono::seconds(5));

        Response res;
        user_handler(req,res);

        SendResponse(s,res);
    }
    close(*s);
    free(pnewsock);

}

void Server::ListenLoop(int sock, int backlog, int threads) {

    if (listen(sock, backlog) == -1) {
        std::cerr << "Could not listen" << std::endl;
        return; // return false;
    }

    ThreadPool pool(threads);

    run=true;
    while (run) {
        socklen_t size = sizeof(struct sockaddr_in);
        struct sockaddr_in their_addr;
        int newsock = accept(sock, (struct sockaddr*)&their_addr, &size);
        if (newsock == -1) {
            std::cerr << "Could not accept" << std::endl;
        }
        else {
            int *safesock = (int*)malloc(sizeof(int));
            if (safesock) {
                *safesock = newsock;

                if (false) {
                    std::thread t=std::thread(&Server::Handle, this, safesock);
                    t.detach();
//                std::thread thread(handle, safesock);
//                thread.join();

                } else {

//                    pool.enqueue([](int answer) { return answer; }, 42);
                    pool.enqueue(&Server::Handle, this, safesock);
                }



            }
            else {
                std::cerr << "Malloc error" << std::endl;
            }
        }
    }

    close(sock);
  //  return true;
}

int Server::Start(std::function<HandlerSignature> handle, std::string port, int backlog, bool background, int threads) {
    user_handler=handle;

    int sock;
    pthread_t thread;
    struct addrinfo hints, *res;
    int reuseaddr = 1;

    /* Get the address info */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(NULL, port.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        std::cerr << "Could not create socket" << std::endl;
        return 1;
    }

    /* Enable the socket to reuse the address */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        std::cerr << "Could not setsockopt" << std::endl;
        return 1;
    }

    /* Bind to the address */
    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "Could not bind the socket" << std::endl;
        return 0;
    }

    freeaddrinfo(res);

    std::cout << "Starting server on port " << port << std::endl;

    if (background) {
        server_thread=std::thread( &Server::ListenLoop, this, sock, backlog, threads);
    } else {
        ListenLoop(sock, backlog, threads);
    }

    return 0;
}

void Server::Wait() {
    if (server_thread.joinable()) {
        server_thread.join();
    }
}


std::function<HandlerSignature> Logger(std::function<HandlerSignature> next_handle) {
    return [next_handle](Request &req, Response &res) {
//        std::cout << "LOGGER: " << req.url << std::endl;
        next_handle(req,res);
    };
}


std::function<HandlerSignature> Assets(std::function<HandlerSignature> next_handle, std::string root, std::string path) {
    return [next_handle,root,path](Request &req, Response &res) {
        if (req.url.starts_with(path)) {
            std::string fp=req.url.substr(path.size(),req.url.size()-path.size());
            fp=root+fp;
            if (std::filesystem::exists(fp)) {
                res.out.writefile(fp);
                res.headers.Add("content-type", "image/jpeg");
            }
        }
        next_handle(req,res);
    };
}
