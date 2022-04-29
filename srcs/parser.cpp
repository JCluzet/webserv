#include "server.hpp"
#include <sys/types.h>

class Response;

int readFile(std::string filename, std::string *fileContent)
{
    std::string s;
    std::ifstream ifs;
    ifs.open(filename.c_str());

    if (!ifs)
    {
        // *fileContent = "\n";
        // *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>"; // --> pouvoir mettre le fichier d'erreur par default ou celui inndique dans le fichier de config
        return (404);
    }
    getline(ifs, s);
    if (s == "")
    {
        ifs.close();
        // *fileContent = "\n";
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
    return (200);
}


void output_log(int ans, std::string filetosearch)
{
    if (ans == 200)
        std::cout << GREEN << "[⊛ 200] => " << WHITE << filetosearch << RESET << std::endl;
    if (ans == 404)
        std::cout << RED << "[⊛ 404] => " << YELLOW << "Redirect to 404 page: " << WHITE << filetosearch << RESET << std::endl;
}

Response response_sender(std::string client, Request *request, s_server *conf)
{
    request->add(client);
    if (request->ready()) //Si elle est prete a etre envoyer
    {
        Response response(request, conf);
    std::cout << "response ready to send" << std::endl;
        output_log(response.getstat(), response.get_pathfile());
        return(response);
    }
    Response tmp;
    output_log(tmp.getstat(), tmp.get_pathfile());
    return(tmp);
}