#pragma once

class Response
{
public:
    Response(Request *request, t_server *conf) : _conf(conf), _request(request), _header(""),  _content_type("text/html"), _filecontent(""), _filepath(""), _stat_rd(400)
    {
        _isaCGI = false;
        get_filepath();
        set_redirection();
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
        _isaCGI = false;
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
        _isaCGI = src._isaCGI;
    }

    Response operator=(const Response &src)
    {
        _stat_rd = src._stat_rd;
        _header = src._header;
        _content_type = src._content_type;
        _filecontent = src._filecontent;
        _filepath = src._filepath;
        _response = src._response;
        _isaCGI = src._isaCGI;
        return (*this);
    }

    std::string getHeader();
    std::string get_pathfile() { return (_filepath); }
    int readFile(std::string filename, std::string *fileContent);
    std::string get_response() { return (_response); }
    std::string getBody() { return (_filecontent); }
    bool is_aCGI(std::string path); 
    int get_status();
    int set_redirection();
    int getstat() { return (_stat_rd); }
    int get_content_type();

    bool is_waiting() { return (_is_waiting); }
    void get_filepath();

    ~Response(void){};

private:
    t_server *_conf;
    Request *_request;
    std::string _status;
    int _is_waiting;
    bool _isaCGI;
    std::string _header;
    std::string _content_type;
    std::string _response;
    std::string _filecontent;
    std::string _filepath;
    int _stat_rd;
};

// getter de ready, constructeur par defaut et copie, et fonction clear et getter qui dit si cest vide (is_empty), et sil le texte est faux il faut clear