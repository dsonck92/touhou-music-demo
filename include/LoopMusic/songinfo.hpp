#ifndef SONGINFO_H
#define SONGINFO_H

#include <stdlib.h>
#include <iostream>
#include <streambuf>
#include <fstream>

#include <SSVUtils/FileSystem/FileSystem.hpp>

struct SongInfo {
    std::string name;
    ssvufs::Path   file;
    double start;
    double loop_start;
    double loop_end;
};

std::ostream & operator<<(std::ostream & left, const SongInfo & right);

#endif //SONGINFO_H
