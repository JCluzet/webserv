#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
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

bool    writeFile(std::string const str, std::string filename)
{
    std::ofstream ofs(filename.c_str());

    if (!ofs) {
        std::cerr << "Can't open " << filename << "." << std::endl;
        return (1);
    }
    ofs << str;
    ofs.close();
    return (0);
}

bool	indexGenerator(std::string* codeHTML, std::string path = ".")
{
    DIR *dir = opendir(path.c_str());
    if (!dir){
        std::cerr << "Can't open " << path << "for generate index.*codeHTML." << std::endl;
        return (1);
    }
    struct dirent *ent;
//HEAD & TITLE
	*codeHTML = "<html>\n";
	*codeHTML += "<head><title>autoindex</title></head>\n";
	*codeHTML += "<body>\n";
	*codeHTML += "<h1>Index of /";
	*codeHTML += (path != ".") ? path : ""; //actual_file
	*codeHTML += "</h1><hr/>\n";
	*codeHTML += "<table width=\"100%\" border=\"0\">\n";
	*codeHTML += "<tr>\n";
	*codeHTML += "<th align=\"left\">Name</th>\n";
	*codeHTML += "<th align=\"left\">Last modified</th>\n";
	*codeHTML += "<th align=\"left\">size</th>\n";
	*codeHTML += "</tr>\n"; //10
//LISTING
    while ((ent = readdir(dir))){
        if (ent->d_name[0] != '.'){
            std::string filepath = path + "/" + ent->d_name;
            struct stat s;
            stat(filepath.c_str(), &s);
            std::string size = sizetToStr(s.st_size);
            std::string date = ctime(&s.st_mtime);
            date.erase(date.size() - 1);
            *codeHTML += "<tr>\n";
            *codeHTML += "<td><a href=\"";
            *codeHTML += (path != ".") ? path : ""; //actual_file
            *codeHTML += "/";
            *codeHTML += ent->d_name;
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
    // return (0);
	return (writeFile(*codeHTML, "index.html"));
}

// int main(){
//     std::string codeHTML;
//     indexGenerator(&codeHTML, ".");
//     return 0;
// }