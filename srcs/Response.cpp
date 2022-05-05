/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/05 20:40:37 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::string Response::getHeader()
{
    std::string head = "";
    head += "HTTP/1.1 " + _status;
    head += "\nServer: WebServ/1.0";
    head += "\nDate : " + getDate();
    head += "\nContent-Type: " + _content_type;
    head += "\nContent-Length: " + sizetToStr(_filecontent.length());
    head += "\r\n\r\n";
    return (head);
}

std::string Response::getDate()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %X %Z", &tstruct);
    return (buf);
}

int Response::set_redirection(std::string cgi_response)
{
    if (_filepath == "")
    {
        _stat_rd = 400;
        return (1);
    }
    if (is_directory(_filepath))
    {
        if (_request->get_method() != "GET")
        {
            _stat_rd = 400;
            return (1);
        }
        if (_conf->autoindex)
        {
            indexGenerator(&_filecontent, _filepath);
            _stat_rd = 200;
        }
    }
    else
    {
        
        std::string method = _request->get_method();
        if (method == "DELETE")
        {
            _stat_rd = method_delete();
            if (_stat_rd != 200)
                return (1);
        }
        else if (method == "GET" && cgi_response == "")
            _stat_rd = readFile(_filepath.c_str(), &_filecontent);
        else
        {
            _filecontent = cgi_response;
            _stat_rd = 200;
        }
    }
    return (0);
}

int Response::method_delete(void)
{
    
    std::ifstream ifs;
    ifs.open(_filepath.c_str());

    if (!ifs)
        return (404);
    ifs.close();
    if (_request->get_header("Authorization") == "user42")
    {
        if (remove(_filepath.c_str()) < 0)
            return (500); // 500 Internal Server Error / avec strerror ?/// bonne erreur ?
        return (200);
    }
    return (403); // Forbidden: you don't have permission /// bonne erreur ??
}

int Response::get_content_type()
{
    _content_type = "text/html";
    if (_request->get_path() != "")
    {
    if (_filepath.substr(_filepath.length() - 4, 4) == ".svg")
        _content_type = "image/svg+xml";
    else if (_filepath.substr(_filepath.length() - 4, 4) == ".pdf")
        _content_type = "application/pdf";
    else if (_filepath.substr(_filepath.length() - 4, 4) == ".png")
        _content_type = "image/apng";
    else if (_filepath.substr(_filepath.length() - 4, 4) == ".css")
        _content_type = "text/css";
    }
    return (0);
}

void Response::get_filepath()
{
    if (_request->get_path() != "")
    {
        _filepath = _conf->root + _request->get_path();
        // if (is_a_cgi(_filepath))
        //     _isaCGI = true;
        if (is_directory(_filepath) && _filepath[_filepath.length() - 1] != '/' && !_conf->autoindex)
            _filepath += "/";
        if (is_directory(_filepath) && !_conf->autoindex)
            _filepath += _conf->index;
    }
}

int Response::get_status()
{
    if (_stat_rd == 404)
    {
        if (_conf->error404 == "")
            _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        else
        {
            _filepath = _conf->root + "/" + _conf->error404;
            readFile(_filepath.c_str(), &_filecontent);
            if (_stat_rd == 404 && _conf->error404 == "")
                _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        }
        _status = "404 Not Found";
    }
    if (_stat_rd == 400)
    {
        _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 400</h1>\n    <p>Bad Request.</p>\n</body>\n\n</html>";
        _status = "400 Bad Request";
    }
    else if (_stat_rd == 200)
        _status = "200 OK";
    return (0);
}

bool    Response::is_cgi(void)
{
    if (_request->get_method() == "POST")
    {
        std::cout << GREEN << "[⊛ CGI]        => " << WHITE << "Output treated as CGI POST" << RESET << std::endl;
        return (true);
    }
        return true;
    if (_request->get_method() == "GET" && _request->get_path().find(".php?") != std::string::npos)
    {
        for (unsigned int i = 0; i < _conf->cgi.size(); i++)
        {
            std::cout << "allow CGI : " << _conf->cgi[i] << std::endl;
            if (_request->get_path().find(_conf->cgi[i]) != std::string::npos)
            {
                std::cout << GREEN << "[⊛ CGI]        => " << WHITE << "Output treated as CGI" << RESET << std::endl;

                return true;
            }
        }
        std::cout << RED << "[⊛ CGI]        => " << WHITE << "This CGI does not seem to be specified in the config file" << RESET << std::endl;
        return false;
    }
    return false;
}

std::string&    Response::treat_cgi(Client* client)
{
    std::string str;
    std::string cmd_cgi = "/home/user42/Documents/Projets/webserv/groupe_git/cgi-bin/php-cgi_ubuntu";
    std::string cmd_path = "/home/user42/Documents/Projets/webserv/groupe_git/www" + _request->get_path();
    // char**  cmd = new char*[3];
    std::vector<std::string> cmd(2);

    if (_request->get_method() == "POST")
    {
        if (_request->get_path().find(".php") != std::string::npos)
        {
            cmd[0] = cmd_cgi;
            cmd[1] = cmd_path;
            _filepath = _filepath.substr(0, _filepath.find(".php") + 4);
        }
        _response = cgi_exec(cmd, cgi_env(cmd_cgi, "", client, _conf), _request);
    }
    if (_request->get_method() == "GET" && _request->get_path().find(".php?") != std::string::npos)
    {
        cmd_path = cmd_path.substr(0, cmd_path.find(".php?") + 4);
        cmd[0] = cmd_cgi;
        cmd[1] = cmd_path;
        str = _request->get_path().substr(_request->get_path().find(".php?") + 5, _request->get_path().length());
        _filepath = _filepath.substr(0, _filepath.find(".php?") + 4);
        _response = cgi_exec(cmd, cgi_env(cmd_cgi, str, client, _conf), _request);
    }
    //std::cout << std::endl << _response << std::endl;
    _response = _response.substr(_response.find("\r\n\r\n") + 4, _response.length());
    return _response;
}

int Response::readFile(std::string filename, std::string *fileContent)
{
    std::string s;
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs)
        return (404);
    getline(ifs, s);
    if (s == "")
    {
        ifs.close();
        return (404);
    }
    *fileContent += s;
    while (getline(ifs, s))
    {
        *fileContent += "\n";
        *fileContent += s;
    }
    ifs.close();
    return (200);
}