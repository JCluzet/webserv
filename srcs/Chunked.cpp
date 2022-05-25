/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunked.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcluzet <jcluzet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/16 20:07:13 by jcluzet           #+#    #+#             */
/*   Updated: 2022/05/25 20:59:31 by jcluzet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

// std::string chunked(std::string str)
// {
//     std::string _chunked = "";
//     unsigned int i = 0;
//     unsigned int len = strtol(str.c_str(), NULL, 16);
//     if (str == "0\r\n" || str == "0\r\n\r\n")
//     {
//         // std::cout << "_chunked: " << str << std::endl;
//         return(_chunked);
//     }
//     while (i < str.length())
//     {
//         if (str[i] == '\r' && str[i + 1] == '\n')
//         {
//             i += 2;
//             break;
//         }
//         i++;
//     }
//     unsigned int tmp = i;
//     while (i < str.length() && (len > (i - tmp)))
//     {
//         if (str[i] == '\r' && str[i + 1] == '\n')
//         {
//             i += 2;
//             if (str[i] == '\r' && str[i + 1] == '\n')
//             {
//                 i += 2;
//                 _chunked += "\r\n";
//             }
//             break;
//         }
//         _chunked += str[i];
//         i++;
//     }
//     if (i != str.length())
//         _chunked += chunked(str.substr(i, str.length()));
//     return (_chunked);
// }

// Create a function that take a chunked HTTP response and return a string
// that is the unchunked version of the response.
//
// The chunked HTTP response is a string that looks like this:
//
//    "4\r\ndata\r\n0\r\n\r\n"
//
// The unchunked version of this response is:
//
//    "data"
//

bool chunked(std::string *strrec)
{
    unsigned int len;
    std::string str = *strrec;
    unsigned int i = 0;
    unsigned int f = 0;
    std::string str1;
    if (str == "0\r\n\r\n")
    {
        *strrec = "";
        return(true);
    }
    if (str.find("\r\n", i) == std::string::npos)
        return (false);
    str1 = str.substr(0, str.find("\r\n", i));
    if ((len = strtol(str1.c_str(), NULL, 16)) == 0)
        return (false);
    str1 = str.substr(str.find("\r\n") + 2);

    if (str1.length() != len + 2 || str1[str1.length() - 2] != '\r' || str1[str1.length() - 1] != '\n')
        return (false);
    *strrec = str1.substr(0, len);
    return (true);
}

// int main()
// {
//     std::string str = "26\r\n";
//     str += "Voici les donnees \r\ndu premier morceau\r\n";
//     // str = "0\r\n\r\n";
//     std::cout << chunked(str);
//     // str += "1C\r\n";
//     // str += "et voici un second morceau\r\n\r\n";
//     // str += "0\r\n\r\n";
//     return (0);
// }