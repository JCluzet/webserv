#pragma once

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define  WHITE "\033[1;37m"
#define RESET "\033[0m"
#include <iostream>
#include <vector>
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

struct t_server
{
  // std::vector<t_location> locations;
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

bool            readinFile(std::string filename, std::string *fileContent);
bool            is_number(char c);
bool            is_space(const char c);
bool            is_blanck(const char c);
std::string     intToStr(int n);
void            pass_space(const std::string s, std::string::size_type *i);
void            pass_blanck(const std::string s, std::string::size_type *i, std::string::size_type *line_i);
void            pass_not_blanck(const std::string s, std::string::size_type *i);
bool            s_a_have_b(const std::string a, const std::string::size_type i, const std::string b);
bool            error_msg(std::string msg);
bool            is_directory(std::string path);

class Config
{
  public:

    std::vector<t_server> servers;
    bool                  valid;

    Config();
    Config(const Config& src);
    ~Config();
    Config& operator=(const Config &src);

    bool  init(const std::string filename);    
    // bool	operator==(const Config &c) const;

  private:

    void    init_server(t_server* s);
    bool    error_config_message(const std::string s, const std::string::size_type i);
    // bool pass_location_line(std::string s, std::string::size_type *i, std::string::size_type *line_i); // PAS FINIT
    bool    get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, t_server *serv_tmp);
    bool    get_conf(const std::string s);
};

std::ostream&	operator<<(std::ostream& ostream, const Config& src);