#include "config.h"
#include <stdlib.h>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <time.h>
#include <sndfile.hh>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SSVUtils/SSVUtils.hpp>

using namespace std;

struct SongInfo {
    std::string name;
    ssvufs::Path   file;
    double start;
    double loop_start;
    double loop_end;
};

ostream & operator<<(ostream & left, const SongInfo & right)
{
    left << "Song: " << right.name << endl;
    left << "Path: " << right.file << endl;
    left << "Start:" << right.start << endl;
    left << "Loop: " << right.loop_start << " - " << right.loop_end << endl;

    return left;
}

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

LoopMusic::LoopMusic(SongInfo file)
{
    m_snd = 0;

    load(file);
}

bool LoopMusic::onGetData(Chunk &data)
{
    const unsigned int samples_per_call = m_buffer.size()/m_snd->channels();

    cout << "GetData:" << endl;

    sf_count_t samples_handled = 0;

    cout << "--------------------------------" << endl;
    cout << "Loop start: " << m_loop_start << endl;
    cout << "Loop end  : " << m_loop_end << endl;
    while(samples_handled < samples_per_call)
    {
        cout << "Pos: " << m_pos << endl;
        sf_count_t samples_to_handle,samples_sent;
        samples_to_handle = samples_per_call - samples_handled;
        cout << "Samples left: " << samples_to_handle << endl;
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
        cout << "Read: " << samples_sent << endl;
    }
    cout << "--------------------------------" << endl;

    data.samples = m_buffer.data();
    data.sampleCount = samples_handled*m_snd->channels();

    return true;
}

void LoopMusic::onSeek(sf::Time timeOffset)
{

}

void LoopMusic::load(SongInfo file)
{
    if(m_snd)
    {
        stop();

        delete m_snd;
    }

    cout << "Loading: "<<endl;
    cout << file;

    if(file.file.getExtension().compare(".dat")==0)
    {
        m_snd = new SndfileHandle((file).file,SFM_READ,SF_FORMAT_RAW|SF_FORMAT_PCM_16,2,44100);
    }
    else
    {
        m_snd = new SndfileHandle((file).file);
    }
    m_buffer.resize(m_snd->samplerate()*5*m_snd->channels(),0);

    cout << "Samplerate: "<< m_snd->samplerate() << endl;
    cout << "Channels: "<<m_snd->channels() << endl;


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

int main(int argc, char* argv[]) {

    cout << "Version " << myproject_VERSION_MAJOR << "." << myproject_VERSION_MINOR << endl;

    std::ifstream songs;
    songs.open("./tracks.idx");

    std::string data;
    SongInfo info;
    std::vector<SongInfo> song_list;
    if(songs.is_open())
    {
        char c;
        int I=0;
        while(songs.good())
        {
            songs.get(c);
            if(c == '\n')
            {

            } else
            if(c == ';')
            {
                switch(I)
                {
                case 0:
                    info.name = data;
                    break;
                case 1:
                    info.file = ssvufs::Path("./")+data;
                    break;
                case 2:
                    info.start = atof(data.data());
                    break;
                case 3:
                    info.loop_start = atof(data.data());
                    break;
                case 4:
                    info.loop_end = atof(data.data());
                    I = -1;
                    song_list.push_back(info);
                    break;
                }
                I ++;
                data = "";
            }
            else
            {
                data += c;
            }
        }
        songs.close();
    }
    else
    {
        cout << "Err: could not open" << endl;
    }

    int I=0;
    for(SongInfo info:song_list)
    {
        cout << I << ":" << endl << "----------------------" << endl;
        cout << info <<endl;
        cout << "++++++++++++++++++++++" << endl;
        I++;
    }

    int sel = 0;

    LoopMusic music(song_list[sel]);
    music.play();

    sf::RenderWindow screen(sf::VideoMode(800, 600), "Touhou Music Demo");
    sf::Font font;

    sf::Text text;

    font.loadFromFile("Roboto-Regular.ttf");
    text.setFont(font);
    while (screen.isOpen()) {

        screen.clear();

        for(int I=0;I<song_list.size();I++)
        {
            text.setString(song_list[I].name);

            text.setPosition(0,I*font.getLineSpacing(10));
            text.setCharacterSize(10);

            text.setColor(sf::Color(sel==I?0:255,sel==I?0:255,255));

            if(I == sel)
            {
                sf::RectangleShape rect(sf::Vector2f(screen.getSize().x,font.getLineSpacing(10)));

                rect.setFillColor(sf::Color(0,0,128));

                rect.setPosition(0,I*font.getLineSpacing(10));

                screen.draw(rect);

                rect.setSize(sf::Vector2f(screen.getSize().x*music.getProgress(),font.getLineSpacing(10)));

                rect.setFillColor(sf::Color(0,255,255));
                screen.draw(rect);
            }
            screen.draw(text);
        }
        screen.display();



        sf::Event event;
        while (screen.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                screen.close();

            if (event.type == sf::Event::KeyPressed)
            {
                int new_sel = sel;
                if(event.key.code == sf::Keyboard::Up)
                {
                    new_sel --;
                }
                else if(event.key.code == sf::Keyboard::Down)
                {
                    new_sel ++;
                }
                if(new_sel < 0)
                    new_sel = 0;
                if(new_sel >= song_list.size())
                    new_sel = song_list.size()-1;

                if(sel != new_sel)
                {
                    sel = new_sel;
                    music.load(song_list[sel]);
                    music.play();
                }
            }
        }
        sf::sleep(sf::seconds(.1));
    }

}
