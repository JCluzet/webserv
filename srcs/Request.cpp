#include "Request.hpp"


Request::Request() : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(0), _body(""), _line(""), _chunked(false)
{
    init_header_map();
}

Request::Request(const Request &src)
{
    *this = src;
    return;
}

Request&    Request::operator=(const Request &src)
{
    if (this != &src)
    {
        init_header_map();
        _m = src._m;
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

Request::~Request() {};


// getter
std::string Request::get_path() const { return _path; };

void        Request::set_path(std::string newpath) { _path = newpath; return ; };

std::string Request::get_body() const { return _body; };

std::string Request::get_method() const { return _method; };

std::string Request::get_version() const { return _version; };

std::string Request::get_header(std::string key) { return _m[key]; };

std::string Request::get_request() const { return _request; };

bool Request::ready() const { return _end; };

int Request::is_valid() const { return _valid; };


void Request::add(Client* client, Server* conf_o, std::string new_request)
{
    if ((_valid = addp(client, conf_o, new_request)) != 0)
    {
        clear();
        _end = true;
        return;
    }
    if (_valid == 0 && _end == true && _method == "POST")
    {
        if (_m["Content-Type"] == "")
        {
            _m["Content-Type"] = "text/plain";
        }
        if (_m["Content-Type"] != "text/plain" && (_m["Content-Type"].find(";") == std::string::npos || _m["Content-Type"].substr(0, _m["Content-Type"].find(";")) != "multipart/form-data") && _m["Content-Type"] != "application/x-www-form-urlencoded")
        {
            _valid = 400;
            clear();
        }
    }
}

void Request::clear()
{
    _request = "";
    _path = "";
    _method = "";
    _version = "";
    _end = false;
    _valid = 0;
    _chunked = false;
    _body = "";
    init_header_map();
}

// modifier
int Request::addp(Client* client, Server* conf_o, std::string r)
{
    std::string::size_type nl;

    if (_request == "")
        client->response->setConf(conf_o);
    if (_line == "" && _request.find("\r\n\r\n") == std::string::npos && (r == "" || r.substr(0, 1) == " "))
        return 400;
    if (_chunked == true)
    {
        r = chunked(r);
    }
    r = _line + r;
    _line = "";
    while ((nl = r.find(NL)) != std::string::npos)
    {
        if (_request.empty() == true)
        {
            if (!get_request_first_line(r.substr(0, nl + NLSIZE)))
                return 400;
        }
        else if (r[0] == '\r' && r[1] == '\n' && _request.substr(_request.length() - 2, _request.length()) == "\r\n"
            && _request.find("\r\n\r\n") == std::string::npos)
        {
            int ret;
            if ((ret = checkHeader(client, conf_o, r)) != -1)
                return ret;
        }
        else if (_method == "POST" && _request.find("\r\n\r\n") != std::string::npos)
        {
            _body += r.substr(0, nl + NLSIZE);
            if (static_cast<int>(_body.length()) >= atoi(_m["Content-Length"].c_str()))
            {
                if (static_cast<int>(_body.length()) > atoi(_m["Content-Length"].c_str()))
                {
                    _line = _body.substr(atoi(_m["Content-Length"].c_str()), std::string::npos);
                    _body = _body.substr(0, atoi(_m["Content-Length"].c_str()));
                }
                _request += r.substr(0, nl + NLSIZE);
                _end = true;
                return 0;
            }
        }
        else
        {
            get_request_line(r.substr(0, nl + NLSIZE));
        }
        if (r != "\r\n" || _request != "")
            _request += r.substr(0, nl + NLSIZE);
        r.erase(0, nl + NLSIZE);
    }
    if (r.empty() == false && _method == "POST" && _request.find("\r\n\r\n") != std::string::npos && static_cast<int>(_body.length() + r.length()) >= atoi(_m["Content-Length"].c_str()))
    {
        _body += r;
        _request += r;
        if (static_cast<int>(_body.length()) > atoi(_m["Content-Length"].c_str()))
        {
            _line = _body.substr(atoi(_m["Content-Length"].c_str()), std::string::npos);
            _body = _body.substr(0, atoi(_m["Content-Length"].c_str()));
        }
        _end = true;
        return 0;
    }
    _line = r;
    return 0;
}

int Request::checkHeader(Client* client, Server* conf_o, std::string r)
{
   	Server*		conf_local;
	std::string	location;

    if (checkHost(client, conf_o->ip, conf_o->port, conf_o->server_name) == 400)
       return 400;
    location = apply_location(_path, conf_o, &conf_local);
    client->response->setConf(conf_local);
    if (conf_o->root != conf_local->root)
    {
        _path = _path.substr(_path.find(location) + location.length());
        if (_path == "")
            _path = "/";
    }
    if (is_directory(conf_local->root + _path) == false && _path[_path.length() - 1] == '/' && _path != "/")
        _path = _path.substr(0, _path.length() - 1);
    if ((_method == "POST" && !conf_local->methods[1]) || (_method == "GET" && !conf_local->methods[0]) || (_method == "DELETE" && !conf_local->methods[2])) // check error 405 Method not allowed
 	    return 405;
    if (_method == "GET" || _method == "DELETE")
    {
        _request += r;
        _end = true;
        return 0;
    }
    if (_method == "POST" && (_m["Content-Length"] == "" || _m["Transfer-Encoding"] == "chunked"))
        _chunked = true;
    else if (_method == "POST" && (_m["Content-Length"] == "" || _m["Content-Length"].find_first_not_of("0123456789") != std::string::npos))
        return 411;
    else if (_method == "POST" && atoi(_m["Content-Length"].c_str()) > atoi(conf_local->client_max_body_size.c_str()))
        return 413;
    return -1;
}

int	Request::checkHost(Client* client, std::string ip, std::string port, std::vector<std::string> server_name)
{
    if (client->request->get_header("Host") != ip + ":" + port)
    {
    	for (size_t k = 0; k < server_name.size(); k++)
    	{
    		if (server_name[k] == _m["Host"]
    			|| server_name[k] + ":" + port == _m["Host"])
    			break ;
    		else if (k + 1 == server_name.size())
    		{
    			if (ip == "0.0.0.0" && _m["Host"].find(":") != std::string::npos
    				&& _m["Host"].find(":") == _m["Host"].rfind(":")
    				&& _m["Host"].substr(_m["Host"].find(":") + 1, _m["Host"].length()) == port)
    				break ;
    			return 400;
    		}
    	}
    }
    return -1;
}

bool Request::get_request_line(std::string r)
{
    std::string ctn;
    std::string::size_type pos;
    if ((pos = r.find(": ")) == std::string::npos)
    {
        return (false);
    }
    ctn = r.substr(0, pos);
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

bool Request::get_request_first_line(std::string r) // Check request first line conformitys
{
    std::string::size_type next_space = std::string::npos;
    std::string::size_type next_nl = std::string::npos;
    std::string::size_type n = std::string::npos;
    std::string request = r;
    if (r == NL)
        return (true);
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
    request.erase(0, next_space);
    if ((next_space = request.find_first_not_of(" ")) == n)
        return (false);
    request.erase(0, next_space);
    if ((next_space = request.find(" ")) == n)
        return (false);
    _path = request.substr(0, next_space);
    request.erase(0, next_space);
                                  //  Check version
    request.erase(0, request.find_first_not_of(" "));
    next_space = request.find(" ");
    if (next_space == n)
        return (false);
    if (request.substr(0, next_space) != "HTTP/1.1" && request.substr(0, next_space) != "HTTP/1.0")
        return (false);
    _version = request.substr(0, next_space);
    return true;
}

bool Request::is_header(std::string s) const // verifie si s fait partie des headers
{
    for (int i = 0; i < NB_HEADERS; i++)
    {
        if (_header[i] == s)
            return (true);
    }
    return (false);
}

void Request::init_header_map()
{
    _header[0] = "Accept-Charsets";
    _header[1] = "Accept-Language";
    _header[2] = "Auth-Scheme";
    _header[3] = "Authorization";
    _header[4] = "Content-Language";
    _header[5] = "Content-Length";
    _header[6] = "Content-Location";
    _header[7] = "Content-Type";
    _header[8] = "Date";
    _header[9] = "Host";
    _header[10] = "Last-Modified";
    _header[11] = "Location";
    _header[12] = "Referer";
    _header[13] = "Retry-After";
    _header[14] = "Transfer-Encoding";
    _header[15] = "User-Agent";
    _header[16] = "Connection";
    _header[17] = "Accept";
    _header[18] = "Cookie";
    _header[19] = "Accept-Encoding";
    _header[20] = "From";
    _m["Accept-Charsets"] = "";
    _m["Accept-Language"] = "";
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
    _m["Connection"] = "keep-alive";
    _m["Cookie"] = "";
    _m["Accept-Encoding"] = "";
    _m["From"] = "";
    _m["Accept"] = "";
}

bool Request::file_exist(const char *file)
{
    FILE *f = fopen(file, "r");
    if (f)
    {
        fclose(f);
        return (true);
    }
    return (false);
}