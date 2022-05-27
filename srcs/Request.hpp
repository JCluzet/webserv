#pragma once

#include "server.hpp"
#include "Config.hpp"
#include "Client.hpp"

class Server;
class Client;

#define NB_HEADERS 21
#define NL "\r\n"
#define NLSIZE 2

class Request
{
public:
    // constructors
    Request();

    Request(const Request &src);

    Request &operator=(const Request &src);

    ~Request();

    // getter
    std::string get_path() const;

    std::string get_path_o() const;

    void        set_path(std::string newpath);

    std::string get_body() const;

    std::string get_method() const;

    std::string get_version() const;

    std::string get_header(std::string key);

    std::string get_request() const;

    bool ready() const;

    int is_valid() const;

    void add(Client* client, Server* conf_o, std::string new_request);

    void clear();
    

    private:
        std::map<const std::string, std::string> _m;
        std::string _request;
        std::string _path;
        std::string _method;
        std::string _version;
        std::string _header[NB_HEADERS];
        bool        _end;
        int         _valid;
        std::string _body;
        std::string _line;
        int         _chunked;
        std::string _path_o;

        int addp(Client* client, Server* conf_o, std::string r);

        int checkHeader(Client* client, Server* conf_o, std::string r);

        int	checkHost(std::string ip, std::string port, std::vector<std::string> server_name);

        bool get_request_line(std::string r);

        bool get_request_first_line(std::string r); // Check request first line conformitys

        void init_header_map();

        bool is_header(std::string s) const;

        bool file_exist(const char *file);

    };