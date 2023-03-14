#ifndef GAME_DATA_STRUCTS_H
#define GAME_DATA_STRUCTS_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>

#define ColorCustomBlue     sf::Color(28,19,145)
#define ColorCustomDarkBlue sf::Color(9,6,49)

const std::string totalProjectPath = "/home/dumonten/Documents/GitHub/OOP-4-term/1-lab";
const std::string settingsFileName = totalProjectPath + "settings.bin";
const std::string historyFileName  = totalProjectPath + "history.bin";

enum gameState  { ON, OFF, WAIT };
enum difficulty { HARD, MEDIUM, LOW };
enum blockState { CRASHED, MAX_ATTACKED, MID_ATTACKED, MIN_ATTACKED, NOT_ATTACKED };
enum btnState   { PRESSED, NORMAL, HOVERED};


typedef sf::Vector2f resolution;
const   std::vector<sf::Vector2f> mResolution
{
        {1920, 1080},
        {1600,  720},
        {1560,  720},
        { 1520, 720},
        {1480,  720},
        {1366,  768},
        { 1280, 720}
};

//StartMenu
const uint32_t S_BUTTON_WIDTH     = 450;
const uint32_t S_BUTTON_HEIGHT    = 60;
const uint32_t S_BUTTON_BORDERTHICKNESS = 1;
const uint32_t S_MAINTEXT_SIZE    = 150;
const uint32_t S_MAINTEXT_BORDERTHICKNESS = 6;


#endif //GAME_DATA_STRUCTS_H
