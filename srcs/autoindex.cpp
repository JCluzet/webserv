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

bool	indexGenerator(std::string* codeHTML, std::string path, std::string defaultFolder)
{
    DIR *dir = opendir(path.c_str());
    if (!dir){
        // std::cerr << "Can't open " << path << "for generate index.*codeHTML." << std::endl;
        return (1);
    }
    struct dirent *ent;
//HEAD & TITLE
	*codeHTML = "<html>\n";
	*codeHTML += "<head><title>autoindex</title></head>\n";
	*codeHTML += "<body>\n";
	*codeHTML += "<h1>Index of /";
	*codeHTML += (path != defaultFolder + "/") ? path.substr(defaultFolder.length()+1, path.length() - defaultFolder.length()+1) : ""; //actual_file
	*codeHTML += "</h1><hr/>\n";
	*codeHTML += "<table width=\"100%\" border=\"0\">\n";
	*codeHTML += "<tr>\n";
	*codeHTML += "<th align=\"left\">Name</th>\n";
	*codeHTML += "<th align=\"left\">Last modified</th>\n";
	*codeHTML += "<th align=\"left\">size</th>\n";
	*codeHTML += "</tr>\n"; //10
//LISTING
    while ((ent = readdir(dir))){
        // if (!strcmp(ent->d_name, ".")){
        if (!(ent->d_name[0] == '.' && ent->d_name[1] == '\0')){
            // std::cout << "*******************" << ent->d_name << std::endl;
            std::string filepath = path + "/" + ent->d_name;
            struct stat s;
            stat(filepath.c_str(), &s);
            std::string size = sizetToStr(s.st_size);
            std::string date = ctime(&s.st_mtime);
            date.erase(date.size() - 1);
            *codeHTML += "<tr>\n";
            *codeHTML += "<td><a href=\"";
            // *codeHTML += path.substr(4, path.size() - 4);
            // *codeHTML += "/";
            *codeHTML += ent->d_name;
            *codeHTML += S_ISDIR(s.st_mode) ? "/" : "";
            *codeHTML += "\">";
            *codeHTML += ent->d_name;
            *codeHTML += S_ISDIR(s.st_mode) ? "/" : "";
            *codeHTML += "</a></td>\n";
            *codeHTML += "<td>";
            *codeHTML += date;
            *codeHTML += "</td>\n";
            *codeHTML += "<td>";
            *codeHTML += S_ISDIR(s.st_mode) ? "_" : size;
            *codeHTML += "</td>\n";
            *codeHTML += "</tr>\n";
        }
	}
//END
	*codeHTML += "</able>\n";
	*codeHTML += "</body>\n";
	*codeHTML += "</html>\n";
    // std::cout << *codeHTML << std::endl;
    closedir(dir);
    return (0);
}