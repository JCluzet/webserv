#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>

typedef struct s_server
{
    std::string host;
    std::string server_name;
    std::string port;
    std::string default_folder;
    std::string default_page;
    std::string page404;
    std::string max_body_size;
    bool valid;

} t_server;

class Config
{
public:
    t_server *serv;
    size_t nb_servers;

    Config(std::string filename) : serv(NULL), nb_servers(0)
    {
        std::string data;
        if (this->readinFile(filename, &data))
        {
            std::cerr << "Wrong config file." << std::endl;
            // serv = new t_server[1];
            return;
        }
        if (count_servers(data, &nb_servers))
        {
            std::cerr << "Wrong accolade in config file." << std::endl;
            // serv = new t_server[1];
            return;
        }
        serv = new t_server[nb_servers];
        parse_server(data);
    }

        Config(Config const &src) : serv(NULL), nb_servers(0)
        {
            if (!src.nb_servers)
                return;
            serv = new t_server[src.nb_servers];
            for (size_t i = 0; i < src.nb_servers; i++)
                serv[i] = src.serv[i];
        }

        Config &operator=(const Config &src) {
            if (this->nb_servers)
                delete [] serv;
            serv = new t_server[src.nb_servers];
            for (size_t i = 0; i < src.nb_servers; i++)
                serv[i] = src.serv[i];
            return *this;
        }

    ~Config()
    {
        // if (serv)
            delete[] serv;
    }

private:
    std::string findInServer(std::string header, std::string s)
    {
        size_t pos;
        std::string tmp;
        while ((pos = header.find("\n")) != std::string::npos)
        {
            tmp = header.substr(0, pos);
            tmp.erase(0, tmp.find_first_not_of(" "));
            if (tmp.length() > s.length() && !strncmp(s.c_str(), tmp.c_str(), s.length()))
            {
                tmp.erase(0, s.length());
                tmp.erase(0, tmp.find_first_not_of(" "));
                if (tmp[0] != '=' && tmp[0] != ':')
                    return ("");
                tmp.erase(0, tmp.find_first_not_of(" ", 1));
                if (tmp.find("#") != std::string::npos)
                    tmp.erase(tmp.find("#"), tmp.length());
                return tmp.substr(0, tmp.find_first_of(" "));
            }
            header.erase(0, pos + 1);
        }
        return "";
    }

    void parse_server2(std::string conf, t_server *s)
    {
        s->host = findInServer(conf, "host");
        s->server_name = findInServer(conf, "server_name");
        // std::string tmp = findInServer(conf, "port");
        // s->port = tmp.size() ? std::stoi(tmp) : 0;
        s->port = findInServer(conf, "port");
        s->default_folder = findInServer(conf, "default_folder");
        s->default_page = findInServer(conf, "default_page");
        s->page404 = findInServer(conf, "404_page");
        // tmp = findInServer(conf, "max_body_size");
        // s->max_body_size = tmp.size() ? std::stoull(tmp) : 0;
        s->max_body_size = findInServer(conf, "max_body_size");
        s->valid = (s->host.length() && s->server_name.length() && s->port.length() && s->default_folder.length() && s->default_page.length() && s->page404.length() && s->max_body_size.length());
    }

    void parse_server(std::string data)
    {
        std::string conf;
        for (size_t i = 0; i < nb_servers; i++)
        {
            data.erase(0, data.find("{") + (data[data.find("{") + 1] == '\n' ? 2 : 1));
            conf = data.substr(0, data.find("}") - (data[data.find("}") - 1] == '\n' ? 2 : 1));
            conf += "\n";
            parse_server2(conf, &serv[i]);
        }
    }

    bool count_servers(std::string data, size_t *nb_servers)
    {
        size_t i = 0;
        bool open = 0;
        while (data[i])
        {
            if (data[i] == '{')
            {
                if (open)
                    return (1);
                open = 1;
                *nb_servers += 1;
            }
            if (data[i] == '}')
            {
                if (!open)
                    return (1);
                open = 0;
            }
            i++;
        }
        if (open)
            return (1);
        return (0);
    }
    bool readinFile(std::string filename, std::string *fileContent)
    {
        std::string s;
        // std::ifstream ifs(filename);
        std::ifstream ifs;
        ifs.open(filename.c_str());

        if (!ifs)
        {
            // std::cerr << "Not Found " << filename << "." << std::endl;
            *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>"; // --> pouvoir mettre le fichier d'erreur par default ou celui inndique dans le fichier de config
            return (1);
        }
        getline(ifs, s);
        if (s == "")
        {
            // std::cerr << "Empty file." << std::endl;
            ifs.close();
            *fileContent = "\n";
            *fileContent = "<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>Empty file.</p>\n</body>\n\n</html>"; // --> pareil
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
};