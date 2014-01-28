#include "LoopMusic/loopmusic.hpp"

#include <stdlib.h>
#include <iostream>
#include <streambuf>
#include <fstream>

LoopMusic::LoopMusic(SongInfo file)
{
    m_snd = 0;

    load(file);
}

bool LoopMusic::onGetData(Chunk &data)
{
    const unsigned int samples_per_call = m_buffer.size()/m_snd->channels();

    std::cout << "GetData:" << std::endl;

    sf_count_t samples_handled = 0;

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Loop start: " << m_loop_start << std::endl;
    std::cout << "Loop end  : " << m_loop_end << std::endl;
    while(samples_handled < samples_per_call)
    {
        std::cout << "Pos: " << m_pos << std::endl;
        sf_count_t samples_to_handle,samples_sent;
        samples_to_handle = samples_per_call - samples_handled;
        std::cout << "Samples left: " << samples_to_handle << std::endl;
        if(m_pos + samples_to_handle > m_loop_end)
        {
            samples_to_handle = m_loop_end - m_pos;
            samples_handled += samples_sent = m_snd->readf(m_buffer.data()+samples_handled*m_snd->channels(),samples_to_handle);
            m_pos = m_loop_start;
            m_snd->seek(m_loop_start,SEEK_SET);
        }
        else
        {
            samples_handled += samples_sent = m_snd->readf(m_buffer.data()+samples_handled*m_snd->channels(),samples_to_handle);
            m_pos += samples_sent;
        }
        std::cout << "Read: " << samples_sent << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;

    data.samples = m_buffer.data();
    data.sampleCount = samples_handled*m_snd->channels();

    return true;
}

void LoopMusic::onSeek(sf::Time )
{

}

void LoopMusic::load(SongInfo file)
{
    if(m_snd)
    {
        stop();

        delete m_snd;
    }

    std::cout << "Loading: "<<std::endl;
    std::cout << file;

    if(file.file.getExtension().compare(".dat")==0)
    {
        m_snd = new SndfileHandle((file).file,SFM_READ,SF_FORMAT_RAW|SF_FORMAT_PCM_16,2,44100);
    }
    else
    {
        m_snd = new SndfileHandle((file).file);
    }
    m_buffer.resize(m_snd->samplerate()*5*m_snd->channels(),0);

    std::cout << "Samplerate: "<< m_snd->samplerate() << std::endl;
    std::cout << "Channels: "<<m_snd->channels() << std::endl;


    initialize(m_snd->channels(),m_snd->samplerate());

    m_snd->seek(file.start*m_snd->samplerate(),SEEK_SET);
    m_pos = m_start = file.start*m_snd->samplerate();
    m_loop_start = file.loop_start*m_snd->samplerate();
    m_loop_end = file.loop_end*m_snd->samplerate();
}

float LoopMusic::getProgress()
{
    return (float)(m_pos-m_start)/(float)(m_loop_end-m_start);
}

float LoopMusic::getLoopStart()
{
    return (float)(m_loop_start-m_start)/(float)(m_loop_end-m_start);
}
