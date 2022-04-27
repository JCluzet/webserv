#include "server.hpp"

bool is_directory(std::string path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) < 0)
        return (0);
    else
        return (1);
}

std::string findInHeader(std::string header, std::string s)
{
    size_t pos;
    std::string tmp;
    if (!strcmp(s.c_str(), "File"))
    {
        header.erase(0, 4);
        return header.substr(0, header.find(' '));
    }
    if (!strcmp(s.c_str(), "Version"))
    {
        header.erase(0, 4);
        header.erase(0, header.find(' ') + 1);
        return header.substr(0, header.find('\n'));
    }
    while ((pos = header.find("\n")) != std::string::npos)
    {
        tmp = header.substr(0, pos);
        if (tmp.length() >= s.length() && !strncmp(s.c_str(), tmp.c_str(), s.length()))
        {
            tmp.erase(0, s.length() + 2);
            return tmp;
        }
        header.erase(0, pos + 1);
    }
    return "";
}

int readFile(std::string filename, std::string *fileContent)
{
    std::string s;
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs)
    {
        // std::cerr << "Not Found " << filename << "." << std::endl;
        // std::cout << RED << "[⊛ 404] => " << WHITE << "not found " << filename << std::endl;
        *fileContent = "\n";
        // *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>"; // --> pouvoir mettre le fichier d'erreur par default ou celui inndique dans le fichier de config
        return (404);
    }
    getline(ifs, s);
    if (s == "")
    {
        // std::cout << RED << "[⊛ 404] => "<< WHITE << "empty " << filename << std::endl;
        // std::cerr << "Empty file."  << std::endl;
        ifs.close();
        *fileContent = "\n";
        // *fileContent = "<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>Empty file.</p>\n</body>\n\n</html>";      // --> pareil
        return (404);
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

std::string cut_aftercomma(std::string s)
{
    int i = 0;
    while (s[i] != ',')
        i++;
    return (s.substr(0, i));
}

std::string getContentType(std::string client_data) // --> refaire propremment cette fonction qui recupere le bon content-type
{
    std::string content_type = "";
    for (unsigned int i = 0; i < client_data.length() - 7; i++)
    {
        if (client_data[i] == 'A' && client_data[i + 1] == 'c' && client_data[i + 2] == 'c' && client_data[i + 3] == 'e' && client_data[i + 4] == 'p' && client_data[i + 5] == 't' && client_data[i + 6] == ':')
        {
            i += 8;
            std::string tmp = findInHeader(client_data, "File");
            // std::cout << "tmp >> " << tmp << std::endl;
            if (tmp[tmp.length() - 1] == 'g' && tmp[tmp.length() - 2] == 'v' && tmp[tmp.length() - 3] == 's') // --> if it's a svg file, use image/svg+xml content type
            {
                content_type = "image/svg+xml";
                return (content_type);
            }
            else if (tmp[tmp.length() - 1] == 'f' && tmp[tmp.length() - 2] == 'd' && tmp[tmp.length() - 3] == 'p')
            {
                content_type = "application/pdf";
                return (content_type);
            }
            else
            {
                while (client_data[i] != ',')
                {
                    content_type += client_data[i];
                    i++;
                }
            }
            break;
        }
    }
    return (content_type);
}

std::string getFile(std::string client_data)
{
    std::string file;
    for (size_t i = 4; i < client_data.length(); i++)
    {
        if (client_data[i] == ' ')
            break;
        file += client_data[i];
    }
    return (file);
}

std::string get_status(int ans)
{
    std::string status = "HTTP/1.1 ";
    if (ans == 0)
    {
        status += "200 OK";
        // std::cout << GREEN << "[⊛] => " << RESET;
    }
    if (ans == 404)
    {
        status += "404";
        // std::cout << RED << "[ ⊛ 404 ] => " << RESET;
    }
    return (status);
}

std::string getHeader(std::string client_data, std::string file_content, int ans)
{
    std::string response = get_status(ans);
    response += "\nContent-Type: ";
    response += getContentType(client_data);
    response += "\nContent-Length: ";
    response += sizetToStr(file_content.length());
    response += "\n\n";

    return (response);
}

#include <sys/types.h>

std::string set_default_page(std::string filetosearch, std::string client_data, std::string conf)
{
    // std::cout << "present" << conf.serv[0].default_page << std::endl;
    (void)client_data; // ---> need to check if the file is a directory and then set the default page
    if (filetosearch[filetosearch.length() - 1] == '/')
    {
        std::string tmp = conf;
        filetosearch += tmp; // <-- replace here the default page in config file
        // std::cout << "setting DEFAULT to index.html" << std::endl;
    }
    return (filetosearch);
}

void output_log(int ans, std::string filetosearch)
{
    if (ans == 0)
        std::cout << GREEN << "[⊛ 200] => " << WHITE << filetosearch << RESET << std::endl;
    if (ans == 404)
        std::cout << RED << "[⊛ 404] => " << WHITE << filetosearch << RESET << std::endl;
}

void response_sender(server_data *server, std::string client_data, Config conf)
{
    std::string response;
    std::string filecontent;
    size_t ans;

    std::string filetosearch = conf.serv[0].default_folder + findInHeader(client_data, "File"); // <-- replace "root" here with config file

    filetosearch = set_default_page(filetosearch, client_data, conf.serv[0].default_page);

    ans = readFile(filetosearch.c_str(), &filecontent);
    if (ans == 404)
    {
        if (conf.serv[0].page404 != "")
        {
            std::cout << RED << "[⊛ 404] => " << YELLOW << "Redirect to 404 page " << RESET << std::endl;

            filetosearch = conf.serv[0].default_folder + "/" + conf.serv[0].page404;
            readFile(filetosearch.c_str(), &filecontent);
        }
        else
        {
            std::cout << BLUE << "[⊛] => " << YELLOW << "Setting error default page" << std::endl;
            filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        }
    }
    output_log(ans, filetosearch);
    server->response = getHeader(client_data, filecontent, ans);
    server->response += filecontent;
}
