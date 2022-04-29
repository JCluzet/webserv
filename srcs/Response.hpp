#pragma once

// #include "server.hpp"

class Response
{
public:
    Response(std::string client_data, std::string default_f) : _defaultfolder(default_f), _request(client_data), _status(400), _ready(false), _header(""), _body(""), _filecontent(""), _filepath(""), _content_lenght(0)
    {
        _client_data = client_data;
        Generate_response();
    }

    std::string getDate()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%a, %d %b %Y %X %Z", &tstruct);
        return (buf);
    }

    int getHeader()
    {
        std::string head = "";
        head += "HTTP/1.1 " + _status;
        head += "\nContent-Type: " + _content_type;
        head += "\nContent-Length: " + _filecontent.length();
        head += "\nServer: WebServ v1.0";
        head += "\nDate : " + getDate();
    }

    int get_status()
    {
        if (_stat_rd == 404)
        {
            if (_conf.serv[0].page404 == "")
                _filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
            else
            {
                _filepath = _conf.serv[0].default_folder + "/" + _conf.serv[0].page404;
                readFile(_filepath.c_str(), &_filecontent);
            }
            _status = "404 Not Found"
        }
        else if (_stat_rd == 0)
        {
            _status = "200 OK";
        }
    }

    int reading_file()
    {
        _stat_rd = readFile(_filepath.c_str(), &_filecontent);
        get_status();
    }

    int Generate_response()
    {
        get_filepath();
        if (_filepath == "")
        {
            _status = "400 Bad Request";
            _ready = true;
        }
        else
        {
            if (_conf.serv[0].autoindex && is_directory(_filepath))
                indexGenerator(&_filecontent, _filepath);
            else
                reading_file();
        }
        getHeader();
    }

    Response(Response const &src);
    ~Response(void);

    Response &operator=(Response const &src);

private:
    void get_filepath()
    {
        if (_request.get_path() != "")
        {
            _filepath = _defaultfolder + _request.get_path();
            if (is_directory(_filepath))
                _filepath += _conf.serv[0].default_folder;
        }
    }
    std::string _defaultfolder;
    Config _conf; // --> a rajouter
    Request _request;
    std::string _status;
    int _stat_rd;
    std::string _header;
    std::string _client_data;
    std::string _content_type;
    // unsigned long _content_lenght;
    std::string _response;
    std::string _filecontent;
    std::string _filepath;
    int _ready;
};

// std::ostream    &operator<<(std::ostream &ostream, const Response &src);