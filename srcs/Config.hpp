#pragma once

#include <iostream> // For std
#include <vector> // for vector
#include <sys/types.h> // For mkdir
#include <dirent.h> // For mkdir
#include <fstream> // for readInFile

#include "server.hpp"


class Client;

class Server 
{
  public :

    Server();
    Server(const Server &src);
    ~Server();

    Server& operator=(const Server &src);
    bool	operator==(const Server &c) const;

    int                       id;
    std::string               ip;
    std::string               host;
    std::string               port;
    std::string               root;
    std::string               index;
    std::string               error404;
    std::string               client_body_buffer_size;
    bool                      methods[3];
    std::vector<std::string>  cgi;
    std::vector<Server>       loc;
    bool                      autoindex;
    bool                      valid;
    size_t                    lvl;
    std::string               path;
    bool                      alias;
    std::vector<Client>       client;   
};

class Config
{
  public:

    std::vector<Server> server;
    bool                  valid;

    Config();
    Config(const std::string s);
    Config(const Config& src);
    ~Config();
    Config& operator=(const Config &src);

    bool  init(const std::string filename);    
    bool	operator==(const Config &c) const;

  private:

    void    init_server(Server* s);
    bool    get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i);
    bool    get_listen_line(const std::string tmp, Server* serv_tmp);
    bool    error_config_message(const std::string s, const std::string::size_type i, const int a) const;
    bool    get_error_page_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i);
    bool    error_config_message(const std::string s, const std::string::size_type i) const;
    bool    get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp, bool *a, bool *b, size_t calling_lvl, size_t *loc_i);
    bool    get_conf(const std::string s);
    bool    check_server(Server s);
};

std::ostream&	operator<<(std::ostream& ostream, const Server& src);
std::ostream&	operator<<(std::ostream& ostream, const Config& src);