#pragma once

#define NB_TYPE 10
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
// #include for strncmp
#include <stdlib.h>
// #include <

class client_data
{
public:
    client_data(std::string client_data) : content_type(client_data){};
    client_data() : content_type(""){};
    ~client_data(){};

    // bool add_client_data(std::string data);
    std::string getContentType();
    std::string getFile();
    std::string get_status(int ans);
    std::string getHeader(std::string file_content, int ans);

    bool add_client_data(std::string data)
    {
        for (size_t i = 0; i < NB_TYPE; i++)
        {
            if (strncmp(data.c_str(), types[i].c_str(), strlen(types[i].c_str())) == 0)
            {
                std::cout << "GOOD" << std::endl;
            }
        }
        return(1);
    }

    std::string file;
    std::string Host;
    std::string content_type;
private:
    // create a double array of string containing the different type of file
    char* strarray[] = {"hey", "sup", "dogg"};
    // vector<string> strvector(strarray, strarray + 3);
    // types[0] = "text/html";
    // types[1] = "text/css";
    // types = {"text/html", "text/css", "text/javascript", "text/plain", "text/xml", "image/gif", "image/jpeg", "image/png", "image/svg+xml", "application/x-javascript"};
    // std::string types[NB_TYPE] = {"GET", "Host:", "Accept:", "Connection:", "User-Agent:", "Content-Length:", "Content-Type:", "Content-Disposition:", "Content-Transfer-Encoding:", "Content-Location:"};
};
