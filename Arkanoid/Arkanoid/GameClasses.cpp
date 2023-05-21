#include "GameClasses.hpp"

std::mt19937_64 rng;
std::uniform_real_distribution<double> unif(0, 100);
std::vector<sf::Clock> clocks;
/*---------------------------------------------------------------------/*---------------------------------------------------------------------MovableObject*/
/*---------------------------------------------------------------------/*---------------------------------------------------------------------DisplayObject*/
sf::Vector2f Arkanoid::DisplayObject::getRelativeCenter(sf::FloatRect parentSize, sf::FloatRect childSize)
{
    return { parentSize.left + parentSize.width / 2 - childSize.width / 2,parentSize.top + parentSize.height / 2 - childSize.height / 2 };
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------EventObject*/
bool Arkanoid::EventObject::pollEvent(EventType& event)
{
    if (!eventsQueue.empty())
    {
        event = eventsQueue.front().event;
        eventsQueue.pop();
        return true;
    }
    return false;
}

bool Arkanoid::EventObject::pollEvent(doEvent& event)
{
    if (!eventsQueue.empty())
    {
        event = eventsQueue.front();
        eventsQueue.pop();
        return true;
    }
    return false;
}

void Arkanoid::EventObject::clearQueue()
{
    std::queue<doEvent > empty;
    std::swap(eventsQueue, empty);
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------TextField*/
Arkanoid::TextField::TextField(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness,
    sf::Color normal, sf::Color border)
{
    font.loadFromFile(fnt); text.setFont(font);
    text.setString(str = txt);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal = normal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder = border);
}

Arkanoid::TextField::TextField()
{
    setDefaultProperties();
}

void Arkanoid::TextField::setDefaultProperties()
{
    clrNormal = sf::Color::White;
    clrBorder = sf::Color::Black;
    font.loadFromFile(totalProjectPath + "/Fonts/simple.ttf");
    borderThickness = 0;
    text.setString(str = "Click me!");
    text.setFont(font);
    text.setStyle(textStyle = sf::Text::Bold);
    text.setCharacterSize(charSize = 10);
    text.setFillColor(clrNormal);
}

void Arkanoid::TextField::setAllProperties(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness,
    sf::Color normal, sf::Color border)
{
    clrNormal = normal;
    clrBorder = border;
    text.setString(str = txt);
    font.loadFromFile(fnt); text.setFont(font);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder);
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Button*/
Arkanoid::Button::Button(const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : DisplayObject(s, p)
{
    setDefaultProperties();
    selfRect->setOutlineThickness(borderThickness = 0.02f * s.x);
    textfield->setCharSize(s.x * 0.08f);
    textfield->setText(name);
    textfield->setTextPosition(DisplayObject::getRelativeCenter(selfRect->getGlobalBounds(), textfield->getTextGlobal()));
    textfield->setScales(s);
    event = evnt;
}

Arkanoid::Button::Button(EventObject* parent, const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : Button(name, s, p, evnt)
{
    ptrParent = parent;
}

void Arkanoid::Button::setDefaultProperties()
{
    clrBgNormal = ColorCustomGreen;
    clrBgClicked = ColorCustomGreen;
    clrBgHovered = ColorCustomDarkGreen;
    clrBorder = sf::Color::Yellow;
    selfRect->setFillColor(clrBgNormal);
    selfRect->setOutlineColor(clrBorder);
    state = BtnState::NORMAL;
    textfield = new TextField();
    textfield->setTextStyle(sf::Text::Bold);
}

void Arkanoid::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*selfRect, states);
    textfield->update(size);
    if (textfield->isEmpty() != 0)
        target.draw(*textfield, states);
}

void Arkanoid::Button::update(sf::Event& e, sf::RenderWindow& window)
{
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(window);
    bool mouseInButton = mousePosition.x >= selfRect->getPosition().x
        && mousePosition.x <= selfRect->getPosition().x + selfRect->getGlobalBounds().width
        && mousePosition.y >= selfRect->getPosition().y
        && mousePosition.y <= selfRect->getPosition().y + selfRect->getGlobalBounds().height;

    if (mouseInButton)
    {
        state = BtnState::HOVERED;
    }
    else
        state = BtnState::NORMAL;
    if (e.type == sf::Event::MouseButtonReleased)
    {
        switch (e.mouseButton.button)
        {
        case sf::Mouse::Left:
            if (mouseInButton)
                state = BtnState::PRESSED;
            else
                state = BtnState::NORMAL;
            break;
        default:
            break;
        }
    }
    switch (state)
    {
    case BtnState::NORMAL:
        selfRect->setFillColor(clrBgNormal);
        selfRect->setOutlineColor(clrBorder);
        break;
    case BtnState::HOVERED:
        selfRect->setFillColor(clrBgHovered);
        selfRect->setOutlineColor(clrBorder);
        break;
    case BtnState::PRESSED:
        selfRect->setFillColor(clrBgClicked);
        selfRect->setOutlineColor(clrBorder);
        sendEvent(ptrParent, event);
        break;
    }
}

Arkanoid::Button::~Button()
{
    delete textfield;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Menu*/
Arkanoid::Menu::Menu(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
}

Arkanoid::Menu::Menu(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : Menu(s, p, color)
{
    ptrParent = parent;
}

void Arkanoid::Menu::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case RES_CHANGE:
        {
            resolution res = Settings::strToRes(menuItems[0]->getText());
            int offset = (std::find(mResolution.begin(), mResolution.end(), res) - mResolution.begin() + 1) % mResolution.size();
            menuItems[0]->setText(Settings::resToStr(mResolution[offset]));
            reliefButtons();
            break;
        }
        case DIFF_CHANGE:
        {
            Difficulty diff = Difficulty((Settings::strToDiff(menuItems[1]->getText()) + 1) % Difficulty::ediffSize);
            menuItems[1]->setText(Settings::diffToStr(diff));
            reliefButtons();
            break;
        }
        case SAVE_SETTINGS:
        {
            static resolution res;
            res = Settings::strToRes(menuItems[0]->getText());
            static Difficulty diff;
            diff = Difficulty(Settings::strToDiff(menuItems[1]->getText()));
            std::vector<void*>* data = new std::vector<void*>;
            data->push_back(&res);
            data->push_back(&diff);
            sendEvent(ptrParent, { SAVE_SETTINGS, data });
            reliefButtons();
            break;
        }
        case LOAD_THE_GAME:
        case RESTART_OPEN_GAME_FIELD:
        case OPEN_SETTINGS:
        case OPEN_GAME_FIELD:
        case QUIT:
        case OPEN_START_MENU:
            clearQueue();
            sendEvent(ptrParent, event.event);
            hide();
            break;
        case SAVE_GAME:
            sendEvent(ptrParent, event.event);
            hide();
            break;
        default:
            break;
        }
    }
}

void Arkanoid::Menu::display(sf::RenderWindow* window, sf::View* view)
{
    sf::Event event{};
    while (window->pollEvent(event))
    {
        for (auto& button : menuItems)
            button->update(event, *window);  //Change button colors depending on the event
        if (eventsQueue.size() != 0) //Urgent transition to personal event processing
            break;
        switch (event.type)
        {
        case sf::Event::Closed:
            window->close();
            break;
        default:
            break;
        }
    }
    eventsHandler(); //Processing the queue of events from other objects, in particular, from the button
    window->setView(*view);
    window->clear();
    //window->draw(*selfRect);
}

Arkanoid::Menu::~Menu()
{
    for (auto& t : textFields)
    {
        delete t;
    }
    for (auto& m : menuItems)
    {
        delete m;
    }
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Messagebox*/
Arkanoid::Messagebox::Messagebox(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
    window = NULL;
}

Arkanoid::Messagebox::Messagebox(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : Messagebox(s, p, color)
{
    ptrParent = parent;
}

void Arkanoid::Messagebox::eventsHandler()
{
}

void Arkanoid::Messagebox::display()
{
    sf::Vector2f res = Settings::getResolution();
    position = DisplayObject::getRelativeCenter({ 0, 0, res.x, res.y }, { 0, 0, size.x , size.y });
    window = new sf::RenderWindow(sf::VideoMode(size.x, size.y), "Arkanoid", sf::Style::Titlebar | sf::Style::Close);
    window->setPosition((sf::Vector2i)position);
    sf::View* view = new sf::View(window->getDefaultView());

    window->setVisible(true);
    window->setVerticalSyncEnabled(true);
    window->setFramerateLimit(60);

    while (window->isOpen())
    {
        sf::Event event{};
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window->close();
                break;
            default:
                break;
            }
        }
        eventsHandler(); //Processing the queue of events from other objects, in particular, from the button

        window->setView(*view);
        window->clear();
        window->draw(*selfRect);
        for (auto& textField : textFields)
            window->draw(*textField);
        window->display();
    }
    window->setVisible(false);
    setVisibility(false);
}

Arkanoid::Messagebox::~Messagebox()
{
    for (auto& textField : textFields)
        delete textField;
    delete window;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------StatusBar*/
Arkanoid::StatusBar::StatusBar(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, sf::Color c) : DisplayObject(s, p, s_parent)
{
    selfRect->setFillColor(c);

    textField = new TextField;

    textField->setAllProperties("", totalProjectPath + "/Fonts/header.otf",
        sf::Text::Style::Regular, STATUS_BAR_BUTTON_TEXT_FONT_SIZE * s_parent.y, 1,
        sf::Color::White, ColorCustomMidPurpleBlueUltraDark);
    textField->setTextPosition({ STATUS_BAR_TEXT_OFFSET_LEFT * s_parent.x, STATUS_BAR_TEXT_OFFSET_TOP * s_parent.y });
    textField->setScales(s);

    data = NULL;
    menuButton = NULL;
}

void Arkanoid::StatusBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*selfRect, states);
    target.draw(*menuButton, states);
    target.draw(*textField, states);
}

Arkanoid::StatusBar::~StatusBar()
{
    if (_CrtIsValidHeapPointer(data))
        data = NULL;
    if (_CrtIsValidHeapPointer(textField))
        delete textField;
    if (_CrtIsValidHeapPointer(menuButton))
        delete menuButton;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Platform*/
Arkanoid::Platform::Platform(sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : MovableObject(s, p, s_parent, sp)
{
    selfRect->setFillColor(c);
    initPosition = { PLATFORM_WIDTH_INIT_OFFSET * s_parent.x,PLATFORM_HEIGHT_INIT_OFFSET * s_parent.y };
}

Arkanoid::Platform::Platform(EventObject* parent, sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : Platform(s_parent, s, p, c, sp)
{
    ptrParent = parent;
}

void Arkanoid::Platform::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case BG_COLLISION:
        {
            sf::FloatRect fr = *((sf::FloatRect*)event.data->at(0));
            CollisionDirection cd = *((CollisionDirection*)event.data->at(1));

            sf::FloatRect commonRect = getCommonRect(fr);
            if (cd == CollisionDirection::HORIZONTAL_COLLISION_DIRECTION)
            {
                float x = size.x - commonRect.width;
                if (speed.x > 0)
                    MovableObject::move({ -x, 0 });
                else
                    MovableObject::move({ x, 0 });
            }
            break; 
        }
        default:
            break;
        }
    }
}

Arkanoid::Platform::~Platform() = default;

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Block*/
Arkanoid::Block::Block(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st)
    : DisplayObject(s, p, s_parent)
{
    clrNotAttacked = ColorCustomDarkGreen;
    clrMidAttacked = ColorCustomOrange;
    clrMinAttacked = ColorCustomYellow;
    clrMaxAttacked = ColorCustomRed;

    state = st;
    selfRect->setOutlineThickness(1);
    selfRect->setOutlineColor(sf::Color::White);

    switch (state)
    {
    case NOT_ATTACKED:
        numOfLives = 1;
        selfRect->setFillColor(clrNotAttacked);
        break;
    case CRASHED:
        numOfLives = 0;
        selfRect->setFillColor(clrMaxAttacked);
        break;
    case MAX_ATTACKED:
        numOfLives = 0;
        selfRect->setFillColor(clrMaxAttacked);
        break;
    default:
        numOfLives = 1;
        selfRect->setFillColor(clrNotAttacked);
        break;
    }
}

Arkanoid::Block::Block(EventObject* parent, sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st) : Block(s_parent, s, p, st)
{
    ptrParent = parent;
}

void Arkanoid::Block::trigger()
{
    numOfLives--;
    switch (numOfLives)
    {
    case 0:
        state == BlockState::CRASHED;
        setVisibility(false);
        break;
    case 1:
        state == BlockState::MAX_ATTACKED;
        selfRect->setFillColor(clrMaxAttacked);
        break;
    case 2:
        state == BlockState::MID_ATTACKED;
        selfRect->setFillColor(clrMidAttacked);
        break;
    case 3:
        state == BlockState::MIN_ATTACKED;
        selfRect->setFillColor(clrMinAttacked);
        break;
    default:
        break;
    }
}

void Arkanoid::Block::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case COLLIDE_REACTION:
            trigger();
            break;
        default:
            break;
        }
    }
}

Arkanoid::Block::~Block()
{
    try
    {
        for (auto& b : bonuses)
            delete b;
    }
    catch (...)
    {
    }
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Bonus*/
Arkanoid::Bonus::Bonus(sf::Vector2f s_parent, sf::Vector2f p_parent, BonusDescriptor descr) 
    : MovableObject({ 0.8f * s_parent.x, 0.8f * s_parent.y }, { p_parent.x + 0.1f * s_parent.x, p_parent.y })
{
    setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y });
    std::string path = "";
    switch (descr)
    {
    case BONUS_PLATFORM_LENGTH_UP:
        path = totalProjectPath + "/BonusIcons/PlatformLengthUp.png";
        break;
    case BONUS_PLATFORM_LENGTH_DOWN:
        path = totalProjectPath + "/BonusIcons/PlatformLengthDown.png";
        break;
    case BONUS_PLATFORM_SPEED_DOWN:
        path = totalProjectPath + "/BonusIcons/PlatformSpeedDown.png";
        break;
    case BONUS_PLATFORM_SPEED_UP:
        path = totalProjectPath + "/BonusIcons/PlatformSpeedUp.png";
        break;
    case BONUS_BALL_SPEED_UP:
        path = totalProjectPath + "/BonusIcons/BallSpeedUp.png";
        break;
    case BONUS_BALL_SPEED_DOWN:
        path = totalProjectPath + "/BonusIcons/BallSpeedDown.png";
        break;
    default:
        break;
    }
    if (path != "") setTexture(path);
    descriptor = descr;
}
/*---------------------------------------------------------------------/*---------------------------------------------------------------------Player*/
Arkanoid::Player::Player() {

}

Arkanoid::Player::~Player()
{
    if (_CrtIsValidHeapPointer(platform))
        delete platform;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Player*/
Arkanoid::Ball::Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : MovableObject(radius, p, s_parent, sp)
{
    Difficulty diff = Settings::getDifficulty();
    resolution res = Settings::getResolution(); 
    sf::Vector2f new_speed; 
    switch (diff)
    {
    case HARD:
        new_speed.x = res.x * BALL_SPEED_HARD.x; 
        new_speed.y = res.x * BALL_SPEED_HARD.y; 
        break;
    case MEDIUM:
        new_speed.x = res.x * BALL_SPEED_MID.x;
        new_speed.y = res.x * BALL_SPEED_MID.y;
        break;
    case LOW:
        new_speed.x = res.x * BALL_SPEED_LOW.x;
        new_speed.y = res.x * BALL_SPEED_LOW.y;
        break;
    default:
        break;
    }
    speed = new_speed; 
    selfRect->setFillColor(c);
    initPosition = { BALL_WIDTH_INIT_OFFSET * s_parent.x, BALL_HEIGHT_INIT_OFFSET * s_parent.y };
}

Arkanoid::Ball::Ball(EventObject* parent, sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : Ball(s_parent, radius, p, c, sp)
{
    ptrParent = parent;
}

void Arkanoid::Ball::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case BG_COLLISION: 
        {
            sf::FloatRect fr = *((sf::FloatRect*)event.data->at(0));
            CollisionDirection cd = *((CollisionDirection*)event.data->at(1));

            sf::FloatRect commonRect = getCommonRect(fr);
            if (cd == CollisionDirection::HORIZONTAL_COLLISION_DIRECTION)
            {
                float x = getGlobalBounds().width - commonRect.width;
                if (speed.x > 0)
                    MovableObject::move({ -x, 0 });
                else
                    MovableObject::move({ x, 0 });
                changeSpeedDirectionX();
            }
            if (cd == CollisionDirection::VERTICAL_COLLISION_DIRECTION)
            {
                float y = getGlobalBounds().height - commonRect.height;
                if (speed.y > 0)
                    this->move({ 0, -y });
                else
                    this->move({ 0, y });
                changeSpeedDirectionY();
            }
            break;
        }
        case COLLIDE_REACTION:
        {
             sf::FloatRect fr = *((sf::FloatRect*)event.data->at(0));
            CollisionDirection cd = *((CollisionDirection*)event.data->at(1));

            sf::FloatRect commonRect = this->getCommonRect(fr); 
            if (cd == CollisionDirection::HORIZONTAL_COLLISION_DIRECTION)
            {
                float x = commonRect.width;
                if (speed.x > 0)
                    this->move({ -x, 0 });
                else
                    this->move({ x, 0 });
                changeSpeedDirectionX();
            }
            if (cd == CollisionDirection::VERTICAL_COLLISION_DIRECTION)
            {
                float y = commonRect.height;
                if (speed.y > 0)
                    this->move({0, -y});
                else
                    this->move({ 0, y});
                changeSpeedDirectionY();
            }
            delete event.data;
            break;
        }
        case FALL_DOWN:
            reset();
            break;
        case BALL_SPEED_UP:
            increaseSpeedBonus(); 
            break; 
        case BALL_SPEED_DOWN: 
            decreaseSpeedBonus();
            break;
        default:
            break;
        }
    }
}

Arkanoid::Ball::~Ball() = default;
/*---------------------------------------------------------------------/*---------------------------------------------------------------------Ball*/
Arkanoid::Balls::Balls() {

}

Arkanoid::Balls::~Balls()
{
    for (auto& b : balls)
        delete b;
}

/*--------------------------------------------------------------------/*---------------------------------------------------------------------Players*/
Arkanoid::Players::Players() {

}

void Arkanoid::Players::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case LIVES_DOWN:
        {
            for (auto& p : players)
            {
                p->getStatistics()->numOfLives--;
                if (p->getStatistics()->numOfLives == 0)
                {
                    std::vector<void*>* data = new std::vector<void*>;
                    data->push_back(p);
                    this->sendEvent(ptrParent, { PLAYER_IS_LOSE, data });
                }
            }
            break;
        }
        case RESET_PLATFORMS_TO_INITIAL_STATE:
            for (auto& p : players)
                p->getPlatform()->reset();
            break;
        case SCORE_UP_FOR_HITTING:
        {
            Player* p = getPlayer((Platform*)(event.data->at(0)));
            if (p)
                p->getStatistics()->score += SCORE_PER_HITTED_BLOCK;
            delete event.data;
            break;
        }
        case SCORE_UP_FOR_CRASHING:
        {
            Player* p = getPlayer((Platform*)(event.data->at(0)));
            if (p)
            {
                p->getStatistics()->score += SCORE_PER_CRASHED_BLOCK;
                p->getStatistics()->numOfCrashedBlocks++;
            }
            delete event.data;
            break;
        }
        case PLATFORM_LENGTH_UP:
        {
            Platform* searchPlatform = (Platform*)event.data->at(0);
            Player* p = getPlayer(searchPlatform);
            if (p)
            {
                searchPlatform->increasePlatformLengthBonus();
            }
            delete event.data;
            break;
        }
        case PLATFORM_LENGTH_DOWN:
        {
            Platform* searchPlatform = (Platform*)event.data->at(0);
            Player* p = getPlayer(searchPlatform);
            if (p)
            {
                searchPlatform->decreasePlatformLengthBonus();
            }
            delete event.data;
            break;
        }
        case PLATFORM_SPEED_UP:
        {
            Platform* searchPlatform = (Platform*)event.data->at(0);
            Player* p = getPlayer(searchPlatform);
            if (p)
            {
                searchPlatform->increaseSpeedBonus();
            }
            delete event.data;
            break;
        }
        case PLATFORM_SPEED_DOWN:
        {
            Platform* searchPlatform = (Platform*)event.data->at(0);
            Player* p = getPlayer(searchPlatform);
            if (p)
            {
                searchPlatform->decreaseSpeedBonus();
            }
            delete event.data;
            break;
        }
        case BONUS_UP:
            for (int i = 0, n = players.size(); i < n; i++)
                players[i]->getStatistics()->numOfBonuses++;
            break; 
        default:
            break;
        }
    }
}

Arkanoid::Player* Arkanoid::Players::getPlayer(Arkanoid::Platform* platform)
{
    for (auto& p : players)
    {
        if (p->getPlatform() == platform)
            return p;
    }
    return nullptr;
}

int Arkanoid::Players::getPlayerIndex(Platform* platform)
{
    for (int i = 0, n = players.size(); i < n; i++)
    {
        if (players[i]->getPlatform() == platform)
            return i;
    }
    return -1;
}

Arkanoid::Players::~Players()
{
    clear();
}

/*--------------------------------------------------------------------/*---------------------------------------------------------------------Blocks*/
Arkanoid::Blocks::Blocks()
{

}

Arkanoid::Blocks::~Blocks()
{
    for (auto& b : blocks)
        delete b;
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------History*/
Arkanoid::History::History()
{

}

bool Arkanoid::History::pathThrough(std::fstream& file, std::string& buffer)
{
    textFileGame >> buffer;
    while (!textFileGame.eof() && !isFine(buffer))
        textFileGame >> buffer;

    textFileGame >> buffer;
    while (!textFileGame.eof() && !isFine(buffer))
        textFileGame >> buffer;
    return true;
}

bool Arkanoid::History::simpleRead(std::fstream& file, std::string& buffer)
{
    textFileGame >> buffer;
    while (!textFileGame.eof() && !isFine(buffer))
        textFileGame >> buffer;
    return true;
}

bool Arkanoid::History::isFine(std::string& buffer)
{
    if (buffer.find('\n') == -1 && buffer.find('\t') == -1)
        return true;
    return false;
}


void Arkanoid::History::writeData(BonusesTimers bonusesTimers, 
                                  Settings& settings, Players& players, 
                                  Balls& balls, Blocks& blocks)
{
    clearTextFile();
    settings.save_to_txt(textFileGame, TEXT_FILE_GAME_PATH);
    players.save_to_txt(textFileGame, TEXT_FILE_GAME_PATH);
    balls.save_to_txt(textFileGame, TEXT_FILE_GAME_PATH);
    blocks.save_to_txt(textFileGame, TEXT_FILE_GAME_PATH);
    {
        textFileGame.open(TEXT_FILE_GAME_PATH, std::fstream::app);
        textFileGame << "bonusesTimers:" << " " << "num:" << " " << bonusesTimers.size() << "\n";
        for (int i = 0, n = bonusesTimers.size(); i < n; i++)
        {
            textFileGame << "player_index:" << " " << bonusesTimers[i].first.first << "\n";
            textFileGame << "bonus_descriptor:" << " " << bonusesTimers[i].first.second << "\n";
            textFileGame << "max_time:" << " " << BONUS_MAX_TIME - bonusesTimers[i].second.second->GetElapsedSeconds() << "\n";
            textFileGame << "\n";
        }
        textFileGame << "\n\n";
        textFileGame.close(); 
    }
}

void Arkanoid::History::readData(Game* game)
{
    textFileGame.open(TEXT_FILE_GAME_PATH, std::fstream::in);
    if (textFileGame.is_open())
    {
        std::string buffer;
        bool state = true;
        while (state && !textFileGame.eof())
        {
            state = simpleRead(textFileGame, buffer);
            {
                if (buffer.find("settings:") != -1)
                {
                    state = simpleRead(textFileGame, buffer);
                    std::string res_str(buffer);

                    state = simpleRead(textFileGame, buffer);
                    std::string diff_str(buffer);

                    resolution res = Settings::strToRes(res_str);
                    Difficulty diff = Settings::strToDiff(diff_str);

                    game->settings.setDifficulty(diff);
                    game->settings.setResolution(res);

                    //the part, where everything changes
                    game->resetMenus(Settings::getResolution(), Settings::getDifficulty());
                    if (game->window && game->view)
                    {
                        game->view->reset(sf::FloatRect(0, 0, res.x, res.y));
                        game->window->setSize({ (uint32_t)res.x, (uint32_t)res.y });
                    }
                    if (game->gameField)
                        delete game->gameField;
                    game->gameField = new GameField(game, Settings::getResolution(), { 0, 0 }, ColorCustomLightBlue);
                    game->gameField->setVisibility(false);
                    game->players.clear();
                }
                if (buffer.find("players:") != -1)
                {
                    state = pathThrough(textFileGame, buffer);
                    uint64_t numOfPlayers = stoi(buffer);

                    for (uint64_t i = 0; i < numOfPlayers; i++)
                    {
                        Player* player = new Player();
                        /*statistics*/
                        {
                            Statistics st;

                            state = pathThrough(textFileGame, buffer);
                            st.userName = buffer;

                            state = pathThrough(textFileGame, buffer);
                            st.numOfLives = std::stoi(buffer);

                            state = pathThrough(textFileGame, buffer);
                            st.score = std::stoi(buffer);

                            state = pathThrough(textFileGame, buffer);
                            st.numOfBonuses = std::stoi(buffer);

                            state = pathThrough(textFileGame, buffer);
                            st.numOfCrashedBlocks = std::stoi(buffer);

                            player->setStatistics(st);
                        }
                        /*platform*/
                        state = simpleRead(textFileGame, buffer);
                        {
                            sf::Vector2f position, size, speed;
                            bool visibility;

                            state = pathThrough(textFileGame, buffer);
                            position.x = stof(buffer);
                            state = pathThrough(textFileGame, buffer);
                            position.y = stof(buffer);

                            state = pathThrough(textFileGame, buffer);
                            size.x = stof(buffer);
                            state = pathThrough(textFileGame, buffer);
                            size.y = stof(buffer);

                            state = pathThrough(textFileGame, buffer);
                            speed.x = stof(buffer);
                            state = pathThrough(textFileGame, buffer);
                            speed.y = stof(buffer);

                            state = pathThrough(textFileGame, buffer);
                            visibility = stoi(buffer);

                            Platform* platform = new Platform(
                                game->gameField, Settings::getResolution(),
                                size, position, sf::Color::Black, speed
                            );
                            platform->setVisibility(visibility);
                            player->setPlatform(platform);

                            game->gameField->displayObjects.push_back(platform);
                            game->gameField->movableObjects.push_back(platform);
                            game->players.addPlayer(player);
                            game->players.setParent(game);
                        }
                    }
                    /*status bar*/
                    game->gameField->statusBar->setData(game->players.players[0]->getStatistics());
                }
                if (buffer.find("balls:") != -1)
                {
                    state = pathThrough(textFileGame, buffer);
                    uint64_t numOfBalls = stoi(buffer);
                    for (uint64_t i = 0; i < numOfBalls; i++)
                    {
                        sf::Vector2f position, size, speed;
                        bool visibility;

                        state = pathThrough(textFileGame, buffer);
                        position.x = stof(buffer);
                        state = pathThrough(textFileGame, buffer);
                        position.y = stof(buffer);

                        state = pathThrough(textFileGame, buffer);
                        size.x = stof(buffer);
                        state = pathThrough(textFileGame, buffer);
                        size.y = stof(buffer);

                        state = pathThrough(textFileGame, buffer);
                        speed.x = stof(buffer);
                        state = pathThrough(textFileGame, buffer);
                        speed.y = stof(buffer);

                        state = pathThrough(textFileGame, buffer);
                        visibility = stoi(buffer);

                        Ball* ball = new Ball(
                            game->gameField, Settings::getResolution(),
                            size.x, position, sf::Color::Black,
                            speed
                        );
                        ball->setVisibility(visibility);
                        game->gameField->balls.addBall(ball);
                        game->gameField->displayObjects.push_back(ball);
                        game->gameField->movableObjects.push_back(ball);
                    }
                }
                if (buffer.find("blocks:") != -1)
                {
                    state = pathThrough(textFileGame, buffer);
                    uint64_t numOfBlocks = stoi(buffer);
                    for (uint64_t i = 0; i < numOfBlocks; i++)
                    {
                        sf::Vector2f position, size;
                        int lives;
                        BlockState blockState;

                        bool visibility;

                        state = pathThrough(textFileGame, buffer);
                        position.x = stof(buffer);
                        state = pathThrough(textFileGame, buffer);
                        position.y = stof(buffer);

                        state = pathThrough(textFileGame, buffer);
                        size.x = stof(buffer);
                        state = pathThrough(textFileGame, buffer);
                        size.y = stof(buffer);

                        state = pathThrough(textFileGame, buffer);
                        lives = stoi(buffer);
                        switch (lives)
                        {
                        case 0:
                            blockState = BlockState::CRASHED;
                            break;
                        case 1:
                           // blockState = BlockState::MAX_ATTACKED;
                            blockState = BlockState::NOT_ATTACKED;
                            break;
                        case 2:
                            blockState = BlockState::MID_ATTACKED;
                            break;
                        case 3:
                            blockState = BlockState::MIN_ATTACKED;
                            break;
                        case 4:
                            blockState = BlockState::NOT_ATTACKED;
                            break;
                        default:
                            break;
                        }

                        state = pathThrough(textFileGame, buffer);
                        visibility = stoi(buffer);

                        Block* block = new  Block(game->gameField, Settings::getResolution(),
                            size, position, blockState);
                        block->setVisibility(visibility);
                        
                        state = pathThrough(textFileGame, buffer);
                        state = simpleRead(textFileGame, buffer);
                        size_t numOfBonuses = stoi(buffer);
                        for (size_t b = 0; b < numOfBonuses; b++)
                        {
                            sf::Vector2f bposition, bsize;
                            bool bvisibility;
                            BonusDescriptor descriptor;

                            state = pathThrough(textFileGame, buffer);
                            bposition.x = stof(buffer);
                            state = pathThrough(textFileGame, buffer);
                            bposition.y = stof(buffer);

                            state = pathThrough(textFileGame, buffer);
                            bsize.x = stof(buffer);
                            state = pathThrough(textFileGame, buffer);
                            bsize.y = stof(buffer);

                            state = pathThrough(textFileGame, buffer);
                            bvisibility = stoi(buffer);
                            state = pathThrough(textFileGame, buffer);
                            descriptor = BonusDescriptor(stoi(buffer));

                            Bonus* bonus = new Bonus(bsize, bposition);
                            bonus->setVisibility(bvisibility);
                            bonus->setBonusDescriptor(descriptor);
                            block->bonuses.push_back(bonus);
                            if (!visibility)
                            {
                                game->gameField->displayObjects.push_back(bonus);
                                game->gameField->movableObjects.push_back(bonus);
                            }
                        }

                        game->gameField->blocks.addBlock(block);
                        game->gameField->displayObjects.push_back(block);
                    }
                }
                if (buffer.find("bonusesTimers:") != -1)
                {
                    state = pathThrough(textFileGame, buffer);
                    uint64_t numOfBonusesTimers = stoi(buffer);
                    for (uint64_t i = 0; i < numOfBonusesTimers; i++)
                    {
                        int index;
                        BonusDescriptor bd; 
                        float maxTime; 
                        BonusTimer* clock = new BonusTimer;

                        state = pathThrough(textFileGame, buffer);
                        index = stoi(buffer);
                        state = pathThrough(textFileGame, buffer);
                        bd = (BonusDescriptor)stoi(buffer);
                        state = pathThrough(textFileGame, buffer);
                        maxTime= stof(buffer);
                        

                        clock->Reset(); 
                        clock->Pause(); 
                        game->gameField->bonusesTimers.push_back(
                            { {index, bd}, {maxTime, clock} }
                        ); 
                    }
                }

            }   
        }
        textFileGame.close();
    }
}


void Arkanoid::History::to_json(BonusesTimers bonusesTimers,
                                Settings& settings, Players& players, Balls& balls, Blocks& blocks)
{
    clearJsonFile();

    json j;

    settings.save_to_json(j);
    players.save_to_json(j);
    balls.save_to_json(j);
    blocks.save_to_json(j);
    {
        j["num_of_bonuses_timers"] = bonusesTimers.size();
        for (int i = 0, n = bonusesTimers.size(); i < n; i++)
        {
            j["bonuses_timers"].push_back
            ({
                {"player_index", bonusesTimers[i].first.first},
                {"bonus_descriptor", bonusesTimers[i].first.second},
                {"max_time", BONUS_MAX_TIME - bonusesTimers[i].second.second->GetElapsedSeconds()},
            });
        }
    }

    jsonFileGame.open(JSON_FILE_GAME_PATH, std::fstream::out);
    if (jsonFileGame.is_open())
    {
        jsonFileGame << std::setw(4) << j << "\n";
        jsonFileGame.close();
    }
    jsonFileGame.close();

}

void Arkanoid::History::from_json(Game* game)
{
    json j;
    jsonFileGame.open(JSON_FILE_GAME_PATH, std::fstream::in);

    if (jsonFileGame.is_open())
    {
        std::stringstream strStream;
        strStream << jsonFileGame.rdbuf(); //read the file
        std::string str = strStream.str(); //str holds the content of the fil*/
        j = json::parse(str);
        jsonFileGame.close();
    }

    /*settings*/
    sf::Vector2f res = { j["settings"]["resolution_x"].get<float>(), j["settings"]["resolution_y"].get<float>() };
    Difficulty   diff = (Difficulty)j["settings"]["difficulty"].get<int>();
    game->settings.setResolution(res);
    game->settings.setDifficulty(diff);

    //the part, where everything changes
    game->resetMenus(res, diff);
    if (game->window && game->view)
    {
        game->view->reset(sf::FloatRect(0, 0, res.x, res.y));
        game->window->setSize({ (uint32_t)res.x, (uint32_t)res.y });
    }
    if (game->gameField)
        delete game->gameField;
    game->gameField = new GameField(game, Settings::getResolution(), { 0, 0 }, ColorCustomLightBlue);
    game->gameField->setVisibility(false);
    game->players.clear();

    /*players*/
    {
        uint64_t numOfPlayers = j["num_of_players"].get<uint64_t>();
        for (uint64_t i = 0; i < numOfPlayers; i++)
        {
            Player* player = new Player();
            /*statistics*/
            {
                Statistics st;

                st.userName = j["players"][i]["name"].get<std::string>();
                st.numOfLives = j["players"][i]["num_of_lives"].get<uint64_t>();
                st.score = j["players"][i]["score"].get<uint64_t>();
                st.numOfBonuses = j["players"][i]["num_of_bonuses"].get<uint64_t>();
                st.numOfCrashedBlocks = j["players"][i]["num_of_crashed_blocks"].get<uint64_t>();

                player->setStatistics(st);
            }
            /*platform*/
            {
                sf::Vector2f position, size, speed;
                bool visibility;

                position.x = j["players"][i]["platform"]["position_x"].get<float>();
                position.y = j["players"][i]["platform"]["position_y"].get<float>();

                size.x = j["players"][i]["platform"]["size_x"].get<float>();
                size.y = j["players"][i]["platform"]["size_y"].get<float>();

                speed.x = j["players"][i]["platform"]["speed_x"].get<float>();
                speed.y = j["players"][i]["platform"]["speed_y"].get<float>();

                visibility = j["players"][i]["platform"]["visibility"].get<float>();

                Platform* platform = new Platform(
                    game->gameField, Settings::getResolution(),
                    size, position, sf::Color::Black, speed
                );

                platform->setVisibility(visibility);
                player->setPlatform(platform);

                game->gameField->displayObjects.push_back(platform);
                game->gameField->movableObjects.push_back(platform);
                game->players.addPlayer(player);
                game->players.setParent(game);
            }
        }

    }
    /*balls*/
    {
        uint64_t numOfBalls = j["num_of_balls"].get<uint64_t>();
        for (uint64_t i = 0; i < numOfBalls; i++)
        {
            sf::Vector2f position, size, speed;
            bool visibility;

            position.x = j["balls"][i]["position_x"].get<float>();
            position.y = j["balls"][i]["position_y"].get<float>();

            size.x = j["balls"][i]["size_x"].get<float>();
            size.y = j["balls"][i]["size_y"].get<float>();

            speed.x = j["balls"][i]["speed_x"].get<float>();
            speed.y = j["balls"][i]["speed_y"].get<float>();

            visibility = j["balls"][i]["visibility"].get<float>();

            Ball* ball = new Ball(
                game->gameField, Settings::getResolution(),
                size.x, position, sf::Color::Black,
                speed
            );
            ball->setVisibility(visibility);
            game->gameField->balls.addBall(ball);
            game->gameField->displayObjects.push_back(ball);
            game->gameField->movableObjects.push_back(ball);
        }
    }
    /*block*/
    {
        uint64_t numOfBlocks = j["num_of_blocks"].get<uint64_t>();
        for (uint64_t i = 0; i < numOfBlocks; i++)
        {
            sf::Vector2f position, size;
            int lives;
            BlockState blockState;

            bool visibility;

            position.x = j["blocks"][i]["position_x"].get<float>();
            position.y = j["blocks"][i]["position_y"].get<float>();

            size.x = j["blocks"][i]["size_x"].get<float>();
            size.y = j["blocks"][i]["size_y"].get<float>();

            visibility = j["blocks"][i]["visibility"].get<float>();

            lives = j["blocks"][i]["lives"].get<float>();

            switch (lives)
            {
            case 0:
                blockState = BlockState::CRASHED;
                break;
            case 1:
                //blockState = BlockState::MAX_ATTACKED;
                blockState = BlockState::NOT_ATTACKED;
                break;
            case 2:
                blockState = BlockState::MID_ATTACKED;
                break;
            case 3:
                blockState = BlockState::MIN_ATTACKED;
                break;
            case 4:
                blockState = BlockState::NOT_ATTACKED;
                break;
            default:
                break;
            }

            Block* block = new  Block(game->gameField, Settings::getResolution(), size, position, blockState);
            block->setVisibility(visibility);
            size_t numOfBonuses = j["blocks"][i]["num_of_bonuses"].get<size_t>();
            for (size_t b = 0; b < numOfBonuses; b++)
            {
                sf::Vector2f bposition, bsize;
                bool bvisibility;
                BonusDescriptor descriptor;

                bposition.x = j["blocks"][i]["bonuses"][b]["position_x"].get<float>();
                bposition.y = j["blocks"][i]["bonuses"][b]["position_y"].get<float>();

                bsize.x = j["blocks"][i]["bonuses"][b]["size_x"].get<float>();
                bsize.y = j["blocks"][i]["bonuses"][b]["size_y"].get<float>();

                bvisibility = j["blocks"][i]["bonuses"][b]["visibility"].get<float>();
                descriptor = (BonusDescriptor)j["blocks"][i]["bonuses"][b]["descriptor"].get<float>();


                Bonus* bonus = new Bonus(bsize, bposition);
                bonus->setVisibility(bvisibility);
                bonus->setBonusDescriptor(descriptor);
                block->bonuses.push_back(bonus);
                if (!visibility)
                {
                    game->gameField->displayObjects.push_back(bonus);
                    game->gameField->movableObjects.push_back(bonus);
                }
            }
            
            game->gameField->blocks.addBlock(block);
            game->gameField->displayObjects.push_back(block);
        }
    }
    /*status bar*/
    game->gameField->statusBar->setData(game->players.players[0]->getStatistics());
    /*bonuses timers*/
    {
        size_t numOfBonusesTimers = j["num_of_bonuses_timers"].get<size_t>();
        for (size_t i = 0; i < numOfBonusesTimers; i++)
        {
            int index;
            BonusDescriptor bd;
            float maxTime;
            BonusTimer* clock = new BonusTimer;

            index = j["bonuses_timers"][i]["player_index"].get<int>();
            bd = (BonusDescriptor)j["bonuses_timers"][i]["bonus_descriptor"].get<int>();
            maxTime = j["bonuses_timers"][i]["max_time"].get<float>();
            
            clock->Reset();
            clock->Pause(); 
            game->gameField->bonusesTimers.push_back(
                { {index, bd}, {maxTime, clock} }
            );
        }
    }
}

/*Balls*/
void Arkanoid::Balls::save_to_json(json& j)
{
    j["num_of_balls"] = balls.size();
    for (const auto& b : balls)
    {
        j["balls"].push_back
        ({
            {"position_x",   b->getSelfRect()->getPosition().x},
            {"position_y",   b->getSelfRect()->getPosition().y},
            {"size_x",       b->getSize().x},
            {"size_y",       b->getSize().y},
            {"speed_x",      b->getSpeed().x},
            {"speed_y",      b->getSpeed().y},
            {"visibility",   b->isVisible()}, 
            });
    }
}

void Arkanoid::Balls::save_to_txt(std::fstream& file, std::string fileName)
{
    file.open(fileName, std::fstream::app);
    file << "balls:" << " " << "num:" << " " << balls.size() << "\n";
    for (const auto& b : balls)
    {
        file << "position_x:" << " " << b->getSelfRect()->getPosition().x << "\n";
        file << "position_y:" << " " << b->getSelfRect()->getPosition().y << "\n";
        file << "size_x:" << " " << b->getSize().x << "\n";
        file << "size_y:" << " " << b->getSize().y << "\n";
        file << "speed_x:" << " " << b->getSpeed().x << "\n";
        file << "speed_y:" << " " << b->getSpeed().y << "\n";
        file << "visibility:" << " " << b->isVisible() << "\n";
        file << "\n";
    }
    file << "\n\n";
    file.close();
}

/*Blocks*/
void Arkanoid::Blocks::save_to_json(json& j)
{
    j["num_of_blocks"] = blocks.size();

    for (const auto& b : blocks)
    {
        json bonuses;
        for (int i = 0, n = b->bonuses.size(); i < n; i++)
        {
            bonuses["bonuses"].push_back
            ({
                {"position_x",   b->bonuses[i]->getSelfRect()->getPosition().x},
                {"position_y",   b->bonuses[i]->getSelfRect()->getPosition().y},
                {"size_x",       b->bonuses[i]->getSize().x},
                {"size_y",       b->bonuses[i]->getSize().y},
                {"visibility",   b->bonuses[i]->isVisible()}, 
                {"descriptor",   b->bonuses[i]->getBonusDescriptor()}
            });
        }
        j["blocks"].push_back
        ({
            {"position_x",   b->getSelfRect()->getPosition().x},
            {"position_y",   b->getSelfRect()->getPosition().y},
            {"size_x",       b->getSize().x},
            {"size_y",       b->getSize().y},
            {"lives",        b->getLives()},
            {"visibility",   b->isVisible()},
            {"num_of_bonuses", b->bonuses.size()},
            {"bonuses", bonuses["bonuses"]}
        });
    }
}

void Arkanoid::Blocks::save_to_txt(std::fstream& file, std::string fileName)
{
    file.open(fileName, std::fstream::app);
    file << "blocks:" << " " << "num:" << " " << blocks.size() << "\n";
    for (const auto& b : blocks)
    {
        file << "position_x:" << " " << b->getSelfRect()->getPosition().x << "\n";
        file << "position_y:" << " " << b->getSelfRect()->getPosition().y << "\n";
        file << "size_x:" << " " << b->getSize().x << "\n";
        file << "size_y:" << " " << b->getSize().y << "\n";
        file << "lives:" << " " << b->getLives() << "\n";
        file << "visibility:" << " " << b->isVisible() << "\n";
        file << "bonuses:" << " " << "num:" << " " << b->bonuses.size() << "\n"; 
        for (int i = 0, n = b->bonuses.size(); i < n; i++)
        {
            file << "\tposition_x:" << " " << b->bonuses[i]->getSelfRect()->getPosition().x << "\n";
            file << "\tposition_y:" << " " << b->bonuses[i]->getSelfRect()->getPosition().y << "\n";
            file << "\tsize_x:" << " " << b->bonuses[i]->getSize().x << "\n";
            file << "\tsize_y:" << " " << b->bonuses[i]->getSize().y << "\n";
            file << "\tvisibility:" << " " << b->bonuses[i]->isVisible() << "\n";
            file << "\tdescriptor:" << " " << b->bonuses[i]->getBonusDescriptor() << "\n";
        }
        file << "\n";
    }
    file.close();
}

/*Players*/
void Arkanoid::Players::save_to_json(json& j)
{
    j["num_of_players"] = players.size();
    for (const auto& p : players)
    {
        j["players"].push_back
        ({
            {"name",                  p->getStatistics()->userName},
            {"num_of_lives",          p->getStatistics()->numOfLives},
            {"score",                 p->getStatistics()->score},
            {"num_of_bonuses",        p->getStatistics()->numOfBonuses},
            {"num_of_crashed_blocks", p->getStatistics()->numOfCrashedBlocks},
            {"platform",
                {
                    {"position_x",   p->getPlatform()->getSelfRect()->getPosition().x},
                    {"position_y",   p->getPlatform()->getSelfRect()->getPosition().y},
                    {"size_x",       p->getPlatform()->getSize().x},
                    {"size_y",       p->getPlatform()->getSize().y},
                    {"speed_x",      p->getPlatform()->getSpeed().x},
                    {"speed_y",      p->getPlatform()->getSpeed().y},
                    {"visibility",   p->getPlatform()->isVisible()},
                }
            }
            });
    }
}

void Arkanoid::Players::save_to_txt(std::fstream& file, std::string fileName)
{
    file.open(fileName, std::fstream::app);
    file << "players:" << " " << "num:" << " " << players.size() << "\n";
    for (const auto& p : players)
    {
        file << "name:" << " " << p->getStatistics()->userName << "\n";
        file << "num_of_lives:" << " " << p->getStatistics()->numOfLives << "\n";
        file << "score:" << " " << p->getStatistics()->score << "\n";
        file << "num_of_bonuses:" << " " << p->getStatistics()->numOfBonuses << "\n";
        file << "num_of_crashed_blocks:" << " " << p->getStatistics()->numOfCrashedBlocks << "\n";
        file << "platform:" << " " << "\n";
        file << "\tposition_x:" << " " << p->getPlatform()->getSelfRect()->getPosition().x << "\n";
        file << "\tposition_y:" << " " << p->getPlatform()->getSelfRect()->getPosition().y << "\n";
        file << "\tsize_x:" << " " << p->getPlatform()->getSize().x << "\n";
        file << "\tsize_y:" << " " << p->getPlatform()->getSize().y << "\n";
        file << "\tspeed_x:" << " " << p->getPlatform()->getSpeed().x << "\n";
        file << "\tspeed_y:" << " " << p->getPlatform()->getSpeed().y << "\n";
        file << "\tvisibility:" << " " << p->getPlatform()->isVisible() << "\n";
        file << "\n";
    }
    file << "\n\n";

    file.close();
}

/*Settings*/
void Arkanoid::Settings::save_to_json(json& j)
{
    j["settings"] =
    {
        {"resolution_x", Settings::getResolution().x},
        {"resolution_y", Settings::getResolution().y},
        {"difficulty",   Settings::getDifficulty()}
    };
}

void Arkanoid::Settings::save_to_txt(std::fstream& file, std::string fileName)
{
    file.open(fileName, std::fstream::app);
    file << "settings:" << " " << Settings::resToStr(Settings::getResolution()) << " " << Settings::diffToStr(Settings::getDifficulty()) << "\n";
    file << "\n\n\n";
    file.close();
}

Arkanoid::History::~History() {

}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Settings*/
Difficulty Arkanoid::Settings::diff;
resolution Arkanoid::Settings::res;

Arkanoid::Settings::Settings()
{
    setDefaultSettings();
}

void Arkanoid::Settings::setDefaultSettings()
{
    diff = MEDIUM;
    res = { mResolution[0].x, mResolution[0].y };
}

Arkanoid::Settings::~Settings()
{
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Gamefield*/
bool Arkanoid::GameField::isFirstStart = true;

Arkanoid::GameField::GameField(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
    displayObjects.push_back(this);
    initMessageBoxes();
    if (isFirstStart)
        mBoxes[0]->setVisibility(true);
    state = GameState::OFF;

    /*init status bar*/
    statusBar = new StatusBar(Settings::getResolution(), { Settings::getResolution().x, STATUS_BAR_HEIGHT * Settings::getResolution().y }, { 0, 0 }, ColorCustomMidPurpleBlueLight);
    Button* btn = new Button(this, "Pause", { STATUS_BAR_BUTTON_WIDTH * Settings::getResolution().x, STATUS_BAR_BUTTON_HEIGHT * Settings::getResolution().y },
        { STATUS_BAR_BUTTON_OFFSET_LEFT * Settings::getResolution().x, STATUS_BAR_BUTTON_OFFSET_TOP * Settings::getResolution().y },
        EventType::OPEN_WAIT_MENU);
    btn->setTextSize(STATUS_BAR_BUTTON_FONT_SIZE * Settings::getResolution().y);
    btn->setScales(Settings::getResolution());
    btn->setColors(ColorCustomMidPurpleBlueDark, ColorCustomMidPurpleBlueUltraDark, ColorCustomMidPurpleBlueUltraDark, sf::Color::White);
    statusBar->menuButton = btn;
    displayObjects.push_back(statusBar);
}

Arkanoid::GameField::GameField(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : GameField(s, p, color)
{
    ptrParent = parent;
}

uint64_t Arkanoid::GameField::getNumberOfDisplayedBlocks()
{
    uint64_t displayedBlocks = 0;
    for (auto& b : this->blocks.blocks)
    {
        if (b->isVisible())
            displayedBlocks++;
    }
    return displayedBlocks;
}

void Arkanoid::GameField::initMessageBoxes()
{
    /*Init Start MessageBox*/
    {
        Messagebox* mStart = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, { 0, 0 }, ColorCustomMidPurpleBlueUltraDark);
        mStart->setVisibility(false);

        auto* textField = new TextField();
        textField->setAllProperties(MSTART_TEXT, totalProjectPath + "/Fonts/simple.ttf",
            sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0,
            sf::Color::White, ColorCustomPink);

        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mStart->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mStart->addTextField(textField);
        mBoxes.push_back(mStart);
    }
    /*Init End Box*/
    {
        Messagebox* mEnd = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, { 0, 0 }, ColorCustomMidPurpleBlueUltraDark);
        mEnd->setVisibility(false);

        auto* textField = new TextField();
        textField->setAllProperties(MEND_TEXT, totalProjectPath + "/Fonts/simple.ttf",
            sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0,
            sf::Color::White, ColorCustomPink);

        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mEnd->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mEnd->addTextField(textField);
        mBoxes.push_back(mEnd);
    }
    /*Init Win Box*/
    {
        Messagebox* mWin = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, { 0, 0 }, ColorCustomMidPurpleBlueUltraDark);
        mWin->setVisibility(false);

        auto* textField = new TextField();
        textField->setAllProperties(MWIN_TEXT, totalProjectPath + "/Fonts/simple.ttf",
            sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0,
            sf::Color::White, ColorCustomPink);

        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mWin->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mWin->addTextField(textField);
        mBoxes.push_back(mWin);
    }
}

void Arkanoid::GameField::collisionDetection()
{
    std::vector<Collision> data; //Need for event
    for (auto& dynamicObj : movableObjects)
    {
        for (auto& staticObj : displayObjects)
        {
            if (dynamicObj != staticObj && staticObj->isVisible() && dynamicObj->isVisible())
            {
                Collision collision;
                collision.type = GameField::getCollisionType(dynamicObj, staticObj);
                if (collision.type != CollisionType::NONE_COLLISION)
                {
                    if (dynamicObj->isCollideWith(staticObj))
                    {
                        if (collision.type == CollisionType::BALL_PANEL_COLLISION)
                            int aknk = 1020121; 
                        collision.direction = CollisionDirection::NONE_COLLISION_DIRECTION;
                        sf::FloatRect fr_dynamic = dynamicObj->getGlobalBounds();
                        sf::FloatRect fr_static = staticObj->getGlobalBounds();
                        if (staticObj == *displayObjects.begin()) //Intersection with background
                        {
                            if (fr_dynamic.left < 0 || fr_dynamic.left + fr_dynamic.width > fr_static.width)
                                collision.direction = CollisionDirection::HORIZONTAL_COLLISION_DIRECTION;
                            if (fr_dynamic.top + fr_dynamic.height > fr_static.height) //The number of lives decreases - loss
                            {
                                collision.direction = CollisionDirection::VERTICAL_COLLISION_DIRECTION;
                                if (collision.type == BALL_BG_COLLISION)  collision.type = CollisionType::BALL_FALL_COLLISION;
                                if (collision.type == BONUS_BG_COLLISION) collision.type = CollisionType::BONUS_FALL_COLLISION;
                            }
                        }
                        else //Intersection with other objects
                        {
                            sf::FloatRect commonRect = dynamicObj->getCommonRect(fr_static);
                            if (commonRect.width > commonRect.height)
                                collision.direction = CollisionDirection::VERTICAL_COLLISION_DIRECTION;
                            else
                                collision.direction = CollisionDirection::HORIZONTAL_COLLISION_DIRECTION;
                            if (collision.type == PLATFORM_COLLISION && collision.direction == CollisionDirection::HORIZONTAL_COLLISION_DIRECTION)
                                collision.direction = CollisionDirection::VERTICAL_COLLISION_DIRECTION;
                        }
                        if (collision.direction != CollisionDirection::NONE_COLLISION_DIRECTION)
                        {
                            collision.dynamicObj = dynamicObj;
                            collision.staticObj = staticObj;
                            data.push_back(collision);
                        }
                    }
                }
            }
        }
    }

    std::vector<Collision> tempoBallsBgCollisions;
    for (uint32_t i = 0, size = data.size(); i < size; i++)
    {
        if (data[i].type == BALL_FALL_COLLISION) tempoBallsBgCollisions.push_back(data[i]);
        else if (data[i].type == PLATFORM_COLLISION)
        {
            for (uint32_t j = 0, n = tempoBallsBgCollisions.size(); j < n; j++)
            {
                if (tempoBallsBgCollisions[j].dynamicObj == data[j].dynamicObj)
                    tempoBallsBgCollisions.erase(tempoBallsBgCollisions.begin() + j);
            }
            this->collisionSolution(data[i]);
        }
        else this->collisionSolution(data[i]);
    }
    for (uint32_t j = 0, n = tempoBallsBgCollisions.size(); j < n; j++)
    {
        this->collisionSolution(tempoBallsBgCollisions[j]);
    }
}

void Arkanoid::GameField::collisionSolution(Collision collision)
{
    static Platform* prevHitInitiator = NULL;
    switch (collision.type)
    {
    case BONUS_FALL_COLLISION:
    {
        Bonus* bonus = dynamic_cast<Bonus*>(collision.dynamicObj);
        bonus->setVisibility(false);
        break;
    }
    case BONUS_COLLISION:
    {
        Platform* platform = dynamic_cast<Platform*>(collision.dynamicObj);
        Bonus* bonus = dynamic_cast<Bonus*>(collision.staticObj);
        bonus->setVisibility(false);

        std::vector<void*>* initiator = new std::vector<void*>;
        initiator->push_back(platform);
        initiator->push_back(bonus);

        sendEvent(ptrParent, { EventType::REFORM_AND_APPLY_BONUS, initiator });
        break;
    }
    case BALL_PANEL_COLLISION: 
    {
        std::vector<void*>* data = new std::vector<void*>;
        sf::FloatRect* bounds = new sf::FloatRect;
        *bounds = collision.staticObj->getGlobalBounds();
        CollisionDirection* dr = new CollisionDirection;
        *dr = collision.direction;
        data->push_back(bounds);
        data->push_back(dr);

        sendEvent(dynamic_cast<EventObject*>(collision.dynamicObj), { EventType::COLLIDE_REACTION, data });

        break; 
    }
    case PLATFORM_BG_COLLISION:
    case BALL_BG_COLLISION:
    {
        std::vector<void*>* data = new std::vector<void*>;
        sf::FloatRect* bounds = new sf::FloatRect;
        *bounds = collision.staticObj->getGlobalBounds();
        CollisionDirection* dr = new CollisionDirection;
        *dr = collision.direction;
        data->push_back(bounds);
        data->push_back(dr);

        sendEvent(dynamic_cast<EventObject*>(collision.dynamicObj), { EventType::BG_COLLISION, data });
        break;
    }
    case PLATFORM_COLLISION:
    {
        std::vector<void*>* data = new std::vector<void*>;
        sf::FloatRect* bounds = new sf::FloatRect;
        *bounds = collision.staticObj->getGlobalBounds();
        CollisionDirection* dr = new CollisionDirection;
        *dr = collision.direction;
        data->push_back(bounds);
        data->push_back(dr);

        sendEvent(dynamic_cast<EventObject*>(collision.dynamicObj), { EventType::COLLIDE_REACTION, data });
        prevHitInitiator = dynamic_cast<Platform*>(collision.staticObj);
        break;
    }
    case BALL_FALL_COLLISION:
    {
        sendEvent(dynamic_cast<EventObject*>(collision.dynamicObj), EventType::FALL_DOWN);
        sendEvent(ptrParent, EventType::RESET_PLATFORMS_TO_INITIAL_STATE);
        sendEvent(ptrParent, EventType::LIVES_DOWN);
        break;
    }
    case BLOCK_COLLISION:
    {
        Block* block = dynamic_cast<Block*>(collision.staticObj);
       
        std::vector<void*>* data = new std::vector<void*>;
        sf::FloatRect* bounds = new sf::FloatRect;
        *bounds = collision.staticObj->getGlobalBounds();
        CollisionDirection* dr = new CollisionDirection;
        *dr = collision.direction;
        data->push_back(bounds);
        data->push_back(dr);
        
        sendEvent(dynamic_cast<EventObject*>(collision.dynamicObj), { EventType::COLLIDE_REACTION, data });
        sendEvent(dynamic_cast<EventObject*>(collision.staticObj), EventType::COLLIDE_REACTION);

        std::vector<void*>* initiator_player_information = new std::vector<void*>;
        initiator_player_information->push_back(prevHitInitiator);

        /*if block is crashed*/
        if (block->getLives() == 1)
        {
            /*victory detection*/
            if (getNumberOfDisplayedBlocks() - 1 == 0) sendEvent(this, WIN);

            /*update score*/
            sendEvent(ptrParent, { SCORE_UP_FOR_CRASHING, initiator_player_information });

            /*activate bonus moving*/
            if (block->bonuses.size() != 0)
            {
                Bonus* bonus = block->bonuses[0];
                this->displayObjects.push_back(bonus);
                this->movableObjects.push_back(bonus);
            }
        }
        else /*simple block-hit*/
            sendEvent(ptrParent, { SCORE_UP_FOR_HITTING,  initiator_player_information });
        break;
    }
    default:
        break;
    }
}

void Arkanoid::GameField::display(sf::RenderWindow* window, sf::View* view)
{
    sf::Event event{};
    while (window->pollEvent(event))
    {
        this->statusBar->menuButton->update(event, *window);  //Change button colors depending on the event
        if (eventsQueue.size() != 0)
            break;

        switch (event.type)
        {
        case sf::Event::KeyReleased:
            if (event.key.code == sf::Keyboard::Space && (state == GameState::OFF))
            {
                state = GameState::ON;
                for (int i = 0, n = bonusesTimers.size(); i < n; i++)
                    bonusesTimers[i].second.second->Start(); 
            }
            else if (event.key.code == sf::Keyboard::Escape)
            {
                this->sendEvent(this, EventType::OPEN_WAIT_MENU);
                state = GameState::OFF;
            }
            break;
        case sf::Event::Closed:
            window->close();
            break;
        default:
            break;
        }
    }
    this->update();
    this->eventsHandler(); //Processing the queue of events from other objects, in particular, from the button

    window->setView(*view);
    window->clear();
    this->draw(window, view);
    for (auto& m : mBoxes)
    {
        if (m->isVisible())
        {
            if (m == *mBoxes.begin() && isFirstStart)
                continue;
            m->display();
        }
    }
    isFirstStart = false;
    this->eventsHandler();
}

void Arkanoid::GameField::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case OPEN_WAIT_MENU:
            pauseAllBonusesTimers(); 
            if (state != GameState::OFF)
                state = GameState::OFF;
            clearQueue();
            sendEvent(ptrParent, OPEN_WAIT_MENU);
            hide();
            break;
        case WIN:
            clearQueue();
            mBoxes[2]->setVisibility(true);
            sendEvent(ptrParent, OPEN_START_MENU);
            hide();
            break;
        case SHOW_END_MESSAGE:
            clearQueue();
            mBoxes[1]->setVisibility(true);
            sendEvent(ptrParent, RESET_GAME);
            hide();
            break;
        case BALL_SPEED_UP: 
            for (auto ball : balls.balls)
                sendEvent(ball, BALL_SPEED_UP);
            break; 
        case BALL_SPEED_DOWN: 
            for (auto ball : balls.balls)
                sendEvent(ball, BALL_SPEED_DOWN);
            break; 
        default:
            break;
        }
    }
}

void Arkanoid::GameField::addNewBonus(int index, BonusDescriptor bd)
{
    BonusTimer* clock = new BonusTimer;
    clock->Reset();
    bonusesTimers.push_back({ {index, bd}, {BONUS_MAX_TIME, clock} });
}

void Arkanoid::GameField::checkBonusTimers()
{
    BonusesTimers new_bonusesTimers;
    for (int i = 0, n = bonusesTimers.size(); i < n; i++)
    {
        if (bonusesTimers[i].second.second->GetElapsedSeconds() > bonusesTimers[i].second.first)
        {
            BonusDescriptor originBonus = bonusesTimers[i].first.second; 
            BonusDescriptor oppositeBonus = OPPOSITE_BONUS.at(originBonus);
           
            std::vector<void*>* initiator = new std::vector<void*>;
            
            int* index = new int; 
            *index = bonusesTimers[i].first.first; 
            initiator->push_back(index);
            
            BonusDescriptor* bd = new BonusDescriptor; 
            *bd = oppositeBonus;
            initiator->push_back(bd);

            sendEvent(ptrParent, { APPLY_BONUSES, initiator});

            delete bonusesTimers[i].second.second; //timer 
        }
        else new_bonusesTimers.push_back(bonusesTimers[i]);
    }
    bonusesTimers = new_bonusesTimers; 
}

void Arkanoid::GameField::update()
{
    checkBonusTimers(); 
    statusBar->update();
    if (state == GameState::ON)
    {
        for (auto& obj : movableObjects)
            obj->move();
        if (balls.balls[0]->getGlobalBounds().top > 980)
            int ebalaYaVRotHochuKuklu = 100; 
        collisionDetection();
        for (int i = 1, n = displayObjects.size(); i < n; i++)
        {   
            auto event_obj = dynamic_cast<EventObject*>(displayObjects[i]);
            if (event_obj != NULL)
                event_obj->eventsHandler();
        }
    }
}

Arkanoid::GameField::~GameField()
{
    balls.~Balls();
    blocks.~Blocks();
    delete statusBar;
    for (auto& m : mBoxes)
        delete m;
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Game*/
Arkanoid::Game::Game()
{
    history = new History();
    window = NULL;
    view = NULL;
    nullization();
    init();
    sendEvent(this, OPEN_START_MENU);
    /**/
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
}

void Arkanoid::Game::resetStartMenu(resolution res, Difficulty diff)
{
    if (startMenu)
        delete startMenu;
    startMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    startMenu->setVisibility(false);

    auto* textField = new TextField();
    textField->setAllProperties("Arkanoid", totalProjectPath + "/Fonts/header.otf",
        sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS,
        sf::Color::White, ColorCustomPink);
    sf::Vector2f wCenter = DisplayObject::getRelativeCenter(startMenu->getGlobalBounds(), textField->getTextGlobal());
    textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y });
    textField->setScales(res);
    startMenu->addTextField(textField);

    wCenter = DisplayObject::getRelativeCenter(startMenu->getGlobalBounds(), { 0, 0, S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y });
    std::vector<Button*> buttons;
    buttons.push_back(new Button(startMenu, "START", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , wCenter.y }, EventType::OPEN_GAME_FIELD));
    buttons.push_back(new Button(startMenu, "SETTINGS", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 1.2f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::OPEN_SETTINGS));
    buttons.push_back(new Button(startMenu, "LOAD THE GAME", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 2.4f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::LOAD_THE_GAME));
    buttons.push_back(new Button(startMenu, "QUIT", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 3.6f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::QUIT));

    for (auto& button : buttons)
    {
        button->setScales(res);
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        startMenu->addButton(button);
    }
}

void Arkanoid::Game::resetSettingsMenu(resolution res, Difficulty diff)
{
    if (settingsMenu)
        delete settingsMenu;
    settingsMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    settingsMenu->setVisibility(false);

    auto* textField = new TextField;
    textField->setAllProperties("Settings", totalProjectPath + "/Fonts/header.otf",
        sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS,
        sf::Color::White, ColorCustomPink);

    sf::Vector2f wCenter = DisplayObject::getRelativeCenter(settingsMenu->getGlobalBounds(), textField->getTextGlobal());
    textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y });
    textField->setScales(res);
    settingsMenu->addTextField(textField);

    /*Subheaders*/
    auto* changeResolutionField = new TextField;
    auto* changeDifficultyField = new TextField;

    changeResolutionField->setAllProperties("CHANGE THE RESOLUTION", totalProjectPath + "/Fonts/simple.ttf",
        sf::Text::Bold, SUB_HEADER_TEXT_SIZE * res.y, SUB_HEADER_BORDERTHICKNESS,
        sf::Color::White, ColorCustomPink);
    changeDifficultyField->setAllProperties("CHANGE THE DIFFICULTY", totalProjectPath + "/Fonts/simple.ttf",
        sf::Text::Bold, SUB_HEADER_TEXT_SIZE * res.y, SUB_HEADER_BORDERTHICKNESS,
        sf::Color::White, ColorCustomPink);

    float subHeaderHeight = changeDifficultyField->getTextGlobal().height;

    sf::FloatRect headerPosition = textField->getTextGlobal();
    float headerButtom = headerPosition.top + headerPosition.height;

    changeResolutionField->setTextPosition({ headerPosition.left / 4, headerButtom + subHeaderHeight * 3 });
    changeResolutionField->setScales(res);
    settingsMenu->addTextField(changeResolutionField);

    changeDifficultyField->setTextPosition({ headerPosition.left / 4, headerButtom + subHeaderHeight * 8 });
    changeDifficultyField->setScales(res);
    settingsMenu->addTextField(changeDifficultyField);

    std::vector<Button*> buttons;
    buttons.push_back(new Button(settingsMenu, Settings::resToStr(res), { ST_BUTTON_WIDTH * res.x, 2 * subHeaderHeight }, { headerPosition.left / 4 ,  headerButtom + subHeaderHeight * 5 }, EventType::RES_CHANGE));
    buttons.push_back(new Button(settingsMenu, Settings::diffToStr(diff), { ST_BUTTON_WIDTH * res.x, 2 * subHeaderHeight }, { headerPosition.left / 4 ,  headerButtom + subHeaderHeight * 10 }, EventType::DIFF_CHANGE));
    buttons.push_back(new Button(settingsMenu, "SAVE SETTINGS", { ST_BUTTON_WIDTH * res.x, 2 * subHeaderHeight }, { headerPosition.left / 4 ,  headerButtom + subHeaderHeight * 14 }, EventType::SAVE_SETTINGS));
    buttons.push_back(new Button(settingsMenu, "BACK TO THE MAIN MENU", { ST_BUTTON_WIDTH * res.x, 2 * subHeaderHeight }, { headerPosition.left / 4 ,  headerButtom + subHeaderHeight * 18 }, EventType::OPEN_START_MENU));

    for (auto& button : buttons)
    {
        button->setTextSize(ST_BUTTON_TEXT_SIZE * res.y);
        button->setScales(res);
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        settingsMenu->addButton(button);
    }
}

void Arkanoid::Game::resetWaitMenu(resolution res, Difficulty diff)
{
    if (waitMenu)
        delete waitMenu;
    waitMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    waitMenu->setVisibility(false);

    auto* textField = new TextField;
    textField->setAllProperties("Pause", totalProjectPath + "/Fonts/header.otf",
        sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS,
        sf::Color::White, ColorCustomPink);

    sf::Vector2f wCenter = DisplayObject::getRelativeCenter(waitMenu->getGlobalBounds(), textField->getTextGlobal());
    textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y });
    textField->setScales(res);
    waitMenu->addTextField(textField);

    wCenter = DisplayObject::getRelativeCenter(waitMenu->getGlobalBounds(), { 0, 0, S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y });
    std::vector<Button*> buttons;
    buttons.push_back(new Button(waitMenu, "CONTINUE", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , wCenter.y }, EventType::OPEN_GAME_FIELD));
    buttons.push_back(new Button(waitMenu, "RESTART", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 1.2f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::RESTART_OPEN_GAME_FIELD));
    buttons.push_back(new Button(waitMenu, "SAVE", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 2.4f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::SAVE_GAME));
    buttons.push_back(new Button(waitMenu, "BACK TO MAIN MENU", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 3.6f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::OPEN_START_MENU));
    buttons.push_back(new Button(waitMenu, "QUIT", { S_BUTTON_WIDTH * res.x, S_BUTTON_HEIGHT * res.y }, { wCenter.x , 4.8f * S_BUTTON_HEIGHT * res.y + wCenter.y }, EventType::QUIT));

    for (auto& button : buttons)
    {
        button->setScales(res);
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        waitMenu->addButton(button);
    }
}

void Arkanoid::Game::resetGameField(resolution res, Difficulty diff)
{
    if (gameField)
        delete gameField;
    gameField = new GameField(this, res, { 0, 0 }, ColorCustomLightBlue);
    gameField->setVisibility(false);

    /*Init Player*/
    players.clear();

    Player* player = new Player();
    Platform* platform = new Platform(gameField, res, { PlATFORM_WIDTH * res.x ,PlATFORM_HEIGHT * res.y }, { PLATFORM_WIDTH_INIT_OFFSET * res.x,PLATFORM_HEIGHT_INIT_OFFSET * res.y }, sf::Color::Black, sf::Vector2f(PLATFORM_SPEED.x * res.x, PLATFORM_SPEED.y * res.y));
    player->setPlatform(platform);
    gameField->displayObjects.push_back(platform);
    gameField->movableObjects.push_back(platform);

    this->players.addPlayer(player);
    this->players.setParent(this);
    this->gameField->statusBar->setData(player->getStatistics()); 

    /*Init blocks*/
    float blockWidth = 0, blockHeight = 0;
    uint32_t blockRows, blockColumns;
    {
        switch (diff)
        {
        case HARD:
            blockWidth = res.x / BLOCK_NUM_WIDTH_HARD;
            blockHeight = (0.4 * res.y - STATUS_BAR_HEIGHT) / BLOCK_NUM_HEIGHT_HARD;
            blockRows = BLOCK_NUM_WIDTH_HARD;
            blockColumns = BLOCK_NUM_HEIGHT_HARD;
            break;
        case MEDIUM:
            blockWidth = res.x / BLOCK_NUM_WIDTH_MEDIUM;
            blockHeight = (0.4 * res.y - STATUS_BAR_HEIGHT) / BLOCK_NUM_HEIGHT_MEDIUM;
            blockRows = BLOCK_NUM_WIDTH_MEDIUM;
            blockColumns = BLOCK_NUM_HEIGHT_MEDIUM;
            break;
        case LOW:
            blockWidth = res.x / BLOCK_NUM_WIDTH_LOW;
            blockHeight = (0.4 * res.y - STATUS_BAR_HEIGHT) / BLOCK_NUM_HEIGHT_LOW;
            blockRows = BLOCK_NUM_WIDTH_LOW;
            blockColumns = BLOCK_NUM_HEIGHT_LOW;
            break;
        default:
            blockWidth = 0;
            blockHeight = 0;
            break;
        }
    }
    float offsetX = blockWidth / blockRows;
    float offsetY = blockHeight / blockColumns;
    blockRows--;
    blockColumns--;
    for (int i = 0; i < blockRows; i++)
    {
        for (int j = 0; j < blockColumns; j++)
        {
            Block* block = new Block(gameField, res, { blockWidth, blockHeight }, { blockWidth * i + offsetX * (i + 1), blockHeight * j + offsetY * (j + 1) + STATUS_BAR_HEIGHT * res.y }, BlockState::NOT_ATTACKED);
            block->InitDefaultBonus(rng, unif);
            gameField->blocks.addBlock(block);
            gameField->displayObjects.push_back(block);
        }
    }

    /*Init ball*/
    Ball* ball = new Ball(gameField, res, sqrt(BALL_SQUARE * res.x * res.y), { BALL_WIDTH_INIT_OFFSET * res.x, BALL_HEIGHT_INIT_OFFSET * res.y }, sf::Color::Black, sf::Vector2f(BALL_SPEED_MID.x * res.x, BALL_SPEED_MID.y * res.y));
    gameField->balls.addBall(ball);
    gameField->displayObjects.push_back(ball);
    gameField->movableObjects.push_back(ball);
}

void Arkanoid::Game::resetMenus(resolution res, Difficulty diff)
{
    resetStartMenu(res, diff);
    resetWaitMenu(res, diff);
    resetSettingsMenu(res, diff);
}

void Arkanoid::Game::nullization()
{
    startMenu = settingsMenu = waitMenu = NULL;
    gameField = NULL;
}

void Arkanoid::Game::init()
{
    sf::Vector2f res = Settings::getResolution();
    Difficulty   diff = Settings::getDifficulty();

    resetMenus(res, diff);
    resetGameField(res, diff);

    if (window && view)
    {
        view->reset(sf::FloatRect(0, 0, res.x, res.y));
        window->setSize({ (uint32_t)res.x, (uint32_t)res.y });
    }
}

void Arkanoid::Game::start()
{
    sf::Vector2f res = Settings::getResolution();
    Difficulty   diff = Settings::getDifficulty();

    window = new sf::RenderWindow(sf::VideoMode((int)res.x, (int)res.y), "Arkanoid", sf::Style::Titlebar | sf::Style::Close);
    window->setPosition({ 0, 0 });
    view = new sf::View(window->getDefaultView());
    window->setVisible(true);
    window->setVerticalSyncEnabled(true);

    while (window->isOpen()) {
        clock.restart();
        sendEvent(this, DRAW_FRAME);
        eventsHadler(); 
        int slp = cps - clock.getElapsedTime().asMicroseconds();
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(slp, 0)));
    }
}

void Arkanoid::Game::eventsHadler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case OPEN_START_MENU:
            resetGameField(Settings::getResolution(), Settings::getDifficulty());
            startMenu->setVisibility(true);
            break;
        case DRAW_FRAME:
        {
            if (gameField->isVisible())
            {
                gameField->display(window, view);
            }
            else if (startMenu->isVisible())
            {
                startMenu->display(window, view);
                gameField->draw(window, view);
                startMenu->draw(window, view);
            }
            else if (settingsMenu->isVisible())
            {
                settingsMenu->display(window, view);
                gameField->draw(window, view);
                settingsMenu->draw(window, view);
            }
            else if (waitMenu->isVisible())
            {
                waitMenu->display(window, view);
                gameField->draw(window, view);
                waitMenu->draw(window, view);
            }
            players.eventsHandler();
            window->display();
            clock.restart();
            break;
        }
        case SAVE_SETTINGS:
        {
            resolution res = *((resolution*)event.data->at(0));
            Difficulty diff = *((Difficulty*)event.data->at(1));
            settings.setDifficulty(diff);
            settings.setResolution(res);

            event.data->clear();
            delete event.data;

            this->reset();
            settingsMenu->setVisibility(true);
            break;
        }
        case OPEN_SETTINGS:
            settingsMenu->setVisibility(true);
            break;
        case OPEN_GAME_FIELD:
            gameField->setVisibility(true);
            break;
        case QUIT:
            window->close();
            break;
        case OPEN_WAIT_MENU:
            waitMenu->setVisibility(true);
            break;
            //Events from gamefield
        case LIVES_DOWN:
            sendEvent(&players, LIVES_DOWN);
            break;
        case RESET_PLATFORMS_TO_INITIAL_STATE:
            sendEvent(&players, RESET_PLATFORMS_TO_INITIAL_STATE);
            break;
        case SCORE_UP_FOR_HITTING:
            sendEvent(&players, { SCORE_UP_FOR_HITTING, event.data });
            break;
        case SCORE_UP_FOR_CRASHING:
            sendEvent(&players, { SCORE_UP_FOR_CRASHING, event.data });
            break;
        case PLAYER_IS_LOSE:
            if (players.numOfPlayers() - 1 > 0)
                sendEvent(gameField, { PLAYER_IS_LOSE, event.data });
            else
                sendEvent(gameField, { SHOW_END_MESSAGE });
            break;
        case RESTART_OPEN_GAME_FIELD:
        case RESET_GAME:
            resetGameField(Settings::getResolution(), Settings::getDifficulty());
            gameField->setVisibility(true);
            break;
        case REFORM_AND_APPLY_BONUS: 
            {
            Platform* platform = (Platform*)event.data->at(0); 
            Bonus* bonus = (Bonus*)event.data->at(1);
            int* index = new int; 
            *index = players.getPlayerIndex(platform);
            BonusDescriptor* bd = new BonusDescriptor;
            *bd = bonus->getBonusDescriptor(); 
            event.data->at(0) = index; 
            event.data->at(1) = bd; 
            gameField->addNewBonus(*index, *bd); 
            sendEvent(&players, BONUS_UP);
            }
        case APPLY_BONUSES:
            this->bonusesSolution(event.data);
            break;
        case SAVE_GAME:
            history->to_json(gameField->bonusesTimers, settings, players, gameField->balls, gameField->blocks);
            history->writeData(gameField->bonusesTimers, settings, players, gameField->balls, gameField->blocks);
            waitMenu->setVisibility(true);
            break;
        case LOAD_THE_GAME:
            history->readData(this);
            //history->from_json(this);
            gameField->setVisibility(true);
            break;
        default:
            break;
        }
    }
}

void Arkanoid::Game::bonusesSolution(std::vector<void*>* eventData)
{
    int index = *((int*)eventData->at(0)); 
    Platform* initiator = players.players[index]->getPlatform();
    BonusDescriptor bd = *((BonusDescriptor*)eventData->at(1));
    eventData->at(0) = initiator; 

    switch (bd)
    {
    case BONUS_PLATFORM_LENGTH_UP:
        sendEvent(&players, { PLATFORM_LENGTH_UP, eventData });
        break;
    case BONUS_PLATFORM_LENGTH_DOWN:
        sendEvent(&players, { PLATFORM_LENGTH_DOWN, eventData });
        break;
    case BONUS_PLATFORM_SPEED_DOWN:
        sendEvent(&players, { PLATFORM_SPEED_DOWN, eventData });
        break;
    case BONUS_PLATFORM_SPEED_UP:
        sendEvent(&players, { PLATFORM_SPEED_UP, eventData });
        break;
    case BONUS_BALL_SPEED_UP:
        sendEvent(gameField, BALL_SPEED_UP);
        delete eventData; 
        break;
    case BONUS_BALL_SPEED_DOWN:
        sendEvent(gameField, BALL_SPEED_DOWN);
        delete eventData;
        break;
    default:
        break;
    }
}

void Arkanoid::Game::reset()
{
    delete startMenu;
    delete settingsMenu;
    delete waitMenu;
    delete gameField;
    players.clear();

    this->nullization();
    this->init();
}

Arkanoid::Game::~Game()
{
    delete startMenu;
    delete settingsMenu;
    delete waitMenu;
    delete gameField;
    players.clear();
    delete window;
    delete view;
}

