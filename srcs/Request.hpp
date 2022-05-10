#pragma once

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <netinet/in.h>


#define NB_HEADERS 24
#define NL "\r\n"
#define NLSIZE 2

class Request
{
public:
    Request();
    Request(std::string new_request);
    Request(const Request &src);
    Request &operator=(const Request &src);
    ~Request();

    bool        operator==(const Request &c) const;

    std::string get_path() const;
    void        set_path(std::string new_path);
    std::string get_body() const;
    std::string get_method() const;
    std::string get_version() const;
    std::string get_header(std::string key);
    std::string get_request() const;
    bool        ready() const;
    void        add(std::string new_request);
    void        print();
    void        clear();

private:

    std::map<const std::string, std::string>    _m;
    std::string                                 _request;
    std::string                                 _path;
    std::string                                 _method;
    std::string                                 _version;
    std::string                                 _header[NB_HEADERS];
    bool                                        _end;
    bool                                        _valid;
    std::string                                 _body;
    std::string                                 _line;

    bool addp(std::string r);
    void pprint();
    bool get_request_line(std::string r); // Check request line conformitys
    bool get_request_first_line(std::string r); // Check request first line conformitys
    bool is_header(std::string s) const; //verifie si s fait partie des headers
    void init_header_map();
    bool file_exist(const char *file);
};