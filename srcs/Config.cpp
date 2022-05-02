#include "Config.hpp"
#include "server.hpp"

Server::Server() : ip_address(""), server_name(""), port(""), root(""), index("")
                    , page404(""), client_body_buffer_size(""), autoindex(false), valid(false) 
{ methods[0] = false; methods[1] = false; methods[2] = false;}

Server::Server(const Server &src) : ip_address(src.ip_address), server_name(src.server_name), port(src.port)
                            , root(src.root), index(src.index)
                            , page404(src.page404), client_body_buffer_size(src.client_body_buffer_size), autoindex(src.autoindex)
                            , valid(src.valid) {
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];}

Server::~Server() {}

Server& Server::operator=(const Server &src)
{
    ip_address = src.ip_address;
    server_name = src.server_name;
    port = src.port;
    root = src.root;
    index = src.index;
    page404 = src.page404;
    client_body_buffer_size = src.client_body_buffer_size;
    autoindex = src.autoindex;
    valid = src.valid;
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
    return (*this);
}

bool	Server::operator==(const Server &c) const
    { return (ip_address == c.ip_address && server_name == c.server_name && port == c.port && root == c.root
        && index == c.index && page404 == c.page404 && client_body_buffer_size == c.client_body_buffer_size
        && autoindex == c.autoindex && valid == c.valid && methods[0] == c.methods[0]
        && methods[1] == c.methods[1] && methods[2] == c.methods[2]); }


Config::Config() : valid(0) {}

Config::Config(const std::string filename) : valid(0) { init(filename); }

Config::Config(const Config& src) : servers(src.servers), valid(src.valid) {}

Config::~Config() {}

Config& Config::operator=(const Config &src)
{
    if (*this == src)
        return *this;
    if (servers.size())
        servers.clear();
    servers = src.servers;
    valid = src.valid;
    return *this;
}

void    Config::init_server(Server* s)
{
    // if (!s->locations.empty())
        // s->locations.clear();    
    s->ip_address = "";
    s->server_name = "";
    s->port = "";
    s->root = "";
    s->index = "";
    s->page404 = "";
    s->client_body_buffer_size = "";
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

bool    Config::error_config_message(const std::string s, const std::string::size_type i) const
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

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp)
{
    std::string::size_type  p;
    const int               nb_serv_types = 8;
    std::string             serv_type[nb_serv_types] = {"server_name", "listen", "root", "index"
                                                        , "client_body_buffer_size", "404_page"
                                                        , "autoindex", "allow_methods"};
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
                    serv_tmp->server_name = tmp;
                    break;
                case (1):
                    for (int j = 0; j < 4; j++)
                    {
                        while (p < s.length() && is_number(s[p]))
                        {
                            serv_tmp->ip_address += s[p];
                            p++;
                        }
                        if (p >= s.length() || (j != 3 && s[p] != '.'))
                            return (error_config_message(s, *line_i) + 1);
                        if (j != 3)
                        {
                            serv_tmp->ip_address += '.';
                            p++;
                        }
                    }
                    if (s[p] != ':')
                        return (error_config_message(s, *line_i) + 1);
                    p++;
                    if (s.length() <= p + 4)
                        return (error_config_message(s, *line_i) + 1);
                    if (!is_number(s[p]) || !is_number(s[p + 1]) || !is_number(s[p + 2]) || !is_number(s[p + 3]))
                        return (error_config_message(s, *line_i) + 1);
                    serv_tmp->port = s.substr (p, 4);
                    break;
                case (2):
                    if (!is_directory(tmp))
                    {
                        std::cerr << "Error: Config file line: " << *line_i << ": Wrong root directory path.";
                        return 1;
                    }
                    serv_tmp->root = tmp;
                    break;
                case (3):
                    serv_tmp->index = tmp;
                    break;
                case (4):
                    serv_tmp->client_body_buffer_size = tmp;
                    break;
                case (5):
                    serv_tmp->page404 = tmp;
                    break;
                case (6):
                    if (tmp != "on" && tmp != "off")
                        return (error_config_message(s, *line_i) + 1);
                    serv_tmp->autoindex = (tmp == "on");
                    break;
                case (7):
                    *i = p;
                    if (!((s.length() > *i + 3 && s.substr(*i, 3) == "GET")
                    || (s.length() > *i + 4 && s.substr(*i, 3) == "POST")
                    || (s.length() > *i + 6 && s.substr(*i, 3) == "DELETE")))
                        return (error_config_message(s, *line_i) + 1);
                    for (int j = 0; j < 3; j++)
                    {
                        if (s.length() > *i + 3 && s.substr(*i, 3) == "GET")
                        {
                            if (serv_tmp->methods[0])
                                return (error_config_message(s, *line_i) + 1);
                            else
                                serv_tmp->methods[0] = 1;
                        }
                        else if (s.length() > *i + 4 && s.substr(*i, 4) == "POST")
                        {
                            if (serv_tmp->methods[1])
                                return (error_config_message(s, *line_i) + 1);
                            else
                                serv_tmp->methods[1] = 1;
                        }
                        else if (s.length() > *i + 6 && s.substr(*i, 6) == "DELETE")
                        {
                            if (serv_tmp->methods[2])
                                return (error_config_message(s, *line_i) + 1);
                            else
                                serv_tmp->methods[2] = 1;
                        }
                        pass_not_blanck(s, i);
                        pass_space(s, i);
                    }
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
    std::string::size_type line_i = 1, i = 0;
    Server serv_tmp;
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
        if (serv_tmp.ip_address.size() && serv_tmp.port.size() &&
        (serv_tmp.methods[0] || serv_tmp.methods[1] || serv_tmp.methods[2]))
           serv_tmp.valid = 1;
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

bool	Config::operator==(const Config& c) const
    { return (servers == c.servers && valid == c.valid); }

std::ostream&	operator<<(std::ostream& ostream, const Config& src)
{
    for (std::string::size_type i = 0; i < src.servers.size(); i++)
    {
        ostream << WHITE << "server " << i + 1 << ":" << RESET << std::endl
        << "\t ip_address: " << WHITE << src.servers[i].ip_address << RESET << std::endl
        << "\t server name: " << WHITE << src.servers[i].server_name << RESET << std::endl
        << "\t listen: " << WHITE << src.servers[i].port << RESET << std::endl
        << "\t default page: " << WHITE << src.servers[i].index << RESET << std::endl
        << "\t default folder: " << WHITE << src.servers[i].root << RESET << std::endl
        << "\t 404 page: " << WHITE << src.servers[i].page404 << RESET << std::endl
        << "\t max body size: " << WHITE << src.servers[i].client_body_buffer_size << RESET << std::endl
        << "\t autoindex: " << WHITE << (src.servers[i].autoindex ? "on" : "off") << RESET << std::endl
        << "\t allowed methods: " << WHITE << (src.servers[i].methods[0] ? "GET " : "") << (src.servers[i].methods[1] ? "POST " : "") << (src.servers[i].methods[2] ? "DELETE " : "") << RESET << std::endl
        << "\t status: " << WHITE << (src.servers[i].valid ? GREEN : RED) << (src.servers[i].valid ? "OK" : "KO") << RESET << std::endl;
    }
    return ostream;
}