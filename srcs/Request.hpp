#pragma once

#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <map>
// #include <pair>

#define NB_HEADERS 20


class Request 
{
public:
// constructors
    Request(std::string request) : _request(request), _end(false), _valid(false), _method("")
    , _path(""), _version("")
    {
        if (_request.find("\r\n\r\n") != std::string::npos)
        {
            _request.erase(_request.begin() + _request.find("\r\n\r\n"), _request.end() - 1);
            _end = true;
        }
        std::string r = _request;
        if (!get_request_first_line(r))
        {
            std::cout << "bad request. (first line)" << std::endl;
            return;
        }
        _valid = true;
        r.erase(0, r.find("\r\n") + 2);
        init_header();
        while (r.find("\r\n") != std::string::npos)
        {
            if (!get_request_line(r.substr(0, r.find("\r\n"))))
            {
                std::cout << "bad request. (" << r.substr(0, r.find("\r\n")) << ")" << std::endl;
                _valid = false;
                return;
            }
            r.erase(0, r.find("\r\n") + 2);
        }
    }

    ~ Request(){};
//geter
    std::string get_path() const {return _path;}
    std::string get_method() const {return _method;}
    std::string get_version() const {return _version;}
    std::string get_header(std::string key) {return _m[key];}
    std::string get_request() const {return _request;}
    void        print() {pprint();}
//modifier
    bool    add(std::string r)
    {
        if (_end)
            return (false);
        if (r.find("\r\n\r\n") != std::string::npos)
        {
            r.erase(r.begin() + r.find("\r\n\r\n"), r.end() - 1);
            _end = true;
        }
        std::string tmp = r;
        while (r.find("\r\n") != std::string::npos)
        {
            if (!get_request_line(r.substr(0, r.find("\r\n"))))
            {
                std::cout << "bad request." << std::endl;
                return false;
            }
            r.erase(0, r.find("\r\n") + 2);
        }
        _request += tmp;
        return(true);
    }

private:
    std::string         _request;
    std::string         _path;
    std::string         _method;
    std::string         _version;
    std::string         _header[NB_HEADERS];
    std::map<std::string, std::string> _m;
    bool                _end;
    bool                _valid;

    void    pprint(){
        std::cout << "method :" << _method << std::endl;
        std::cout << "path :" << _path << std::endl;
        std::cout << "version :" << _version << std::endl;
        for (int i = 0; i < NB_HEADERS; i++)
        {
            if (_m[_header[i]].length())
                std::cout << _header[i] << " :" << _m[_header[i]] << std::endl;
        }
        std::cout << "end :" << _end << std::endl;
    }

    bool    get_request_line(std::string r) // Check request line conformitys
    {   
        std::string ctn;

        if (!is_header(r))
        {
            return(true);
        }
        ctn = r.substr(0, r.find(":"));
        r.erase(0, r.find_first_of(" "));
        r.erase(0, r.find_first_not_of(" "));
        _m[ctn] = r;    
        return true;
    }

    bool    get_request_first_line(std::string request) // Check request first line conformitys
    {
    // Check & get method
        if (request.substr(0,3) == "GET"
            && request.substr(0, 4) == "POST"
            && request.substr(0, 6) == "DELETE")
            return false;
        _method = request.substr(0, request.find(" "));
        request.erase(0, request.find_first_of(" "));
        request.erase(0, request.find_first_not_of(" "));
    // Check & get path
        if (!file_exist((request.substr(0, request.find_first_of(" "))).c_str()))
            return false;
        _path = request.substr(0, request.find(" "));
        request.erase(0, request.find_first_of(" "));
        request.erase(0, request.find_first_not_of(" "));
    // Check version
        if (request.substr(0, request.find('\n')) == "HTTP/1.1"
            && request.substr(0, request.find('\n')) ==  "HTTP/1.0")
            return false;
        _version = request.substr(0, request.find("\r\n"));
        return true;
    }

    bool    is_header(std::string s) const //verifie si s fait partie des headers
    {
        if (s.find(":") == std::string::npos)
                return false;
        for (int i = 0; i < NB_HEADERS; i++)
            if (_header[i] == s.substr(0, s.find(":")))
                return (true);
        return (false);
    }

    void    init_header()
    {
        _header[0] = "Accept-Charsets";
	    _header[1] = "Accept-Language";
	    _header[2] = "Allow";
	    _header[3] = "Auth-Scheme";
	    _header[4] = "Authorization";
	    _header[5] = "Content-Language";
	    _header[6] = "Content-Length";
	    _header[7] = "Content-Location";
	    _header[8] = "Content-Type";
	    _header[9] = "Date";
	    _header[10] = "Host";
	    _header[11] = "Last-Modified";
	    _header[12] = "Location";
	    _header[13] = "Referer";
	    _header[14] = "Retry-After";
	    _header[15] = "Server";
	    _header[16] = "Transfer-Encoding";
	    _header[17] = "User-Agent";
	    _header[18] = "Www-Authenticate";
	    _header[19] = "Connection";
        _m["Accept-Charsets"] = "";
        _m["Accept-Language"] = "";
        _m["Allow"] = "";
        _m["Auth-Scheme"] = "";
        _m["Authorization"] = "";
        _m["Content-Language"] = "";
        _m["Content-Length"] = "";
        _m["Content-Location"] = "";
        _m["Content-Type"] = "";
        _m["Date"] = "";
        _m["Host"] = "";
        _m["Last-Modified"] = "";
        _m["Location"] = "";
        _m["Referer"] = "";
        _m["Retry-After"] = "";
        _m["Server"] = "";
        _m["Transfer-Encoding"] = "";
        _m["User-Agent"] = "";
        _m["Www-Authenticate"] = "";
        _m["Connection"] = "";        
    }


    bool    file_exist(const char* file)
    {
        FILE* f = fopen(file, "r");
        if (f)
        {
            fclose(f);
            return (true);
        }
        return (false);
    }


};
// std::ostream&	operator<<(std::ostream & ostream, Request const & src) {
// 	ostream << src.get_method() << "\t" << src.get_file() << "\t" << src.get_version() << std::endl;
//     for (unsigned int i; i < NB_HEADERS; i++)
//     {
//         if(_m.get_header(_header[i]) != "")
//             ostream << _header[i] << ": " << _m[_header[i]] << std::endl;])
//     }

// 	return (ostream);
// }