#include "server.hpp"
#include <sys/types.h>

class Response;

void output_log(int ans, std::string filetosearch)
{
    if (ans == 200)
        std::cout << GREEN << "    [⊛ 200]    => " << WHITE << filetosearch << RESET << std::endl;
    if (ans == 404)
        std::cout << RED << "    [⊛ 404]    => " << YELLOW << "Redirect to 404 page: " << WHITE << filetosearch << RESET << std::endl;
}

Response response_sender(std::string client, Request *request, s_server *conf)
{
    request->add(client);
    if (request->ready()) //Si elle est prete a etre envoyer
    {
        Response response(request, conf);
        output_log(response.getstat(), response.get_pathfile());
        return(response);
    }
    Response tmp;
    output_log(tmp.getstat(), tmp.get_pathfile());
    return(tmp);
}