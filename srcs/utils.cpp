#include "server.hpp"
#include <sys/types.h> // For mkdir
#include <dirent.h>

std::string sizetToStr(size_t n){
    std::string s;
    size_t x = 1;
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

bool is_directory(std::string path)
{
  DIR* ptr;
    if ((ptr = opendir(path.c_str())) == NULL)
           return (0);
    else{
        closedir(ptr);
        return (1);
    }
}
