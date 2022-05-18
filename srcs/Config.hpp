#pragma once
#include "server.hpp"

#pragma once

class Client;

class Server 
{
  public :

    Server();
    Server(const Server &src);
    ~Server();

    Server& operator=(const Server &src);
    bool	operator==(const Server &c) const;
  
    int                                                 id; // Server id
    std::string                                         loc_id; // Location id
    std::string                                         ip; // Server adresse ip
    std::string                                         port;
    std::string                                         host; // Server hostname
    std::string                                         root;
    std::vector<std::string>                            index;
    std::map<int, std::string>                          error_page;
    std::string                                         client_max_body_size;
    bool                                                methods[3]; // [0] = GET, [1] = POST, [2] = DELETE
    std::vector<std::string>                            cgi;
    std::vector<Server>                                 loc; // locations
    bool                                                autoindex;
    std::vector<std::pair<std::string, std::string> >   redirect;
    bool                                                valid;
    //---------\/-------------\/---------\/-------------ONLY FOR LOCATIONS                          
    size_t                                              lvl; // if > 0, it's a location
    std::string                                         path; // location path
    std::vector<Client>                                 client;
    std::map<std::string, Server*>                      locations;
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
    bool    init(const std::string filename);    
    bool    get_debug() const {return _debug;}
    void    set_debug() {std::cout << YELLOW << "--debug" << RESET; _debug = 1;}

  private:
    bool                  _debug;

    void    init_server(Server* s);
    bool    get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i);
    char    check_ip_line(const std::string s);
    char    check_port_line(const std::string s);
    // char    get_listen_line(const std::string tmp, Server *serv_tmp);
    char    get_listen_line(const std::string tmp, std::vector<std::pair<std::string, std::string> >*vp);
    bool    error_config_message(const std::string s, const std::string::size_type i, const int a) const;
    bool    get_error_page_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i);
    bool    error_config_message(const std::string s, const std::string::size_type i) const;
    bool    get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp, bool *a, size_t calling_lvl, size_t *loc_i, std::vector<std::pair<std::string, std::string> >*vp, std::map<std::string, Server*> *locations);
    bool    get_conf(const std::string s);
    bool    check_server(Server* s);
    bool    check_location(Server* s, const std::string calling_root);
};

std::ostream&	operator<<(std::ostream& ostream, const Server& src);
std::ostream&	operator<<(std::ostream& ostream, const Config& src);