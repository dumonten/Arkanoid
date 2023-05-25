#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#define NOMINMAX
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <string>
#include "GameDataStructs.hpp"
#include <random>


using namespace nlohmann;

namespace Arkanoid
{
    struct BonusTimer 
    {
        sf::Clock mC;
        float runTime;
        bool bPaused;

        BonusTimer() {
            bPaused = false;
            runTime = 0;
            mC.restart();
        }

        void Reset() {
            mC.restart();
            runTime = 0;
            bPaused = false;
        }

        void Start() {
            if (bPaused) {
                mC.restart();
            }
            bPaused = false;
        }

        void Pause() {
            if (!bPaused) {
                runTime += mC.getElapsedTime().asSeconds();
            }
            bPaused = true;
        }

        float GetElapsedSeconds() {
            if (!bPaused) {
                return runTime + mC.getElapsedTime().asSeconds();
            }
            return runTime;
        }
    };

    typedef std::vector<std::pair <std::pair<int, BonusDescriptor>, std::pair<float, BonusTimer*>>> BonusesTimers;

    class DisplayObject : public sf::Drawable
    {
    protected:
        sf::Vector2f position, size, scalePosition, scaleSize;
        sf::Vector2f s_parent; 
        bool         visible{ true };
        sf::Shape*   selfRect;
    protected:
        DisplayObject()
        {
            selfRect = nullptr;
            position = size = { 0, 0 };
        }
        //For rectangle
        DisplayObject(sf::Vector2f s, sf::Vector2f p)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
        }
        //For rectangle
        DisplayObject(sf::Vector2f s, sf::Vector2f p, sf::Vector2f s_parent)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            setScales(s_parent);
            this->s_parent = s_parent; 
        }
        //For circle
        DisplayObject(float radius, sf::Vector2f p)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
        }
        //For circle
        DisplayObject(float radius, sf::Vector2f p, sf::Vector2f s_parent)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
            setScales(s_parent);
            this->s_parent = s_parent; 
        }
    public:
        //For rectangle
        virtual void setSize(sf::Vector2f sz)
        {
            ((sf::RectangleShape*)selfRect)->setSize(size = sz);
        }
        //For circle
        virtual void setSize(float radius)
        {
            size = { radius, radius };
            ((sf::CircleShape*)selfRect)->setRadius(radius);
        }
        //Set scales and position: same for rectangle and circle
        virtual void setScales(const sf::Vector2f parentSize)
        {
            s_parent = parentSize; 
            scaleSize = { (float)size.x / parentSize.x, (float)size.y / parentSize.y };
            scalePosition = { (float)position.x / parentSize.x, (float)position.y / parentSize.y };
        }
        virtual void setPosition(sf::Vector2f ps)
        {
            selfRect->setPosition(position = ps);
        }
        virtual void setTexture(std::string path) 
        { 
            sf::Texture* texture = new sf::Texture; 
            if (!(*texture).loadFromFile(path))
                { std::cout << "error"; }
            ((sf::RectangleShape*)selfRect)->setTexture(texture, true); // texture is a sf::Texture
            //((sf::RectangleShape*)selfRect)->setTextureRect(rect);
        }

        virtual void setVisibility(bool visibility)
        {
            visible = visibility;
        }
        virtual void display(sf::RenderWindow* window, sf::View* view) {}
        virtual void hide()
        {
            visible = false;
        }
        

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const
        {
            target.draw(*selfRect); 
        }

        virtual sf::Vector2f  getSize()
        {
            return size;
        }
        virtual sf::Vector2f  getPosition()
        {
            return position;
        }
        virtual sf::FloatRect getGlobalBounds()
        {
            return selfRect->getGlobalBounds();
        }
        static  sf::Vector2f   getRelativeCenter(sf::FloatRect parentSize, sf::FloatRect childSize);
        virtual sf::Shape* getSelfRect()
        { 
            return selfRect;
        }
        virtual bool isVisible()
        {
            return visible;
        }
        virtual bool isCollideWith(DisplayObject *obj2)
        {
            sf::FloatRect commonRect = getCommonRect(obj2->getGlobalBounds());
            if (commonRect.width >= 0 && commonRect.height >= 0) return true; 
            return false; 
        }
        virtual sf::FloatRect getCommonRect(sf::FloatRect r2)
        {
            sf::FloatRect r1 = selfRect->getGlobalBounds(); 
            sf::FloatRect length;
            length.left = std::max(r1.left, r2.left);
            length.width = std::min(r1.left + r1.width, r2.left + r2.width) - length.left;
            length.top = std::max(r1.top, r2.top);
            length.height = std::min(r1.top + r1.height, r2.top + r2.height) - length.top;
            return length;
        }
        virtual ~DisplayObject()
        {
            if (!selfRect) 
                delete selfRect;
        }
    };

    struct doEvent
    {
        EventType event;
        std::vector<void* > *data;
    };

    class EventObject
    {
    protected:
        EventObject* ptrParent;
        std::queue<doEvent> eventsQueue;
    protected:
        EventObject() { ptrParent = nullptr; }
        EventObject(EventObject* ptr) { ptrParent = ptr; }

    public: 
        virtual void pushEvent(const EventType type)
        {
            eventsQueue.push({ type, NULL });
        }
        virtual void pushEvent(doEvent event)
        {
            eventsQueue.push(event);
        }
        virtual void setParent(EventObject* ptr) { ptrParent = ptr; }
        virtual void sendEvent(EventObject* obj, const EventType type) 
        { 
            obj->pushEvent({ type, NULL }); 
        }
        virtual void sendEvent(EventObject* obj, doEvent event)
        {
            obj->pushEvent(event);
        }
        virtual void clearQueue();
        //Take the first event from the queue and push it out
        virtual bool pollEvent(EventType& event);
        virtual bool pollEvent(doEvent& event);
        //Event handling is different for each descendant
    
        virtual void eventsHandler() {}
       
        virtual ~EventObject()
        {
            try 
            {
                clearQueue();
                ptrParent = NULL; 
            }
            catch (...)
            {}
        }
    };

    class MovableObject : public DisplayObject
    {
    protected:
        sf::Vector2f  speed;
    protected:
        MovableObject() : DisplayObject()
        {
            speed = { 0.0, 0.0 };
        }
        //For rectangle
        MovableObject(sf::Vector2f s, sf::Vector2f p)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            speed = { 0.0, 0.0 };
        }
        //For rectangle
        MovableObject(sf::Vector2f s, sf::Vector2f p, sf::Vector2f sp) : DisplayObject(s, p)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            speed = sp;
        }
        //For rectangle
        MovableObject(sf::Vector2f s, sf::Vector2f p, sf::Vector2f s_parent, sf::Vector2f sp) : DisplayObject(s, p, s_parent)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            setScales(s_parent);
            speed = sp;
        }
        //For circle
        MovableObject(float radius, sf::Vector2f p, sf::Vector2f sp) : DisplayObject(radius, p)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
            speed = sp;
        }
        //For circle
        MovableObject(float radius, sf::Vector2f p, sf::Vector2f s_parent, sf::Vector2f sp) : DisplayObject(radius, p, s_parent)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
            setScales(s_parent);
            speed = sp;
        }
    public:
        virtual void move()
        {
            selfRect->move(speed.x, 0);
            selfRect->move(0, speed.y);
        }
        virtual void move(sf::Vector2f spd)
        {
            selfRect->move(spd.x, 0);
            selfRect->move(0, spd.y);
        }

        virtual void setSpeed(sf::Vector2f spd)
        {
            speed = spd;
        }
        virtual sf::Vector2f getSpeed()
        {
            return speed;
        }

        virtual void changeSpeedDirectionX()
        {
            speed.x = -speed.x;
            selfRect->move((float)speed.x / 100, 0);
            selfRect->move(0, speed.y);
        }
        virtual void changeSpeedDirectionY()
        {
            speed.y = -speed.y;
            selfRect->move(speed.x, 0);
            selfRect->move(0, (float)speed.y / 100);
        }
        virtual void changeSpeedDirection()
        {
            changeSpeedDirectionY();
            changeSpeedDirectionX();
        }

        //Bonus 
        virtual void increaseSpeedBonus() {}
        virtual void decreaseSpeedBonus() {}
    };

    struct Collision
    {
        CollisionType type;
        CollisionDirection direction;
        MovableObject* dynamicObj;
        DisplayObject* staticObj;
    };

    class Statistics
    {
    public:
        std::string userName;
        uint16_t numOfLives;
        uint32_t score,
            numOfBonuses,
            numOfCrashedBlocks;
    public:
        Statistics()
        {  
            setDefault(); 
        }
        Statistics(Statistics& s)
        {
            userName = s.userName; 
            numOfLives = s.numOfLives;
            score = s.score; 
            numOfBonuses = s.numOfBonuses; 
            numOfCrashedBlocks = s.numOfCrashedBlocks; 
        }
        void copyingFrom(Statistics& s)
        {
            userName = s.userName;
            numOfLives = s.numOfLives;
            score = s.score;
            numOfBonuses = s.numOfBonuses;
            numOfCrashedBlocks = s.numOfCrashedBlocks;
        }
        void setDefault() 
        {
            userName = "Player_1";
            numOfLives = 3;
            score = 0;
            numOfBonuses = 0;
            numOfCrashedBlocks = 0;
        }
        std::string toString()
        {
            return  "User name: " + userName + "\n"
                "Num of lives: " + std::to_string(numOfLives) + "  |  " +
                "Score: " + std::to_string(score) + "  |  " +
                "Num of achieved bonuses: " + std::to_string(numOfBonuses) + "  |  " +
                "Num of crashed blocks: " + std::to_string(numOfCrashedBlocks);
        }
        ~Statistics() = default;
    };

    class TextField  : public sf::Drawable
    {
    public: 
        sf::Text     text;
    private:
        std::string  str;
        float        borderThickness;
        sf::Color    clrNormal, clrBorder;
        sf::Font     font;
        uint32_t     textStyle{}, charSize;
        sf::Vector2f scaleSize, scalePosition;
    public:
        TextField(); 
        TextField(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness, sf::Color normal, sf::Color border);

        void setScales(const sf::Vector2f parentSize)
        {
            scaleSize.x = (float)text.getCharacterSize() / parentSize.x;
            scaleSize.y = scaleSize.x;
            scalePosition = { (float)text.getPosition().x / parentSize.x, (float)text.getPosition().y / parentSize.y };
        }
        void update(const sf::Vector2f parentSize)
        {
            setCharSize(scaleSize.x * parentSize.x);
            setTextPosition({ scalePosition.x * parentSize.x, scalePosition.y * parentSize.y });
        }

        void setAllProperties(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness, sf::Color normal, sf::Color border);
        void setDefaultProperties();
       
        void setBorderThickness(const float th) 
        { 
            borderThickness = th; 
            text.setOutlineThickness(borderThickness); 
        }
        void setFont(const std::string fnt) 
        { 
            font.loadFromFile(fnt); 
            text.setFont(font); 
        }
        void setText(const std::string txt) 
        { 
            text.setString(str = txt); 
        }
        void setTextStyle(const uint32_t style)   
        { 
            text.setStyle(style); 
        }
        void setCharSize(const uint32_t size) 
        { 
            charSize = size; 
            text.setCharacterSize(charSize); 
        }

        void setClrTextNormal(const sf::Color clr) 
        { 
            clrNormal = clr;
            text.setFillColor(clrNormal);  
        }
        void setClrBorder(const sf::Color clr) 
        { 
            clrBorder = clr; 
            text.setOutlineColor(clrBorder);
        }
    
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            target.draw(text, states); 
        }

        sf::Vector2f getCenterPosition() 
        {
            return { text.getPosition().x + text.getGlobalBounds().width / 2,
                   text.getPosition().y + text.getGlobalBounds().height / 2 };
        }
        
        std::string getText() 
        { 
            return text.getString(); 
        }
        sf::FloatRect getTextGlobal() 
        {
            return text.getGlobalBounds(); 
        }
       
        void setTextPosition(sf::Vector2f pos) 
        { 
            text.setPosition(pos); 
        }
        void moveText(const sf::Vector2f pos) 
        { 
            text.move(pos); 
        }
        bool isEmpty() 
        { 
            return str.length() == 0 ? 0 : 1; 
        }

        ~TextField()
        {}
    };

    class Button : public DisplayObject, public EventObject
    {
    private:
        sf::Color  clrBgNormal, clrBgHovered, clrBgClicked, clrBorder;
        BtnState   state;
        float      borderThickness;
        EventType  event;
    public:
        TextField* textfield;
    public:
        Button(const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt);
        Button(EventObject* parent, const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update(sf::Event& e, sf::RenderWindow& window);

        void setColors(const sf::Color normal, const sf::Color hovered,
            const sf::Color clicked, const sf::Color border)
        {
            clrBgNormal = normal; clrBgClicked = clicked; clrBgHovered = hovered; clrBorder = border;
            selfRect->setFillColor(clrBgNormal);
            selfRect->setOutlineColor(clrBorder);
        }
        void setClrBgNormal(const sf::Color clr) { clrBgNormal = clr; selfRect->setFillColor(clrBgNormal); }
        void setClrBgClicked(const sf::Color clr) { clrBgClicked = clr; }
        void setClrBgHovered(const sf::Color clr) { clrBgHovered = clr; }
        void setClrBorder(const sf::Color clr) { clrBorder = clr; selfRect->setOutlineColor(clrBorder); }

        void setDefaultProperties();

        void reset()
        {
            state = BtnState::NORMAL;
            selfRect->setFillColor(clrBgNormal);
            selfRect->setOutlineColor(clrBorder);
        }

        void setTextSize(const uint32_t size)
        {
            textfield->setCharSize(size);
            textfield->setTextPosition(DisplayObject::getRelativeCenter(selfRect->getGlobalBounds(), textfield->getTextGlobal()));
            textfield->setScales(getSize());
        }
        void setText(std::string str)
        {
            textfield->setText(str);
            textfield->setTextPosition(DisplayObject::getRelativeCenter(selfRect->getGlobalBounds(), textfield->getTextGlobal()));
            textfield->setScales(size);
        }
        std::string getText() { return textfield->getText(); }
        void setState(const BtnState st) { state = st; }
        void setBorderThickness(const float th) { borderThickness = th;  selfRect->setOutlineThickness(borderThickness); }


        BtnState getState()const { return state; }
        ~Button();
    };

    class Messagebox : public DisplayObject, public EventObject
    {
    public:
        std::vector<Button*> menuItems;
        std::vector<TextField*>  textFields;

        Messagebox(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color);
        Messagebox(sf::Vector2f s, sf::Vector2f p, sf::Color color);

        void eventsHandler() override;
        void hide() override
        {
            DisplayObject::hide();
            reliefButtons();
        }
        void reliefButtons()
        {
            for (auto& button : menuItems)
            {
                button->reset();
            }
        }
        void addTextField(TextField* txt) { textFields.push_back(txt); }
        void addButton(Button* btn) { menuItems.push_back(btn); }
        void deleteText(const uint32_t i) { textFields.erase(textFields.begin() + i); }
        void deleteButton(const uint32_t i) { menuItems.erase(menuItems.begin() + i); }
        void draw(sf::RenderWindow* window, sf::View* view)const
        {
            window->draw(*selfRect); 
            for (auto& textField : textFields)
                window->draw(*textField);
            for (const auto& button : menuItems)
                window->draw(*button);
        }
        ~Messagebox();
    };

    class Menu : public DisplayObject, public EventObject
    {
    private:
        std::vector<Button*> menuItems;
        std::vector<TextField*>  textFields;
    public:
        Menu(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color);
        Menu(sf::Vector2f s, sf::Vector2f p, sf::Color color);

        void eventsHandler() override;
        void display(sf::RenderWindow* window, sf::View* view) override;
        void hide() override
        {
            DisplayObject::hide();
            reliefButtons();
        }
        void reliefButtons()
        {
            for (auto& button : menuItems)
            {
                button->reset();
            }
        }
        void addTextField(TextField* txt) { textFields.push_back(txt); }
        void addButton(Button* btn) { menuItems.push_back(btn); }
        void deleteText(const uint32_t i) { textFields.erase(textFields.begin() + i); }
        void deleteButton(const uint32_t i) { menuItems.erase(menuItems.begin() + i); }
        void draw(sf::RenderWindow* window, sf::View* view)const 
        {
            for (auto& textField : textFields)
                window->draw(*textField);
            for (const auto& button : menuItems)
                window->draw(*button);
        }
        ~Menu();
    };

    class StatusBar : public DisplayObject
    {
    private:
        Statistics* data;
        TextField*  textField;
    public:
        Button* menuButton;
    public:
        StatusBar(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, sf::Color c);
        void setData(Statistics* st) { data = st; }
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void update() { if (data) textField->setText(data->toString()); }
        ~StatusBar();
    };

    class Platform : public MovableObject, public EventObject
    {
    private: 
        sf::Vector2f initPosition; 
    public:
        Platform(EventObject* parent, sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        Platform(sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        void move()override
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (speed.x < 0) speed.x *= -1;
                selfRect->move(speed.x, 0);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (speed.x > 0) speed.x *= -1;
                selfRect->move(speed.x, 0);
            }
        }
        void eventsHandler() override;
        void reset()
        {
            setPosition(initPosition); 
        }
        ~Platform();
        
        void increasePlatformLengthBonus()
        {
            this->setSize({ (float)this->size.x * PLATFORM_CHANGE_LENGTH_VALUE.x, (float)this->size.y * PLATFORM_CHANGE_LENGTH_VALUE.y});
        }
        void decreasePlatformLengthBonus()
        {
            this->setSize({ (float)this->size.x / PLATFORM_CHANGE_LENGTH_VALUE.x, (float)this->size.y / PLATFORM_CHANGE_LENGTH_VALUE.y });
        }
        void increaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x * PLATFORM_CHANGE_SPEED_VALUE.x, (float)this->speed.y * PLATFORM_CHANGE_SPEED_VALUE.y });
        }
        void decreaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x / PLATFORM_CHANGE_SPEED_VALUE.x, (float)this->speed.y / PLATFORM_CHANGE_SPEED_VALUE.y });
        }
    };

    class Player
    {
    private:
        Statistics statistics; 
        Platform*  platform;
    public:
        Player();

        void setStatistics(Statistics& st)
        {
            statistics.copyingFrom(st); 
        }
        
        void setPlatform(Platform* pl) 
        {
            platform = pl; 
        }
        Statistics* getStatistics()
        {
            return &statistics; 
        }
        void clearStatistics() 
        {
            statistics.setDefault();
        }
        Platform* getPlatform() 
        {
            return platform; 
        }

        ~Player();
    };

    class Ball : public MovableObject, public EventObject
    {
    private:
        sf::Vector2f initPosition;
    public:
        Ball(EventObject* parent, sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c); 
        void reset()
        {
            setPosition(initPosition);
        }
        void eventsHandler() override; 
        void increaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x * BALL_CHANGE_SPEED_VALUE.x, (float)this->speed.y * BALL_CHANGE_SPEED_VALUE.y });
        }
        void decreaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x / BALL_CHANGE_SPEED_VALUE.x, (float)this->speed.y / BALL_CHANGE_SPEED_VALUE.y });
        }
        ~Ball();
    };
  
    class Balls 
    {
        friend class Proxy;
        friend class GameField;
    private:
        std::vector<Ball*> balls;

    public:
        Balls();
        void addBall(Ball* ball) { balls.push_back(ball); }
        void deleteBall(Ball* ball)
        {
            for (uint64_t i = 0; i < balls.size(); i++)
            {
                if (balls[i] == ball)
                {
                    balls.erase(balls.begin() + i);
                    break;
                }
            }
        }
        uint64_t numOfBalls()
        {
            return balls.size(); 
        }
        void save_to_json(json& j);
        void save_to_txt(std::fstream& file, std::string fileName);
        ~Balls();
    };
    
    class Settings
    {
        friend class Game; 
    private:
        static Difficulty diff;
        static resolution res;
    public:
        Settings();
        std::string to_string(const Difficulty diff) {
            switch (diff)
            {
            case HARD:
                return "HARD";
            case MEDIUM:
                return "MEDIUM";
            case LOW:
                return "LOW";
            }
        }

        void setResolution(const resolution r) { res = r; }
        void setDifficulty(const Difficulty d) { diff = d; }
        void setDefaultSettings(); 
        static resolution  getResolution() { return res; }
        static Difficulty  getDifficulty() { return diff; }
        static std::string diffToStr(Difficulty diff)
        {
            switch (diff)
            {
            case LOW:
                return "LOW";
            case MEDIUM:
                return "MEDIUM";
            case HARD:
                return "HARD";
            }
        }
        static std::string resToStr(resolution res)
        {
            return std::to_string(int(res.x)) + "x" + std::to_string(int(res.y));
        }
        static Difficulty  strToDiff(std::string str)
        {
            if (str == "LOW")
                return LOW;
            else if (str == "MEDIUM")
                return MEDIUM;
            else if (str == "HARD")
                return HARD;
        }
        static resolution  strToRes(std::string str)
        {
            std::replace(str.begin(), str.end(), 'x', ' ');
            std::stringstream ss(str);
            std::string temp;
            int num[2], i = 0;
            while (!ss.eof())
            {
                ss >> temp;
                if (std::stringstream(temp) >> num[i])
                    i++;
                temp = "";
            }
            return sf::Vector2f(num[0], num[1]);
        }
        void save_to_json(json& j);
        void save_to_txt(std::fstream& file, std::string fileName);
        ~Settings();
    };

    class Bonus : public MovableObject
    {
        friend class Proxy; 
    public: 
        int bonusScoreValue = 0;
    private:
        TextField* bonusScore = NULL;
        BonusDescriptor descriptor; 
    public:
        void move()override;
        void draw(sf::RenderTarget& target, sf::RenderStates states)const override; 
        Bonus(sf::Vector2f size, sf::Vector2f position) : MovableObject(size, position)
        {
            setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y });
            descriptor = BonusDescriptor::BONUS_PLATFORM_SPEED_UP;
        }
        Bonus(sf::Vector2f s_parent, sf::Vector2f p_parent, BonusDescriptor d);
        void setBonusScoreValue(int value) { bonusScoreValue = value; generateTextField(size, value); }
        void setBonusDescriptor(BonusDescriptor d) 
        { 
            std::string path = "";
            descriptor = d; 
            switch (descriptor)
            {
            case BONUS_PLATFORM_LENGTH_UP:
                path = "PlatformLengthUp.png";
                break;
            case BONUS_PLATFORM_LENGTH_DOWN:
                path = "PlatformLengthDown.png";
                break;
            case BONUS_PLATFORM_SPEED_DOWN:
                path = "PlatformSpeedDown.png";
                break;
            case BONUS_PLATFORM_SPEED_UP:
                path = "PlatformSpeedUp.png";
                break;
            case BONUS_BALL_SPEED_UP:
                path = "BallSpeedUp.png";
                break;
            case BONUS_BALL_SPEED_DOWN:
                path = "BallSpeedDown.png";
                break;
            default:
                break;
            }
            if (path != "") setTexture(path);
        }
        void generateTextField(sf::Vector2f blockSize, int score); 
        BonusDescriptor getBonusDescriptor()
        {
            return descriptor; 
        }
        ~Bonus() = default; 
    };
    
    class Block : public DisplayObject, public EventObject
    {
        friend class GameField; 
        friend class Blocks; 
        friend class Proxy; 
    public: 
        bool cantBreak = false;
    private: 
        uint16_t  numOfLives;
        std::vector<Bonus* > bonuses; 
        BlockState state;
        sf::Color  clrNotAttacked,
                   clrMidAttacked, 
                   clrMinAttacked, 
                   clrMaxAttacked;
    public:
        Block(EventObject* parent, sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st);
        Block(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st);
        void eventsHandler() override;
        void setColor(sf::Color color)
        {
            selfRect->setFillColor(clrNotAttacked = color);
        }
        void trigger();
        void InitDefaultBonus(std::mt19937_64& rng, std::uniform_real_distribution<double>& unif); 
        uint16_t getLives() 
        {
            return numOfLives; 
        }
        std::vector<Bonus* >& getBonuses() 
        {
            return bonuses; 
        }
        ~Block();
    };

    class Players : public EventObject 
    {
        friend class Proxy; 
        friend class Game; 
    private:
        std::vector<Player*> players;
    public:
        Players();
        void addPlayer(Player* player) { players.push_back(player); }
        void deletePlayer(Player* player) 
        { 
            for (uint64_t i = 0; i < players.size(); i++)
            {
                if (players[i] == player)
                {
                    players.erase(players.begin() + i);
                    break; 
                }
            }
        }
        uint64_t numOfPlayers()
        {
            return players.size(); 
        }
        Player* getPlayer(Platform* platform); 
        int getPlayerIndex(Platform* platform);
        void eventsHandler()override; 
        void save_to_json(json& j);
        void save_to_txt(std::fstream& file, std::string fileName);
        void clear()
        {
            for (auto& p : players)
                delete p;
            players.clear(); 
        }
        ~Players();
    };

    class Blocks
    {
        friend class Proxy;
        friend class GameField;
    private:
        std::vector<Block*> blocks;
    public:
        Blocks();
        std::vector<Block*>& getVector()
        {
            return blocks;
        }
        void  addBlock(Block* block) { blocks.push_back(block); }
        void  deleteBlock(const uint32_t i) { blocks.erase(blocks.begin() + i); }
        void  save_to_json(json& j);
        void  save_to_txt(std::fstream& file, std::string fileName);
        ~Blocks();
    };

    class GameField : public DisplayObject, public EventObject
    {
        friend class Proxy;
    public:
        static bool isFirstStart; 
        GameState  state;
        
        Balls             balls;
        Blocks            blocks; 
        std::vector<Messagebox*> mBoxes;
        StatusBar* statusBar;
        std::vector<DisplayObject*> displayObjects;
        std::vector<MovableObject*> movableObjects;
        BonusesTimers bonusesTimers; 
    private:
        void pauseAllBonusesTimers()
        {
            for (int i = 0, n = bonusesTimers.size(); i < n; i++)
                bonusesTimers[i].second.second->Pause();
        }
        void checkBonusTimers(); 
        uint64_t getNumberOfDisplayedBlocks(); 
        void collisionDetection();
        void collisionSolution(Collision collision);
        void initMessageBoxes(); 
        void deleteDisplayObject(DisplayObject* dobj)
        {
            for (uint64_t i = 0; i < displayObjects.size(); i++)
            {
                if (displayObjects[i] == dobj)
                {
                    displayObjects.erase(displayObjects.begin() + i);
                    break;
                }
            }
        }
        void deleteMovableObject(MovableObject* mobj)
        {
            for (uint64_t i = 0; i < movableObjects.size(); i++)
            {
                if (movableObjects[i] == mobj)
                {
                    movableObjects.erase(movableObjects.begin() + i);
                    break;
                }
            }
        }
    public:
        GameField(sf::Vector2f s, sf::Vector2f p, sf::Color color);
        GameField(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color);
        void display(sf::RenderWindow* window, sf::View* view) override;
        void update();
        void addNewBonus(int index, BonusDescriptor bd); 
        void eventsHandler()override;
        static CollisionType getCollisionType(MovableObject* dynamicObj, DisplayObject* staticObj)
        {
            Classes classOfDynamic = classesNames.at(std::string(typeid(*dynamicObj).name()));
            Classes classOfStatic = classesNames.at(std::string(typeid(*staticObj).name()));
            if (classOfDynamic == Classes::PLATFORM && classOfStatic == Classes::BONUS)
                return CollisionType::BONUS_COLLISION;
            if (classOfDynamic == Classes::BALL && classOfStatic == Classes::BLOCK)
                return CollisionType::BLOCK_COLLISION;
            if (classOfDynamic == Classes::BALL && classOfStatic == Classes::PLATFORM)
                return CollisionType::PLATFORM_COLLISION;
            if (classOfDynamic == Classes::BALL && classOfStatic == Classes::GAME_FIELD)
                return CollisionType::BALL_BG_COLLISION;
            if (classOfDynamic == Classes::BALL && classOfStatic == Classes::STATUS_BAR)
                return CollisionType::BALL_PANEL_COLLISION;
            if (classOfDynamic == Classes::BONUS && classOfStatic == Classes::GAME_FIELD)
                return CollisionType::BONUS_BG_COLLISION;
            if (classOfDynamic == Classes::PLATFORM && classOfStatic == Classes::GAME_FIELD)
                return CollisionType::PLATFORM_BG_COLLISION;
            return CollisionType::NONE_COLLISION;
        }
        void setGameState(const GameState st) { state = st; }
        void draw(sf::RenderWindow* window, sf::View* view)const
        {
            window->draw(*selfRect);
            for (auto& obj : displayObjects)
            {
                if (obj->isVisible())
                    window->draw(*obj);
            }
        }
        GameState getGameState()const { return state; }

        ~GameField();
    };

    class Game : public EventObject
    {
        friend class Proxy; 
    private:
        sf::RenderWindow  *window;
        sf::View          *view;
        sf::Clock         clock; 
        Menu              *startMenu,
                          *settingsMenu,
                          *waitMenu;
        GameField         *gameField;
        Players           players;

        Proxy      *history; 
        Settings     settings;
        sf::Vector2f centerPosition;
    private:

        void nullization(); 
        void bonusesSolution(std::vector<void*>* eventData);
        //the restart is based on the preponderance of pointers (i.e. it is similar to complete deletion and creation)
        void resetStartMenu(resolution res, Difficulty diff);
        void resetSettingsMenu(resolution res, Difficulty diff);
        void resetWaitMenu(resolution res, Difficulty diff);
        void resetGameField(resolution res, Difficulty diff);
        void resetMenus(resolution res, Difficulty diff);
        void init(); 
        void eventsHadler();
        void reset();
    public:
        Game();
        void start();
        ~Game();
    };

    class Proxy
    {
    private:
        std::fstream textFileGame;
        std::fstream jsonFileGame;
    private:
        bool pathThrough(std::fstream& file, std::string& buffer); 
        bool simpleRead(std::fstream& file, std::string& buffer);
        bool isFine(std::string& buffer);
        void clearJsonFile()
        {
            jsonFileGame.open(JSON_FILE_GAME_PATH, std::fstream::out | std::fstream::trunc);
            jsonFileGame.close();
        }
        void clearTextFile()
        {
            textFileGame.open(TEXT_FILE_GAME_PATH, std::fstream::out | std::fstream::trunc);
            textFileGame.close();
        }

    public:
        Proxy();
        
        void writeData(BonusesTimers bonusesTimers, Settings& settings, Players& players, Balls& balls, Blocks& blocks);
        void readData(Game* game);
       
        void to_json(BonusesTimers bonusesTimers,Settings& settings, Players& players, Balls& balls, Blocks& blocks);
        void from_json(Game* game);

        ~Proxy();
    };
}
#endif 
