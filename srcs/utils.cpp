#include "server.hpp"
#include <sys/types.h> // For mkdir
#include <dirent.h> // For mkdir
#include <stdio.h>
// std::string sizetToStr(size_t n){
//     std::string s;
//     size_t x = 1;
//     while (n / x > 9)
// 		x *= 10;
// 	while (x)
// 	{
// 		s += (n / x) + '0';
// 		n %= x;
// 		x /= 10;
// 	}
//     return s;
// }

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

bool is_file(const std::string path)
{
  std::ifstream ifs;

  ifs.open(path.c_str());
  if (!ifs)
    return 0;
  else
    ifs.close();
  return 1;
}