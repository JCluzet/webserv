/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 18:40:00 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/30 23:45:48 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

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

int Response::openFile()
{
    int fd_file = -1;
    if (_stat_rd == 0)
    {
        if (access(_filepath.c_str(), F_OK) && is_directory(_filepath) == false)
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
    if (_stat_rd != 200 && _stat_rd != 201 && _stat_rd != 301 && _stat_rd != 302 && _stat_rd != 401)
    {
        if (_conf == NULL || _conf->error_page.empty() || _conf->error_page.find(_stat_rd) == _conf->error_page.end())
            _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR " + intToStr(_stat_rd) + "</h1>\n    <p>" + error_page_message(_stat_rd) + "</p>\n</body>\n\n</html>";
        else
        {
            _filepath = _conf->root + _conf->error_page[_stat_rd];
            if (access(_filepath.c_str(), F_OK) || (fd_file = open(_filepath.c_str(), O_RDONLY)) < 0)
            {
                _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR " + intToStr(_stat_rd) + "</h1>\n    <p>" + error_page_message(_stat_rd) + "</p>\n</body>\n\n</html>";
            }
        }
    }
    return (fd_file);
}

int Response::treatRequest()
{
    int fd_file = -1;
    std::string method = _request->get_method();
    
    if (_request->is_valid() != 0)
    {
        _stat_rd = _request->is_valid();
        fd_file = openFile();
        return (fd_file);
    }

    get_filepath();
    /* --Redirections-- */
    std::vector<Redirect>::iterator it = _conf->redirect.begin();
    while (it != _conf->redirect.end())
    {
        if (it->redirect1.length() <= _request->get_path_o().length() && it->redirect1 == _request->get_path_o().substr(0, it->redirect1.length()))
        {
            if (it->permanent == true)
                _stat_rd = 301;
            else
                _stat_rd = 302;
            transfer = it->redirect2;
            return -1;
        }
        it++;
    }
    if (is_directory(_filepath) == true
        && _request->get_path_o()[_request->get_path_o().length() - 1] != '/')
    {
        _stat_rd = 301;
        transfer = _request->get_path_o() + "/";
        return -1;
    }
    else if (is_directory(_filepath) == false && access(std::string(_filepath).c_str(), F_OK) == 0
        && _request->get_path_o()[_request->get_path_o().length() - 1] == '/' && _request->get_path_o() != "/")
    {
        _stat_rd = 301;
        transfer = _request->get_path_o().substr(0, _request->get_path_o().length() - 1);
        return -1;
    }
    /* ---- */

    /* --Url_Decode-- */
	if (access(_filepath.c_str(), F_OK) != 0)
	{
        std::string tmp;
    	if (_request->get_method() == "GET" && _filepath.find("?") != std::string::npos)
    	{
            std::string jquery = "";
	        for (size_t i = 0; i < _conf->cgi.size(); i++)
	        {
	            if (_filepath.find("." + _conf->cgi[i].first + "?") != std::string::npos)
	    		{
	                jquery = _filepath.substr(_filepath.rfind("." + _conf->cgi[i].first + "?") + _conf->cgi[i].first.length() + 1);
	    			tmp = _filepath.substr(0, _filepath.rfind("." + _conf->cgi[i].first + "?") + _conf->cgi[i].first.length() + 1);
	    			tmp = url_decode(tmp) + jquery;
	    			break ;
	    		}
                if (i == _conf->cgi.size() - 1)
                    tmp = url_decode(_filepath);
	    	}
       }
       else
	        tmp = url_decode(_filepath);
	    if (access(tmp.c_str(), F_OK) == 0)
	    	_filepath = tmp;
	}
    /* ---- */

    if (is_cgi(_request, _conf) == true && _stat_rd != 301 && _stat_rd != 302)
        return -1;
    if (_filepath == "")
    {
        _stat_rd = 400;
    }
    else if (is_directory(_filepath))
    {
        if (_conf->autoindex && method == "GET")
        {
            indexGenerator(&_filecontent, _filepath, _conf->root);
            _stat_rd = 200;
        }
        else
            _stat_rd = 403;
    }
    else if (method == "DELETE")
    {
        if (_request->get_header("Authorization") == "")
            _stat_rd = 401;
        else
            _stat_rd = method_delete();
    }
    fd_file = openFile();
    return (fd_file);
}

void Response::makeResponse()
{
    std::string set_cookie = "";

    if (_stat_rd != 301 && _stat_rd != 302 && _stat_rd != 401)
    {
        if (is_cgi(_request, _conf) && (_stat_rd == 0 || _stat_rd == 200 || _stat_rd == 201))
        {
            size_t  i = 0;
            for (; i < _conf->cgi.size(); i++)
            {
                    if (_filepath.rfind("." + _conf->cgi[i].first + "?") != std::string::npos)
                        break ;
            }
            if (i == _conf->cgi.size())
            {
                if (_request->get_method() == "GET")
                    _filepath = _filepath.substr(0, _filepath.rfind("?"));
            }
            else
            {
                if (_request->get_method() == "GET")
                    _filepath = _filepath.substr(0, _filepath.rfind("." + _conf->cgi[i].first + "?"));
                if (_request->get_method() == "POST" && _filepath.find("." + _conf->cgi[i].first + "?") != std::string::npos)
                    _filepath = _filepath.substr(0, _filepath.rfind("." + _conf->cgi[i].first + "?"));
            }
            if (transfer.find("Content-type: ") != std::string::npos && transfer.find("\r\n", transfer.find("Content-type: ")) != std::string::npos)
                _content_type = transfer.substr(transfer.find("Content-type: ") + 14, transfer.find("\r\n", transfer.find("Content-type: ")) - transfer.find("Content-type: ") - 14);
            else
            {
                _content_type = "text/html";
            }
            if (transfer.find("\r\n\r\n") != std::string::npos)
            {
                std::string tmp = transfer;
                while (tmp.substr(0, 4) != "\r\n\r\n")
                {
                    if (tmp.substr(0, 2) == "\r\n")
                        tmp.erase(0, 2);
                    if (tmp.length() > 10 && tmp.substr(0, 11) == "Set-Cookie:")
                        set_cookie += "\r\n" + tmp.substr(0, tmp.find("\r\n"));
                    tmp.erase(0, tmp.find("\r\n"));
                }
                transfer = transfer.substr(transfer.find("\r\n\r\n") + 4, transfer.length());
            }
        }
        else
            get_content_type();
    }
    _filecontent += transfer;
    if ((_request->get_method() == "GET" || _request->get_method() == "POST") && _stat_rd == 200 && _filecontent == "")
        _stat_rd = 204;
    _response = getHeader(set_cookie) + _filecontent;
    return;
}

void Response::get_filepath()
{
    if (_request->get_path() != "")
    {
        _filepath = _conf->root + _request->get_path();
        if (is_directory(_filepath))
        {
            int fd;
            for (size_t i = 0; i < _conf->index.size(); i++)
            {
                if ((fd = open(std::string(_filepath + _conf->index[i]).c_str(), O_RDONLY)) >= 0)
                {
                    close(fd);
                    if (_filepath[_filepath.length() - 1] == '/')
                        _filepath = _filepath.substr(0, _filepath.length() - 1);
                    _filepath += _conf->index[i];
                    
                }
            }
        }
    }
}

std::string Response::getHeader(std::string set_cookie)
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
    if (_stat_rd == 401)
    {
        head += "\r\nWWW-Authenticate: Basic";
        head += "\r\n\r\n";
        return (head);
    }
    head += "\r\nContent-Length: " + sizetToStr(_filecontent.length());
    head += "\r\nContent-Type: " + _content_type;
    head += set_cookie;
    head += "\r\n\r\n";
    return (head);
}

int Response::method_delete(void)
{
    std::ifstream ifs;
    ifs.open(_filepath.c_str());

    if (!ifs)
        return (404);
    ifs.close();
    if (_request->get_header("Authorization") == "Basic user42:user42")
    {
        if (remove(_filepath.c_str()) < 0)
            return (500);
        return (204);
    }
    return (403);
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

void    Response::setStatus(const int new_status)
{
    _stat_rd = new_status;
    return;
}

void    Response::setConf(Server *newconf)
{
    _conf = newconf;
    return ;
}

Server* Response::get_conf()
{
    return (_conf);
}

std::string Response::get_fpath()
{
    return (_filepath);
}

const std::string Response::error_page_message(const int status)
{
    if (status == 200)
        return ("OK");
    if (status == 201)
        return ("Created");
    if (status == 204)
        return ("No Content");
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
    if (status == 401)
        return ("Unauthorized");
    return ("Bad Request");
}

int Response::get_content_type()
{
    _content_type = "text/html";

        if (_request->get_path() != "" && _filepath.length() >= 5)
    {
        // TEXT TYPE
        if (_filepath.substr(_filepath.length() - 4, 4) == ".css" )
           _content_type = "text/css";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".csv")
            _content_type = "text/csv";
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".html")
            _content_type = "text/html";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".xml")
            _content_type = "text/xml";
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".scss")
            _content_type = "text/css";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".txt")
            _content_type = "text/plain";

        // IMAGE TYPE
        if (_filepath.substr(_filepath.length() - 4, 4) == ".svg")
            _content_type = "image/svg+xml";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".gif")
            _content_type = "image/gif";
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".jpeg")
            _content_type = "image/jpeg";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".png")
            _content_type = "image/webp";                               // show png as a webp
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".tiff")
            _content_type = "image/tiff";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".webp")
            _content_type = "image/webp";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".ico")
            _content_type = "image/x-icon";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".bmp")
            _content_type = "image/bmp";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jpg")
            _content_type = "image/jpeg";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jpe")
            _content_type = "image/jpeg";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jif")
            _content_type = "image/jif";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jfif")
            _content_type = "image/jfif";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jfi")
            _content_type = "image/jfi";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jpx")
            _content_type = "image/jpx";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jp2")
            _content_type = "image/jp2";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".j2k")
            _content_type = "image/j2k";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".j2c")
            _content_type = "image/j2c";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".jpc")
            _content_type = "image/jpc";
        
        // AUDIO TYPE
        if (_filepath.substr(_filepath.length() - 4, 4) == ".mp3")
            _content_type = "audio/mpeg";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".wav")
            _content_type = "audio/wav";
        else if (_filepath.substr(_filepath.length() - 5, 5) == ".flac")
            _content_type = "audio/flac";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".aac")
            _content_type = "audio/aac";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".ogg")
            _content_type = "audio/ogg";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".oga")
            _content_type = "audio/oga";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4a")
            _content_type = "audio/m4a";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4b")
            _content_type = "audio/m4b";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4p")
            _content_type = "audio/m4p";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4r")
            _content_type = "audio/m4r";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4v")
            _content_type = "audio/m4v";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4s")
            _content_type = "audio/m4s";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4a")
            _content_type = "audio/m4a";
        
        // VIDEO TYPE

        if (_filepath.substr(_filepath.length() - 4, 4) == ".mp4")
            _content_type = "video/mp4";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4v")
            _content_type = "video/m4v";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4p")
            _content_type = "video/m4p";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4b")
            _content_type = "video/m4b";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4r")
            _content_type = "video/m4r";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4s")
            _content_type = "video/m4s";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4a")
            _content_type = "video/m4a";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4v")
            _content_type = "video/m4v";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4p")
            _content_type = "video/m4p";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4b")
            _content_type = "video/m4b";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4r")
            _content_type = "video/m4r";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".m4s")
            _content_type = "video/m4s";

        // APPLICATION TYPE
        
        if (_filepath.substr(_filepath.length() - 4, 4) == ".pdf")
            _content_type = "application/pdf";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".doc")
            _content_type = "application/msword";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".docx")
            _content_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".xls")
            _content_type = "application/vnd.ms-excel";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".xlsx")
            _content_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".ppt")
            _content_type = "application/vnd.ms-powerpoint";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".pptx")
            _content_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".pps")
            _content_type = "application/vnd.ms-powerpoint";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".ppsx")
            _content_type = "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".odt")
            _content_type = "application/vnd.oasis.opendocument.text";
        else if (_filepath.substr(_filepath.length() - 4, 4) == ".odp")
            _content_type = "application/vnd.oasis.opendocument.presentation";
    }

    return (0);
}
