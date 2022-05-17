#include "Config.hpp"
#include "server.hpp"

Server::Server() : id(0), ip(""), port(""), host(""), root(""), index(""), client_body_buffer_size(""), autoindex(0), valid(0), lvl(0)
, path(""), client()
{
    methods[0] = 0;
    methods[1] = 0;
    methods[2] = 0;
}

Server::Server(const Server &src) : id(src.id), ip(src.ip), port(src.port), host(src.host), root(src.root), index(src.index)
, error_page(src.error_page), client_body_buffer_size(src.client_body_buffer_size), cgi(src.cgi), loc(src.loc), autoindex(src.autoindex)
,redirect(src.redirect), valid(src.valid), lvl(src.lvl), path(src.path), client(src.client), vp(src.vp)
{
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
}

Server::~Server() {}

Server& Server::operator=(const Server &src)
{
    id = src.id;
    ip = src.ip;
    port = src.port;
    cgi = src.cgi;
    host = src.host;
    vp = src.vp;
    root = src.root;
    index = src.index;
    error_page = src.error_page;
    client_body_buffer_size = src.client_body_buffer_size;
    autoindex = src.autoindex;
    redirect = src.redirect;
    valid = src.valid;
    client = src.client;
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
    lvl = src.lvl;
    path = src.path;
    loc = src.loc;
    client = src.client;
    return *this;
}

// CONFIG
Config::Config() : valid(0), _debug(0) {}

Config::Config(const std::string filename) : valid(0), _debug(0) { init(filename); }

Config::Config(const Config &src) : server(src.server), valid(src.valid), _debug(src._debug) {}

Config::~Config() {}

Config& Config::operator=(const Config &src)
{
    if (server.size())
        server.clear();
    server = src.server;
    valid = src.valid;
    _debug = src._debug;
    return *this;
}

void    Config::init_server(Server *s)
{
    s->id = 0;
    s->ip = "";
    s->port = "";
    s->host = "";
    s->vp.clear();
    s->root = "";
    s->index = "";
    s->error_page.clear();
    s->client_body_buffer_size = "";
    s->cgi.clear();
    s->methods[0] = 0;
    s->methods[1] = 0;
    s->methods[2] = 0;
    s->autoindex = 0;
    s->loc.clear();
    s->redirect.clear();
    s->valid = 0;
    s->lvl = 0;
    s->path = "";
    s->client.clear();
}

bool    Config::init(const std::string filename)
{
    std::string data;
    std::string::size_type a, b;
    if (filename.substr(filename.length() - 5, 5) != ".conf" || readinFile(filename, &data)) // verifie que le fichier existe et est en .conf
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

char     Config::check_ip_line(const std::string s)
{
    std::string tmp = s;
    char a = 0, b = 0;
    if (tmp[0] == '.' || tmp[tmp.length() - 1] == '.' || tmp.find("..") != std::string::npos)
                return 1;
    for (std::string::size_type i = 0; i < tmp.length(); i++)
    {
        if (tmp[i] == '.')
        {
            a++;
            b = 0;
        }
        else if (isdigit(s[i]))
        {
            if (b == 3)
                return 2;
            b++;
        }
        else
            return 3;
    }
    if (a != 3)
        return 4;
    return 0;
}

char     Config::check_port_line(const std::string s)
{
    if (s.length() > 4 || s.empty())
        return 1;
    for (std::string::size_type i = 0; i < s.length(); i++)
        if (!isdigit(s[i]))
            return 2;
    return 0;
}

char    Config::get_listen_line(const std::string tmp, Server *serv_tmp)
{
    char a = 0;
    if (!tmp.length())
        return (1);
    std::pair<std::string, std::string> pairTmp;
    if (tmp.find(':') == std::string::npos && tmp.find('.') == std::string::npos)
    {
        if ((a = check_port_line(tmp)))
            return a;
        pairTmp.first = "0.0.0.0";
// std::cout << "PROUT" << tmp << std::endl;
        pairTmp.second = tmp;
    }
    else if (tmp.find(':') == std::string::npos && tmp.find('.') != std::string::npos)
    {
        if ((a = check_port_line(tmp)))
            return a;
        pairTmp.first = tmp;
        pairTmp.second = "80";
    }
    else
    {
        pairTmp.first = tmp.substr(0, tmp.find(':'));
        pairTmp.second = tmp.substr(tmp.find(':') + 1, tmp.length());
        if ((a = check_ip_line(pairTmp.first)))
            return a;
        if ((a = check_port_line(pairTmp.second)))
            return a;
    }
    serv_tmp->vp.push_back(pairTmp);
    return 0;
}

bool    Config::get_error_page_line(const std::string s, Server *serv_tmp, std::string::size_type *i, std::string::size_type *line_i)
{
    std::string path = "";
    int         n = 0;

    while (!is_blanck(s[*i]))
    {
        path += s[*i];
        *i += 1;
    }
    n = atoi(path.c_str());
    if (n != 400 && n != 403 && n != 404 && n != 500)
    {
        std::cerr << "Error: config file: error page can't be (" << n << ")." << std::endl;
        return 1;
    }
    if (serv_tmp->error_page.count(n))
        return (error_config_message(s, *line_i, 1) + 1);
    if (*i >= s.length() ||!is_blanck(s[*i]))
        return (error_config_message(s, *line_i, 2) + 1);
    pass_blanck(s, i, line_i);
    path = "";
    while (*i <= s.length() && !is_blanck(s[*i]))
    {
        path += s[*i];
        *i += 1;
    }
    if (!path.length())
        return (error_config_message(s, *line_i, 3) + 1);
    if (path[0] != '/')
        path.insert(0, "/");
    serv_tmp->error_page[n] = path;
    return 0;
}

bool    Config::get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i)
{
    std::string tmp;
    std::string::size_type k = 0;
    for (char j = 0; j < 3; j++)
    {
        pass_blanck(s, i, line_i);
        k = *i;
        while (*i < s.length() && !is_blanck(s[*i]))
            *i += 1;
        tmp = s.substr(k, *i - k);
        if (tmp == ";")
        {
            *i -= 1;
            return (j ? 0 : error_config_message(s, *line_i, 4) + 1);
        }
        else if (tmp == "GET")
        {
            if (serv_tmp->methods[0])
                return (error_config_message(s, *line_i, 5) + 1);
            serv_tmp->methods[0] = 1;
        }
        else if (tmp == "POST")
        {
            if (serv_tmp->methods[1])
                return (error_config_message(s, *line_i, 6) + 1);
            serv_tmp->methods[1] = 1;
        }
        else if (tmp == "DELETE")
        {
            if (serv_tmp->methods[2])
                return (error_config_message(s, *line_i, 7) + 1);
            serv_tmp->methods[2] = 1;
        }
        else
            return (error_config_message(s, *line_i, 8) + 1);
    }
    return 0;
}

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp, bool *a, size_t calling_lvl, size_t *i_loc)
{
    std::string::size_type  p;
    const int               nb_serv_types = 11;
    std::string             serv_type[nb_serv_types] = {"server_name", "listen", "root", "index"
                                                        , "client_body_buffer_size", "error_page"
                                                        , "autoindex", "allow_methods", "limit_except"
                                                        , "cgi_pass", "rewrite"};
    std::string tmp, tmp1;
    Server      loc_tmp;
    size_t      j_loc;
    pass_blanck(s, i, line_i);
    if (s_a_have_b(s, *i, "location"))
    {
        *i += 8;
        pass_blanck(s, i, line_i);
        p = *i;
        pass_not_blanck(s, i);
        loc_tmp.path = s.substr(p, *i - p);
        if (!loc_tmp.path.length())
            return (error_config_message(s, *line_i, 9) + 1);
        if (loc_tmp.path[0] != '/')
            loc_tmp.path.insert(0, "/");
        pass_blanck(s, i, line_i);
        if (s[*i] != '{')
            return (error_config_message(s, *line_i, 10) + 1);
        *i += 1;
        j_loc = 1;
        for (bool b = 0; *i <= s.length() && s[*i] != '}';)
            if (get_server_line(s, i, line_i, &loc_tmp, &b, calling_lvl + 1, &j_loc))
                return 1;
        if (*i <= s.length() && s[*i] != '}')
            return (error_config_message(s, *line_i, 11) + 1);
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
                    return (error_config_message(s, *line_i, 12) + 1);
                pass_blanck(s, i, line_i);
                if (s.length() <= *i)
                    return (error_config_message(s, *line_i, 13) + 1);
                o -= (o == 8 ? 1 : 0);
                switch (o)
                {
                case (0):
                    if (serv_tmp->host.length())
                        return (error_config_message(s, *line_i, 14) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->host = tmp;
                    break;
                case (1):
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (get_listen_line(tmp, serv_tmp))
                        return 1;
                    break;
                case (2):
                    if (serv_tmp->root.length())
                        return (error_config_message(s, *line_i, 16) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->root = tmp;
                    if (serv_tmp->root[serv_tmp->root.length() - 1] == '/')
                        serv_tmp->root.erase(serv_tmp->root.length() - 1);
                    break;
                case (3):
                    if (serv_tmp->index.length())
                        return (error_config_message(s, *line_i, 17) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->index = tmp;
                    if (serv_tmp->index[0] != '/')
                        serv_tmp->index.insert(0, "/");
                    break;
                case (4):
                    if (serv_tmp->client_body_buffer_size.length())
                        return (error_config_message(s, *line_i, 18) + 1);
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
                        return (error_config_message(s, *line_i, 19) + 1);
                    *a = 1;
                    serv_tmp->autoindex = (tmp == "on");
                    break;
                case (7):
                    if (get_methods_line(s, serv_tmp, i, line_i))
                        return 1;
                    break;
                case (9):
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->cgi.push_back(tmp);
                    break;
                case (10):
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (!tmp.size())
                        return (error_config_message(s, *line_i, 20) + 1);
                    if (*i >= s.length() || !is_space(s[*i]))
                        return (error_config_message(s, *line_i, 21) + 1);
                    pass_blanck(s, i, line_i);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp1 = s.substr(p, *i - p);
                    if (!tmp1.size())
                        return (error_config_message(s, *line_i, 22) + 1);
                    serv_tmp->redirect.push_back(std::make_pair(tmp, tmp1));
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

bool    Config::check_server(Server* s)
{
    std::string p; 
    if (s->root.empty())
    {
        std::cerr << "Error: server " << s->id << ": need a root" << std::endl;
        return 1;
    }
    if (!is_directory(s->root))
    {
        std::cerr << "Error: server " << s->id << ": root directory path is wrong." << std::endl;
        return 1;
    }
    if (s->index.size() && !is_file(s->root + s->index))
    {
        std::cerr << "Error: server " << s->lvl << "." << s->id << ": index file path is wrong." << std::endl;
        return 1;
    }
    for (std::vector<Server>::size_type i = 0; i < s->loc.size(); i++)
    {
        p = s->root;
        if (!is_directory(p + s->loc[i].path))
        {
            std::cerr << "Error: server " << s->id << ": location " << s->loc[i].id << ": invalid directory path(" << s->root << "+" << s->loc[i].path << ")." << std::endl;
            return 1;
        }
    }
    return 0;
}

bool    Config::get_conf(const std::string s)
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
        for (bool a = 0; i < s.length() && s[i] != '}';)
        {
            if (get_server_line(s, &i, &line_i, &serv_tmp, &a, 0, &i_loc))
                return (1);
        }
        if (i >= s.length() || s[i] != '}')
            return error_config_message(s, line_i, 25) + 1;
        i += 1;
        serv_tmp.id = i_serv;
        i_serv++;
        if (!(serv_tmp.valid = (check_server(&serv_tmp) ? 0 : 1)))
            return 1;
        if (serv_tmp.vp.empty())
        {
            serv_tmp.ip = "0.0.0.0";
            serv_tmp.port = "80";
            server.push_back(serv_tmp);
        }
        else
        {
            for (std::vector<std::pair<std::string, std::string> >::size_type i = 0; i < serv_tmp.vp.size(); i++)
            {
                serv_tmp.ip = serv_tmp.vp[i].first;
                serv_tmp.port = serv_tmp.vp[i].second;
                server.push_back(serv_tmp);
            }
            for (std::vector<Server>::size_type i = 0; i < server.size(); i++)
                server[i].vp.clear();
        } 
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
    for (size_t i = 0; i < src.lvl + 1; i++)
        ostream << "\t";
    ostream << WHITE << "port: " << RESET << src.port << std::endl;
    for (size_t i = 0; i < src.lvl + 1; i++)
        ostream << "\t";
    ostream << WHITE << "ip: " << RESET << src.ip << std::endl;
    if (src.host.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "host: " << RESET << src.host << std::endl;
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
    for (std::map<int, std::string>::const_iterator it = src.error_page.begin(); it != src.error_page.end(); it++)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << (*it).first << "page: " << RESET << (*it).second << std::endl;
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
    for (std::vector<std::pair<std::string, std::string> >::size_type j = 0; j != src.redirect.size(); j++)
    {
        for (size_t k = 0; k < src.lvl + 1; k++)
            ostream << "\t";
        ostream << WHITE << "redirection: " << RESET << src.redirect[j].first << " " << src.redirect[j].second << std::endl;
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