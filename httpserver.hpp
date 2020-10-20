//
// Created by Martin Drewes on 18/10/2020.
//

#ifndef SUPERLUMINAL_HTTPSERVER_HPP
#define SUPERLUMINAL_HTTPSERVER_HPP

#include <iostream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <errno.h>
#include <chrono>
#include <functional>
#include <sstream>


#include "utils.hpp"

class Headers {
    std::map<std::string,std::string> values;
public:
    Headers() {}
    void Add(std::string key,std::string val) {
        values.emplace(to_lower(trim_copy(key)), trim_copy(val));
    }
    void AddRaw(std::string line) {
        if (!line.empty()) {
            std::vector<std::string> parts = split(line, ": ");
            if (parts.size() == 2) {
                Add(parts[0], parts[1]);
            }
        }
    }
    std::vector<std::string> GetRaw() {
        std::vector<std::string> result;
        for (std::pair<std::string, std::string> e : values) {
            result.emplace_back(e.first+": "+e.second);
        }
        return result;
    }
    std::string Get(std::string key, std::string notfound="") {
        std::map<std::string,std::string>::iterator it = values.find(key);
        if(it == values.end()) {
            return notfound;
        }
        return it->second;
    }
};

class Request {
public:
    Headers headers;
    std::string method;
    std::string url;

    Request(std::string data) {
        std::vector<std::string> lines=split(data,'\n');
        if (lines.size()>0) {
                std::vector<std::string> l0 = split(lines[0], ' ');
                if (l0.size()>2) {
                        method = to_upper(trim_copy(l0[0]));
                        url = trim_copy(l0[1]);
                }
        }
        // Using a for loop with iterator
        for(auto it = ++std::begin(lines); it != std::end(lines); ++it) {
            std::cout << "H: " << *it << "\n";
            if (!it->empty()) {
                headers.AddRaw(*it);
            }
        }
    }
};

class Response {
public:
    std::stringstream out;
    Headers headers;
};

using HandlerSignature = void(Request &, Response &);

class Server {
    std::function<HandlerSignature> user_handler;
public:
    void Handle(int*);
    int Start(std::function<HandlerSignature> handle, std::string port, int backlog=10);
};


#endif //SUPERLUMINAL_HTTPSERVER_HPP
