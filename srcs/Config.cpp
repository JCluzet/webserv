#include "Config.hpp"
#include "server.hpp"

Server::Server() : id(0), loc_id(""), ip(""), port(""), host(""), root(""), client_body_buffer_size(""), autoindex(0), valid(0), lvl(0)
, path(""), client(CO_MAX){
    methods[0] = 0;
    methods[1] = 0;
    methods[2] = 0;
}

Server::Server(const Server &src) : id(src.id), loc_id(src.loc_id), ip(src.ip), port(src.port), host(src.host), root(src.root), index(src.index)
, error_page(src.error_page), client_body_buffer_size(src.client_body_buffer_size), cgi(src.cgi), loc(src.loc), autoindex(src.autoindex)
,redirect(src.redirect), valid(src.valid), lvl(src.lvl), path(src.path), client(src.client){
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
}

Server::~Server() {}

Server& Server::operator=(const Server &src){
    id = src.id;
    loc_id = src.loc_id;
    ip = src.ip;
    port = src.port;
    cgi = src.cgi;
    host = src.host;
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
    s->loc_id = "";
    s->ip = "";
    s->port = "";
    s->host = "";
    s->root = "";
    s->index.clear();
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
        if ((data[i] == ';' || data[i] == '{' || data[i] == '}') && i != 0 && !is_blanck(data[i - 1]))
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
        return 5;
    for (std::string::size_type i = 0; i < s.length(); i++)
        if (!isdigit(s[i]))
            return 6;
    return 0;
}

char    Config::get_listen_line(const std::string tmp, std::vector<std::pair<std::string, std::string> >*vp)
{
    char a = 0;
    std::string ip, port;
    if (!tmp.length())
        return (1);
    if (tmp.find(':') == std::string::npos && tmp.find('.') == std::string::npos)
    {
        if ((a = check_port_line(tmp)))
            return a;
        vp->push_back(std::make_pair("0.0.0.0", tmp));
    }
    else if (tmp.find(':') == std::string::npos && tmp.find('.') != std::string::npos)
    {
        if ((a = check_ip_line(tmp)))
            return a;
        vp->push_back(std::make_pair(tmp, "80"));
    }
    else
    {
        if ((a = check_ip_line(tmp.substr(0, tmp.find(':')))))
            return a;
        if ((a = check_port_line(tmp.substr(tmp.find(':') + 1, tmp.length() - (tmp.find(':') + 1)))))
            return a;
        vp->push_back(std::make_pair(tmp.substr(0, tmp.find(':')), tmp.substr(tmp.find(':') + 1, tmp.length() - (tmp.find(':') + 1))));
    }
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

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp
, bool *a, size_t calling_lvl, size_t *i_loc, std::vector<std::pair<std::string, std::string> >*vp)
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
    char        c;
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
        loc_tmp.id = *i_loc;
        loc_tmp.loc_id =  serv_tmp->loc_id + "." + intToStr(*i_loc);
        if (s[*i] != '{')
            return (error_config_message(s, *line_i, 10) + 1);
        *i += 1;
        j_loc = 1;
        for (bool b = 0; *i <= s.length() && s[*i] != '}';)
            if (get_server_line(s, i, line_i, &loc_tmp, &b, calling_lvl + 1, &j_loc, NULL))
                return 1;
        if (*i <= s.length() && s[*i] != '}')
            return (error_config_message(s, *line_i, 11) + 1);
        *i += 1;
        pass_blanck(s, i, line_i);
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
                case (0): //server_name
                    if (serv_tmp->host.length())
                        return (error_config_message(s, *line_i, 14) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->host = tmp;
                    break;
                case (1): //listen
                    if (calling_lvl)
                        return (error_config_message(s, *line_i, 15) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if ((c = get_listen_line(tmp, vp)))
                        return (error_config_message(s, *line_i, 15 + c) + 1);
                    break;
                case (2): //root
                    if (serv_tmp->root.length())
                        return (error_config_message(s, *line_i, 22) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->root = tmp;
                    if (serv_tmp->root[serv_tmp->root.length() - 1] == '/')
                        serv_tmp->root.erase(serv_tmp->root.length() - 1);
                    break;
                case (3): //index
                    if (*i >= s.length() || s[*i] == ';')
                        return (error_config_message(s, *line_i, 23) + 1);
                    while (*i < s.length() && s[*i] != ';')
                    {
                        p = *i;
                        pass_not_blanck(s, i);
                        if (p == *i)
                            return (error_config_message(s, *line_i, 23) + 1);
                        tmp = s.substr(p, *i - p);
                        if (tmp[0] != '/')
                            tmp.insert(0, "/");
                        serv_tmp->index.push_back(tmp);
                        pass_blanck(s, i, line_i);
                    }
                    break;
                case (4): //client_body_buffer_size
                    if (serv_tmp->client_body_buffer_size.length())
                        return (error_config_message(s, *line_i, 24) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->client_body_buffer_size = tmp;
                    break;
                case (5): //error_page
                    if (get_error_page_line(s, serv_tmp, i, line_i))
                        return 1;
                    break;
                case (6): //autoindex
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (*a || (tmp != "on" && tmp != "off"))
                        return (error_config_message(s, *line_i, 25) + 1);
                    *a = 1;
                    serv_tmp->autoindex = (tmp == "on");
                    break;
                case (7): //allow_methods/limit_except
                    if (get_methods_line(s, serv_tmp, i, line_i))
                        return 1;
                    break;
                case (9): //cgi_pass
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->cgi.push_back(tmp);
                    break;
                case (10): //rewrite
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (!tmp.size())
                        return (error_config_message(s, *line_i, 26) + 1);
                    if (*i >= s.length() || !is_space(s[*i]))
                        return (error_config_message(s, *line_i, 27) + 1);
                    pass_blanck(s, i, line_i);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp1 = s.substr(p, *i - p);
                    if (!tmp1.size())
                        return (error_config_message(s, *line_i, 28) + 1);
                    serv_tmp->redirect.push_back(std::make_pair(tmp, tmp1));
                    break;
                }
                pass_blanck(s, i, line_i);
                if (*i >= s.length() || s[*i] != ';')
                    return (error_config_message(s, *line_i - 1, 29) + 1);
                *i += 1;
                pass_blanck(s, i, line_i);
                return (0);
            }
        }
        return error_config_message(s, *line_i, 30) + 1;
    }
}

bool    Config::get_conf(const std::string s)
{
    std::string::size_type line_i = 1, i = 0, i_serv = 1, i_loc = 1;
    std::vector<std::pair<std::string, std::string> >   vp;
    Server serv_tmp;
    bool    e_ipport = 0;
    if (s.empty())
        return error_msg("Error: Empty config file.") + 1;
    pass_blanck(s, &i, &line_i);
    if (s.length() <= i)
        return error_msg("Error: Empty config file") + 1;
    if (!s_a_have_b(s, i, "server"))
        return error_config_message(s, line_i, 31) + 1;
    while (1)
    {
        i += 6;
        pass_blanck(s, &i, &line_i);
        if (s.length() <= i || s[i] != '{')
            return error_config_message(s, line_i, 32) + 1;
        i += 1;
        init_server(&serv_tmp);
        serv_tmp.id = i_serv;
        serv_tmp.loc_id = intToStr(i_serv);
        for (bool a = 0; i < s.length() && s[i] != '}';)
        {
            if (get_server_line(s, &i, &line_i, &serv_tmp, &a, 0, &i_loc, &vp))
                return (1);
        }
        if (i >= s.length() || s[i] != '}')
            return error_config_message(s, line_i, 33) + 1;
        i += 1;
        if (!(serv_tmp.valid = (check_server(&serv_tmp) ? 0 : 1)))
            return 1;
        if (vp.empty())
        {
            for (std::vector<Server>::size_type j = 0; j < server.size(); j++)
            {
                if (server[j].ip == DEFAULT_IP)
                {
                    std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the default ip(" << DEFAULT_IP << ")." << std::endl;
                    e_ipport = 1;
                }
                if (server[j].port == DEFAULT_PORT)
                {
                    std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the same port(" << DEFAULT_PORT << ")." << std::endl;
                    e_ipport = 1;
                }
                if (e_ipport)
                    return 1;
            }
            serv_tmp.ip = DEFAULT_IP;
            serv_tmp.port = DEFAULT_PORT;
            server.push_back(serv_tmp);
        }
        else
        {
            for (std::vector<std::pair<std::string, std::string> >::size_type i = 0; i < vp.size(); i++)
            {
                for (std::vector<Server>::size_type j = 0; j < server.size(); j++)
                {
                    if (server[j].ip == vp[i].first)
                    {
                        std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the same ip(" << vp[i].first << ")." << std::endl;
                        e_ipport = 1;
                    }
                    if (server[j].port == vp[i].second)
                    {
                        std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the same port(" << vp[i].second << ")." << std::endl;
                        e_ipport = 1;
                    }
                    if (e_ipport)
                        return 1;
                }
                serv_tmp.ip = vp[i].first;
                serv_tmp.port = vp[i].second;
                server.push_back(serv_tmp);
            }
        }
        i_serv++;
        vp.clear(); 
        i_loc = 1;
        pass_blanck(s, &i, &line_i);
        if (s.length() == i)
            break;
        if (!s_a_have_b(s, i, "server"))
            return error_config_message(s, line_i, 34) + 1;
    }
    valid = 1;

    return 0;
}

bool    Config::check_server(Server* s)
{
    bool r = 0;
    if (s->root.empty())
    {
        std::cerr << "Error config: server " << s->id << ": need a root" << std::endl;
        return 1;
    }
    if (s->root[0] != '/')
    {
        std::cerr << "Error config: server " << s->id << ": root must be absolut directory path.(" << s->root << ") is invalid." << std::endl;
        return 1;
    }
    if (!is_directory(s->root))
    {
        std::cerr << "Error config: server " << s->id << ": can't open root directory path.(" << s->root << ")" << std::endl;
        return 1;
    }
    for (std::vector<std::string>::iterator it = s->index.begin(); it != s->index.end(); it++)
    {
        if (!is_file(s->root + *it))
        {
            std::cerr << "Error config: server " << s->id << ": can't open index file path.(" << s->root << *it << std::endl;
            r = 1;
        }
    }
    for (std::map<int, std::string>::const_iterator it = s->error_page.begin(); it != s->error_page.end(); it++)
    {
        if (!is_file(s->root + it->second))
        {
            std::cerr << "Error config: server " << s->id << ": can't open error " << it->first << " page file path.(" << s->root << it->second << ")." << std::endl;
            r = 1;
        }
    }
    for (std::vector<std::string>::size_type i = 0; i < s->cgi.size(); i++)
    {
        if (!is_directory(s->root + s->cgi[i]))
        {
            std::cerr << "Error config: server " << s->id << ": can't open cgi directory path (" << s->root << s->cgi[i] << ")." << std::endl;
            r = 1;
        }
    }
    for (std::vector<Server>::size_type i = 0; i < s->loc.size(); i++)
    {
        if (!is_directory(s->root + s->loc[i].path))
        {
            std::cerr << "Error config: server " << s->id << ": can't open location directory path (" << s->root << s->loc[i].path << ")." << std::endl;
            r = 1;
        }
        if (check_location(&s->loc[i], s->root))
            r = 1;
    }
    return r;
}

bool    Config::check_location(Server* s, const std::string calling_root)
{
    bool r = 0;
    if (s->root.empty())
        s->root = calling_root;
    else
    {
        if (s->root[0] != '/')
        {
            std::cerr << "Error config: location " << s->loc_id << ": root must be absolut directory path.(" << s->root << ") is invalid." << std::endl;
            return 1;
        }
        if (!is_directory(s->root))
        {
            std::cerr << "Error config: location " << s->loc_id << ": can't open root directory path.(" << s->root << ")" << std::endl;
            return 1;
        }
    }
    for (std::vector<std::string>::iterator it = s->index.begin(); it != s->index.end(); it++)
    {
        if (!is_file(s->root + *it))
        {
            std::cerr << "Error config: server " << s->id << ": can't open index file path.(" << s->root << *it << std::endl;
            r = 1;
        }
    }
    for (std::map<int, std::string>::const_iterator it = s->error_page.begin(); it != s->error_page.end(); it++)
    {
        if (!is_file(s->root + it->second))
        {
            std::cerr << "Error config: location " << s->loc_id << ": can't open error " << it->first << " page file path.(" << s->root << it->second << ")." << std::endl;
            r = 1;
        }
    }
    for (std::vector<std::string>::size_type i = 0; i < s->cgi.size(); i++)
    {
        if (!is_directory(s->root + s->cgi[i]))
        {
            std::cerr << "Error config: location " << s->loc_id << ": can't open cgi directory path (" << s->root << s->cgi[i] << ")." << std::endl;
            r = 1;
        }
    }
    for (std::vector<Server>::size_type i = 0; i < s->loc.size(); i++)
    {
        if (!is_directory(s->root + s->loc[i].path))
        {
            std::cerr << "Error config: location " << s->loc_id << ": can't open location directory path (" << s->root << s->loc[i].path << ")." << std::endl;
            r = 1;
        }
        if (check_location(&s->loc[i], s->root))
            r = 1;
    }
    return r;
}

std::ostream&	operator<<(std::ostream& ostream, const Server& src)
{
    if (!src.lvl)
        ostream << WHITE << "server " << src.id << ":" << std::endl;
    else
    {
        for (size_t i = 0; i < src.lvl; i++)
            ostream << "\t";
        ostream << WHITE << "location " << src.loc_id << ": " << RESET << src.path << std::endl;
    }
    if (!src.lvl)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "port: " << RESET << src.port << std::endl;
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "ip: " << RESET << src.ip << std::endl;
    }
    if (src.host.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "host: " << RESET << src.host << std::endl;
    }
    for (std::vector<std::string>::const_iterator it = src.index.begin(); it != src.index.end(); it++)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << (it == src.index.begin() ? "index : " : "        ") << RESET << *it << std::endl;
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
        ostream << WHITE << (*it).first << (it == src.error_page.begin() ? "page: " : "      ") << RESET << (*it).second << std::endl;
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
    for (std::vector<std::string>::const_iterator it = src.cgi.begin(); it < src.cgi.end(); it++)
    {
        for (size_t j = 0; j < src.lvl + 1; j++)
            ostream << "\t";
        ostream << WHITE << (it == src.cgi.begin() ? "cgi pass: " : "          ") << RESET << *it << std::endl;
    }
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = src.redirect.begin(); it != src.redirect.end(); it++)
    {
        for (size_t k = 0; k < src.lvl + 1; k++)
            ostream << "\t";
        ostream << WHITE << (it == src.redirect.begin() ? "redirection: " :  "             ") << RESET << it->first << " " << it->second << std::endl;
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