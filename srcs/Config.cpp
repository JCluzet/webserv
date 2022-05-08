#include "Config.hpp"
#include "server.hpp"

Server::Server() : id(0), ip(""), host(""), port(""), root(""), index("")
                    , error404(""), client_body_buffer_size(""), autoindex(0), valid(0)
                    , lvl(0), path(""), alias(0), client()
{ methods[0] = 0; methods[1] = 0; methods[2] = 0;}

Server::Server(const Server &src) : id(src.id), ip(src.ip), host(src.host), port(src.port)
                            , root(src.root), index(src.index), error404(src.error404)
                            , client_body_buffer_size(src.client_body_buffer_size)
                            , cgi(src.cgi), loc(src.loc), autoindex(src.autoindex)
                            , valid(src.valid), lvl(src.lvl), path(src.path), alias(src.alias)
                            , client(src.client){
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];}

Server::~Server() {}

Server& Server::operator=(const Server &src)
{
    id = src.id;
    ip = src.ip;
    cgi = src.cgi;
    host = src.host;
    port = src.port;
    root = src.root;
    index = src.index;
    error404 = src.error404;
    client_body_buffer_size = src.client_body_buffer_size;
    autoindex = src.autoindex;
    valid = src.valid;
    client = src.client;
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
    alias = src.alias;
    lvl = src.lvl;
    path = src.path;
    loc = src.loc;
    client = src.client;
    return *this;
}

bool	Server::operator==(const Server &c) const
    { return (cgi == c.cgi && id == c.id && ip == c.ip && host == c.host && port == c.port && root == c.root
        && index == c.index && error404 == c.error404 && client_body_buffer_size == c.client_body_buffer_size
        && autoindex == c.autoindex && valid == c.valid && methods[0] == c.methods[0]
        && methods[1] == c.methods[1] && methods[2] == c.methods[2] && alias == c.alias
        && lvl == c.lvl && path == c.path && loc == c.loc && client == c.client); }

//CONFIG
Config::Config() : valid(0) {}

Config::Config(const std::string filename) : valid(0) { init(filename); }

Config::Config(const Config& src) : server(src.server), valid(src.valid) {}

Config::~Config() {}

Config& Config::operator=(const Config &src)
{
    if (*this == src)
        return *this;
    if (server.size())
        server.clear();
    server = src.server;
    valid = src.valid;
    return *this;
}


void    Config::init_server(Server* s)
{
    s->id = 0;
    s->ip = "";
    s->host = "";
    s->port = "";
    s->root = "";
    s->index = "";
    s->error404 = "";
    s->client_body_buffer_size = "";
    s->cgi.clear();
    s->methods[0] = 0;
    s->methods[1] = 0;
    s->methods[2] = 0;
    s->autoindex = 0;
    s->loc.clear();
    s->valid = 0;
    s->alias = 0;
    s->lvl = 0;
    s->path = "";
    s->client.clear();
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
    for (std::string::size_type i = 0; i < data.length(); i++)
        if ((data[i] == ';' || data[i] == '}') && i != 0 && !is_blanck(data[i - 1]))
            data.insert(i, " "); 
       return get_conf(data);
}

bool    Config::error_config_message(const std::string s, const std::string::size_type i, const int a) const
{
    std::string::size_type p = s.find("\n", i);
    if (p == std::string::npos)
        std::cout << "Error(" << a << "): config file: line: " << i << "." << std::endl;
    else
    {
        std::string tmp = s.substr(i, p - i);
        std::cout << "Error(" << a << "): config file: line: " << i << "." << std::endl;
    }
    return (0);
}

bool    Config::get_listen_line(const std::string tmp, Server* serv_tmp)
{
    std::string::size_type i = 0;
    if (tmp.find(":") != std::string::npos)
    {
        for (int j = 0; j < 4; j++)
        {
            while (i < tmp.length() && is_number(tmp[i]))
            {
                serv_tmp->ip += tmp[i];
                i++;
            }
            if (i >= tmp.length() || (j != 3 && tmp[i] != '.'))
            {
                std::cerr << "Error: config file: IP adress (" << tmp.substr(0, tmp.find(":")) << ") is not valid." << std::endl;
                return 1;
            }
            if (j != 3)
            {
                serv_tmp->ip += '.';
                i++;
            }
        }
        if (tmp[i] != ':')
        {
            std::cerr << "Error: config file: IP adress (" << tmp.substr(0, tmp.find(":")) << ") is not valid." << std::endl;
            return 1;
        }
        i++;
    }
    if (tmp.length() != i + 4 || !is_number(tmp[i]) || !is_number(tmp[i + 1]) || !is_number(tmp[i + 2]) || !is_number(tmp[i + 3]))
    {
        std::cerr << "Error: config file: port can't be (" << tmp.substr(i, tmp.length() - i) << ") is not valid." << std::endl;
        return 1;
    }
    serv_tmp->port = tmp.substr (i, 4);
    return 0;
}

bool    Config::get_error_page_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i)
{
    std::string tmp = "", tmp2 = "";

    while (!is_blanck(s[*i]))
    {
        tmp += s[*i];
        *i += 1;
    }
    if (tmp != "404")
    {
        std::cerr << "Error: config file: error page can't be (" << tmp << ")." << std::endl;
        return 1;
    }
    if (serv_tmp->error404.length())
        return (error_config_message(s, *line_i, 1) + 1);
    if (*i >= s.length() ||!is_blanck(s[*i]))
        return (error_config_message(s, *line_i, 2) + 1);
    pass_blanck(s, i, line_i);
    while (*i <= s.length() && !is_blanck(s[*i]))
    {
        tmp2 += s[*i];
        *i += 1;
    }
    if (!tmp2.length())
        return (error_config_message(s, *line_i, 3) + 1);
    serv_tmp->error404 = tmp2;
    return 0;
}

bool    Config::get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i)
{
    std::string tmp;
    std::string::size_type k = 0;
    for (int j = 0; j < 3; j++)
    {
        pass_blanck(s, i, line_i);
        k = *i;
        tmp = "";
        for (;*i < s.length() && !is_blanck(s[*i]); *i += 1)
            tmp += s[*i];
        if (tmp == "GET")
        {
            if (serv_tmp->methods[0])
                return (error_config_message(s, *line_i, 4) + 1);
            serv_tmp->methods[0] = 1;
        }
        else if (tmp == "POST")
        {
            if (serv_tmp->methods[1])
                return (error_config_message(s, *line_i, 5) + 1);
            serv_tmp->methods[1] = 1;
        }
        else if (tmp == "DELETE")
        {
            if (serv_tmp->methods[2])
                return (error_config_message(s, *line_i, 6) + 1);
            serv_tmp->methods[2] = 1;
        }
        else if (!j)
            return (error_config_message(s, *line_i, 7) + 1);
        else
        {
            *i = k;
            return 0;
        }
    }
    return 0;
}

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp, bool *a, bool *b, size_t calling_lvl, size_t *i_loc)
{
    std::string::size_type  p;
    const int               nb_serv_types = 11;
    std::string             serv_type[nb_serv_types] = {"server_name", "listen", "root", "index"
                                                        , "client_body_buffer_size", "error_page"
                                                        , "autoindex", "allow_methods", "limit_except"
                                                        , "cgi_pass", "alias"};
    std::string tmp;
    Server      loc_tmp;
    pass_blanck(s, i, line_i);
    if (s_a_have_b(s, *i, "location"))
    {
        *i += 8;
        pass_blanck(s, i, line_i);
        p = *i;
        pass_not_blanck(s, i);
        loc_tmp.path = s.substr(p, *i - p);
        if (!loc_tmp.path.length())
            return (error_config_message(s, *line_i, 8) + 1);
        pass_blanck(s, i, line_i);
        if (s[*i] != '{')
            return (error_config_message(s, *line_i, 9) + 1);
        *i += 1;
        for (bool c = 0, d = 0; *i <= s.length() && s[*i] != '}';)
            if (get_server_line(s, i, line_i, &loc_tmp, &c, &d, calling_lvl + 1, i_loc))
                return 1;
        if (*i <= s.length() && s[*i] != '}')
            return (error_config_message(s, *line_i, 10) + 1);
        *i += 1;
        pass_blanck(s, i, line_i);
        loc_tmp.id = *i_loc;
        *i_loc += 1;
        loc_tmp.lvl = calling_lvl + 1;
        serv_tmp->loc.push_back(loc_tmp);
        return 0;
    }
    else
    {
        for (int o = 0; o < nb_serv_types; o++)
        {
            if (s_a_have_b(s, *i, serv_type[o]))
            {
                *i += serv_type[o].length();
                if (*i >= s.length() || !is_blanck(s[*i]))
                    return (error_config_message(s, *line_i, 11) + 1);
                pass_blanck(s, i, line_i);
                if (s.length() <= *i)
                    return (error_config_message(s, *line_i, 12) + 1);
                o -= (o == 8 ? 1 : 0);
                switch (o)
                {
                case (0):
                    if (serv_tmp->host.length())
                        return (error_config_message(s, *line_i, 13) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->host = tmp;
                    break;
                case (1):
                    if (serv_tmp->port.length())
                        return (error_config_message(s, *line_i, 14) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (get_listen_line(tmp, serv_tmp))
                        return 1;
                    break;
                case (2):
                    if (serv_tmp->root.length())
                        return (error_config_message(s, *line_i, 15) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->root = tmp;
                    break;
                case (3):
                    if (serv_tmp->index.length())
                        return (error_config_message(s, *line_i, 16) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->index = tmp;
                    break;
                case (4):
                    if (serv_tmp->client_body_buffer_size.length())
                        return (error_config_message(s, *line_i, 17) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->client_body_buffer_size = tmp;
                    break;
                case (5):
                    if (get_error_page_line(s, serv_tmp, i, line_i))
                        return 1;
                    break;
                case (6):
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (*a || (tmp != "on" && tmp != "off"))
                        return (error_config_message(s, *line_i, 18) + 1);
                    *a = 1;
                    serv_tmp->autoindex = (tmp == "on");
                    break;
                case (7):
                    if (*b)
                        return (error_config_message(s, *line_i, 19) + 1);
                    if (get_methods_line(s, serv_tmp, i, line_i))
                        return 1;
                    *b = 1;
                    break;
                case (9):
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->cgi.push_back(tmp);
                    break;
                case (10):
                    if (!calling_lvl || serv_tmp->alias)
                        return (error_config_message(s, *line_i, 20) + 1);
                    serv_tmp->alias = 1;
                    break;
                }
                pass_blanck(s, i, line_i);
                if (*i >= s.length() || s[*i] != ';')
                    return (error_config_message(s, *line_i, 21) + 1);
                *i += 1;
                pass_blanck(s, i, line_i);
                return (0);
            }
        }
        return error_config_message(s, *line_i, 22) + 1;
    }
}

bool Config::check_server(Server s)
{
    if (s.port.empty())
    {
        std::cerr << "Error: server " << s.id << ": need a port" << std::endl;
        return 1;
    }
    if (s.root.empty())
    {
        std::cerr << "Error: server " << s.id << ": need a root" << std::endl;
        return 1;
    }
    if (!is_directory(s.root))
    {
        std::cerr << "Error: server " << s.id << ": root directory path is wrong." << std::endl;
        return 1;
    }
    return 0;
}

bool Config::get_conf(const std::string s)
{
    std::string::size_type line_i = 1, i = 0, i_serv = 1, i_loc = 1;
    Server serv_tmp;
    if (s.empty())
        return error_msg("Error: Empty config file.") + 1;
    pass_blanck(s, &i, &line_i);
    if (s.length() <= i)
        return error_msg("Error: Empty config file") + 1;
    if (!s_a_have_b(s, i, "server"))
        return error_config_message(s, line_i, 23) + 1;
    while (1)
    {
        i += 6;
        pass_blanck(s, &i, &line_i);
        if (s.length() <= i || s[i] != '{')
            return error_config_message(s, line_i, 24) + 1;
        i += 1;
        init_server(&serv_tmp);
        for (bool a = 0, b = 0; i < s.length() && s[i] != '}';)
        {
            if (get_server_line(s, &i, &line_i, &serv_tmp, &a, &b, 0, &i_loc))
                return (1);
        }
        if (i >= s.length() || s[i] != '}')
            return error_config_message(s, line_i, 25) + 1;
        i += 1;
        serv_tmp.id = i_serv;
        i_serv++;
        if (!(serv_tmp.valid = (check_server(serv_tmp) ? 0 : 1)))
            return 1;
        server.push_back(serv_tmp);
        i_loc = 1;
        pass_blanck(s, &i, &line_i);
        if (s.length() == i)
            break;
        if (!s_a_have_b(s, i, "server"))
            return error_config_message(s, line_i, 26) + 1;
    }
    valid = 1;
    return 0;
}

bool	Config::operator==(const Config& c) const
    { return (server == c.server && valid == c.valid); }

std::ostream&	operator<<(std::ostream& ostream, const Server& src)
{
    if (!src.lvl)
        ostream << WHITE << "server " << src.id << ":" << std::endl;
    else
    {
        for (size_t i = 0; i < src.lvl; i++)
            ostream << "\t";
        ostream << WHITE << "location : " << RESET << src.path << std::endl;
    }
    if (src.alias)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "alias" << std::endl;
    }
    if (src.host.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "host: " << RESET << src.host << std::endl;
    }
    if (src.ip.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "ip: " << RESET << src.ip << std::endl;
    }
    if (src.port.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "port: " << RESET << src.port << std::endl;
    }
    if (src.index.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "index : " << RESET << src.index << std::endl;
    }
    if (src.root.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "root: " << RESET << src.root << std::endl;
    }
    if (src.error404.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "404 page: " << RESET << src.error404 << std::endl;
    }
    if (src.client_body_buffer_size.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "client body size buffer : " << RESET << src.client_body_buffer_size << std::endl;
    }
    if (src.autoindex)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "autoindex: " << RESET << (src.autoindex ? "on" : "off") << std::endl;
    }
    if (src.methods[0] || src.methods[1] || src.methods[2])
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "allow methods: " << RESET << (src.methods[0] ? "GET " : "") << (src.methods[1] ? "POST " : "") << (src.methods[2] ? "DELETE " : "") << std::endl;
    }
    for (std::vector<std::string>::size_type i = 0; i < src.cgi.size(); i++)
    {
        for (size_t j = 0; j < src.lvl + 1; j++)
            ostream << "\t";
        ostream << WHITE << "cgi pass: " << RESET << src.cgi[i] << std::endl;
    }
    for (std::vector<Server>::size_type i = 0; i < src.loc.size(); i++)
        ostream << src.loc[i];
    return ostream;
}

std::ostream&	operator<<(std::ostream& ostream, const Config& src)
{
    for (std::string::size_type i = 0; i < src.server.size(); i++)
        ostream << src.server[i];
    return ostream;
}