// #pragma once
// #include <iostream>
// #include <fstream>

// bool readinFile(std::string filename, std::string *fileContent)
// {
//     std::string s;
//     std::ifstream ifs(filename);

//     if (!ifs)
//     {
//         std::cerr << "Not Found " << filename << "." << std::endl;
//         *fileContent = "\n<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>File not found.</p>\n</body>\n\n</html>"; // --> pouvoir mettre le fichier d'erreur par default ou celui inndique dans le fichier de config
//         return (1);
//     }
//     getline(ifs, s);
//     if (s == "")
//     {
//         std::cerr << "Empty file." << std::endl;
//         ifs.close();
//         *fileContent = "\n";
//         *fileContent = "<!DOCTYPE html>\n\n<html>\n\n<body>\n  \n  <h1>ERROR 404</h1>\n    <p>Empty file.</p>\n</body>\n\n</html>";      // --> pareil
//         return (1);
//     }
//     *fileContent += s;
//     while (getline(ifs, s))
//     {
//         *fileContent += "\n";
//         *fileContent += s;
//     }
//     ifs.close();
//     return (0);
// }