#ifndef LOOPMUSIC_H
#define LOOPMUSIC_H

#include <SFML/Audio.hpp>
#include <sndfile.hh>

#include "songinfo.hpp"

class LoopMusic : public sf::SoundStream
{
    SndfileHandle *m_snd;

    sf_count_t m_pos,m_start,m_loop_start,m_loop_end;

    std::vector<int16_t> m_buffer;
public:
    LoopMusic(SongInfo file);
    virtual bool onGetData(Chunk& data);

    virtual void onSeek(sf::Time timeOffset);

    void load(SongInfo file);

    float getProgress();
    float getLoopStart();
};

#endif //LOOPMUSIC_H
