#include "server.hpp"

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
    if (!strcmp(s.c_str(), "ref_extension"))
    {
        s = "Referer";
        while ((pos = header.find("\n")) != std::string::npos)
        {
            tmp = header.substr(0, pos);
            if (tmp.length() >= s.length() && !strncmp(s.c_str(), tmp.c_str(), s.length()))
            {
                tmp.erase(0, s.length() + 2);
                for (int i = 0; i < 3; i++){
                    if (tmp.find("/") == std::string::npos)
                        return "";
                    tmp.erase(0, tmp.find("/") + 1);
                }
                return tmp;
            }
            header.erase(0, pos + 1);
        }
        return "";
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
        *fileContent = "\n";
        // *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>"; // --> pouvoir mettre le fichier d'erreur par default ou celui inndique dans le fichier de config
        return (404);
    }
    getline(ifs, s);
    if (s == "")
    {
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
        if (client_data.size() >= i + 7 && client_data.substr(i, 7) == "Accept:")
        {
            i += 8;
            std::string tmp = findInHeader(client_data, "File");
            if (tmp.length() >= 4 && tmp.substr(tmp.length() - 4, tmp.length() - 1) == (std::string)".svg")
                return ("image/svg+xml");
            else if (tmp.length() >= 4 && tmp.substr(tmp.length() - 4, tmp.length() - 1) == (std::string)".pdf")
                return ("application/pdf");
            else if (tmp.length() >= 4 && tmp.substr(tmp.length() - 4, tmp.length() - 1) == (std::string)".png")
                return("image/apng");
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
        status += "200 OK";
    if (ans == 404)
        status += "404 Not Found";
    return (status);
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

std::string getHeader(std::string client_data, std::string file_content, int ans)
{
    std::string header = get_status(ans);
    header += "\nContent-Type: ";
    header += getContentType(client_data);
    header += "\nContent-Length: ";
    header += sizetToStr(file_content.length());
    header += "\nServer: WebServ v1.0";
    header += "\nDate : ";
    header += getDate();
    header += "\n\n";

    return (header);
}

#include <sys/types.h>

std::string set_default_page(std::string filetosearch, std::string client_data, std::string conf)
{
    (void)client_data; // ---> need to check if the file is a directory and then set the default page
    // std::cout << "Referer catching: " << findInHeader(client_data, "ref_extension") << std::endl;
    // std::cout << "is_directory=" << is_directory(filetosearch) << std::endl;
    if (is_directory(filetosearch))
    {
        // if ()
        if (filetosearch[filetosearch.length() - 1] == '/')
            filetosearch += conf;
        else
        {
            // filetosearch += "/";                         // --> if there is no slash at the end of the path, how to do it ?? 
            // filetosearch += conf;
        }
            // filetosearch += conf;
    }
    // if (filetosearch[filetosearch.length() - 1] == '/') // -> if it's a directory
    // {
    //     std::string tmp = conf;
    //     filetosearch += tmp; // <-- replace here the default page in config file
    // }
    return (filetosearch);
}

void output_log(int ans, std::string filetosearch)
{
    if (ans == 0)
        std::cout << GREEN << "[⊛ 200] => " << WHITE << filetosearch << RESET << std::endl;
    if (ans == 404)
        std::cout << RED << "[⊛ 404] => " << YELLOW << "Redirect to 404 page: " << WHITE << filetosearch << RESET << std::endl;
}

void response_sender(server_data *server, std::string client_data, s_server *conf)
{
    // std::cout << "client_data:" << client_data << "<<" << std::endl;
    if (client_data == "")                 // --> Replace by BadRequest detector 
    {
        std::cout << "Bad request." << std::endl;
        server->response = get_status(400);
        server->response += "\nContent-Type: txt/html\n";
        server->response += "\nContent-Length: 110\n\n";
        server->response += "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 400</h1>\n    <p>Bad request.</p>\n</body>\n\n</html>";
        return;
    }

    // ------------------------- OBTENTION DU FICHIER A ALLER CHERCHER -------------------------
    std::string filetosearch = getFile(client_data);
    server->filetosearch = conf->default_folder + findInHeader(client_data, "File");         // --> Creation du path a alleer chercher www + / + file
    std::string tmp;
    bool temp;
    temp = indexGenerator(&tmp, server->filetosearch);                                       // --> si l'autoindex est activé, on tente de generer l'index

    server->filetosearch = set_default_page(server->filetosearch, client_data, conf->default_page); // --> si c'est un dossier, rajouter le default page
    // std::cout << "file to search: " << server->filetosearch << std::endl;
    // -----------------------------------------------------------------------------------------

    server->filecontent = "";
    server->status_code = readFile(server->filetosearch.c_str(), &server->filecontent);       // --> tentative de l'ecture du fichier 
    if (server->status_code == 404)                                                           // --> si le fichier n'existe pas, on redirige vers une erreur 404 (le status code devient 404)  (on doit aussi verifier les droits pour rediriger vers un bad permission ou autres)
    {
        if (conf->page404 != "")                                                                           // --> si la page 404 default est dans le fichier de config, on rajoute un '/' + la 404 default page
        {
            server->filetosearch = conf->default_folder + "/" + conf->page404;
            readFile(server->filetosearch.c_str(), &server->filecontent);
        }
        else                                                                                               // --> sinon on met le code html de base de error 404 dedans
        {
            std::cout << BLUE << "[⊛] => " << YELLOW << "Setting error default page" << std::endl;
            server->filecontent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>";
        }
    }
    if (!temp && conf->autoindex)                                                       // --> si l'autoindex est active et qu'on a reussi a le generer, on le met dans le filecontent
    {
        std::cout << BLUE << "[⊛] => " << YELLOW << "Setting autoindex" << std::endl;
        server->filecontent = tmp;
    }
    output_log(server->status_code, server->filetosearch);
    server->response = getHeader(client_data, server->filecontent, server->status_code);   // --> on met le header en envoyant le status code, le file_content(pour le content-lenght) et le client_data (pour le content-type) dans la reponse
    server->response += server->filecontent;                                               // --> on ajoute le file_content a la fin de la reponse
}
