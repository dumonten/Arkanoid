#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include <iostream>
#include <vector>
#include <fstream>
#include "game_data_structs.h"

namespace Arkanoid
{
    class Property {
    protected:
        sf::Vector2f position, size;
        sf::Color color;
    public:
        virtual void setPosition(const sf::Vector2f &p);

        virtual void setSize(const sf::Vector2f &s);

        virtual void setColor(const sf::Color &c);

        virtual sf::Vector2f getPosition();

        virtual sf::Vector2f getSize();

        virtual sf::Color getColor();
    };

    class Movable {
    protected:
        float speed;
    public:
        /*Bonuses*/
        void speedUp();

        void speedDown();

        /*Bonuses*/
        virtual void move() = 0;
    };

    class Button : public sf::Drawable, public Property {
    private:
        sf::Color clrBgNormal, clrBgHover, clrBgClicked,
                clrTextNormal, clrTextHover, clTextClicked,
                clrBorder;
        float borderThickness,
                borderRadius;
        sf::Sprite button;
        sf::Uint32 btnState;
        sf::Font font;
        uint32_t fontSize;
        sf::Text text;
        sf::Text shadow;
    private:
        void draw() const;

    public:
        Button();

        Button(const std::string &name, const sf::Font &font, const sf::Vector2f p, const sf::Vector2f s,
               sf::Uint32 style);

        void setColorTextNormal(sf::Color text);

        void setColorTextHover(sf::Color text);

        void setColorTextClicked(sf::Color text);

        void setColorNormal(sf::Color bgNormal);

        void setColorHover(sf::Color bgHover);

        void setColorClicked(sf::Color bgClicked);

        void setBorderColor(sf::Color border);

        void setBorderThickness(float t);

        void setBorderRadius(float r);

        void setText(std::string s);

        void setStyle(sf::Uint32 style);

        void setFont(sf::Font &font);

        sf::Uint32 getState();

        void update(sf::Event &e, sf::RenderWindow &window);

        ~Button();
    };

    class Platform : public sf::Drawable, public Property, public Movable {
    private:
        sf::Sprite platform;
    private:
        void draw() const;

    public:
        Platform();

        Platform(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp);

        /*Bonuses*/
        void lengthUp();

        void lengthDown();

        /*Bonuses*/

        void move();

        void update(sf::Event &e, sf::RenderWindow &window);

        ~Platform();
    };

    class Player {
    private:
        statistics statistic;
        Platform platform;
    public:
        Player();

        /*Bonuses*/
        void downScore();

        void upScore();

        void upLives();

        void downLives();

        /*Bonuses*/
        statistics getStatistic();

        void clearStatistic();

        void update(sf::Event &e, sf::RenderWindow &window);

        ~Player();
    };

    class Ball : public sf::Drawable, public Property, public Movable {
    private:
        sf::Sprite ball;
    private:
        void draw() const;

    public:
        Ball();

        Ball(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp);

        void move();

        void update(sf::Event &e, sf::RenderWindow &window);

        ~Ball();
    };

    class Balls {
    private:
        std::vector<Ball> balls;
    public:
        Balls();

        void addBall(const Ball &ball);

        void deleteBall(Ball &ball);

        /*Bonuses*/
        void numBallsUp();

        /*Bonuses*/

        ~Balls();
    };

    class Bonuses {
    private:
        void random();

        /*Bonuses*/
        friend void Movable::speedUp();

        friend void Movable::speedDown();

        friend void Player::downScore();

        friend void Player::upScore();

        friend void Player::upLives(); 

        friend void Player::downLives();

        friend void Balls::numBallsUp();
        /*Bonuses*/
    public:
        Bonuses();

        void applyBonus();

        void update(sf::Event &e, sf::RenderWindow &window);

        ~Bonuses();
    };

    class Block : public sf::Drawable, public Property, public Movable {
    private:
        sf::Sprite block;
        bool bonusStatus;
        Bonuses *bonus;
    private:
        void draw() const;

    public:
        Block();

        Block(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp, bool bonus);

        void move();

        void update(sf::Event &e, sf::RenderWindow &window);

        bool getBonusStatus();

        void setBonusStatus(const bool &status);

        ~Block();
    };

    class Players {
    private:
        std::vector<Player> players;
    public:
        Players();

        void addPlayer(const Player &player);

        void deletePlayer(Player &player);

        ~Players();
    };

    class Blocks {
    private:
        std::vector<Ball> balls;
        float bonusPercentage;
    public:
        Blocks();

        void addBlock(const Ball &ball);

        void deleteBlock(Ball &ball);

        void setBonusPercentage(const difficulty &diff);

        float getBonusPercentage();

        void initBonuses();

        ~Blocks();
    };

    class Statistics : public sf::Drawable {
    private:
        statistics data;
        //other sfml fields
    private:
        void draw() const;

    public:
        Statistics();

        void setData(const statistics &st);

        void update();

        ~Statistics();
    };

    class History {
    private:
        std::fstream file;
    public:
        History();

        void saveHistory();

        void importHistory();

        void clearHistory();

        ~History();
    };

    class Menu : public sf::Drawable {
    private:
        sf::RenderWindow &window;
        std::vector<Button> buttons;
    private:
        void draw() const;

    public:
        Menu();

        void init();

        void eventsHandler();

        void update();

        ~Menu();
    };

    class Settings {
    private:
        std::fstream file;
        difficulty diff;
        resolution res;
    public:
        Settings();

        void setResolution(const resolution &r);

        void setDifficulty(const difficulty &d);

        resolution getResolution();

        difficulty getDifficulty();

        void saveSettings();

        void clearSettings();

        void importSettings();

        ~Settings();
    };

    class GameField {
    private:
        Menu mainWindow;
        Players players;
        Balls balls;
        Blocks blocks;
        std::vector<sf::Event> events;
    public:
        GameField();

        void init();

        void eventsHandler();

        void update();

        ~GameField();
    };

    class Game {
    private:
        Menu startMenu, restartMenu, waitMenu, endMenu;
        gui::state st;
        GameField gameField;
        History history;
        Statistics statistics;
        Settings settings;
    public:
        Game();

        void initMenus();

        void start();

        void setGameState(const gameState state);

        gameState getGameState();

        ~Game();
    };
}

#endif //GAME_CLASSES_H

