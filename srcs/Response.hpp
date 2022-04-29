#pragma once

// #include "server.hpp"

class Response
{
public:
    Response(std::string client_data, s_server *conf) : _conf(conf), _client_data(client_data),  _request(client_data), _status("400 Bad Request"), _header(""), _filecontent(""), _filepath("")
    {
        _response = get_response();
    }

    std::string getDate();
    std::string getHeader();
    std::string getBody() { return (_filecontent); }
    int get_status();
    int reading_file();
    int set_redirection();
    int get_content_type();
    void get_filepath();


    std::string get_response()
    {
        get_filepath();
        // std::cout << "1." << _filepath << std::endl;
        set_redirection();
        // std::cout << "2." << _filepath << std::endl;
        get_status();
        // std::cout << "3." << _status << std::endl;
        get_content_type();
        _header = getHeader();
        // std::cout << "4." << _header << std::endl;
        return(_header + _filecontent);
    }

    Response(Response const &src);
    Response &operator=(Response const &src);
    ~Response(void) {};

private:
    std::string _defaultfolder;
    s_server *_conf;
    Request _request;
    std::string _status;
    int _stat_rd;
    std::string _header;
    std::string _client_data;
    std::string _content_type;
    std::string _response;
    std::string _filecontent;
    std::string _filepath;
};

// std::ostream    &operator<<(std::ostream &ostream, const Response &src);