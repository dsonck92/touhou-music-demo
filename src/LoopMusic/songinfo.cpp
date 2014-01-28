#include "LoopMusic/songinfo.hpp"

std::ostream & operator<<(std::ostream & left, const SongInfo & right)
{
    left << "Song: " << right.name << std::endl;
    left << "Path: " << right.file << std::endl;
    left << "Start:" << right.start << std::endl;
    left << "Loop: " << right.loop_start << " - " << right.loop_end << std::endl;

    return left;
}

