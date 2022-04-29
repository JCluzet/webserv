#include "server.hpp"


std::string Response::getHeader()
{
    std::string head = "";
    head += "HTTP/1.1 " + _status;
    head += "\nContent-Type: " + _content_type;
    head += "\nContent-Length: " + sizetToStr(_filecontent.length());
    head += "\nServer: WebServ v1.0";
    head += "\nDate : " + getDate();
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
    {
        _status = "200 OK";
    }
    return (0);
}

int Response::reading_file()
{
    _stat_rd = readFile(_filepath.c_str(), &_filecontent);
    return (0);
}

int Response::set_redirection()
{
    if (_filepath == "")
        _stat_rd = 400;
    else
    {
        if (_conf->autoindex && is_directory(_filepath))
            indexGenerator(&_filecontent, _filepath);
        else
            reading_file();
    }
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
    if (_request.get_path() != "")
    {
        _filepath = _conf->default_folder + _request.get_path();
        if (is_directory(_filepath) && _filepath[_filepath.length() - 1] != '/')
            _filepath += "/";
        if (is_directory(_filepath))
            _filepath += _conf->default_page;
    }
}