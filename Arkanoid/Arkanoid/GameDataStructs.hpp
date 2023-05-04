#ifndef GAME_DATA_STRUCTS_H
#define GAME_DATA_STRUCTS_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <algorithm>
#include <string>
#include <regex>


#define ColorCustomBlue                    sf::Color(28,19,145)
#define ColorCustomLightBlue               sf::Color(141,222,251,255)
#define ColorCustomPink                    sf::Color(251, 143, 184)
#define ColorCustomGreen                   sf::Color(165,243,74,255)
#define ColorCustomDarkGreen               sf::Color(98, 209, 42, 255)
#define ColorCustomDarkBlue                sf::Color(9,6,49)
#define ColorCustomMidPurpleBlueLight      sf::Color(112, 146, 190)
#define ColorCustomMidPurpleBlueDark       sf::Color(69, 104, 152)
#define ColorCustomMidPurpleBlueUltraDark  sf::Color(45, 68, 100)
#define ColorCustomRed                     sf::Color(240, 70, 79)
#define ColorCustomOrange                  sf::Color(252, 179, 124)
#define ColorCustomYellow                  sf::Color(240, 216, 117)

const std::string totalProjectPath = "C:/Users/anila/OneDrive/���������/GitHub/OOP-4-term/Arkanoid";
const std::string settingsFileName = totalProjectPath + "settings.bin";
const std::string historyFileName = totalProjectPath + "history.bin";
 
enum GameState { ON, OFF };
enum Difficulty { HARD, MEDIUM, LOW, ediffSize = 3};
enum BlockState { CRASHED, MAX_ATTACKED, MID_ATTACKED, MIN_ATTACKED, NOT_ATTACKED};
enum BtnState   { PRESSED, NORMAL, HOVERED};
enum EventType {
    OPEN_SETTINGS,
    OPEN_START_MENU,
    OPEN_GAME_FIELD,
    RESTART_OPEN_GAME_FIELD,
    OPEN_WAIT_MENU,
    QUIT,
    LOAD_THE_GAME, 
    RES_CHANGE,
    DIFF_CHANGE,
    SAVE_SETTINGS,
    RESET_GAME,
    COLLISIONS,
    FALL_DOWN,
    WIN, 
    DELETE_EXCESS_BALL,
    COLLIDE_REACTION,
    BACKGROUND_COLLISION,
    SCORE_UP_FOR_HITTING,
    SCORE_UP_FOR_CRASHING,
    LIVES_DOWN,
    RESET_PLATFORMS_TO_INITIAL_STATE, 
    PLAYER_IS_LOSE, 
    SHOW_END_MESSAGE,
    SAVE_GAME, 
    APPLY_BONUSES, 
    DRAW_FRAME
};

enum BonusDescriptor
{
    BONUS_BALLS_NUM_UP,
    BONUS_CHANGE_SPEED_PLATFORM,
    BONUS_CHANGE_SPEED_BALL,
    BONUS_EMPTY, 
    ebonusSize = 4
};

enum CollisionType
{
    PLATFORM_BG_COLLISION, 
    BG_COLLISION, 
    BLOCK_COLLISION, 
    PLATFORM_COLLISION,
    BALL_FALL_COLLISION, 
    NONE_COLLISION
};

enum CollisionDirection
{
    VERTICAL_COLLISION_DIRECTION,
    HORIZONTAL_COLLISION_DIRECTION,
    NONE_COLLISION_DIRECTION
};


enum Classes {
    STATUS_BAR, 
    GAME_FIELD,
    BLOCK, 
    PLATFORM, 
    BALL
};

const std::map<std::string, Classes> classesNames =
{
    { "class Arkanoid::StatusBar", Classes::STATUS_BAR },
    { "class Arkanoid::GameField", Classes::GAME_FIELD },
    { "class Arkanoid::Block",     Classes::BLOCK      },
    { "class Arkanoid::Ball",      Classes::BALL       },
    { "class Arkanoid::Platform",  Classes::PLATFORM   }
};



typedef sf::Vector2f resolution;
const   std::vector<sf::Vector2f> mResolution
{ 
    {1920, 1000},
    {800,   450}, 
    {1480,  720},
    {1560,  877},
    {1600,  900},
    {1520,  720},
    {1366,  768},
    {1280,  720}
};

const float cps = 17; 
//StartMenu
const float S_BUTTON_WIDTH           = 0.2343;
const float S_BUTTON_HEIGHT          = 0.0556;
const float S_BUTTON_BORDERTHICKNESS = 1;
const float HEADER_TEXT_SIZE         = (float)150/1080;
const float HEADER_BORDERTHICKNESS   = 6;

//Settings Menu
const float ST_BUTTON_WIDTH            = 0.234375;
const float ST_BUTTON_BORDERTHICKNESS  = 0;
const float SUB_HEADER_TEXT_SIZE       = (float)30/1080;
const float ST_BUTTON_TEXT_SIZE        = (float)20/1080;
const float SUB_HEADER_BORDERTHICKNESS = 0;

//GameField
//GameField --- Block
const uint32_t BLOCK_NUM_WIDTH_HARD    = 20;
const uint32_t BLOCK_NUM_WIDTH_MEDIUM  = 15;
const uint32_t BLOCK_NUM_WIDTH_LOW     = 3;

const uint32_t BLOCK_NUM_HEIGHT_HARD   = 10;
const uint32_t BLOCK_NUM_HEIGHT_MEDIUM = 7;
const uint32_t BLOCK_NUM_HEIGHT_LOW    = 2;

//GameField --- StatusBar
const float STATUS_BAR_BUTTON_WIDTH          = 0.05729;
const float STATUS_BAR_BUTTON_HEIGHT         = (float)40 / 1000;
const float STATUS_BAR_BUTTON_FONT_SIZE      = 0.0185;
const float STATUS_BAR_BUTTON_TEXT_FONT_SIZE = 0.0185;
const float STATUS_BAR_HEIGHT                = (float)60 / 1000;
const float STATUS_BAR_TEXT_OFFSET_LEFT      = 0.104167;
const float STATUS_BAR_TEXT_OFFSET_TOP       = 0.010416;
const float STATUS_BAR_BUTTON_OFFSET_LEFT    = 0.014;
const float STATUS_BAR_BUTTON_OFFSET_TOP     = 0.00926;

//GameField --- Ball 
const float BALL_SQUARE = (float)(17.5f * 17.5f)/(1920*1000);
const sf::Vector2f BALL_SPEED = { (float)3 / 1920, (float)10 / 1000 };
const float BALL_WIDTH_INIT_OFFSET = 0.5 - (float)8.75f/1920;
const float BALL_HEIGHT_INIT_OFFSET = 0.75;

//GameField --- Platform 
const float PlATFORM_WIDTH  = (float)250 / 1920;
const float PlATFORM_HEIGHT = (float)25 / 1000;
const float PLATFORM_WIDTH_INIT_OFFSET = 0.5 - (float)PlATFORM_WIDTH / 2;
const float PLATFORM_HEIGHT_INIT_OFFSET = 1 - (float)25 / 1000;
const sf::Vector2f PLATFORM_SPEED = { (float)20 / 1920, 0 };

const sf::Vector2f MAX_SPEED = { (float)30 / 1920,  (float)20 / 1920 };


//GameField
const float MBOX_WIDTH  = (float)480/1920;
const float MBOX_HEIGHT = (float)100/1000;
const std::string MSTART_TEXT = "To start the game \n press the 'Space' button \n At each next fall, also press the 'Space'"; 
const float MBOX_TEXT_SIZE = (float)18/1000;

//GameField --- Score 
const uint32_t SCORE_PER_CRASHED_BLOCK = 15; 
const uint32_t SCORE_PER_HITTED_BLOCK  = 5; 

//GameField --- Game is over
const std::string MEND_TEXT = "All players are dead! You lose. Try again";
const std::string MWIN_TEXT = "Congratulations you are the WINNER!";

//GameField --- Bonuses
const float BONUS_PERCANTAGE_HARD   = 0.4; 
const float BONUS_PERCANTAGE_MEDIUM = 0.6; 
const float BONUS_PERCANTAGE_LOW    = 0.9; 

const float BONUS_ATTEMPS_HARD   = 1; 
const float BONUS_ATTEMPS_MEDIUM = 1; 
const float BONUS_ATTEMPS_LOW    = 1;

const int BONUS_MAX_BALLS_COUNT = 1; 

//Save game History 
const std::string TEXT_FILE_GAME_PATH     = "C:/Users/anila/OneDrive/���������/GitHub/OOP-4-term/Arkanoid/SaveFiles/gameData.txt"; 
const std::string JSON_FILE_GAME_PATH     = "C:/Users/anila/OneDrive/���������/GitHub/OOP-4-term/Arkanoid/SaveFiles/gameData.json"; 

#endif