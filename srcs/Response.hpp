#pragma once

class Response
{
public:
    Response(Request *request, Server *conf) : _conf(conf), _request(request), _header(""),  _content_type("text/html"), _filecontent(""), _filepath(""), _stat_rd(400)
    {
        std::string cgi_response = "";
        get_filepath();
        if (is_cgi() == true)
            cgi_response = treat_cgi();
        set_redirection(cgi_response);
        get_status();
        get_content_type();
        _response = getHeader() + _filecontent + "\r\n\r\n";
    }
    Response()
    {
        _stat_rd = 400;
        _header = "";
        _content_type = "text/html";
        _filecontent = "";
        _filepath = "";
        _response = "";
    }

    Response(Response const &src)
    {
        _conf = src._conf;
        _request = src._request;
        _header = src._header;
        _content_type = src._content_type;
        _filecontent = src._filecontent;
        _filepath = src._filepath;
        _stat_rd = src._stat_rd;
        _response = src._response;
    }

    Response operator=(const Response &src)
    {
        _stat_rd = src._stat_rd;
        _header = src._header;
        _content_type = src._content_type;
        _filecontent = src._filecontent;
        _filepath = src._filepath;
        _response = src._response;
        return (*this);
    }

    std::string getDate();
    std::string getHeader();
    std::string get_pathfile() { return (_filepath); }
    int readFile(std::string filename, std::string *fileContent);
    std::string get_response() { return (_response); }
    std::string getBody() { return (_filecontent); }
    int get_status();
    int set_redirection(std::string cgi_response);
    int getstat() { return (_stat_rd); }
    int get_content_type();

    bool is_waiting() { return (_is_waiting); }
    void get_filepath();

    ~Response(void){};

private:
    Server *_conf;
    Request *_request;
    std::string _status;
    int _is_waiting;
    std::string _header;
    std::string _content_type;
    std::string _response;
    std::string _filecontent;
    std::string _filepath;
    int _stat_rd;

    bool    is_cgi(void);
    std::string&    treat_cgi(void);
};

// getter de ready, constructeur par defaut et copie, et fonction clear et getter qui dit si cest vide (is_empty), et sil le texte est faux il faut clear