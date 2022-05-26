#include "server.hpp"

std::string apply_location(std::string path, Server *conf, Server **dest)
{
  std::string tmp = path;

  while (tmp.length() != 0)
  {
    if (conf->locations.find(tmp) != conf->locations.end())
    {
      *dest = &conf->locations[tmp];
      return (tmp);
    }
    if (tmp.find_last_of("/") != std::string::npos)
      tmp.erase(tmp.find_last_of("/"), tmp.length());
  }
  *dest = conf;
  return (path);
}

std::string url_decode(std::string str)
{
    std::string url;
    for (std::string::size_type i = 0; i < str.size(); i++)
    {
        if (str[i] == '%')
        {
            std::string hex = str.substr(i + 1, 2);
            url += static_cast<char>(std::strtol(hex.c_str(), NULL, 16));
            i += 2;
        }
        else if (str[i] == '+')
            url += ' ';
        else
            url += str[i];
    }
    return url;
}

std::string intToStr(int n)
{
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
    std::cerr << "Can't open " << filename << "." << std::endl;
    return (1);
  }
  getline(ifs, s);
  *fileContent += s;
  while (getline(ifs, s)){
    *fileContent += "\n";
    *fileContent += s;
  }
  ifs.close();
  if (*fileContent == "")
  {
    std::cerr << "Empty file." << std::endl;
    ifs.close();
    return (1);
  }
  return (0);
}

bool is_number(char c)
{
  return (c >= '0' && c <= '9');
}

bool is_space(const char c)
{
  return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

bool is_blanck(const char c)
{
  return (is_space(c) || c == '\n');
}

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

void output_log(int ans, std::string filetosearch)
{
  if (ans == 200 && (filetosearch != ""))
    std::cout << GREEN << "[⊛ 200]        => " << WHITE << filetosearch << RESET << std::endl;
  if (ans == 404)
    std::cout << RED << "[⊛ 404]        => " << YELLOW << "Not Found " << RESET << std::endl;
  if (ans == 201)
    std::cout << GREEN << "[⊛ 201]        => " << WHITE << "CREATED" << RESET << std::endl;
  if (ans == 403)
    std::cout << RED << "[⊛ 403]        => " << YELLOW << "Forbidden" << RESET << std::endl;
  if (ans == 500)
    std::cout << RED << "[⊛ 500]        => " << YELLOW << "Internal Server Error" << RESET << std::endl;
  if (ans == 413)
    std::cout << RED << "[⊛ 413]        => " << YELLOW << "Request Entity Too Large" << RESET << std::endl;
  if (ans == 405)
    std::cout << RED << "[⊛ 405]        => " << YELLOW << "Method Not Allowed " << RESET << std::endl;
  if (ans == 502)
    std::cout << RED << "[⊛ 502]        => " << YELLOW << "Bad Gateway" << RESET << std::endl;
  if (ans == 411)
    std::cout << RED << "[⊛ 411]        => " << YELLOW << "Length Required" << RESET << std::endl;
  if (ans == 501)
    std::cout << RED << "[⊛ 501]        => " << YELLOW << "Not Implemented" << RESET << std::endl;
  if (ans == 406)
    std::cout << RED << "[⊛ 406]        => " << YELLOW << "Not Acceptable" << RESET << std::endl;
  if (ans == 301)
    std::cout << RED << "[⊛ 301]        => " << YELLOW << "Moved Permanently" << RESET << std::endl;
  if (ans == 400)
    std::cout << RED << "[⊛ 400]          => " << YELLOW << "Bad Request" << RESET << std::endl;
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