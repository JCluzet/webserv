#pragma once

#include "server.hpp"

#define NB_HEADERS 24
#define NL "\r\n"
#define NLSIZE 2

std::string chunked(std::string str);


class Request
{
public:
    // constructors
    Request() : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(false), _body(""), _line(""), _chunked(false)
    {
        init_header_map();
    }

    Request(std::string new_request) : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(false), _body(""), _line(""), _chunked(false)
    {
        init_header_map();
        addp(new_request);
    }

    Request(const Request &src)
    {
        *this = src;
        return ;
    }

    Request &operator=(const Request &src)
    {
        if (this != &src)
        {
            init_header_map();
            _request = src._request;
            _path = src._path;
            _method = src._method;
            _version = src._version;
            _end = src._end;
            _valid = src._valid;
            _body = src._body;
            _line = src._line;
            _chunked = src._chunked;
        }
        return *this;
    }

    ~Request() {};

    //getter
    std::string get_path() const { return _path; };

    void set_path(std::string new_path) { _path = new_path; return ; };

    std::string get_body() const { return _body; };

    std::string get_method() const { return _method; };

    std::string get_version() const { return _version; };

    std::string get_header(std::string key) { return _m[key]; };

    std::string get_request() const { return _request; };

    bool ready() const { return _end; };

    void add(std::string new_request)
    {
        if (!(_valid = addp(new_request)))
        {
            clear();
            _end = true;
            return ;
        }
        if (_valid == true && _end == true && _method == "POST")
        {

            if (_m["Content-Type"] == "")
            {
                _m["Content-Type"] = "text/plain";
            }
            if (_m["Content-Type"] != "text/plain" && (_m["Content-Type"].find(";") == std::string::npos
                    || _m["Content-Type"].substr(0, _m["Content-Type"].find(";")) != "multipart/form-data")
                && _m["Content-Type"] != "application/x-www-form-urlencoded")
            {
                _valid = false;
                clear();
            }
        }
    }

    void print() { pprint(); }

    void clear()
    {
        _request = "";
        _path = "";
        _method = "";
        _version = "";
        _end = false;
        _valid = false;
        _line = "";
        _chunked = false;
        init_header_map();
    }
    //modifier
private:
    std::map<const std::string, std::string> _m;
    std::string _request;
    std::string _path;
    std::string _method;
    std::string _version;
    std::string _header[NB_HEADERS];
    bool    _end;
    bool    _valid;
    std::string _body;
    std::string _line;
    bool        _chunked;

    bool addp(std::string r)
    {
        std::string::size_type nl;
        //cut_end(&r);
        if (_request.empty() == true && (r.empty() == true || r == NL))
            return true;
        if (_line == "" && (r == "" || r.substr(0, 1) == " "))
            return false;
        if (_chunked == true)
            r = chunked(r);
        r = _line + r;
        if (r.find(NL) != std::string::npos && r.find_last_of(NL) + 1 < r.length())
            _line = r.substr(r.find_last_of(NL) + 1, r.length());
        else if (r.find(NL) != std::string::npos && r.find_last_of(NL) + 1 == r.length())
            _line = "";
        else
            _line = r;
        while ((nl = r.find(NL)) != std::string::npos)
        {
            if (_request.empty() == true)
            {
                if (!get_request_first_line(r.substr(0, nl + NLSIZE)))
                    return false;
            }
            else if (r[0] == '\r' && r[1] == '\n' && _line == "")
            {
                if (_method == "GET" || _method == "DELETE")
                {    
                    _request += r;
                    _end = true;
                    return true;
                }
                if (_method == "POST" && (_m["Content-Length"] == "" || _m["Transfer-Encoding"] == "chunked"))
                {
                    _chunked = true;
                }
                if (_method == "POST" && (_m["Content-Length"] == "" || _m["Content-Length"].find_first_not_of("0123456789") != std::string::npos))
                        return false;
            }
            else if (_method == "POST" && _request.find("\r\n\r\n") != std::string::npos)
            {
                _body += r;
                if (static_cast<int>(_body.length()) >= atoi(_m["Content-Length"].c_str()))
                {

                    if (static_cast<int>(_body.length()) > atoi(_m["Content-Length"].c_str()))
                        _body = _body.substr(0, atoi(_m["Content-Length"].c_str()) - 1);
                    _request += r;
                    _end = true;
                    return true;
                }
            }
            else
            {
                get_request_line(r.substr(0, nl + NLSIZE));
            }
            _request += r.substr(0, nl + NLSIZE);
            r.erase(0, nl + NLSIZE);
        }
        if (r.empty() == false && _method == "POST" && _request.find("\r\n\r\n") != std::string::npos
            && static_cast<int>(_body.length() + r.length()) >= atoi(_m["Content-Length"].c_str()))
        {
            _body += r;
            _request += r;
            if (static_cast<int>(_body.length()) > atoi(_m["Content-Length"].c_str()))
                _body = _body.substr(0, atoi(_m["Content-Length"].c_str()) - 1);
            _end = true;
            return true;
        }
        return true;
    }

    /*void cut_end(std::string *r)
    {
        std::string end = NL;
        std::string::size_type x;
        end += NL;
        if ((x = r->find(end)) != std::string::npos)
        {
            *r = r->substr(0, x + NLSIZE);
            _end = true;
        }
    }*/

    void pprint()
    {
        std::cout << "method :" << _method << std::endl;
        std::cout << "path :" << _path << std::endl;
        std::cout << "version :" << _version << std::endl;
        for (int i = 0; i < NB_HEADERS; i++)
        {
            if (_m[_header[i]].length())
                std::cout << _header[i] << " :" << _m[_header[i]] << std::endl;
        }
        std::cout << "end :" << _end << std::endl
                  << std::endl;
    }

    bool get_request_line(std::string r) // Check request line conformitys
    {
        std::string ctn;
        std::string::size_type pos;

        if ((pos = r.find(": ")) == std::string::npos)
        {
            return (false);
        }
        ctn = r.substr(0, pos); //secu
        if (ctn.find(" ") != std::string::npos)
        {
            return (false);
        }
        if (!is_header(r.substr(0, pos)))
        {
            return (true);
        }
        r.erase(0, pos + 2);
        _m[ctn] = r.substr(0, r.length() - 2);
        return true;
    }

    bool get_request_first_line(std::string r) // Check request first line conformitys
    {
        std::string::size_type next_space = std::string::npos;
        std::string::size_type next_nl = std::string::npos;
        std::string::size_type n = std::string::npos;
        std::string request = r;

        if ((next_space = request.find(" ")) == n)
            return (false);
        if ((next_nl = request.find(NL)) == n)
            return (false);
        if (next_nl < request.length())
            request = request.substr(0, next_nl);
        request += "   ";
        if (request.substr(0, next_space) != "GET" && request.substr(0, next_space) != "POST" && request.substr(0, next_space) != "DELETE")
            return (false);
        _method = request.substr(0, next_space);
        //std::cout << request.substr(0, next_space) << " " << _method << std::endl;
        request.erase(0, next_space);
        if ((next_space = request.find_first_not_of(" ")) == n)
            return (false);
        request.erase(0, next_space);
        if ((next_space = request.find(" ")) == n)
            return (false);
        _path = request.substr(0, next_space); // secu
        request.erase(0, next_space);          //secu
                                               // Check version
        request.erase(0, request.find_first_not_of(" "));
        next_space = request.find(" ");
        if (next_space == n)
            return (false);
        if (request.substr(0, next_space) != "HTTP/1.1" && request.substr(0, next_space) != "HTTP/1.0")
            return (false);
        _version = request.substr(0, next_space);
        return true;
    }

    bool is_header(std::string s) const //verifie si s fait partie des headers
    {
        for (int i = 0; i < NB_HEADERS; i++)
        {
            if (_header[i] == s)
                return (true);
        }
        return (false);
    }

    void init_header_map()
    {
        _header[0] = "Accept-Charsets";
        _header[1] = "Accept-Language";
        _header[2] = "Allow"; // Pas dans la requete ?
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
        _header[15] = "Transfer-Encoding";
        _header[16] = "User-Agent";
        _header[17] = "Www-Authenticate"; // Pas utile ?
        _header[18] = "Connection";
        _header[19] = "Accept";
        _header[20] = "Cookie";
        _header[21] = "Accept-Encoding";
        _header[22] = "From";
        _header[23] = "Accept";
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
        _m["Transfer-Encoding"] = "";
        _m["User-Agent"] = "";
        _m["Www-Authenticate"] = "";
        _m["Connection"] = "keep-alive";
        _m["Accept"] = "";
        _m["Cookie"] = "";
        _m["Accept-Encoding"] = "";
        _m["From"] = "";
        _m["Accept"] = "";
    }

    bool file_exist(const char *file)
    {
        FILE *f = fopen(file, "r");
        if (f)
        {
            fclose(f);
            return (true);
        }
        return (false);
    }
};