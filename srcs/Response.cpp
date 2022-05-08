/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/03 02:50:07 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Response::Response(Request* request, Server* srv) : _conf(srv), _request(request), _header(""),  _content_type("text/html"), _filecontent(""), _filepath(""), _stat_rd(400)
{
    cgi_response = "";
    _response = "";
    return ;
}

Response::Response()
{
    _stat_rd = 400;
    _header = "";
    _content_type = "text/html";
    _filecontent = "";
    _filepath = "";
    _request = NULL;
    _response = "";
    cgi_response = "";
}

Response::Response(Response const &src)
{
    _conf = src._conf;
    _request = src._request;
    _header = src._header;
    _content_type = src._content_type;
    _filecontent = src._filecontent;
    _filepath = src._filepath;
    _stat_rd = src._stat_rd;
    cgi_response = src.cgi_response;
        
    _response = src._response;
}

Response&   Response::operator=(const Response &src)
{
    _stat_rd = src._stat_rd;
    _header = src._header;
    _content_type = src._content_type;
    _filecontent = src._filecontent;
    _filepath = src._filepath;
    _request = src._request;
    _response = src._response;
    cgi_response = src.cgi_response;
    return (*this);
}

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

void    Response::makeResponse()
{
    get_filepath();
    std::string cmd_cgi = "/home/user42/Documents/Projets/webserv/groupe_git/cgi-bin/php-cgi_ubuntu";
    std::string cmd_path = "/home/user42/Documents/Projets/webserv/groupe_git/www" + _request->get_path();
    if ((_request->get_method() == "POST" && _request->get_path().find(".php") != std::string::npos)
        || (_request->get_method() == "GET" && _request->get_path().find(".php?") != std::string::npos))
    {
        if (_request->get_method() == "GET")
            _filepath = _filepath.substr(0, _filepath.find(".php?") + 4);
        if (cgi_response.find("\r\n\r\n") != std::string::npos)
            cgi_response = cgi_response.substr(cgi_response.find("\r\n\r\n") + 4, cgi_response.length());
        set_redirection(cgi_response);
    }
    else
        set_redirection("");
    get_status();
    get_content_type();
    _response = getHeader() + _filecontent + "\r\n\r\n";
    return ;
}

void    Response::clear()
{
    _header = "";
    _content_type = "text/html";
    _filecontent = "";
    _filepath = "";
    _stat_rd = 400;
    cgi_response = "";
    _response = "";
    return ;
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