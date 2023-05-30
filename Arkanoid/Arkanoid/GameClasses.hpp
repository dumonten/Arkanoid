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
    class Constants
    {
    public:
        static sf::Texture* TEXTURE_BONUS_PLATFORM_LENGTH_UP;
        static sf::Texture* TEXTURE_BONUS_PLATFORM_LENGTH_DOWN;
        static sf::Texture* TEXTURE_BONUS_PLATFORM_SPEED_DOWN;
        static sf::Texture* TEXTURE_BONUS_PLATFORM_SPEED_UP;
        static sf::Texture* TEXTURE_BONUS_BALL_SPEED_UP;
        static sf::Texture* TEXTURE_BONUS_BALL_SPEED_DOWN;
        static sf::Font* FONT_HEADER;
        static sf::Font* FONT_BODY;
    public:
        static void defineConstants();
    };

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

    class SaveloadObject
    {
    public:
        virtual std::string to_string() = 0;
        
        virtual json to_json() = 0;
        
        virtual SaveloadObject* from_string(std::stringstream& data) = 0;
        
        virtual SaveloadObject* from_json(json& data) = 0;
    };
    
    class DisplayObject : public sf::Drawable, public SaveloadObject
    {
    protected:
        sf::Vector2f position, size, scalePosition, scaleSize;
        sf::Vector2f initPosition;
        sf::Vector2f s_parent;
        bool         visibility{ true };
        sf::Shape* selfRect;
    protected:
        DisplayObject()
        {
            selfRect = nullptr;
            position = size = initPosition = { 0, 0 };
        }
        
        //For rectangle
        DisplayObject(sf::Vector2f s, sf::Vector2f p)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            initPosition = position; 
        }
        
        //For rectangle
        DisplayObject(sf::Vector2f s, sf::Vector2f p, sf::Vector2f s_parent)
        {
            selfRect = new sf::RectangleShape(size = s);
            selfRect->setPosition(position = p);
            setScales(s_parent);
            this->s_parent = s_parent;
            initPosition = position;
        }
        
        //For circle
        DisplayObject(float radius, sf::Vector2f p)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
            initPosition = position;
        }
        
        //For circle
        DisplayObject(float radius, sf::Vector2f p, sf::Vector2f s_parent)
        {
            selfRect = new sf::CircleShape(radius);
            size = { radius, radius };
            selfRect->setPosition(position = p);
            setScales(s_parent);
            this->s_parent = s_parent;
            initPosition = position;
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
        
        virtual void setTexture(sf::Texture& texture)
        {
            ((sf::RectangleShape*)selfRect)->setTexture(&texture, true);
        }

        virtual void setVisibility(bool visibility)
        {
            this->visibility = visibility;
        }
        
        virtual void display(sf::RenderWindow* window, sf::View* view) {}
        
        virtual void hide()
        {
            visibility = false;
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
        
        static  sf::Vector2f  getRelativeCenter(sf::FloatRect parentSize, sf::FloatRect childSize);
        
        virtual sf::Shape* getSelfRect()
        {
            return selfRect;
        }
        
        virtual bool isVisible()
        {
            return visibility;
        }
        
        virtual bool isCollideWith(DisplayObject* obj2)
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
        
        virtual void setInitPosition(sf::Vector2f initPos)
        {
            initPosition = initPos; 
        }
        
        virtual void resetToInitPosition()
        {
            setPosition(initPosition);
        }

        std::string to_string() override
        {
            return ""; 
        }

        json to_json() override
        {
            json j = {}; 
            return j; 
        }

        SaveloadObject* from_string(std::stringstream& data) override
        {
            return nullptr;
        }

        SaveloadObject* from_json(json& data) override
        {
            return nullptr; 
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
        std::vector<void* >* data;
    };

    class EventObject
    {
    protected:
        EventObject* ptrParent;
        std::queue<doEvent> eventsQueue;
    protected:
        EventObject() {
            ptrParent = nullptr;
        }
        EventObject(EventObject* ptr) {
            ptrParent = ptr;
        }

    public:
        virtual void pushEvent(const EventType type)
        {
            eventsQueue.push({ type, NULL });
        }
        virtual void pushEvent(doEvent event)
        {
            eventsQueue.push(event);
        }
        virtual void setParent(EventObject* ptr) {
            ptrParent = ptr;
        }
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
            {
            }
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
            initPosition = position;
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
            setPosition(selfRect->getPosition()); 
        }
        
        virtual void move(sf::Vector2f spd)
        {
            selfRect->move(spd.x, 0);
            selfRect->move(0, spd.y);
            setPosition(selfRect->getPosition());
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

    class Settings : public SaveloadObject
    {
    private:
        static Difficulty diff;
        static resolution res;
    public:
        Settings();
    
        void setResolution(const resolution r) {
            res = r;
        }
        
        void setDifficulty(const Difficulty d) {
            diff = d;
        }
        
        void setDefaultSettings();
        
        static resolution  getResolution() {
            return res;
        }
        
        static Difficulty  getDifficulty() {
            return diff;
        }
        
        static std::string diffToStr(Difficulty diff)
        {
            switch (diff)
            {
            case IMPOSSIBLE:
                return "IMPOSSIBLE";
            case HARD:
                return "HARD";
            case MEDIUM:
                return "MEDIUM";
            case LOW:
                return "LOW";
            case KID:
                return "KID";
            }
            return "";
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
            else if (str == "IMPOSSIBLE")
                return IMPOSSIBLE;
            else
                return KID;
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
      
        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Settings();
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
                "Num of lives: " + std::to_string(numOfLives) + "    " +
                "Score: " + std::to_string(score) + "    " +
                "Num of achieved bonuses: " + std::to_string(numOfBonuses) + "    " +
                "Num of crashed blocks: " + std::to_string(numOfCrashedBlocks) + "    " + 
                "Difficulty:  " + Settings::diffToStr(Settings::getDifficulty());
        }
    };

    class TextField : public sf::Drawable
    {
    public:
        sf::Text text;
        std::string str;
        float borderThickness;
        sf::Color clrNormal, clrBorder;
        sf::Font font;
        uint32_t textStyle{}, charSize;
        sf::Vector2f scaleSize, scalePosition;
    public:
        TextField();

        TextField(const std::string& txt, sf::Font& fnt, uint32_t txtStyle, uint32_t size, float thickness, sf::Color normal, sf::Color border);

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

        void setAllProperties(const std::string& txt, const sf::Font& fnt, uint32_t txtStyle, uint32_t size, float thickness, sf::Color normal, sf::Color border);

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
            return {
                text.getPosition().x + text.getGlobalBounds().width / 2,
                text.getPosition().y + text.getGlobalBounds().height / 2
            };

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

        bool isEmpty()
        {
            return str.length() == 0 ? 0 : 1;
        }
    };

    class Button : public DisplayObject, public EventObject
    {
    public:
        sf::Color  clrBgNormal, clrBgHovered, clrBgClicked, clrBorder;
        float      borderThickness;
        BtnState   state;
        EventType  event;
        TextField* textfield;
    public:
        Button(const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt);
        
        Button(EventObject* parent, const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        
        void update(sf::Event& e, sf::RenderWindow& window);

        void setColors(const sf::Color normal, const sf::Color hovered,
            const sf::Color clicked, const sf::Color border)
        {
            clrBgNormal = normal;
            clrBgClicked = clicked;
            clrBgHovered = hovered;
            clrBorder = border;
            selfRect->setFillColor(clrBgNormal);
            selfRect->setOutlineColor(clrBorder);
        }

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
        
        std::string getText() { 
            return textfield->getText(); 
        }

        void setState(const BtnState st) {
            state = st;
        }

        void setBorderThickness(const float th) {
            borderThickness = th;
            selfRect->setOutlineThickness(borderThickness);
        }

        ~Button();
    };

    class Messagebox : public DisplayObject, public EventObject
    {
    public:
        std::vector<Button*> messageboxItems;
        std::vector<TextField*> textFields;
    public: 
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
            for (auto& button : messageboxItems)
            {
                button->reset();
            }
        }

        void addTextField(TextField* txt) {
            textFields.push_back(txt);
        }

        void addButton(Button* btn) {
            messageboxItems.push_back(btn);
        }

        void deleteText(const uint32_t i) {
            textFields.erase(textFields.begin() + i);
        }

        void deleteButton(const uint32_t i) {
            messageboxItems.erase(messageboxItems.begin() + i);
        }

        void draw(sf::RenderWindow* window, sf::View* view)const
        {
            window->draw(*selfRect);
            for (auto& textField : textFields)
                window->draw(*textField);
            for (const auto& button : messageboxItems)
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
        
        void addTextField(TextField* txt) {
            textFields.push_back(txt);
        }
        
        void addButton(Button* btn) {
            menuItems.push_back(btn);
        }
        
        void deleteText(const uint32_t i) {
            textFields.erase(textFields.begin() + i);
        }
        
        void deleteButton(const uint32_t i) {
            menuItems.erase(menuItems.begin() + i);
        }
        
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
    public: 
        Statistics* data;
        TextField* textField; 
        Button* menuButton;
    public:
        StatusBar(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, sf::Color c);
        
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        
        void update() {
            if (data) textField->setText(data->toString());
        }
        
        ~StatusBar();
    };

    class Platform : public MovableObject, public EventObject
    {
    public:
        Platform()
        {}

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
            setPosition(selfRect->getPosition());
        }
        
        void eventsHandler() override;
        
        void increasePlatformLengthBonus()
        {
            this->setSize({ (float)this->size.x * PLATFORM_CHANGE_LENGTH_VALUE.x, (float)this->size.y * PLATFORM_CHANGE_LENGTH_VALUE.y });
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
        
        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Platform();
    };

    class Player : public SaveloadObject
    {
    public:
        Statistics statistics;
        Platform* platform;
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

        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Player();
    };

    class Ball : public MovableObject, public EventObject
    {
    public:
        Ball()
        {}

        Ball(EventObject* parent, sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        
        Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp);
        
        Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c);
        
        void eventsHandler() override;
        
        void increaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x * BALL_CHANGE_SPEED_VALUE.x, (float)this->speed.y * BALL_CHANGE_SPEED_VALUE.y });
        }
        
        void decreaseSpeedBonus() override
        {
            this->setSpeed({ (float)this->speed.x / BALL_CHANGE_SPEED_VALUE.x, (float)this->speed.y / BALL_CHANGE_SPEED_VALUE.y });
        }
        
        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Ball();
    };

    class Balls : public SaveloadObject
    {
    public:
        std::vector<Ball*> balls;
    public:
        Balls();
        
        void addBall(Ball* ball) {
            balls.push_back(ball);
        }
        
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
        
        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Balls();
    };


    class Bonus : public MovableObject
    {
    public:
        int bonusScoreValue = 0;
        TextField* textFieldBonusScoreFall = NULL;
        BonusDescriptor descriptor;
    public:        
        Bonus(sf::Vector2f size, sf::Vector2f position) : MovableObject(size, position)
        {
            setVisibility(false); 
            setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y });
            descriptor = BonusDescriptor::BONUS_PLATFORM_SPEED_UP;
        }
        
        Bonus(sf::Vector2f s_parent, sf::Vector2f p_parent, BonusDescriptor d);
        
        void move()override;

        void draw(sf::RenderTarget& target, sf::RenderStates states)const override;

        void setBonusScoreValue(int value) {
            generateTextField(size, bonusScoreValue = value);
        }
        
        void setBonusDescriptor(BonusDescriptor d)
        {
            descriptor = d;
            switch (descriptor)
            {
            case BONUS_PLATFORM_LENGTH_UP:
                setTexture(*Constants::TEXTURE_BONUS_PLATFORM_LENGTH_UP);
                break;
            case BONUS_PLATFORM_LENGTH_DOWN:
                setTexture(*Constants::TEXTURE_BONUS_PLATFORM_LENGTH_DOWN);
                break;
            case BONUS_PLATFORM_SPEED_DOWN:
                setTexture(*Constants::TEXTURE_BONUS_PLATFORM_SPEED_DOWN);
                break;
            case BONUS_PLATFORM_SPEED_UP:
                setTexture(*Constants::TEXTURE_BONUS_PLATFORM_SPEED_UP);
                break;
            case BONUS_BALL_SPEED_UP:
                setTexture(*Constants::TEXTURE_BONUS_BALL_SPEED_UP);
                break;
            case BONUS_BALL_SPEED_DOWN:
                setTexture(*Constants::TEXTURE_BONUS_BALL_SPEED_DOWN);
                break;
            default:
                selfRect->setFillColor(ColorCustomPink);
                break;
            }
            if (descriptor == BonusDescriptor::BONUS_SCORE_UP)
                setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y - 4 });
            else
                setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y });
        }

        void generateTextField(sf::Vector2f blockSize, int score);

        ~Bonus() = default;
    };

    class Block : public DisplayObject, public EventObject
    {
    public:
        BlockState state;
        bool cantBreak = false;
        uint16_t  numOfLives;
        std::vector<Bonus* > bonuses;
        sf::Color clrNormal, clrMidAttacked, clrMinAttacked, clrMaxAttacked; 
    public:
        Block()
        {}
        
        Block(EventObject* parent, sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st);
        
        Block(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st);
        
        void eventsHandler() override;
        
        void setColor(sf::Color color)
        {
            selfRect->setFillColor(clrNormal = color);
        }
        
        void trigger();
        
        void initBonuses(std::mt19937_64& rng, std::uniform_real_distribution<double>& unif);
        
        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Block();
    };

    class Players : public EventObject, public SaveloadObject
    {
    public:
        std::vector<Player*> players;
    public:
        Players();
        
        void addPlayer(Player* player) {
            players.push_back(player);
        }
        
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
        
        Player* getPlayer(Platform* platform);
        
        int getPlayerIndex(Platform* platform);
        
        void eventsHandler()override;
        
        void clear()
        {
            for (auto& p : players) delete p;
            players.clear();
        }

        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;
        
        ~Players();
    };

    class Blocks : public SaveloadObject
    {
    public:
        std::vector<Block*> blocks;
    public:
        Blocks();
        
        void  addBlock(Block* block) {
            blocks.push_back(block);
        }
        
        void  deleteBlock(const uint32_t i) {
            blocks.erase(blocks.begin() + i);
        }
        
        uint64_t getNumOfRemainingBlocks() {
            uint64_t nRemainingBlocks = 0;
            for (auto& b : blocks)
            {
                if (b->isVisible() && b->cantBreak == false)
                    nRemainingBlocks++;
            }
            return nRemainingBlocks;
        }

        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~Blocks();
    };

    class GameField : public DisplayObject, public EventObject
    {
    public:
        GameState state;
        Balls balls;
        Blocks blocks;
        StatusBar* statusBar;
        BonusesTimers bonusesTimers;
        std::vector<Messagebox*> mBoxes;
        std::vector<DisplayObject*> displayObjects;
        std::vector<MovableObject*> movableObjects;
    private:
        void pauseAllBonusesTimers()
        {
            for (int i = 0, n = bonusesTimers.size(); i < n; i++)
                bonusesTimers[i].second.second->Pause();
        }

        void checkBonusTimers();

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

        void draw(sf::RenderWindow* window, sf::View* view) const
        {
            window->draw(*selfRect);
            for (auto& obj : displayObjects)
            {
                if (obj->isVisible())
                    window->draw(*obj);
            }
        }

        void update();

        void addNewBonus(int index, BonusDescriptor bd);

        void eventsHandler() override;

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

        std::string to_string() override;

        json to_json() override;

        SaveloadObject* from_string(std::stringstream& data) override;

        SaveloadObject* from_json(json& data) override;

        ~GameField();
    };

    class Proxy {
    public:
        static bool pathThrough(std::stringstream& ss, std::string& buffer)
        {
            ss >> buffer;
            while (!ss.eof() && !isFine(buffer))
                ss >> buffer;

            ss >> buffer;
            while (!ss.eof() && !isFine(buffer))
                ss >> buffer;
            return true;
        }

        static bool simpleRead(std::stringstream& ss, std::string& buffer)
        {
            ss >> buffer;
            while (!ss && !isFine(buffer))
                ss >> buffer;
            return true;
        }

        static bool isFine(std::string& buffer)
        {
            if (buffer.find('\n') == -1 && buffer.find('\t') == -1)
                return true;
            return false;
        }

        std::string to_string(std::vector <SaveloadObject*>& objectsToSave);
        std::string to_json_string(std::vector <SaveloadObject*>& objectsToSave);
        void from_string(std::vector <SaveloadObject*>& objectsToLoad, std::stringstream& data);
        void from_json(std::vector <SaveloadObject*>& objectsToLoad, json& data);
    };

    class Game : public EventObject
    {
        friend class Proxy;
    private:
        std::vector<SaveloadObject*> objectsToSave;
        
        sf::RenderWindow* window;
        sf::View* view;
        sf::Clock clock;
        Menu* startMenu, * settingsMenu, * waitMenu;
        GameField* gameField;
        Players players;
        Proxy* proxy;
        Settings settings;
    private:
        void init();
        
        void nullization();
        
        void reset();

        void reInitObjectsToSave(); 

        void load();

        void load_json();
        
        void save();
        
        void save_json();

        void bonusesSolution(std::vector<void*>* eventData);
        
        void eventsHadler();

        void resetStartMenu(resolution res, Difficulty diff);
        
        void resetSettingsMenu(resolution res, Difficulty diff);
        
        void resetWaitMenu(resolution res, Difficulty diff);
        
        void resetGameField(resolution res, Difficulty diff);
        
        void resetMenus(resolution res, Difficulty diff);
    public:
        Game();
        
        void start();
        
        ~Game();
    };
}

#endif
