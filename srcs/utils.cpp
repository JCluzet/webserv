#include "server.hpp"

std::string apply_location(std::string path, Server* conf, Server** dest)
{
  std::string tmp = path;

  while (tmp.length() != 0)
  {
    if (conf->locations.find(tmp) != conf->locations.end() || conf->locations.find(tmp + "/") != conf->locations.end())
    {
      if (LOG == 1)
            std::cout << YELLOW << "[⊛ LOCATION]        => " << WHITE << tmp << RESET << std::endl;
      *dest = &conf->locations[tmp];
      return (tmp);
    }
    if (tmp.find_last_of("/") != std::string::npos)
      tmp.erase(tmp.find_last_of("/"), tmp.length());
  }
  *dest = conf;
  return (path);
}

std::string intToStr(int n){
    std::string s = (n < 0) ? "-" : "";
    n *= (n < 0 ? -1 : 1);
    int x = 1;
    while (n / x > 9)
		x *= 10;
	while (x)
	{
		s += (n / x) + '0';
		n %= x;
		x /= 10;
	}
    return s;
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

std::string getHour()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%X", &tstruct);
    return (buf);
}

bool readinFile(std::string filename, std::string *fileContent)
{
    std::string s;
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs)
    {
        std::cerr << "Not Found " << filename << "." << std::endl;
        return (1);
    }
    getline(ifs, s);
    if (s == "")
    {
        std::cerr << "Empty file." << std::endl;
        ifs.close();
        return (1);
    }
    *fileContent += s;
    while (getline(ifs, s))
    {
        *fileContent += "\n";
        *fileContent += s;
    }
    ifs.close();
    return (0);
}

bool    is_number(char c)
  { return (c >= '0' && c <= '9'); }

bool is_space(const char c)
  { return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'); }

bool is_blanck(const char c)
  { return (is_space(c) || c == '\n'); }

void pass_space(const std::string s, std::string::size_type *i)
{
  while (*i < s.length() && is_space(s[*i]))
    *i += 1;
}

void pass_blanck(const std::string s, std::string::size_type *i, std::string::size_type *line_i)
{
  while (*i < s.length() && is_blanck(s[*i]))
  {
    if (s[*i] == '\n')
      (*line_i) += 1;
    *i += 1;
  }
}

void pass_not_blanck(const std::string s, std::string::size_type *i)
{
  while (*i < s.length() && !is_blanck(s[*i]))
    *i += 1;
}

bool s_a_have_b(const std::string a, const std::string::size_type i, const std::string b)
{
    // std::cout << "**" << a.substr(i, b.length()) << " == " << b << "?" << std::endl;
  return (a.length() >= (b.length() + i) && a.substr(i, b.length()) == b);
}

bool error_msg(const std::string msg)
{
  std::cerr << msg << std::endl;
  return 0;
}

bool is_directory(const std::string path)
{
  DIR *ptr;
  if ((ptr = opendir(path.c_str())) == NULL)
    return 0;
  else
    closedir(ptr);
  return 1;
}

bool fileExist(const std::string s)
{
    DIR* dir;
    bool b = 0;
    struct dirent* ent;
    std::string dirp, filep, tmp = s;
    std::string::size_type i;
    i = (tmp.find_last_of('/'));
    if (i == std::string::npos)
    {
        dirp = "./";
        filep = tmp;
    }
    else
    {
        dirp = tmp.substr(0, i);
        filep = tmp.substr(i + 1, tmp.length() - (i + 1));
    }
    dir = opendir(dirp.c_str());
    if (dir == NULL)
      return (0);
    while ((ent = readdir(dir)))
        if (ent->d_name == filep){
            b = 1;
            break;
        }
    closedir(dir);
    return b;
}

void output_log(int ans, std::string filetosearch)
{
    if (ans == 200)
        std::cout << GREEN << "[⊛ 200]        => " << WHITE << filetosearch << RESET << std::endl;
    if (ans == 404)
        std::cout << RED << "[⊛ 404]        => " << YELLOW << "Not Found: " << WHITE << filetosearch << RESET << std::endl;
}

void output_debug(std::string request, std::string response)
{
	std::cout << WHITE << "\nRequest: \n"
			  << RESET << request;
	std::cout << WHITE << "\nOUR RESPONSE: " << RESET << std::endl
			  << response << RESET << "<" << WHITE << "INVISIBLE BODY" << RESET << ">" << std::endl;
}


size_t ft_atoi(std::string str)
{
    size_t i = 0;
    size_t n = 0;
    while (str[i] >= '0' && str[i] <= '9' && i < str.length())
    {
        n = n * 10 + (str[i] - '0');
        i++;
    }
    return (n);
}