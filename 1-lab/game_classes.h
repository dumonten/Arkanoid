#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include <iostream>
#include <vector>
#include <fstream>
#include "game_data_structs.h"

namespace Arkanoid
{

    class DisplayObject : public sf::Drawable
    {
    protected:
        sf::Shape    *selfRect;
        sf::Vector2f position, size, scaleSize, scalePosition;
        bool         visible{false};
    protected:
        DisplayObject()
        {
            selfRect = nullptr;
            position = size = {0, 0};
        }
        DisplayObject(sf::Vector2f s, sf::Vector2f p)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
        }
        DisplayObject(float radius, sf::Vector2f p)
        {
            selfRect = new sf::CircleShape(radius);
            size = {radius, radius};
            selfRect->setPosition(position = p);
        }
    public:
        static sf::Vector2f getCenter(sf::FloatRect parentSize, sf::FloatRect childSize);

        virtual void setSize(sf::Vector2f sz)           {  ((sf::RectangleShape *)selfRect)->setSize(size = sz);                  }
        virtual void setSize(float radius)              { size = { radius, radius}; ((sf::CircleShape *)selfRect)->setRadius(radius); }

        virtual void updateSizes(const sf::Vector2f parentSize)
        {
            setSize( {parentSize.x * scaleSize.x, parentSize.y * scaleSize.y} );
            setPosition( {parentSize.x * scalePosition.x, parentSize.y * scalePosition.y});
        }

        virtual sf::Vector2f getSize()     { return size; }
        virtual sf::Vector2f getPosition() { return position; }
        virtual void setPosition(sf::Vector2f ps)       { selfRect->setPosition(position = ps);}
        virtual void display()                          { visible = true;                          }
        virtual void hide()                             { visible = false;                         }
        [[nodiscard]] virtual sf::FloatRect getGlobal() { return selfRect->getGlobalBounds();      }
        [[nodiscard]] virtual bool isVisible()const     { return visible;                          }
        void draw(sf::RenderTarget& target,
                  sf::RenderStates states)const override { target.draw(*selfRect); }
        virtual void setScales(const sf::Vector2f parentSize)
        {   scaleSize     = { (float)size.x/parentSize.x, (float)size.y/parentSize.y};
            scalePosition = { (float)position.x/parentSize.x, (float)position.y/parentSize.y}; }
        ~DisplayObject() override                       { if(!selfRect) free(selfRect);        }

    protected:
        virtual void checkCollisions() {}
    };

    class MovableObject : DisplayObject
    {
    protected:
        sf::Vector2f  speed;
    public:
        virtual void move()=0;
        virtual void setSpeed(const sf::Vector2f spd)      { speed = spd;  }
        [[nodiscard]] virtual sf::Vector2f getSpeed()const { return speed; }
        virtual void randomlyChangeSpeed() { /*definition*/}
    };

    class Statistics : public DisplayObject
    {
    private:
        uint16_t numOfLives;
        uint32_t score,
                numOfBonuses,
                numOfCrashedBlocks;
        sf::Clock   deadTime;
        std::string userName;
    public:
        Statistics();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        [[nodiscard]] uint16_t    getNumOfLives()const            { return numOfLives;         }
        [[nodiscard]] uint32_t    getScore()const                 { return score;              }
        [[nodiscard]] uint32_t    getNumOfBonuses()const          { return numOfBonuses;       }
        [[nodiscard]] uint32_t    getNumOfCrashedBlocks()const    { return numOfCrashedBlocks; }
        [[nodiscard]] sf::Clock   getDeadTime()const              { return deadTime;           }
        [[nodiscard]] std::string getUserName()const              { return userName;           }

        void setUserName(const std::string& name)   { userName = name;           }
        void setNumOfLives(const uint16_t lives)    { numOfLives = lives;        }
        void setScore(const uint32_t scr)           { score = scr;               }
        void setDeadTime(const sf::Clock time)      { deadTime = time;           }
        void setNumOfCrashedBlocks(const uint32_t blocks) { numOfCrashedBlocks = blocks; }
        void setNumOfBonuses(const uint32_t bns)          { numOfBonuses = bns;          }
        ~Statistics() override;
    };

    class TextField : public sf::Drawable {
    private:
        float borderThickness;
        sf::Text text;
        std::string str;
        sf::Color clrNormal, clrHovered, clrClicked, clrBorder;
        sf::Font font;
        uint32_t textStyle{}, charSize;
        sf::Vector2f scaleSize, scalePosition;
    public:
        TextField();

        TextField(const std::string &txt, const std::string &fnt, uint32_t txtStyle, uint32_t size, float thickness,
                  sf::Color normal, sf::Color hovered, sf::Color clicked, sf::Color border);

        void setScales(const sf::Vector2f parentSize)
        {
            scaleSize.x = (float) text.getCharacterSize() / parentSize.x;
            scaleSize.y = scaleSize.x;
            scalePosition = {(float) text.getPosition().x / parentSize.x, (float) text.getPosition().y / parentSize.y};
        }
        void update(const sf::Vector2f parentSize)
        {
            setCharSize(scaleSize.x * parentSize.x);
            setTextPosition( { scalePosition.x * parentSize.x, scalePosition.y * parentSize.y} );
        }

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(text); }
        void setAllProperties(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness,
                              sf::Color normal, sf::Color hovered, sf::Color clicked, sf::Color border);
        void setDefaultProperties();
        void setBorderThickness(const float th)     { borderThickness = th; text.setOutlineThickness(borderThickness); }
        void setFont(const std::string& fnt)        { font.loadFromFile( fnt); text.setFont(font);                     }
        void setText(const std::string& txt)        { text.setString(txt);                                                     }
        void setTextStyle(const uint32_t style)     { text.setStyle(style);                                                    }
        void setCurrentColor(const sf::Color clr)   { text.setFillColor(clr);                                            }

        void setCharSize(const uint32_t size) { charSize = size; text.setCharacterSize(charSize);                         }
        void setCurrentColorNormal()     { text.setFillColor(clrNormal);  }
        void setCurrentColorHovered()    { text.setFillColor(clrHovered); }
        void setCurrentColorClicked()    { text.setFillColor(clrClicked); }
        void setCurrentClrBorder()       { text.setOutlineColor(clrBorder); }

        void setClrTextNormal(const sf::Color clr)  { clrNormal  = clr; }
        void setClrHovered(const sf::Color clr)     { clrHovered = clr; }
        void setClrTextClicked(const sf::Color clr) { clrClicked = clr; }
        void setClrBorder(const sf::Color clr)      { clrBorder  = clr; }

        sf::Color getClrTextNormal()  { return clrNormal;  }
        sf::Color getClrHovered()     { return clrHovered; }
        sf::Color getClrTextClicked() { return clrClicked; }
        [[nodiscard]] uint32_t getCharSize()      { return charSize;  }
        sf::Vector2f getCenterPosition()          { return {text.getPosition().x + text.getGlobalBounds().width/2,
                                                            text.getPosition().y + text.getGlobalBounds().height / 2}; }

        sf::FloatRect getTextGetGlobal()          { return text.getGlobalBounds();                          }
        void setTextPosition(sf::Vector2f pos)    { text.setPosition(pos);                                  }
        void moveText(const sf::Vector2f pos)     { text.move(pos);                                   }

        ~TextField();
    };

    class Button : public DisplayObject
    {
    private:
        sf::Color  clrBgNormal, clrBgHovered, clrBgClicked, clrBorder;
        btnState   state;
        TextField* textfield;
        float     borderThickness;
        void (*onClick)(sf::RenderWindow& w){};
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    public:
        Button(const std::string& name, sf::Vector2f s, sf::Vector2f p);
        void eventsHandler(sf::Event& e, sf::RenderWindow& window);

        void setColors(const sf::Color normal, const sf::Color hovered,
                       const sf::Color clicked, const sf::Color border)
        { clrBgNormal = normal; clrBgClicked = clicked; clrBgHovered = hovered; clrBorder = border; }
        void setClrBgNormal(const sf::Color clr)    { clrBgNormal    = clr; }
        void setClrBgClicked(const sf::Color clr)   { clrBgClicked   = clr; }
        void setClrBgHovered(const sf::Color clr)   { clrBgHovered   = clr; }
        void setClrBorder(const sf::Color clr)      { clrBorder      = clr; }
        void setOnClick(void (*ptrFunc)(sf::RenderWindow& w)) { onClick = ptrFunc; }
        void setDefaultProperties();
        void setState(const btnState st)            { state = st;                                                                     }
        void setBorderThickness(const float th)     { borderThickness = th;  selfRect->setOutlineThickness(borderThickness); }
        [[nodiscard]] btnState getState()const      { return state;                                                                  }
        ~Button() override;
    };

    class MessageBox : public DisplayObject
    {
    private:
        sf::RenderWindow*      window;
        std::vector<sf::Text>  textFields;
        std::vector<Button>    buttons;
    public:
        MessageBox();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void eventsHandler(sf::Event& e, sf::RenderWindow& window);
        void addTextField(const sf::Text& txt) { textFields.push_back(txt);                     }
        void addButton(const Button& btn)      { buttons.push_back(btn);                        }
        void deleteText(const uint32_t i)   { textFields.erase(textFields.begin() + i); }
        void deleteButton(const uint32_t i) { buttons.erase(buttons.begin() + i);       }
        ~MessageBox() override;
    };

    class Menu : public DisplayObject
    {
    private:
        sf::RenderWindow*   window;
        sf::View*           view;
        sf::Texture         texture;
        std::vector<Button> buttons;
    public:
        TextField           *textField;
    public:
        Menu();
        Menu(sf::Vector2f s, sf::Vector2f p, sf::Color color, const std::string& title);
        Menu(sf::Vector2f s, sf::Vector2f p, const std::string& background, const std::string& title);

        void display() override;
        void eventsHandler();
        void addButton(const Button& btn)        { buttons.push_back(btn);                     }
        void deleteButton(const uint32_t i)      { buttons.erase(buttons.begin() + i); }

        ~Menu() override;
    };


    class Platform : public MovableObject
    {
    private:
    public:
        Platform(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp);
        void move() override;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void randomlyChangeLength();
        ~Platform() override;
    };

    class Player
    {
    private:
        Statistics statistics;
        Platform   *platform;
    public:
        Player();
        void randomlyChangeScore();
        void randomlyChangeLives();
        [[nodiscard]] Statistics getStatistics()const { return statistics; }
        void clearStatistics()   { statistics = Statistics(); }
        ~Player();
    };

    class Ball : public MovableObject
    {
    private:
    public:
        Ball(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp);
        void move() override;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        ~Ball() override;
    };

    class Balls
    {
    private:
        std::vector<Ball> balls;
    public:
        Balls();
        void addBall(const Ball& ball)     { balls.push_back(ball);                  }
        void deleteBalls(const uint32_t i) { balls.erase(balls.begin() + i); }
        void numBallsUp();
        ~Balls();
    };

    class Bonus
    {
    private:
        int bonusDescriptor;
    public:
        Bonus();
        void applyBonus();
        void setBonusDescriptor(const int n)        { bonusDescriptor = n;    }
        [[nodiscard]] int getBonusDescriptor()const { return bonusDescriptor; }
        ~Bonus();
    };

    class Block : public DisplayObject
    {
    private:
        uint16_t   numOfLives;
        Bonus*     bonus;
        blockState state;
    public:
        Block(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp, bool bonus);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        [[nodiscard]] uint16_t getLives()const      { return numOfLives;                   }
        [[nodiscard]] uint16_t isCrashed()const     { return state == blockState::CRASHED; }
        [[nodiscard]] blockState getState()const    { return  state;                       }
        [[nodiscard]] bool containsBonus()const     { return bonus;                        }
        void setLives(uint16_t lives) { numOfLives = lives;                  }
        void setBonus(Bonus bns)      { bonus = &bns;                        }
        void crashReaction();
        void collideReaction();
        ~Block() override;
    };

    class Players
    {
    private:
        std::vector<Player> players;
    public:
        Players();
        void addPlayer(const Player& player) { players.push_back(player);                  }
        void deletePlayer(const uint32_t i)  { players.erase(players.begin() + i); }
        ~Players();
    };

    class Blocks
    {
    private:
        std::vector<Block> blocks;
        float bonusPercentage;
    public:
        Blocks();
        void addBlock(const Block& block)   { blocks.push_back(block);                  }
        void deleteBlock(const uint32_t i)  { blocks.erase(blocks.begin() + i); }
        void  updateBonusPercentage(difficulty diff);
        [[nodiscard]] float getBonusPercentage()const { return bonusPercentage; }
        ~Blocks();
    };

    class History
    {
    private:
        std::fstream file;
        Statistics   data;
        Menu         wHistory;
    public:
        History();
        Menu& getMenu() { return wHistory; }
        void setData(Statistics st);
        void saveHistory();
        void importHistory();
        void clearHistory();
        ~History();
    };

    class Settings
    {
    private:
        std::fstream file;
        difficulty   diff;
        resolution   res;
        Menu         wSettings;
    public:
        Settings();
        Menu& getMenu() { return wSettings; }
        void setResolution(const resolution r) { res = r;     }
        void setDifficulty(const difficulty d) { diff = d;    }
        resolution getResolution()const        { return res;  }
        difficulty getDifficulty()const        { return diff; }
        void saveSettings();
        void setDefaultSettings();
        void importSettings();
        ~Settings();
    };

    class GameField : public DisplayObject
    {
    private:
        sf::RenderWindow* wField;
        Balls             balls;
        Blocks            blocks;
        MessageBox        messageBox;
        std::vector<DisplayObject *> objects;
    public:
        GameField(sf::Vector2f s, sf::Vector2f p);
        void checkCollisions() override;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update();
        void render();
        ~GameField() override;
    };

    class Game
    {
    private:
        Menu *startMenu,
                *restartMenu,
                *waitMenu,
                *endMenu;
        gameState  state;
        GameField  gameField;
        History    history;
        Statistics statistics;
        Settings   settings;
        Players    players;
    private:
        void initMenus();
    public:
        Game();
        void start();
        void setGameState(const gameState st)       { state = st;   }
        [[nodiscard]] gameState getGameState()const { return state; }
        ~Game();
    };
}

#endif //GAME_CLASSES_H

