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

enum GameState  { ON, OFF, WAIT };
enum Difficulty { HARD, MEDIUM, LOW };
enum BlockState { CRASHED, MAX_ATTACKED, MID_ATTACKED, MIN_ATTACKED, NOT_ATTACKED };
enum BtnState   { PRESSED, NORMAL, HOVERED};
enum EventType  {
                    OPEN_SETTINGS,
                    OPEN_START_MENU,
                    OPEN_GAME_FIELD, QUIT,
                    RES_FORWARD,
                    RES_BACKWARD,
                    DIFF_FORWARD,
                    DIFF_BACKWARD,
                    SAVE_SETTINGS
                };


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
const uint32_t S_BUTTON_WIDTH           = 450;
const uint32_t S_BUTTON_HEIGHT          = 60;
const uint32_t S_BUTTON_BORDERTHICKNESS = 1;
const uint32_t S_TEXT_SIZE              = 150;
const uint32_t S_TEXT_BORDERTHICKNESS   = 6;

//Settings Menu
const uint32_t ST_BUTTON_WIDTH           = 50;
const uint32_t ST_BUTTON_HEIGHT          = 50;
const uint32_t ST_BUTTON_BORDERTHICKNESS = 0;
const uint32_t ST_TEXT_SIZE              = 60;
const uint32_t ST_TEXT_BORDERTHICKNESS   = 0;

#endif //GAME_DATA_STRUCTS_H
