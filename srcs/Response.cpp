/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/20 19:14:04 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp" //  1: CHANGER RESPONSE pour lire en dehors (les erreur) // 2: AJOUTER UPLOAD dans response/request, ecrire en dehors, faire isupload

Response::Response(Request *request) : _conf(NULL), _request(request), _header(""), _content_type("text/html"), _filecontent(""), _filepath(""), _stat_rd(0)
{
    transfer = "";
    writing = false;
    _response = "";
    return;
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

Response &Response::operator=(const Response &src)
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
    if (_stat_rd == 0)
        head = "HTTP/1.1 400 " + error_page_message(_stat_rd);
    else
        head = "HTTP/1.1 " + intToStr(_stat_rd) + " " + error_page_message(_stat_rd);
    head += "\r\nServer: WebServ/1.0";
    head += "\r\nDate: " + getDate();
    if (_request->get_header("Connection") == "close")
        head += "\r\nConnection: close";
    else
        head += "\r\nConnection: keep-alive";
    if (_stat_rd == 301 || _stat_rd == 302)
    {
        head += "\r\nLocation: " + transfer;
        head += "\r\n\r\n";
        return (head);
    }
    head += "\r\nContent-Type: " + _content_type;
    head += "\r\nContent-Length: " + sizetToStr(_filecontent.length());
    head += "\r\n\r\n";
    return (head);
}

int Response::treatRequest()
{
    int fd_file = -1;
    std::string method = _request->get_method();
    std::vector<Redirect>::iterator it = _conf->redirect.begin();

    _filepath = _request->get_path();

    while (it != _conf->redirect.end())
    {
        if (it->redirect1 == _filepath)
        {
            if (it->permanent == true)
                _stat_rd = 301;
            else
                _stat_rd = 302;
            transfer = it->redirect2;
            return(-1);
        }
        it++;
    }

    // if the filepath is a directory and does not end with a / then redirect to the directory with a / at the end of the path

    // std::cout << "_Filepath: " << _conf->root + _filepath << std::endl;
    if (_filepath != "" && is_directory(_conf->root + _filepath) == true && _filepath[_filepath.length() - 1] != '/')
    {
        // std::cout << "yeah" << std::endl;
        _stat_rd = 301;
        transfer = _filepath + "/";
        if(LOG == 1)
            std::cout << YELLOW << "[⊛ REDIRECTION]        => " << WHITE << _filepath + "/" << RESET << std::endl;
        return (-1);
    }
    get_filepath();
    if (_filepath == "")
    {
        _stat_rd = 400;
    }
    else if (is_directory(_filepath))
    {
        if (method != "GET")
            _stat_rd = 400;
        if (_conf->autoindex)
        {
            indexGenerator(&_filecontent, _filepath);
            _stat_rd = 200;
        }
    }
    else if (method == "DELETE")
        _stat_rd = method_delete();
    fd_file = openFile();
    return (fd_file);
}

void Response::makeResponse()
{
    if (_stat_rd != 301 && _stat_rd != 302)
    {
        if (((_request->get_method() == "POST" && _request->get_path().find(".php") != std::string::npos) || (_request->get_method() == "GET" && _request->get_path().find(".php?") != std::string::npos)) && (_stat_rd == 0 || _stat_rd == 200))
        {
            if (_request->get_method() == "GET")
                _filepath = _filepath.substr(0, _filepath.find(".php?") + 4);
            if (transfer.find("Content-type: ") != std::string::npos && transfer.find("\r\n", transfer.find("Content-type: ")) != std::string::npos)
                _content_type = transfer.substr(transfer.find("Content-type: ") + 14, transfer.find("\r\n", transfer.find("Content-type: ")) - transfer.find("Content-type: ") - 14);
            else
            {
                _content_type = "text/html";
            }
            if (transfer.find("\r\n\r\n") != std::string::npos)
                transfer = transfer.substr(transfer.find("\r\n\r\n") + 4, transfer.length());
        }
        else
            get_content_type();
        if (transfer != "")
            _filecontent = transfer;
    }
    _response = getHeader() + _filecontent + "\r\n";
    // if (_request->)       // NEED TO CHECK ACCEPT: REQUEST
    // std::cout << _filecontent.length() << std::endl;
    return;
}

void Response::clear()
{
    _header = "";
    _content_type = "text/html";
    _filecontent = "";
    _filepath = "";
    _stat_rd = 0;
    transfer = "";
    _response = "";
    writing = false;
    return;
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

    if (_request->get_path() != "" && _filepath.length() >= 4)
    {
        if (_filepath.substr(_filepath.length() - 4, 4) == ".svg")
            _content_type = "image/svg+xml";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".pdf")
            _content_type = "application/pdf";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".png")
            _content_type = "image/webp";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".css")
            _content_type = "text/css";
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".scss")
            _content_type = "text/css";
    }
    return (0);
}

void Response::get_filepath()
{
    if (_request->get_path() != "")
    {
        _filepath = _conf->root + _request->get_path();
        if (is_directory(_filepath) && _filepath[_filepath.length() - 1] != '/')
            _filepath += "/";
        if (is_directory(_filepath))
        {
            int fd;
            for (size_t i = 0; i < _conf->index.size(); i++)
            {
                if ((fd = open(std::string(_filepath + _conf->index[i]).c_str(), O_RDONLY)) >= 0)
                {
                    close(fd);
                    _filepath += _conf->index[i];
                }
            }
        }
    }
}

void    Response::setConf(Server *newconf)
{
    _conf = newconf;
    return ;
}

void    Response::setStatus(const int new_status)
{
    _stat_rd = new_status;
    return;
}

const std::string Response::error_page_message(const int status)
{
    if (status == 200)
        return ("OK");
    if (status == 403)
        return ("Forbidden");
    if (status == 404)
        return ("Not Found");
    if (status == 500)
        return ("Internal Server Error");
    if (status == 413)
        return ("Request Entity Too Large");
    if (status == 405)
        return ("Method Not Allowed");
    if (status == 502)
        return ("Bad Gateway");
    if (status == 411)
        return ("Length Required");
    if (status == 501)
        return ("Not Implemented");
    if (status == 406)
        return ("Not Acceptable");
    if (status == 301)
        return ("Moved Permanently");
    if (status == 302)
        return ("Found");
    return ("Bad Request");
}

int Response::openFile()
{
    int fd_file = -1;
    if (_stat_rd == 0)
    {
        if (!fileExist(_filepath))
            _stat_rd = 404;
        if (_stat_rd == 0) // le fichier existe
        {
            char buf[1];
            fd_file = open(_filepath.c_str(), O_RDONLY);
            if (fd_file < 0 || read(fd_file, buf, 0) < 0) // le fichier exite mais n'as pas les droits
                _stat_rd = 403;
            else
                _stat_rd = 200; // le fichier est lisible
        }
    }
    if (_stat_rd != 200 && _stat_rd != 301 && _stat_rd != 302)
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
    return (fd_file);
}