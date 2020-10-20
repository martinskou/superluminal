//
// Created by Martin Drewes on 18/10/2020.
//

#ifndef SUPERLUMINAL_HTTPSERVER_HPP
#define SUPERLUMINAL_HTTPSERVER_HPP

#include <iostream>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>

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
            // std::cout << "H: " << *it << "\n";
            if (!it->empty()) {
                headers.AddRaw(*it);
            }
        }
    }
};

class ByteBuffer {
public:
    std::vector<std::uint8_t> out;

    void write(std::string text) {
        for (auto c : text) {
            out.push_back(c);
        }
    }

    void writefile(std::string filename) {

        std::streampos size;
        char * memblock;

        std::ifstream file ("/Users/martin/Downloads/IMG_1794.JPG", std::ios::in|std::ios::binary|std::ios::ate);
        if (file.is_open())
        {
            size = file.tellg();
            memblock = new char [size];
            file.seekg (0, std::ios::beg);
            file.read (memblock, size);
            file.close();

            out.reserve(size);
            for (int i=0;i<size;i++) {
                out.push_back(memblock[i]);
            }

            delete[] memblock;
        }
    }

    ByteBuffer& operator<<(const char* bb) {
        for (int i=0;i<strlen(bb);i++) {
            out.push_back(bb[i]);
        }
        return *this;
    }

    ByteBuffer& operator<<(const int& bb) {
        std::string bbs=std::to_string(bb);
        for (auto c : bbs) {
            out.push_back(c);
        }
        return *this;
    }

    ByteBuffer& operator<<(const std::string& bb) {
        for (auto c : bb) {
            out.push_back(c);
        }
        return *this;
    }

    ByteBuffer& operator<<(const ByteBuffer& bb) {
        for (auto c : bb.out) {
            out.push_back(c);
        }
        return *this;
    }

    std::size_t size() {
        return out.size();
    }

    std::uint8_t* data() {
        return out.data();
    }

};



class Response {
public:
    ByteBuffer out;
    Headers headers;
    int status=200;

};



using HandlerSignature = void(Request &, Response &);

class Server {
    std::function<HandlerSignature> user_handler;
public:
    void Handle(int*);
    int Start(std::function<HandlerSignature> handle, std::string port, int backlog=10);
};


#endif //SUPERLUMINAL_HTTPSERVER_HPP
