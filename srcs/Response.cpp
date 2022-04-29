/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/04/29 23:39:39 by jcluzet          ###   ########.fr       */
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
    head += "\n\n";
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

int Response::set_redirection()
{
    if (_filepath == "")
    {
        _stat_rd = 400;
        return(1);
    }
    if (_conf->autoindex && is_directory(_filepath))
        indexGenerator(&_filecontent, _filepath);
    else
        _stat_rd = readFile(_filepath.c_str(), &_filecontent);
    return (0);
}

int Response::get_content_type()
{
    _content_type = "text/html";
    if (_filepath.substr(_filepath.length() - 4, 4) == ".svg")
        _content_type = "image/svg+xml";
    if (_filepath.substr(_filepath.length() - 4, 4) == ".pdf")
        _content_type = "application/pdf";
    if (_filepath.substr(_filepath.length() - 4, 4) == ".png")
        _content_type = "image/apng";
    return (0);
}

void Response::get_filepath()
{
    if (_request->get_path() != "")
    {
        _filepath = _conf->default_folder + _request->get_path();
        if (is_directory(_filepath) && _filepath[_filepath.length() - 1] != '/')
            _filepath += "/";
        if (is_directory(_filepath))
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
            _filepath = _conf->default_folder + "/" + _conf->page404;
            readFile(_filepath.c_str(), &_filecontent);
        }
        _status = "404 Not Found";
    }
    if (_stat_rd == 400)
    {
        _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 400</h1>\n    <p>Bad Request.</p>\n</body>\n\n</html>";
        _status = "400 Bad Request";
    }
    else if (_stat_rd == 0)
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