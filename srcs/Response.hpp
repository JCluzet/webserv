#pragma once

#include "server.hpp"

class Server;
class Request;


class Response
{
private:
    Server*     _conf;
    Request*    _request;
    int _is_waiting;
    std::string _header;
    std::string _content_type;
    std::string _response;
    std::string _filecontent;
    std::string _filepath;
    int         _stat_rd;

    int             method_delete(void);

public:
    std::string transfer;
    bool        writing;

    Response(Request* request);
    Response();
    Response(Response const &src);
    ~Response() {};

    Response& operator=(const Response &src);

    void        clear();
    std::string getDate();
    std::string getHeader(std::string set_cookie);
    std::string get_pathfile() { return (_filepath); }
    int readFile(std::string filename, std::string *fileContent);
    std::string get_response() { return (_response); }
    std::string getBody() { return (_filecontent); }
    Request*    getRequest() { return (_request); }
    void        setRequest(Request* req) { _request = req; return ; }
    void        setStatus(int new_status);
    void        setConf(Server *newconf);
    Server*     get_conf();

    const std::string error_page_message(int status);
    int openFile();
    int getstat() { return (_stat_rd); }
    int get_content_type();

    bool is_waiting() { return (_is_waiting); }
    void get_filepath();
    std::string c_type() { return (_content_type); }

    int  treatRequest();
    void makeResponse();

};