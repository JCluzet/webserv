#include "Config.hpp"
#include "server.hpp"

Server::Server() : cb(false), id(0), loc_id(""), ip(""), port(""), server_name(), root(""), o_root(""), index(), error_page(), client_max_body_size("")
, cgi(), cgi_bin(""), autoindex(0), redirect(), upload(""), alias(false), lvl(0), path(""), client(), locations(){
    methods[0] = false;
    methods[1] = false;
    methods[2] = false;
}

Server::Server(const Server &src) : cb(src.cb), id(src.id), loc_id(src.loc_id), ip(src.ip), port(src.port), server_name(src.server_name), root(src.root)
, o_root(src.o_root), index(src.index), error_page(src.error_page), client_max_body_size(src.client_max_body_size), cgi(src.cgi), cgi_bin(src.cgi_bin)
, autoindex(src.autoindex),redirect(src.redirect), upload(src.upload), alias(src.alias), lvl(src.lvl), path(src.path), client(src.client), locations(src.locations){
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
}

Server::~Server() {clear();}

Server& Server::operator=(const Server &src){
    cb = src.cb;
    id = src.id;
    loc_id = src.loc_id;
    ip = src.ip;
    port = src.port;
    cgi = src.cgi;
    cgi_bin = src.cgi_bin;
    server_name = src.server_name;
    root = src.root;
    o_root = src.o_root;
    index = src.index;
    error_page = src.error_page;
    client_max_body_size = src.client_max_body_size;
    autoindex = src.autoindex;
    redirect = src.redirect;
    alias = src.alias;
    client = src.client;
    upload = src.upload;
    methods[0] = src.methods[0];
    methods[1] = src.methods[1];
    methods[2] = src.methods[2];
    lvl = src.lvl;
    path = src.path;
    client = src.client;
    locations = src.locations;
    return *this;
}

void    Server::clear(){
    server_name.clear();
    index.clear();
    error_page.clear();
    cgi.clear();
    redirect.clear();
    client.clear();
    locations.clear();
}

// CONFIG
Config::Config() : _debug(0) {}

Config::Config(const std::string filename) : _debug(0) { init(filename); }

Config::Config(const Config &src) : server(src.server), _debug(src._debug) {}

Config::~Config() {}

Config& Config::operator=(const Config &src)
{
    if (server.size())
        server.clear();
    server = src.server;
    _debug = src._debug;
    return *this;
}

void    Config::init_server(Server *s)
{
    s->cb = 0;
    s->id = 0;
    s->loc_id = "";
    s->ip = "";
    s->port = "";
    s->server_name.clear();
    s->root = "";
    s->o_root = "";
    s->index.clear();
    s->error_page.clear();
    s->client_max_body_size = "";
    s->cgi.clear();
    s->cgi_bin = "";
    s->methods[0] = 0;
    s->methods[1] = 0;
    s->methods[2] = 0;
    s->autoindex = 0;
    s->redirect.clear();
    s->upload = "";
    s->alias = 0;
    s->lvl = 0;
    s->path = "";
    s->client.clear();
    s->locations.clear();
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
    if (get_conf(data))
    {
        server.clear();
        return 1;
    }
    return 0;
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
    if (n != 400 && n != 401 && n != 403 && n != 404 && n != 405 && n != 406 && n != 411 && n != 413 && n != 500 && n != 501)
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
    if (path.empty() || path == "/")
        path = "/";
    else
    {
        if (path[0] != '/')
            path.insert(0, "/");
        if (path[path.length() -1] == '/')
            path.erase(path.length() - 1, 1);
    }
    serv_tmp->error_page[n] = path;
    return 0;
}

bool    Config::get_methods_line(const std::string s, Server* serv_tmp, std::string::size_type *i, std::string::size_type *line_i, bool add)
{
    std::string tmp;
    std::string::size_type k = 0;
    if (s[*i] == ';')
        return error_config_message(s, *line_i, 4) + 1;
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
            serv_tmp->methods[0] = add;
        else if (tmp == "POST")
            serv_tmp->methods[1] = add;
        else if (tmp == "DELETE")
            serv_tmp->methods[2] = add;
        else
            return (error_config_message(s, *line_i, 8) + 1);
    }
    return 0;
}

bool    Config::get_server_line(std::string s, std::string::size_type *i, std::string::size_type *line_i, Server *serv_tmp
, size_t calling_lvl, size_t *i_loc, std::vector<std::pair<std::string, std::string> >*vp, bool* a, std::map<std::string, Server> *ptrmap)
{
    std::string::size_type  p;
    std::string             serv_type[NB_LINETYPE] = {"server_name", "listen", "root", "alias", "index", "client_max_body_size"
                                                    , "error_page", "autoindex", "allow_methods", "prohibit_methods"
                                                    , "cgi", "cgi-bin", "rewrite", "upload"};
    std::string tmp, tmp1, tmp2; 
    Server      loc_tmp;
    size_t      j_loc;
    char        c;
    bool z = 0;
    pass_blanck(s, i, line_i);
    if (s_a_have_b(s, *i, "location"))
    {
        *i += 8;
        pass_blanck(s, i, line_i);
        p = *i;
        pass_not_blanck(s, i);
        tmp = s.substr(p, *i - p);
        if (tmp.empty() || tmp == "/")
            tmp = "/";
        else
        {
            if (tmp[0] != '/')
                tmp.insert(0, "/");
            if (tmp[tmp.length() -1] == '/')
                tmp.erase(tmp.length() - 1, 1);
        }
        loc_tmp.path = serv_tmp->path + tmp;
        pass_blanck(s, i, line_i);
        loc_tmp.id = *i_loc;
        loc_tmp.loc_id =  serv_tmp->loc_id + "." + intToStr(*i_loc);
        if (s[*i] != '{')
            return (error_config_message(s, *line_i, 10) + 1);
        *i += 1;
        j_loc = 1;
        loc_tmp.methods[0] = serv_tmp->methods[0];
        loc_tmp.methods[1] = serv_tmp->methods[1];
        loc_tmp.methods[2] = serv_tmp->methods[2];
        loc_tmp.autoindex = serv_tmp->autoindex;
        for (bool b = 0; *i <= s.length() && s[*i] != '}';)
            if (get_server_line(s, i, line_i, &loc_tmp, calling_lvl + 1, &j_loc, NULL, &b, ptrmap))
                return 1;
        if (*i <= s.length() && s[*i] != '}')
            return (error_config_message(s, *line_i, 11) + 1);
        *i += 1;
        pass_blanck(s, i, line_i);
        *i_loc += 1;
        loc_tmp.lvl = calling_lvl + 1;
        if (ptrmap->count(loc_tmp.path))
        {
            std::cerr << "Error: config file: location " << loc_tmp.loc_id << " and location " << (*ptrmap)[loc_tmp.path].loc_id << " share the same path(" << loc_tmp.path << ")." << std::endl;
            return 1;
        }
        ptrmap->insert(make_pair(loc_tmp.path, loc_tmp));
        return 0;
    }
    else
    {
        for (int o = 0; o < NB_LINETYPE; o++)
        {
            if (s_a_have_b(s, *i, serv_type[o]) && is_space(s[*i + serv_type[o].length()]))
            {
                *i += serv_type[o].length();
                if (*i >= s.length() || !is_blanck(s[*i]))
                    return (error_config_message(s, *line_i, 12) + 1);
                pass_blanck(s, i, line_i);
                if (s.length() <= *i)
                    return (error_config_message(s, *line_i, 13) + 1);
                switch (o)
                {
                case (0): //server_name
                    if (calling_lvl)
                        return (error_config_message(s, *line_i, 14) + 1);
                    while (*i < s.length() && s[*i] != ';')
                    {
                        p = *i;
                        pass_not_blanck(s, i);
                        if (*i == p)
                            return (error_config_message(s, *line_i, 22) + 1);
                        tmp = s.substr(p, *i - p);
                        for (std::vector<std::string>::const_iterator it = serv_tmp->server_name.begin(); it != serv_tmp->server_name.end(); ++it)
                            if (*it == tmp) {
                                z = 1;
                                break;
                            }
                        if (!z)
                            serv_tmp->server_name.push_back(tmp);
                        z = 0;
                        pass_blanck(s, i, line_i);
                    }
                    break;
                case (1): //listen
                    if (calling_lvl)
                        return (error_config_message(s, *line_i, 15) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    if (*i == p)
                        return (error_config_message(s, *line_i, 22) + 1);
                    tmp = s.substr(p, *i - p);
                    if ((c = get_listen_line(tmp, vp)))
                        return (error_config_message(s, *line_i, 15 + c) + 1);
                    break;
                case (2): //root
                    if (serv_tmp->root.length() || calling_lvl)
                        return (error_config_message(s, *line_i, 22) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->root = tmp;
                    if (!serv_tmp->root.length() || serv_tmp->root == "/")
                        serv_tmp->root = "/";
                    else if (serv_tmp->root[serv_tmp->root.length() - 1] == '/')
                        serv_tmp->root.erase(serv_tmp->root.length() - 1);
                    serv_tmp->o_root = serv_tmp->root;
                    break;
                case (3): //alias
                    if (!calling_lvl || serv_tmp->alias)
                        return (error_config_message(s, *line_i, 22) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->root = tmp;
                    if (!serv_tmp->root.length() || serv_tmp->root == "/")
                        serv_tmp->root = "/";
                    if (serv_tmp->root[serv_tmp->root.length() - 1] == '/')
                        serv_tmp->root.erase(serv_tmp->root.length() - 1);
                    serv_tmp->alias = true;
                    break;
                case (4): //index
                    if (*i >= s.length() || s[*i] == ';')
                        return (error_config_message(s, *line_i, 23) + 1);
                    while (*i < s.length() && s[*i] != ';')
                    {
                        p = *i;
                        pass_not_blanck(s, i);
                        tmp = s.substr(p, *i - p);
                        if (!tmp.length() || tmp == "/")
                            tmp = "/";
                        else
                        {
                            if (tmp[0] != '/')
                                tmp.insert(0, "/");
                            if (tmp[tmp.length() - 1] == '/')
                                tmp.erase(tmp.length() - 1, 1);
                        }
                        for (std::vector<std::string>::const_iterator it = serv_tmp->index.begin(); it != serv_tmp->index.end(); ++it)
                            if (*it == tmp){
                                z = 1;
                                break;
                            }
                        if (!z)
                            serv_tmp->index.push_back(tmp);
                        z = 0;
                        pass_blanck(s, i, line_i);
                    }
                    break;
                case (5): //client_max_body_size
                    if ((serv_tmp->client_max_body_size.length() && !calling_lvl) || (*a && calling_lvl))
                        return (error_config_message(s, *line_i, 24) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    if (p == *i)
                        return (error_config_message(s, *line_i, 23) + 1);
                    tmp = s.substr(p, *i - p);
                    serv_tmp->client_max_body_size = tmp;
                    if (calling_lvl)
                        *a = 1;
                    break;
                case (6): //error_page
                    if (get_error_page_line(s, serv_tmp, i, line_i))
                        return 1;
                    break;
                case (7): //autoindex
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if ((tmp != "on" && tmp != "off"))
                        return (error_config_message(s, *line_i, 25) + 1);
                    else if (tmp == "on")
                        serv_tmp->autoindex = true;
                    else
                        serv_tmp->autoindex = false;
                    break;
                case (8): //allow_methods
                    if (get_methods_line(s, serv_tmp, i, line_i, 1))
                        return 1;
                    break;
                case (9): //prohibit_methods
                    if (get_methods_line(s, serv_tmp, i, line_i, 0))
                        return 1;
                    break;
                case (10)://cgi
                    p = *i;
                    pass_not_blanck(s, i);
                    if (p == *i)
                        return (error_config_message(s, *line_i, 26) + 1);
                    tmp = s.substr(p, *i - p);
                    if (*i >= s.length() || !is_blanck(s[*i]))
                        return (error_config_message(s, *line_i, 26) + 1);
                    pass_blanck(s, i, line_i);
                    if (*i >= s.length() || s[*i] == ';')
                        return (error_config_message(s, *line_i, 27) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp1 = s.substr(p, *i - p);
                    if (!tmp1.length() || tmp1 == "/")
                        tmp1 = "/";
                    else
                    {
                        if (tmp1[0] != '/')
                            tmp1.insert(0, "/");
                        if (tmp1[tmp1.length() - 1] == '/')
                            tmp1.erase(tmp1.length() - 1, 1);
                    }
                    for (std::vector<std::pair<std::string,std::string> >::const_iterator it = serv_tmp->cgi.begin(); it != serv_tmp->cgi.end(); ++it)
                        if ((*it).first == tmp)
                            return (error_config_message(s, *line_i, 28) + 1);
                    serv_tmp->cgi.push_back(std::make_pair(tmp, tmp1));
                    break;
                case (11): //cgi_bin
                    if ((!calling_lvl && serv_tmp->cgi_bin.length()) || (calling_lvl && serv_tmp->cb))
                        return (error_config_message(s, *line_i, 29) + 1);
                    if (*i >= s.length() || s[*i] == ';')
                        return (error_config_message(s, *line_i, 30) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    if (p == *i)
                        return (error_config_message(s, *line_i, 31) + 1);
                    if (calling_lvl)
                        serv_tmp->cb = 1;
                    serv_tmp->cgi_bin = s.substr(p, *i - p);
                    if (!serv_tmp->cgi_bin.length() || serv_tmp->cgi_bin == "/")
                        serv_tmp->cgi_bin = "/";
                    else
                    {
                        if (serv_tmp->cgi_bin[serv_tmp->cgi_bin.length() - 1] == '/')
                            serv_tmp->cgi_bin.erase(serv_tmp->cgi_bin.length() - 1);
                        if (serv_tmp->cgi_bin[0] != '/')
                            serv_tmp->cgi_bin.insert(0, "/");
                    }
                    break;
                case (12): //rewrite
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (!tmp.length() || tmp == "/")
                        tmp = "/";
                    else
                    {
                        if (tmp[0] != '/')
                            tmp.insert(0, "/");
                        if (tmp[tmp.length() - 1] == '/')
                            tmp.erase(tmp.length() - 1, 1);
                    }
                    if (*i >= s.length() || !is_space(s[*i]))
                        return (error_config_message(s, *line_i, 27) + 1);
                    pass_blanck(s, i, line_i);
                    p = *i;
                    pass_not_blanck(s, i);
                    if (p == *i)
                        return (error_config_message(s, *line_i, 26) + 1);
                    tmp1 = s.substr(p, *i - p);
                    if (*i >= s.length() || !is_blanck(s[*i]))
                        return (error_config_message(s, *line_i, 29) + 1);
                    pass_blanck(s, i, line_i);
                    if (*i >= s.length() || s[*i] == ';')
                        return (error_config_message(s, *line_i, 30) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    if (p == *i)
                        return (error_config_message(s, *line_i, 26) + 1);
                    tmp2 = s.substr(p, *i - p);
                    if (!tmp2.size() || (tmp2 != "redirect" && tmp2 != "permanent"))
                        return (error_config_message(s, *line_i, 31) + 1);
                    for (std::vector<Redirect>::const_iterator it = serv_tmp->redirect.begin(); it != serv_tmp->redirect.end(); ++it)
                        if ((*it).redirect1 == tmp)
                            return (error_config_message(s, *line_i, 32) + 1);
                    serv_tmp->redirect.push_back(Redirect(tmp, tmp1, tmp2 == "permanent" ? 1 : 0));
                    break;
                case (13): //upload
                    if (serv_tmp->upload.length())
                        return (error_config_message(s, *line_i, 22) + 1);
                    p = *i;
                    pass_not_blanck(s, i);
                    tmp = s.substr(p, *i - p);
                    if (!tmp.length() || tmp == "/")
                        tmp = "/";
                    else
                    {
                        if (tmp[0] != '/')
                            tmp.insert(0, "/");
                        if (tmp[tmp.length() - 1] == '/')
                            tmp.erase(tmp.length() - 1, 1);
                    }
                    serv_tmp->upload = tmp;
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
            if (get_server_line(s, &i, &line_i, &serv_tmp, 0, &i_loc, &vp, &a, &serv_tmp.locations))
                return (1);
        }
        if (i >= s.length() || s[i] != '}')
            return error_config_message(s, line_i, 33) + 1;
        i += 1;
        if (check_server(&serv_tmp, vp))
            return 1;
        if (vp.empty())
        {
            // for (std::vector<Server>::size_type j = 0; j < server.size(); j++)
            // {
            //     if (server[j].port == DEFAULT_PORT)
            //     {
            //         std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the default ip:port (" << DEFAULT_IP << ":" << DEFAULT_PORT << ")." << std::endl;
            //         e_ipport = 1;
            //     }
            //     if (e_ipport)
            //         return 1;
            // }
            serv_tmp.ip = DEFAULT_IP;
            serv_tmp.port = DEFAULT_PORT;
            server.push_back(serv_tmp);
        }
        else
        {
            for (std::vector<std::pair<std::string, std::string> >::size_type i = 0; i < vp.size(); i++)
            {
                // for (std::vector<Server>::size_type j = 0; j < server.size(); j++)
                // {
                //     if ((server[j].ip == vp[i].first && server[j].port == vp[i].second)
                //         || (server[j].port == vp[i].second && (server[j].ip == DEFAULT_IP || vp[i].first == DEFAULT_IP)))
                //     {
                //         std::cerr << "Error: config file: server " << j + 1 << " and " << i_serv << " share the same ip:port(" << vp[i].first << ":" << (server[j].ip == DEFAULT_IP || vp[i].second == DEFAULT_IP ? DEFAULT_IP : vp[i].second) << ")." << std::endl;
                //         e_ipport = 1;
                //     }
                //     if (e_ipport)
                //         return 1;
                // }
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
    return 0;
}

bool    Config::check_server(Server* s, std::vector<std::pair<std::string, std::string> >   vp)
{
    bool r = 0;
    bool a = 0;
    std::string tmp;
    if (s->root.empty())
    {
        s->root = DEFAULT_ROOT;
        s->o_root = DEFAULT_ROOT;
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
    if ((s->cgi_bin.empty() || !s->cgi.size()))
    {
        std::cerr << "Error config: server " << s->id << ": can't create a server withou cgi and cgi bin." << std::endl;
        r = 1;
    }
    if (s->cgi_bin.length() && !is_directory(s->cgi_bin))
    {
        std::cerr << "Error config: server " << s->id << ": can't open cgi_bin (" << s->cgi_bin << ")." << std::endl;
        r = 1;
    }
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = s->cgi.begin(); it != s->cgi.end(); it++)
    {
        tmp = s->cgi_bin + it->second;
        if (access(tmp.c_str(), X_OK))
        {
            std::cerr << "Error config: server " << s->id << ": can't execute cgi (" << tmp << ")." << std::endl;
            r = 1;
        }
    }
    // for (std::vector<std::string>::const_iterator it = s->server_name.begin(); it != s->server_name.end();it++)
    //     if (*it == "localhost")
    //     {
    //         a = 1;
    //         break;
    //     }
    if (s->server_name.empty())
        s->server_name.push_back(DEFAULT_HOSTNAME);
    // for (std::vector<std::pair<std::string, std::string> >::const_iterator it = vp.begin(); it != vp.end(); it++)
    //     if ((it->first == "127.0.0.1" || it->first == "0.0.0.0") && !a)
    //     {
    //         s->server_name.push_back("localhost");
    //         break;
    //     }
    if (!s->upload.length())
        s->upload = s->root;
    if (!is_directory(s->upload))
    {
        std::cerr << "Error config: server " << s->id << ": can't open upload directory path (" << s->upload << ")." << std::endl;
        r = 1;
    }
    if (s->server_name.empty())
        s->server_name.push_back(DEFAULT_HOSTNAME);
    if (s->client_max_body_size.empty())
        s->client_max_body_size = DEFAULT_CLIENT_MAX_BODY_SIZE;
    std::string::size_type biggestId = 1;
    for (std::map<std::string, Server>::const_iterator it = s->locations.begin(); it != s->locations.end(); it++)
        if (it->second.loc_id.length() > biggestId)
            biggestId = it->second.loc_id.length();
    std::map<std::string, Server>::const_iterator jt;
    for (std::string::size_type x = 3; x <= biggestId; x+=2)
    {
        for (std::map<std::string, Server>::iterator it = s->locations.begin(); it != s->locations.end(); it++)
        {
            if (it->second.loc_id.length() == x)
            {
                if (it->second.loc_id.length() == 3)
                    init_loc_tmp(&it->second, *s);
                else 
                {
                    jt = s->locations.begin();
                    while (jt != s->locations.end() && jt->second.loc_id != it->second.loc_id.substr(0, it->second.loc_id.length() - 2))
                        jt++;
                    if (jt == s->locations.end())
                        return error_msg("On a un gros probleme dsl") + 1;
                    else
                        init_loc_tmp(&it->second, jt->second);
                }   
            }
        }
    }
    for (std::map<std::string, Server>::iterator it = s->locations.begin(); it != s->locations.end(); it++)
        if (check_location(&it->second))
            return 1;
    return r;
}

void    Config::init_loc_tmp(Server *dst, Server src)
{
    bool b = 0;
    dst->root = dst->root.length() ? dst->root : src.o_root;
    dst->o_root = src.o_root;
    dst->server_name = src.server_name;
    dst->index = dst->index.size() ? dst->index : src.index;
    for (std::map<int, std::string>::const_iterator it = src.error_page.begin(); it != src.error_page.end(); it++)
        if (dst->error_page.find(it->first) == dst->error_page.end())
            dst->error_page.insert(std::make_pair(it->first, it->second));
    dst->client_max_body_size = dst->client_max_body_size.length() ? dst->client_max_body_size : src.client_max_body_size;
    //METHOD gerer en amont
    std::vector<std::pair<std::string, std::string> >::iterator dst_it = dst->cgi.begin();
    for (std::vector<std::pair<std::string, std::string> >::const_iterator src_it = src.cgi.begin(); src_it != src.cgi.end(); src_it++)
    {
        dst_it = dst->cgi.begin();
        for (; dst_it != dst->cgi.end(); dst_it++)
        {
            if (dst_it->first == src_it->first)
            {
                b = 1;
                break;
            }
        }
        if (b)
            dst_it->second = src_it->second;  
        dst->cgi.push_back(*src_it);
        b = 0;
        dst_it += 1;
    }
    if (src.cgi_bin.length() && dst->cgi_bin.empty())
        dst->cgi_bin = src.cgi_bin;
    //AUTOINDEX gerer en amont
    for (std::vector<Redirect>::const_iterator src_it = src.redirect.begin(); src_it != src.redirect.end(); src_it++)
    {
        for (std::vector<Redirect>::const_iterator dst_it = dst->redirect.begin(); dst_it != dst->redirect.end(); dst_it++)
        {
            if (dst_it->redirect1 == src_it->redirect1)
            {
                b = 1;
                break;
            }
        }
        if (!b)
            dst->redirect.push_back(*src_it);
        b = 0;
    }
    if (!dst->upload.length())
        dst->upload = src.upload;
}

bool    Config::check_location(Server *s)
{
    bool r = 0;
    std::string tmp;
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
    if (s->cgi_bin.length() && !is_directory(s->cgi_bin))
    {
        std::cerr << "Error config: server " << s->id << ": can't open cgi-bin (" << s->cgi_bin << ")." << std::endl;
        r = 1;
    }
    if ((s->cgi_bin.empty() && s->cgi.size()) || (s->cgi_bin.length() && s->cgi.empty()))
    {
        std::cerr << "Error config: server " << s->id << ": can't have cgi-bin or cgi withou the other." << std::endl;
        r = 1;
    }
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = s->cgi.begin(); it != s->cgi.end(); it++)
    {
        tmp = s->cgi_bin + it->second;
        if (access(tmp.c_str(), X_OK))
        {
            std::cerr << "Error config: server " << s->id << ": can't execute cgi (" << tmp << ")." << std::endl;
            r = 1;
        }
    }
    if (s->upload.length() && !is_directory(s->upload))
    {
        std::cerr << "Error config: server " << s->id << ": can't open upload directory path (" << s->upload << ")." << std::endl;
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
    for (size_t i = 0; i < src.lvl + 1; i++)
        ostream << "\t";
    ostream << WHITE << "root: " << RESET << src.root << std::endl;
    for (size_t i = 0; i < src.lvl + 1; i++)
        ostream << "\t";
    ostream << WHITE << "upload: " << RESET << src.upload << std::endl;
    for (std::vector<std::string>::const_iterator it = src.server_name.begin(); it != src.server_name.end(); it++)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << (it == src.server_name.begin() ? "server_name: " : "             ") << RESET << *it << std::endl;
    }
    for (size_t i = 0; i < src.lvl + 1; i++)
        ostream << "\t";
    ostream << WHITE << "upload: " << RESET << src.upload << std::endl;
    for (std::vector<std::string>::const_iterator it = src.index.begin(); it != src.index.end(); it++)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << (it == src.index.begin() ? "index : " : "        ") << RESET << *it << std::endl;
    }
    for (std::map<int, std::string>::const_iterator it = src.error_page.begin(); it != src.error_page.end(); it++)
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << (*it).first << "-page: " << RESET << (*it).second << std::endl;
    }
    if (src.client_max_body_size.length())
    {
        for (size_t i = 0; i < src.lvl + 1; i++)
            ostream << "\t";
        ostream << WHITE << "client body size buffer : " << RESET << src.client_max_body_size << std::endl;
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
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = src.cgi.begin(); it < src.cgi.end(); it++)
    {
        for (size_t j = 0; j < src.lvl + 1; j++)
            ostream << "\t";
        ostream << WHITE << (it == src.cgi.begin() ? "cgi: " : "     ") << RESET << it->first << " " << it->second << std::endl;
    }
    if (src.cgi_bin.length())
    {
        for (size_t j = 0; j < src.lvl + 1; j++)
            ostream << "\t";
        ostream << WHITE << "cgi-bin: " << RESET << src.cgi_bin << std::endl;
    }
    for (std::vector<Redirect>::const_iterator it = src.redirect.begin(); it != src.redirect.end(); it++)
    {
        for (size_t k = 0; k < src.lvl + 1; k++)
            ostream << "\t";
        ostream << WHITE << (it == src.redirect.begin() ? "redirection: " :  "             ") << RESET << it->redirect1 << " " << it->redirect2 << (it->permanent ? " permanent" : " redirect") << std::endl;
    }
    for (std::map<std::string, Server>::const_iterator it = src.locations.begin(); it != src.locations.end(); it++)
        ostream << it->second;
    return ostream;
}

std::ostream&	operator<<(std::ostream& ostream, const Config& src)
{
    for (std::string::size_type i = 0; i < src.server.size(); i++)
        ostream << src.server[i];
    return ostream;
}
