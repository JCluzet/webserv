#pragma once

#include <iostream> // For std
#include <vector> // for vector
#include <sys/types.h> // For mkdir
#include <dirent.h> // For mkdir
#include <fstream> // for readInFile

// struct t_location
// {
  // std::string path;
  // std::string root;
  // std::string index;
  // bool exist;
  // bool valid;
  // std::vector<t_locations> l;
  // int lvl;
// };

class Server 
{
  // std::vector<t_location> locations;
  public:

    Server();
    Server(const Server &src);
    ~Server();

    Server& operator=(const Server &src);
    bool	operator==(const Server &c) const;

    std::string host;
    std::string server_name;
    std::string port;
    std::string default_folder;
    std::string default_page;
    std::string page404;
    std::string max_body_size;
    bool autoindex;
    bool valid;
};



class Config
{
  public:

    std::vector<Server> servers;
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
    bool    error_config_message(const std::string s, const std::string::size_type i);
    // bool pass_location_line(std::string s, std::string::size_type *i, std::string::size_type *line_i); // PAS FINIT
    bool    get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp);
    bool    get_conf(const std::string s);
};

std::ostream&	operator<<(std::ostream& ostream, const Config& src);