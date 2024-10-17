#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <iostream>

struct ZEILE{
    const std::string defaultText{"lorem ipsum"};
    unsigned int length = 0;
    sf::Vector2f pos{100, 0};
    sf::Text name;
    sf::Text wert;

    ZEILE(sf::Font* i_font, uint16_t i_size){
        name.setString(defaultText);
        name.setPosition(pos);
        name.setFont(*i_font);
        name.setCharacterSize(i_size);
        wert.setString(defaultText);
        wert.setPosition(pos);
        wert.setFont(*i_font);
        wert.setCharacterSize(i_size);
    }

    void drawText(sf::RenderWindow* renderer){
        (*renderer).draw(name);
        (*renderer).draw(wert);
    }

    void setPos(uint16_t i_x, uint16_t i_y, uint16_t i_abstand, float i_width){
        pos = sf::Vector2f((i_x + i_abstand), i_y);
        wert.setPosition(pos);
        sf::Vector2f name_pos(i_x - (length*i_width), i_y);
        name.setPosition(name_pos);
        std::cout << name_pos.x <<" / " << name_pos.y << std::endl;

    }
};

struct UI{

    sf::Vector2f pos;
    uint16_t lines, xMargin, yMargin, space, charHeight;
    float charWidth;
    uint16_t besetzt;
    unsigned int longestString;
    bool needClear;
    sf::Font cMono;
    ZEILE* pZeile[8];

    UI(sf::Vector2f i_position, int i_rand, int i_zeilenabstand, int i_abstand, float i_size){       //init
        pos=i_position; lines=8; xMargin=i_rand; yMargin=i_zeilenabstand; space=i_abstand; charWidth = i_size*0.55; charHeight = i_size;
        if (!cMono.loadFromFile("ComicMono.ttf")){
            std::cout << "ComicMono.ttf not found";
        }
        for(uint16_t i = 0; i < lines ; i++){
            pZeile[i] = new ZEILE(&cMono, i_size);
        }
        besetzt = 0;
        longestString = 0;
    }

    void nameInsert(std::string i_name){
        if(besetzt >= lines){
            std::cout << "UI voll" << std::endl;
            return;
        }
        (*pZeile[besetzt]).name.setString(i_name);
        (*pZeile[besetzt]).length = i_name.length();
        besetzt++;
        if(i_name.length() > longestString)
            longestString = i_name.length();
    }

    void placeLines(){
        for(uint16_t i = 0; i < lines ; i++){
            (*pZeile[i]).setPos(xMargin + longestString*charWidth, yMargin + i*(yMargin+charHeight), space, charWidth);
        }
    }

    void draw(sf::RenderWindow* renderer){
        for(uint16_t i = 0; i < lines ; i++){
            (*pZeile[i]).drawText(renderer);
        }
    }

    void updateWert(uint16_t i, int i_wert){
        (*pZeile[i]).wert.setString(std::to_string(i_wert));
    }
    void updateWert(uint16_t i, float i_wert){
        (*pZeile[i]).wert.setString(std::to_string(i_wert));
    }
    void updateWert(uint16_t i, bool i_wert){
        if(i_wert){
            (*pZeile[i]).wert.setFillColor(sf::Color::Green);
            (*pZeile[i]).wert.setString("On");
        } else{
            (*pZeile[i]).wert.setFillColor(sf::Color::Red);
            (*pZeile[i]).wert.setString("Off");
        }
    }

    void updateMode(uint16_t i, int i_wert){
        switch(i_wert){
            case 0: (*pZeile[i]).wert.setString("ID"); break;
            case 1: (*pZeile[i]).wert.setString("Dichte"); break;
            case 2: (*pZeile[i]).wert.setString("Reichweite"); break;
            case 3: (*pZeile[i]).wert.setString("Geschwindigkeit"); break;
            case 4: (*pZeile[i]).wert.setString("Richtung"); break;
            case 5: (*pZeile[i]).wert.setString("Speed+Richtung"); break;
            case 6: (*pZeile[i]).wert.setString("Hash"); break;
            case 7: (*pZeile[i]).wert.setString("Predator"); break;
        }
    }

};

