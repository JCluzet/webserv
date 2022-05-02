/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/01 23:39:22 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::string Response::getHeader()
{
    std::string head = "";
    head += "HTTP/1.1 " + _status;
    head += "\nServer: WebServ v1.0";
    head += "\nDate : " + getDate();
    head += "\nContent-Type: " + _content_type;
    head += "\nContent-Length: " + sizetToStr(_filecontent.length());
    head += "\r\n\r\n";
    return (head);
}

// std::string Response::getDate()
// {
//     time_t now = time(0);
//     struct tm tstruct;
//     char buf[80];
//     tstruct = *localtime(&now);
//     strftime(buf, sizeof(buf), "%a, %d %b %Y %X %Z", &tstruct);
//     return (buf);
// }

int Response::set_redirection()
{
    if (_filepath == "")
    {
        _stat_rd = 400;
        return(1);
    }
    if (_conf->autoindex && is_directory(_filepath))
    {
        indexGenerator(&_filecontent, _filepath);
        _stat_rd = 200; 
    }
    else
        _stat_rd = readFile(_filepath.c_str(), &_filecontent);
    return (0);
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

// bool Response::is_aCGI(std::string path)
// {
    
// }

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
            _filepath += _conf->default_page;
    }
}

int Response::get_status()
{
    if (_stat_rd == 404)
    {
        if (_conf->page404 == "")
            _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        else
        {
            _filepath = _conf->root + "/" + _conf->page404;
            readFile(_filepath.c_str(), &_filecontent);
            if (_stat_rd == 404 && _conf->page404 == "")
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