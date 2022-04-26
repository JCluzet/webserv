#include "server.hpp"

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