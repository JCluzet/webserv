#include "Request.hpp"
// constructors
Request::Request() : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(false)
{
    init_header_map();
}

Request::Request(std::string new_request) : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(false)
{
    init_header_map();
    addp(new_request);
}

Request::Request(const Request &src) : _request(""), _path(""), _method(""), _version(""), _end(false), _valid(false)
{
    init_header_map();
    addp(src._request);
}

Request     &Request::operator=(const Request &src)
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
    }
    return *this;
}

Request::~Request() {}

//getter
std::string Request::get_path() const { return _path; }

std::string Request::get_method() const { return _method; }

std::string Request::get_version() const { return _version; }

std::string Request::get_header(std::string key) { return _m[key]; }

std::string Request::get_request() const { return _request; }

bool Request::ready() const { return _end; }

void Request::add(std::string new_request)
{
    if (!(_valid = addp(new_request)))
    {
        clear();
        _end = true;
    }
}

void Request::print() { pprint(); }

void Request::clear()
{
    _request = "";
    _path = "";
    _method = "";
    _version = "";
    _end = false;
    _valid = false;
    init_header_map();
}
//modifier
bool Request::addp(std::string r)
{
    std::string::size_type nl;
    cut_end(&r);
    if (_request.empty() == true && (r.empty() == true || r == NL))
    {
        return true;
    }
    if (_request.empty() == false && r == NL)
    {
        _end = true;
        return true;
    }
    if (r == "" || r.substr(0, 1) == " ")
    {
        return false;
    }
    if (_request.empty())
    {
        nl = r.find(NL);
        if (nl == std::string::npos)
            return false;
        _request = r;
        if (!get_request_first_line(r.substr(0, nl + NLSIZE)))
        {
            // std::cout << "error bad request 1\n";
            return false;
        }
        nl = r.find(NL);
        if (nl == std::string::npos)
        {
            // std::cout << "error bad request 2\n";
            return false;
        }
        r.erase(0, nl + NLSIZE);
    }
    else
    {
        _request += r;
    }
    nl = r.find(NL);
    if (nl == std::string::npos)
    {
        return true;
    }
    while (nl != std::string::npos)
    {
        if (!get_request_line(r.substr(0, nl + NLSIZE))) /////////////////////////
        {
            // std::cout << "error bad request 4\n";
            return true;
        }
        r.erase(0, nl + NLSIZE);
        nl = r.find(NL);
    }
    return true;
}

void Request::cut_end(std::string *r)
{
    std::string end = NL;
    std::string::size_type x;
    end += NL;
    if ((x = r->find(end)) != std::string::npos)
    {
        *r = r->substr(0, x + NLSIZE);
        _end = true;
    }
}

void Request::pprint()
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

bool Request::get_request_line(std::string r) // Check request line conformitys
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
    _m[ctn] = r;
    return true;
}

bool Request::get_request_first_line(std::string r) // Check request first line conformitys
{
    std::string::size_type next_space = std::string::npos;
    std::string::size_type next_nl = std::string::npos;
    std::string::size_type n = std::string::npos;
    std::string request = r;
    // _request = request;
    // Check & get method
    // std::cout << "HELLO" << std::endl;
    if ((next_space = request.find(" ")) == n)
    {
        // std::cout << "error bad request 1.0\n";
        return (false);
    }
    if ((next_nl = request.find(NL)) == n)
    {
        // std::cout << "error bad request 1.1\n";
        return (false);
    }
    if (next_nl < request.length())
        request = request.substr(0, next_nl);
    request += "   ";
    if (request.substr(0, next_space) != "GET" && request.substr(0, next_space) != "POST" && request.substr(0, next_space) != "DELETE")
    {
        // std::cout << "error bad request 1.2\n";
        return (false);
    }
    _method = request.substr(0, next_space);
    request.erase(0, next_space);
    // Check & get path
    if ((next_space = request.find_first_not_of(" ")) == n)
    {
        // std::cout << "error bad request 1.3\n";
        return (false);
    }
    request.erase(0, next_space);
    if ((next_space = request.find(" ")) == n)
    {
        // std::cout << "error bad request 1.4\n";
        return (false);
    }
    // if (!file_exist((request.substr(0, next_space)).c_str())) //secu
    // return false;
    _path = request.substr(0, next_space); // secu
    request.erase(0, next_space);          //secu
                                           // Check version
    request.erase(0, request.find_first_not_of(" "));
    next_space = request.find(" ");
    if (next_space == n)
    {
        // std::cout << "error bad request 1.5\n";
        return (false);
    }
    if (request.substr(0, next_space) != "HTTP/1.1" && request.substr(0, next_space) != "HTTP/1.0")
    {
        // std::cout << "error bad request 1.6\n";
        return (false);
    }
    _version = request.substr(0, next_space);
    return true;
}

bool Request::is_header(std::string s) const //verifie si s fait partie des headers
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
    _header[20] = "Accept";
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