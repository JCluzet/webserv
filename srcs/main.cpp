#include "server.hpp"

Config check_config(int argc, char const *argv[])
{
	std::string tmp;
	// std::cout << "___       __    ______ ________                               \n__ |     / /_______  /___  ___/______________   ______________\n__ | /| / /_  _ \\_  __ \\____ \\_  _ \\_  ___/_ | / /  _ \\_  ___/\n__ |/ |/ / /  __/  /_/ /___/ //  __/  /   __ |/ //  __/  /    \n____/|__/  \\___//_.___//____/ \\___//_/    _____/ \\___//_/     \n\n";

	std::cout << WHITE << "___       __    ______ " << RED << "________                               \n"
			  << WHITE << "__ |     / /_______  /" << RED << "___  ___/______________   ______________\n"
			  << WHITE << "__ | /| / /_  _ \\_  __ \\" << RED << "____ \\_  _ \\_  ___/_ | / /  _ \\_  ___/\n"
			  << WHITE << "__ |/ |/ / /  __/  /_/ /" << RED << "___/ //  __/  /   __ |/ //  __/  /    \n"
			  << WHITE << "____/|__/  \\___//_.___/" << RED << "/____/ \\___//_/    _____/ \\___//_/     \n\n"
			  << WHITE;
	if (argc == 1 || strcmp("--debug", argv[1]) == 0)
	{
		tmp = "config/default.conf";
	}
	else
		tmp = argv[1];
	if (access(tmp.c_str(), F_OK) == -1)
	{
		std::cout << WHITE << "[" << BLUE << "⊛" << WHITE << "] => " << WHITE << "Config file " << RESET << tmp << WHITE << " not found" << std::endl;
		exit(EXIT_FAILURE);
	}

	Config conf(tmp);
	if (conf.server.empty())
	{
		std::cout << std::endl
				  << WHITE << "[" << getHour() << "] QUIT Web" << RED << "Serv" << WHITE << " : " << RESET << "Configuration ERROR" << std::endl;

		return conf;
	}
	std::cout << WHITE << "[" << getHour() << "] START Web" << RED << "Serv" << WHITE << "   ";
	return conf;
}

int main(int argc, char const *argv[])
{
	Config	conf(check_config(argc, argv));
	if (conf.server.empty())
		return (1);
	if (argc >= 2 && !strcmp(argv[1], "--debug"))
		conf.set_debug();

	signal(SIGINT, quit_sig);

	if (argc > 2 && !strcmp(argv[2], "--confdebug"))
		std::cout << conf << std::endl;

	std::cout << "             ";
	for (size_t j = 0; j < conf.server.size(); j++)
		std::cout << RED << "⊛" << WHITE << conf.server[j].ip << ":" <<  conf.server[j].port << "  " << RESET << std::endl << std::endl;
    if (run_server(conf) == -1)
		return 1;
    return (0);
}