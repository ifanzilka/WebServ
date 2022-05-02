#include "Autoindex.h"
#include <dirent.h>

Autoindex::Autoindex(std::string const &dir) : directory(dir)
{
}

std::string Autoindex::get_html()
{
    const char *dir;

    dir = directory.c_str();
    std::string autoIndexBegin = "<!DOCTYPE html>\n"
                                 "<html>\n"
                                 "<head>\n"
                                 "<title>Index of /</title></head>\n"
                                 "<body bgcolor=\"white\">\n"
                                 "<h1>Index of /</h1>\n"
                                 "<hr>\n"
                                 "<pre>\n";

    std::string autoIndexEnd = "</pre>\n"
                               "<hr>\n"
                               "</body>\n"
                               "</html>\n";

    struct dirent *dir_point;
    DIR    *dir_descript;
    std::string     str;
    std::string     file_name;

    dir_descript = opendir(dir);
    if (dir_descript == NULL)
        return ("ERROR");
    str = "";
    str+= autoIndexBegin;
    while ((dir_point = readdir(dir_descript)))
    {

        if (dir_point->d_type != DT_DIR)
        {
            file_name = dir_point->d_name;
        }
        else
        {
            file_name = dir_point->d_name;
            file_name += "/";
        }
        str+="     <a href=\"./";
        str+=file_name;
        str+="\">";
        str+=file_name;
        str+="</a>";


//        if (dir_point->d_type == DT_DIR)
//        {
//            printf("dir: %s",dir_point->d_name);
//        }
//        else if (dir_point->d_type)
//        {
//            printf(dir_point->d_name);
//        }
//        printf("\n");
    }

    str+= autoIndexEnd;
    closedir(dir_descript);
    return (str);
}