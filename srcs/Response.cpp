/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alebross <alebross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/15 14:20:22 by alebross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"//  1: CHANGER RESPONSE pour lire en dehors (les erreur) // 2: AJOUTER UPLOAD dans response/request, ecrire en dehors, faire isupload

Response::Response(Request* request, Server* srv) : _conf(srv), _request(request), _header(""),  _content_type("text/html"), _filecontent(""), _filepath(""), _stat_rd(0)
{
    transfer = "";
    writing = false;
    _response = "";
    return ;
}

Response::Response()
{
    _stat_rd = 0;
    _header = "";
    _content_type = "text/html";
    _filecontent = "";
    _filepath = "";
    _request = NULL;
    _response = "";
    transfer = "";
    writing = false;
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
    transfer = src.transfer;
    writing = src.writing;
        
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
    transfer = src.transfer;
    writing = src.writing;
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

int     Response::treatRequest()
{
    int fd_file = -1;
    std::string method = _request->get_method();

    get_filepath();
    if (_filepath == "")
    {
        _stat_rd = 400;
    //    return (-1);
    }
    if (is_directory(_filepath))
    {
        if (method != "GET")
        {
            _stat_rd = 400;
    //        return (-1);
        }
        if (_conf->autoindex)
        {
            indexGenerator(&_filecontent, _filepath);
            _stat_rd = 200;
        }
    }
    if (method == "DELETE")
    {
        _stat_rd = method_delete();
    //    if (_stat_rd != 200)
    //        return (-1);
    }
    //std::cout << _stat_rd << std::endl;
    fd_file = openFile();
    return (fd_file);
}

void    Response::makeResponse()
{
    std::string cmd_cgi = "/home/user42/Documents/Projets/webserv/groupe_git/cgi-bin/php-cgi_ubuntu";
    std::string cmd_path = "/home/user42/Documents/Projets/webserv/groupe_git/www" + _request->get_path();
    if ((_request->get_method() == "POST" && _request->get_path().find(".php") != std::string::npos)
        || (_request->get_method() == "GET" && _request->get_path().find(".php?") != std::string::npos))
    {
        if (_request->get_method() == "GET")
            _filepath = _filepath.substr(0, _filepath.find(".php?") + 4);
        if (transfer.find("Content-type: ") != std::string::npos && transfer.find("\r\n", transfer.find("Content-type: ")) != std::string::npos)
            _content_type = transfer.substr(transfer.find("Content-type: ") + 14, transfer.find("\r\n", transfer.find("Content-type: ")) - transfer.find("Content-type: ") - 14);
        else
            _content_type = "text/html";
        if (transfer.find("\r\n\r\n") != std::string::npos)
            transfer = transfer.substr(transfer.find("\r\n\r\n") + 4, transfer.length());
    }
    else
        get_content_type();
    if (transfer != "")
        _filecontent = transfer;
    _response = getHeader() + _filecontent + "\r\n\r\n";
    //std::cout << _filecontent.length() << std::endl;
    return ;
}

void    Response::clear()
{
    _header = "";
    _content_type = "text/html";
    _filecontent = "";
    _filepath = "";
    _stat_rd = 0;
    transfer = "";
    _response = "";
    writing = false;
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

int Response::method_delete(void)
{
    
    std::ifstream ifs;
    ifs.open(_filepath.c_str());

    if (!ifs)
        return (404);
    ifs.close();
    if (_request->get_header("Authorization") == "user42 token")
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

void    Response::setStatus(const int new_status)
{
    _stat_rd = new_status;
    return ; 
}

const std::string Response::error_page_message(const int status)
{
    if (status == 200)
        return ("OK");
    if (status == 403)
        return ("Forbidden");
    if (status == 404)
        return ("Not Found");
    // if (status == 400)
    return ("Bad Request");
    
}

bool fileExist(const std::string s)
{
    DIR* dir;
    bool b = 0;
    struct dirent* ent;
    std::string dirp, filep, tmp = s;
    std::string::size_type i;
    if (tmp[tmp.length() - 1] == '/') // si il y a un '/' au debut du path on le supprime
        tmp.erase(tmp.length() - 1, 1);
    if (tmp[0] == '/') //si il y a un '/' a la fin du path on le supprime
        tmp.erase(0, 1);
    i = (tmp.find_last_of('/'));
    if (i == std::string::npos)
    {
        dirp = "./";
        filep = tmp;
    }
    else
    {
        dirp = tmp.substr(0, i);
        filep = tmp.substr(i + 1, tmp.length() - (i + 1));
    }
    dir = opendir(dirp.c_str());
    while ((ent = readdir(dir)))
        if (ent->d_name == filep)
            b = 1;
    closedir(dir);
    return b;
}

int Response::openFile()
{
    int fd_file = -1;
    if (!fileExist(_filepath))
        _stat_rd = 404;
    if (_stat_rd == 0) // le fichier existe
    {
        fd_file = open(_filepath.c_str(), O_RDONLY);
        if (fd_file < 0) // le fichier exite mais n'as pas les droits
            _stat_rd = 403;
        else
            _stat_rd = 200; // le fichier est lisible
    }
    if (_stat_rd == 400 || _stat_rd == 403 || _stat_rd == 404)
    {
        if (!_conf->error_page.count(_stat_rd))
            _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR " + intToStr(_stat_rd) + "</h1>\n    <p>" + error_page_message(_stat_rd) + "</p>\n</body>\n\n</html>";
        else
        {
            _filepath = _conf->root + _conf->error_page[_stat_rd];
            if (!fileExist(_filepath))
            {
                _stat_rd = 404;
                _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
            }
            else
            {
                fd_file = open(_filepath.c_str(), O_RDONLY);
                if (fd_file < 0)
                {
                    _stat_rd = 403;
                    _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 403</h1>\n    <p>Forbidden.</p>\n</body>\n\n</html>";
                }
            }
        }
    }
    _status = intToStr(_stat_rd);
    _status += " " + error_page_message(_stat_rd);
    return (fd_file);
}