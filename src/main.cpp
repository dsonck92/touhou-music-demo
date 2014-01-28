#include "config.h"
#include <stdlib.h>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <time.h>
#include <LoopMusic/loopmusic.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SSVUtils/FileSystem/FileSystem.hpp>

using namespace std;

int main(int argc, char* argv[]) {

    for(int I=0;I<argc;I++)
    {
        cout << argv[I];
    }

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

    unsigned int sel = 0;

    LoopMusic music(song_list[sel]);
    music.play();

    sf::RenderWindow screen(sf::VideoMode(800, 600), "Touhou Music Demo");
    sf::Font font;

    sf::Text text;

    font.loadFromFile("Roboto-Regular.ttf");
    text.setFont(font);
    while (screen.isOpen()) {

        screen.clear();

        for(unsigned int I=0;I<song_list.size();I++)
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
                unsigned int new_sel = sel;
                if(event.key.code == sf::Keyboard::Up)
                {
                    new_sel --;
                }
                else if(event.key.code == sf::Keyboard::Down)
                {
                    new_sel ++;
                }
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
