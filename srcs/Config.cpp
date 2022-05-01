#include "Config.hpp"

bool readinFile(std::string filename, std::string *fileContent)
{
    std::string s;
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs)
    {
        std::cerr << "Not Found " << filename << "." << std::endl;
        return (1);
    }
    getline(ifs, s);
    if (s == "")
    {
        std::cerr << "Empty file." << std::endl;
        ifs.close();
        return (1);
    }
    *fileContent += s;
    while (getline(ifs, s))
    {
        *fileContent += "\n";
        *fileContent += s;
    }
    ifs.close();
    return (0);
}

bool    is_number(char c)
  { return (c >= '0' && c <= '9'); }

bool is_space(const char c)
  { return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'); }

bool is_blanck(const char c)
  { return (is_space(c) || c == '\n'); }

void pass_space(const std::string s, std::string::size_type *i)
{
  while (*i < s.length() && is_space(s[*i]))
    *i += 1;
}

void pass_blanck(const std::string s, std::string::size_type *i, std::string::size_type *line_i)
{
  while (*i < s.length() && is_blanck(s[*i]))
  {
    if (s[*i] == '\n')
      (*line_i) += 1;
    *i += 1;
  }
}

void pass_not_blanck(const std::string s, std::string::size_type *i)
{
  while (*i < s.length() && !is_blanck(s[*i]))
    *i += 1;
}

bool s_a_have_b(const std::string a, const std::string::size_type i, const std::string b)
{
    // std::cout << "**" << a.substr(i, b.length()) << " == " << b << "?" << std::endl;
  return (a.length() >= (b.length() + i) && a.substr(i, b.length()) == b);
}

bool error_msg(std::string msg)
{
  std::cerr << msg << std::endl;
  return 0;
}

bool is_directory(std::string path)
{
  DIR *ptr;
  if ((ptr = opendir(path.c_str())) == NULL)
    return (0);
  else
  {
    closedir(ptr);
    return (1);
  }
}

Config::Config() : valid(0) {}

Config::Config(const Config& src) : servers(src.servers), valid(src.valid) {}

Config::~Config() {}

Config& Config::operator=(const Config &src)
{
    // if (*this == src)
    //     return *this;
    if (servers.size())
        servers.clear();
    servers = src.servers;
    valid = src.valid;
    return *this;
}

void    Config::init_server(t_server* s)
{
    // if (!s->locations.empty())
        // s->locations.clear();    
    s->host = "";
    s->server_name = "";
    s->port = "";
    s->default_folder = "";
    s->default_page = "";
    s->page404 = "";
    s->max_body_size = "";
    s->autoindex = 0;
    s->valid = 0;
}

bool    Config::init(const std::string filename)
{
    std::string data;
    std::string::size_type a, b;
    if (filename.substr(filename.length() - 5, 5) != ".conf" || readinFile(filename, &data))//verifie que le fichier existe et est en .conf
        return error_msg("Error: Wrong config file extension.") + 1;
    a = data.find('#'); // supprime les commentaires du fichier config
    while (a != std::string::npos)
    {
        b = data.find('\n', a);
        if (b != std::string::npos)
            data.erase(a, b - a);
        else
            data.erase(a, data.length() - a);
        a = data.find('#');
    }
    return get_conf(data);
}

bool    Config::error_config_message(const std::string s, const std::string::size_type i)
{
    std::string::size_type p = s.find("\n", i);
    if (p == std::string::npos)
        std::cout << "Error: config file: line: " << i << "." << std::endl;
    else
    {
        std::string tmp = s.substr(i, p - i);
        std::cout << "Error: config file: line: " << i << "." << std::endl;
    }
    return (0);
}

// bool    Config::pass_location_line(std::string s, std::string::size_type *i, std::string::size_type *line_i)
// {
//     pass_blanck(s, i, line_i);
//     // if (s_have_b(s.substr(0,)))
//     if (s_a_have_b(s, *i, "location"))
//     {
//         *i += 8;
//         pass_blanck(s, i, line_i);
//         if (s[*i] != '{')
//         {
//             std::cout << "Error: config file: line: " << *line_i << "." << std::endl;
//             return (1);
//         }
//     }
// }

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, t_server *serv_tmp)
{
    std::string::size_type  p;
    const int               nb_serv_types = 8;
    std::string             serv_type[nb_serv_types] = {"host", "server_name", "listen", "default_folder", "default_page"
                                                        , "max_body_size", "404_page", "autoindex"};
    std::string tmp;

    pass_blanck(s, i, line_i);
    // if (s_a_have_b(s, *i, "location")) // PAS FINI
    // {
        // *i += 8;
        // pass_blanck(s, i, line_i);
        // if (s[*i] != '{')
        // {
            // std::cout << "Error: config file: line: " << *line_i << "." << std::endl;
            // return (1);
        // }
        // return pass_location_line(s, i, line_i);
    // }
    // else
    // {
        for (int o = 0; o < nb_serv_types; o++)
        {
            if (s_a_have_b(s, *i, serv_type[o]))
            {
                *i += serv_type[o].length();
                if (*i >= s.length() || !is_space(s[*i]))
                    return (error_config_message(s, *line_i) + 1);
                pass_space(s, i);
                if (s.length() <= *i || is_blanck(s[*i]))
                    return (error_config_message(s, *line_i) + 1);
                p = *i;
                pass_not_blanck(s, i);
                tmp = s.substr(p, *i - p);
                switch (o)
                {
                case (0):
                    serv_tmp->host = tmp;
                    break;
                case (1):
                    serv_tmp->server_name = tmp;
                    break;
                case (2):
                    if (tmp.length() != 4 || !is_number(tmp[0]) || !is_number(tmp[1]) || !is_number(tmp[2]) || !is_number(tmp[3]))
                        return (error_config_message(s, *line_i) + 1);
                    serv_tmp->port = tmp;
                    break;
                case (3):
                    serv_tmp->default_folder = tmp;
                    break;
                case (4):
                    serv_tmp->default_page = tmp;
                    break;
                case (5):
                    serv_tmp->max_body_size = tmp;
                    break;
                case (6):
                    serv_tmp->page404 = tmp;
                    break;
                case (7):
                    if (tmp != "on" && tmp != "off")
                        return (error_config_message(s, *line_i) + 1);
                    serv_tmp->autoindex = (tmp == "on");
                    break;
                }
                pass_blanck(s, i, line_i);
                return (0);
            }
        }
        std::cout << "Error: config file: line: " << *line_i << "." << std::endl;
        return (1);
    // }
}

bool Config::get_conf(const std::string s)
{
    std::string::size_type line_i = 1, i = 0, serv_i = 0;
    t_server serv_tmp;
    if (s.empty())
        return error_msg("Error: Empty config file.") + 1;
    pass_blanck(s, &i, &line_i);
    if (s.length() <= i)
        return error_msg("Error: Empty config file") + 1;
    if (!s_a_have_b(s, i, "server"))
        return error_config_message(s, line_i) + 1;
    while (1)
    {
        i += 6;
        pass_blanck(s, &i, &line_i);
        if (s.length() <= i || s[i] != '{')
            return error_config_message(s, line_i) + 1;
        i += 1;
        init_server(&serv_tmp);
        while (i < s.length() && s[i] != '}')
        {
            if (get_server_line(s, &i, &line_i, &serv_tmp))
                return (1);
        }
        if (i >= s.length() || s[i] != '}')
            return error_config_message(s, line_i) + 1;
        i += 1;
        servers.push_back(serv_tmp);
        pass_blanck(s, &i, &line_i);
        if (s.length() == i)
            break;
        if (!s_a_have_b(s, i, "server"))
            return error_msg("Error: config file: line: ") + 1;
    }
    valid = 1;
    return 0;
}

// bool	Config::operator==(const Config& c) const { return (servers == c.servers && valid == c.valid); }

std::ostream&	operator<<(std::ostream& ostream, const Config& src)
{
    for (std::string::size_type i = 0; i < src.servers.size(); i++)
    {
        ostream << WHITE << "server " << i + 1 << ":" << RESET << std::endl
        << "\t host: " << WHITE << src.servers[i].host << RESET << std::endl
        << "\t server name: " << WHITE << src.servers[i].server_name << RESET << std::endl
        << "\t listen: " << WHITE << src.servers[i].port << RESET << std::endl
        << "\t default page: " << WHITE << src.servers[i].default_page << RESET << std::endl
        << "\t default folder: " << WHITE << src.servers[i].default_folder << RESET << std::endl
        << "\t 404 page: " << WHITE << src.servers[i].page404 << RESET << std::endl
        << "\t max body size: " << WHITE << src.servers[i].max_body_size << RESET << std::endl
        << "\t autoindex: " << WHITE << (src.servers[i].autoindex ? "on" : "off") << RESET << std::endl
        << "\t status: " << WHITE << (src.servers[i].valid ? GREEN : RED) << (src.servers[i].valid ? "OK" : "KO") << RESET << std::endl;
    }
    return ostream;
}