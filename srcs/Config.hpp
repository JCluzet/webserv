#pragma once
#include "server.hpp"

#pragma once

class Client;

struct  Redirect
{
  public:
    std::string   redirect1;
    std::string   redirect2;
    bool          permanent;

    Redirect() : redirect1(""), redirect2(""), permanent(false) {};
    Redirect(std::string red1, std::string red2, bool perm) : redirect1(red1), redirect2(red2), permanent(perm) {};
    Redirect(const Redirect &src) : redirect1(src.redirect1), redirect2(src.redirect2), permanent(src.permanent) {};
    ~Redirect() {};

    Redirect& operator=(const Redirect &src) { redirect1 = src.redirect1; redirect2 = src.redirect2; permanent = src.permanent; return (*this); }; 
};

class Server 
{
  public :

    Server();
    Server(const Server &src);
    ~Server();

    Server& operator=(const Server &src);
    bool	operator==(const Server &c) const;
    Server*     location(const std::string s);
  
    bool cb;

    int                                                 id; // Server id
    std::string                                         loc_id; // Location id
    std::string                                         ip; // Server adresse ip
    std::string                                         port;
    std::string                                         host; // Server hostname
    std::string                                         root;
    std::string                                         o_root;
    std::vector<std::string>                            index;
    std::map<int, std::string>                          error_page;
    std::string                                         client_max_body_size;
    bool                                                methods[3]; // [0] = GET, [1] = POST, [2] = DELETE
    std::vector<std::pair<std::string, std::string> >   cgi; // first = extension, second = cgi filename
    std::string                                         cgi_bin; // cgi path  
    std::vector<Server>                                 loc; // locations
    bool                                                autoindex;
    std::vector<Redirect>                               redirect;
    bool                                                alias;
    //---------\/-------------\/---------\/-------------ONLY FOR LOCATIONS                          
    size_t                                              lvl; // if > 0, it's a location
    std::string                                         path; // location path
    std::vector<Client>                                 client;
    std::map<std::string, Server>                      locations;
};

class Config
{
  public:

    std::vector<Server> server;

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
    bool    get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i, bool add);
    char    check_ip_line(const std::string s);
    char    check_port_line(const std::string s);
    char    get_listen_line(const std::string tmp, std::vector<std::pair<std::string, std::string> >*vp);
    bool    error_config_message(const std::string s, const std::string::size_type i, const int a) const;
    bool    get_error_page_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i);
    bool    error_config_message(const std::string s, const std::string::size_type i) const;
    bool    get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp, size_t calling_lvl, size_t *loc_i, std::vector<std::pair<std::string, std::string> >*vp);
    bool    get_conf(const std::string s);
    bool    check_server(Server* s);
    void    init_loc_tmp(Server *dst, Server src);
    bool    check_location(Server *s, Server parent, Server *original);
};

std::ostream&	operator<<(std::ostream& ostream, const Server& src);
std::ostream&	operator<<(std::ostream& ostream, const Config& src);