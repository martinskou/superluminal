//
// Created by Martin Drewes on 18/10/2020.
//

#include "httpserver.hpp"

void Server::Handle(int *pnewsock) {
    int *s = (int *)pnewsock;

    // std::cout << "Thread ID " << std::this_thread::get_id() << std::endl;

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(*s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char buffer[10240]; // 10 kb, just in case
    int receivedBCount = recv(*s,&buffer,10240,0); // Receive the request
    if (receivedBCount <= 0) {
        std::cout << "Error (" << receivedBCount << ") : " << errno << std::endl;
    } else {

        auto req = Request(std::string(buffer));
        // std::this_thread::sleep_for (std::chrono::seconds(5));

        Response res;
        user_handler(req,res);

        ByteBuffer out;

        out << "HTTP/1.1 " << res.status << " OK\n";
        out << "Server: superluminal\n";
        for (auto x : res.headers.GetRaw()) {
            out << x << "\n";
        }
        out << "Content-Length: " << res.out.size() << "\n\n";
        out << res.out;

        send(*s, out.data(), out.size(), 0);
    }
    close(*s);
    free(pnewsock);

}

int Server::Start(std::function<HandlerSignature> handle, std::string port, int backlog) {
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

    /* Listen */
    if (listen(sock, backlog) == -1) {
        std::cerr << "Could not listen" << std::endl;
        return 0;
    }

    std::cout << "Starting server on port " << port << std::endl;

    while (1) {
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

                std::thread thread(&Server::Handle, this, safesock);
//                std::thread thread(handle, safesock);

                thread.detach();
//                thread.join();
            }
            else {
                std::cerr << "Malloc error" << std::endl;
            }
        }
    }

    close(sock);

    return 0;
}
