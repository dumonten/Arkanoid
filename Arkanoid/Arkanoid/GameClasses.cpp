#include "GameClasses.hpp"

using namespace Arkanoid;

std::mt19937_64 random;
std::uniform_real_distribution<double> range_for_ramdom(0, 100);

/*---------------------------------------------------------------------/*---------------------------------------------------------------------DisplayObject*/
sf::Vector2f DisplayObject::getRelativeCenter(sf::FloatRect parentSize, sf::FloatRect childSize)
{
    return { parentSize.left + parentSize.width / 2 - childSize.width / 2,parentSize.top + parentSize.height / 2 - childSize.height / 2 };
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------EventObject*/
bool EventObject::pollEvent(EventType& event)
{
    if (!eventsQueue.empty())
    {
        event = eventsQueue.front().event;
        eventsQueue.pop();
        return true;
    }
    return false;
}

bool EventObject::pollEvent(doEvent& event)
{
    if (!eventsQueue.empty())
    {
        event = eventsQueue.front();
        eventsQueue.pop();
        return true;
    }
    return false;
}

void EventObject::clearQueue()
{
    std::queue<doEvent > empty;
    std::swap(eventsQueue, empty);
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------TextField*/
TextField::TextField()
{
    setDefaultProperties();
}

TextField::TextField(const std::string& txt, sf::Font& fnt, uint32_t txtStyle, uint32_t size, float thickness, sf::Color normal, sf::Color border)
{
    text.setFont(font = fnt);
    text.setString(str = txt);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal = normal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder = border);
}

void TextField::setDefaultProperties()
{
    clrNormal = sf::Color::White;
    clrBorder = sf::Color::Black;
    borderThickness = 0;
    
    text.setString(str = "Click me!");
    text.setFont(font = *Constants::FONT_BODY);
    text.setStyle(textStyle = sf::Text::Bold);
    text.setCharacterSize(charSize = 10);
    text.setFillColor(clrNormal);
}

void TextField::setAllProperties(const std::string& txt, const sf::Font& fnt, uint32_t txtStyle, uint32_t size, float thickness,
    sf::Color normal, sf::Color border)
{
    text.setString(str = txt);
    text.setFont(font = fnt);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal = normal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder = border);
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Button*/
Button::Button(const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : DisplayObject(s, p)
{
    setDefaultProperties();
    selfRect->setOutlineThickness(borderThickness = 0.02f * s.x);
    textfield->setCharSize(s.x * 0.08f);
    textfield->setText(name);
    textfield->setTextPosition(DisplayObject::getRelativeCenter(selfRect->getGlobalBounds(), textfield->getTextGlobal()));
    textfield->setScales(s);
    event = evnt;
}

Button::Button(EventObject* parent, const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : Button(name, s, p, evnt)
{
    ptrParent = parent;
}

void Button::setDefaultProperties()
{
    clrBgNormal = ColorCustomGreen;
    clrBgClicked = ColorCustomGreen;
    clrBgHovered = ColorCustomDarkGreen;
    clrBorder = sf::Color::Yellow;
    
    selfRect->setFillColor(clrBgNormal);
    selfRect->setOutlineColor(clrBorder);
    state = BtnState::NORMAL;
    textfield = new TextField();
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states)const
{
    target.draw(*selfRect, states);
    textfield->update(size);
    if (textfield->isEmpty() != 0)
        target.draw(*textfield, states);
}

void Button::update(sf::Event& e, sf::RenderWindow& window)
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

Button::~Button()
{
    delete textfield;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Menu*/
Menu::Menu(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
}

Menu::Menu(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : Menu(s, p, color)
{
    ptrParent = parent;
}

void Menu::eventsHandler()
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
        case OPEN_FULL_SCREEN:
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

void Menu::display(sf::RenderWindow* window, sf::View* view)
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

Menu::~Menu()
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
Messagebox::Messagebox(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
}

Messagebox::Messagebox(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : Messagebox(s, p, color)
{
    ptrParent = parent;
}

void Messagebox::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case CLOSE_MBOX:
        {
            clearQueue();
            std::vector<void*>* data = new std::vector<void*>;
            data->push_back(this);
            sendEvent(ptrParent, { CLOSE_MBOX, data });
            hide();
            break;
        }
        default:
            break;
        }
    }
}

Messagebox::~Messagebox()
{
    for (auto& t : textFields)
    {
        delete t;
    }
    for (auto& m : messageboxItems)
    {
        delete m;
    }
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------StatusBar*/
StatusBar::StatusBar(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, sf::Color c) : DisplayObject(s, p, s_parent)
{
    selfRect->setFillColor(c);

    textField = new TextField("", *Constants::FONT_HEADER,  sf::Text::Style::Bold, STATUS_BAR_BUTTON_TEXT_FONT_SIZE * s_parent.y, 1, ColorCustomMidPurpleBlueUltraDark, ColorCustomMidPurpleBlueLight);
    textField->setTextPosition({ STATUS_BAR_TEXT_OFFSET_LEFT * s_parent.x, STATUS_BAR_TEXT_OFFSET_TOP * s_parent.y });
    textField->setScales(s);

    data = NULL;
    menuButton = NULL;
}

void StatusBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*selfRect, states);
    target.draw(*menuButton, states);
    target.draw(*textField, states);
}

StatusBar::~StatusBar()
{
    if (data != NULL)
        data = NULL;
    if (_CrtIsValidHeapPointer(textField))
        delete textField;
    if (_CrtIsValidHeapPointer(menuButton))
        delete menuButton;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Platform*/
Platform::Platform(sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : MovableObject(s, p, s_parent, sp)
{
    selfRect->setFillColor(c);
    initPosition = { PLATFORM_WIDTH_INIT_OFFSET * s_parent.x,PLATFORM_HEIGHT_INIT_OFFSET * s_parent.y };
}

Platform::Platform(EventObject* parent, sf::Vector2f s_parent, const sf::Vector2f& s, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : Platform(s_parent, s, p, c, sp)
{
    ptrParent = parent;
}

void Platform::eventsHandler()
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

std::string Platform::to_string()
{
    std::string data = ""; 

    data = data + "\tposition_x:" + " " + std::to_string(position.x) + "\n";
    data = data + "\tposition_y:" + " " + std::to_string(position.y) + "\n";
    data = data + "\tsize_x:" + " " + std::to_string(size.x) + "\n";
    data = data + "\tsize_y:" + " " + std::to_string(size.y) + "\n";
    data = data + "\tspeed_x:" + " " + std::to_string(speed.x) + "\n";
    data = data + "\tspeed_y:" + " " + std::to_string(speed.y) + "\n";
    data = data + "\tvisibility:" + " " + std::to_string(visibility) + "\n";

    return data; 
}

json Platform::to_json()
{
    json jresult; 
    
    jresult = {
            {"position_x", position.x},
            {"position_y", position.y},
            {"size_x", size.x},
            {"size_y", size.y},
            {"speed_x", speed.x},
            {"speed_y", speed.y},
            {"visibility", visibility}
    };

    return jresult; 
}

SaveloadObject* Platform::from_string(std::stringstream& data)
{
    std::string buffer;
    sf::Vector2f position, size, speed;
    bool visibility;
    
    Proxy::simpleRead(data, buffer);
    
    Proxy::pathThrough(data, buffer);
    position.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    position.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    size.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    size.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    speed.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    speed.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    visibility = stoi(buffer);

    Platform* obj = new Platform(nullptr, Settings::getResolution(), size, position, sf::Color::Black, speed);
    obj->setVisibility(visibility);
  
    return obj;
}

SaveloadObject* Platform::from_json(json& data)
{
    sf::Vector2f position, size, speed;
    bool visibility;

    position.x = data["position_x"].get<float>();
    position.y = data["position_y"].get<float>();
    size.x = data["size_x"].get<float>();
    size.y = data["size_y"].get<float>();
    speed.x = data["speed_x"].get<float>();
    speed.y = data["speed_y"].get<float>();
    visibility = data["visibility"].get<float>();
    
    Platform* obj = new Platform(nullptr, Settings::getResolution(), size, position, sf::Color::Black, speed);
    obj->setVisibility(visibility);
    
    return obj; 
}

Platform::~Platform() = default;

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Block*/
Block::Block(sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st)
    : DisplayObject(s, p, s_parent)
{
    clrNormal = ColorCustomDarkGreen;
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
        selfRect->setFillColor(clrNormal);
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
        selfRect->setFillColor(clrNormal);
        break;
    }
}

void Block::initBonuses(std::mt19937_64& rng, std::uniform_real_distribution<double>& unif)
{
    int bound = 0;
    switch (Settings::getDifficulty())
    {
    case HARD:
        bound = BONUS_PERCANTAGE_HARD;
        break;
    case MEDIUM:
        bound = BONUS_PERCANTAGE_MEDIUM;
        break;
    case LOW:
        bound = BONUS_PERCANTAGE_LOW;
        break;
    case KID:
        bound = BONUS_PERCANTAGE_KID;
        break;
    case IMPOSSIBLE:
        bound = BONUS_PERCANTAGE_IMPOSSIBLE;
        break;
    default:
        break;
    }
    int probability = unif(rng);
    if (probability <= bound)
    {
        probability = (int)unif(rng) % BonusDescriptor::ebonusSize;
        BonusDescriptor descr = BonusDescriptor(probability);
        bonuses.push_back(new Bonus(size, position, descr));
    }
    {
        std::mt19937_64 rngBonus;
        std::uniform_real_distribution<double> unifBonus(10, 30);
        {
            uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
            rngBonus.seed(ss);
        }
        int bonusScore = unifBonus(rngBonus);

        Bonus* bonus = new Bonus(size, position, BonusDescriptor::BONUS_SCORE_UP);
        bonus->setBonusScoreValue(bonusScore);
        bonuses.push_back(bonus);
    }
}

Block::Block(EventObject* parent, sf::Vector2f s_parent, sf::Vector2f s, sf::Vector2f p, BlockState st) : Block(s_parent, s, p, st)
{
    ptrParent = parent;
}

void Block::trigger()
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

void Block::eventsHandler()
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

std::string Block::to_string()
{
    std::string data = "";
    data = data + "position_x:" + " " + std::to_string(position.x) + "\n";
    data = data + "position_y:" + " " + std::to_string(position.y) + "\n";
    data = data + "size_x:" + " " + std::to_string(size.x) + "\n";
    data = data + "size_y:" + " " + std::to_string(size.y) + "\n";
    data = data + "lives:" + " " + std::to_string(numOfLives) + "\n";
    data = data + "visibility:" + " " + std::to_string(visibility) + "\n";
    data = data + "cant_break:" + " " + std::to_string(cantBreak) + "\n";
    data = data + "bonuses:" + " " + "num:" + " " + std::to_string(bonuses.size()) + "\n";
    for (int i = 0, n = bonuses.size(); i < n; i++)
    {
        data = data + "\tposition_x:" + " " + std::to_string(bonuses[i]->getPosition().x) + "\n";
        data = data + "\tposition_y:" + " " + std::to_string(bonuses[i]->getPosition().y) + "\n";
        data = data + "\tsize_x:" + " " + std::to_string(bonuses[i]->getSize().x) + "\n";
        data = data + "\tsize_y:" + " " + std::to_string(bonuses[i]->getSize().y) + "\n";
        data = data + "\tvisibility:" + " " + std::to_string(bonuses[i]->isVisible()) + "\n";
        data = data + "\tdescriptor:" + " " + std::to_string(bonuses[i]->descriptor) + "\n";
        data = data + "\tbonuses_score_value:" + " " + std::to_string(bonuses[i]->bonusScoreValue) + "\n";
    }
    return data; 
}

json Block::to_json()
{
    json jbonuses;
    json jresult;

    for (int i = 0, n = bonuses.size(); i < n; i++)
    {
        jbonuses["bonuses"].push_back
        ({
            {"position_x",   bonuses[i]->getPosition().x},
            {"position_y",   bonuses[i]->getPosition().y},
            {"size_x",       bonuses[i]->getSize().x},
            {"size_y",       bonuses[i]->getSize().y},
            {"visibility",   bonuses[i]->isVisible()},
            {"descriptor",   bonuses[i]->descriptor},
            {"bonus_score_value",   bonuses[i]->bonusScoreValue}
            });
    }
    jresult = {
        {"position_x",  position.x},
        {"position_y",  position.y},
        {"size_x", size.x},
        {"size_y", size.y},
        {"lives", numOfLives},
        {"visibility", visibility},
        {"cant_break", cantBreak},
        {"num_of_bonuses", bonuses.size()},
        {"bonuses", jbonuses["bonuses"]}
        };

    return jresult; 
}

SaveloadObject* Block::from_string(std::stringstream& data)
{
    sf::Vector2f position, size;
    int lives;
    BlockState blockState;
    bool visibility, cantBreak;
    std::string buffer; 

    Proxy::pathThrough(data, buffer);
    position.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    position.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    size.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    size.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    lives = stoi(buffer);
    switch (lives)
    {
    case 0:
        blockState = BlockState::CRASHED;
        break;
    case 1:
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

    Proxy::pathThrough(data, buffer);
    visibility = stoi(buffer);
    
    Proxy::pathThrough(data, buffer);
    cantBreak = stoi(buffer);
    
    Block* obj = new Block(nullptr, Settings::getResolution(), size, position, blockState);
    obj->setVisibility(visibility);
    obj->cantBreak = cantBreak;

    Proxy::pathThrough(data, buffer);
    Proxy::simpleRead(data, buffer);
    size_t numOfBonuses = stoi(buffer);
    for (size_t b = 0; b < numOfBonuses; b++)
    {
        sf::Vector2f bposition, bsize;
        bool bvisibility;
        BonusDescriptor descriptor;
        int bonusScoreValue; 

        Proxy::pathThrough(data, buffer);
        bposition.x = stof(buffer);
        Proxy::pathThrough(data, buffer);
        bposition.y = stof(buffer);

        Proxy::pathThrough(data, buffer);
        bsize.x = stof(buffer);
        Proxy::pathThrough(data, buffer);
        bsize.y = stof(buffer);

        Proxy::pathThrough(data, buffer);
        bvisibility = stoi(buffer);
        Proxy::pathThrough(data, buffer);
        descriptor = BonusDescriptor(stoi(buffer));
        Proxy::pathThrough(data, buffer);
        bonusScoreValue = BonusDescriptor(stoi(buffer));

        Bonus* bonus = new Bonus(bsize, bposition);
        bonus->setVisibility(bvisibility);
        bonus->setBonusDescriptor(descriptor);
        if (bonusScoreValue != 0) bonus->setBonusScoreValue(bonusScoreValue);
        obj->bonuses.push_back(bonus);
    }

    return obj; 
}

SaveloadObject* Block::from_json(json& data)
{
    sf::Vector2f position, size;
    int lives;
    BlockState blockState;

    bool visibility, cantBreak;

    position.x = data["position_x"].get<float>();
    position.y = data["position_y"].get<float>();

    size.x = data["size_x"].get<float>();
    size.y = data["size_y"].get<float>();

    visibility = data["visibility"].get<float>();
    cantBreak = data["cant_break"].get<float>();

    lives = data["lives"].get<float>();

    switch (lives)
    {
    case 0:
        blockState = BlockState::CRASHED;
        break;
    case 1:
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

    Block* obj = new  Block(nullptr, Settings::getResolution(), size, position, blockState);
    obj->setVisibility(visibility);
    obj->cantBreak = cantBreak;
    size_t numOfBonuses = data["num_of_bonuses"].get<size_t>();
    for (size_t b = 0; b < numOfBonuses; b++)
    {
        sf::Vector2f bposition, bsize;
        bool bvisibility;
        BonusDescriptor descriptor;
        int bonusScoreValue; 

        bposition.x = data["bonuses"][b]["position_x"].get<float>();
        bposition.y = data["bonuses"][b]["position_y"].get<float>();

        bsize.x = data["bonuses"][b]["size_x"].get<float>();
        bsize.y = data["bonuses"][b]["size_y"].get<float>();

        bvisibility = data["bonuses"][b]["visibility"].get<float>();
        descriptor = (BonusDescriptor)data["bonuses"][b]["descriptor"].get<float>();
        bonusScoreValue = data["bonuses"][b]["bonus_score_value"].get<int>();

        Bonus* bonus = new Bonus(bsize, bposition);
        bonus->setVisibility(bvisibility);
        bonus->setBonusDescriptor(descriptor);
        if (bonusScoreValue != 0) bonus->setBonusScoreValue(bonusScoreValue);
        obj->bonuses.push_back(bonus);
    }

    return obj; 
}

Block::~Block()
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
Bonus::Bonus(sf::Vector2f s_parent, sf::Vector2f p_parent, BonusDescriptor descr)
    : MovableObject({ 0.8f * s_parent.x, 0.8f * s_parent.y }, { p_parent.x + 0.1f * s_parent.x, p_parent.y })
{
    setVisibility(false);
    if (descriptor == BonusDescriptor::BONUS_SCORE_UP)
        setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y - 4 }); 
    else 
        setSpeed({ BONUS_SPEED.x * Settings::getResolution().x,BONUS_SPEED.y * Settings::getResolution().y });
    setBonusDescriptor(descr);
}

void Bonus::draw(sf::RenderTarget& target, sf::RenderStates states)const
{
    if (textFieldBonusScoreFall != NULL) //для бонусов со скором (не у всех)
        target.draw(*textFieldBonusScoreFall);
    else 
        target.draw(*selfRect);
}

void Bonus::generateTextField(sf::Vector2f blockSize, int score)
{
    textFieldBonusScoreFall = new TextField(std::to_string(bonusScoreValue), *Constants::FONT_HEADER, sf::Text::Bold, blockSize.x * 0.3f, 0, sf::Color::Black, sf::Color::Black);
    textFieldBonusScoreFall->setTextPosition(DisplayObject::getRelativeCenter(selfRect->getGlobalBounds(), textFieldBonusScoreFall->getTextGlobal()));
    textFieldBonusScoreFall->setScales(blockSize);
}

void  Bonus::move()
{
    if (textFieldBonusScoreFall != NULL)
    {
        textFieldBonusScoreFall->text.move(speed.x, 0);
        textFieldBonusScoreFall->text.move(0, speed.y);
    }
    selfRect->move(speed.x, 0);
    selfRect->move(0, speed.y);
    setPosition(selfRect->getPosition());
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Player*/
Player::Player() {

}

std::string Player::to_string()
{
    std::string data = ""; 

    data = data + "name:" + " " + statistics.userName + "\n";
    data = data + "num_of_lives:" + " " + std::to_string(statistics.numOfLives) + "\n";
    data = data + "score:" + " " + std::to_string(statistics.score) + "\n";
    data = data + "num_of_bonuses:" + " " + std::to_string(statistics.numOfBonuses) + "\n";
    data = data + "num_of_crashed_blocks:" + " " + std::to_string(statistics.numOfCrashedBlocks) + "\n";
    data = data + "platform:" + " " + "\n";
    data = data + platform->to_string(); 

    return data; 
}

json Player::to_json()
{
    json jresult; 

    jresult = {
        {"name", statistics.userName},
        {"num_of_lives", statistics.numOfLives},
        {"score", statistics.score},
        {"num_of_bonuses", statistics.numOfBonuses},
        {"num_of_crashed_blocks", statistics.numOfCrashedBlocks},
        {"platform", platform->to_json()}
    }; 

    return jresult; 
}

SaveloadObject* Player::from_string(std::stringstream& data)
{
    std::string buffer; 
    Player* obj = new Player();
    Statistics st;
    Proxy::pathThrough(data, buffer);
    st.userName = buffer;
    Proxy::pathThrough(data, buffer);
    st.numOfLives = std::stoi(buffer);
    Proxy::pathThrough(data, buffer);
    st.score = std::stoi(buffer);
    Proxy::pathThrough(data, buffer);
    st.numOfBonuses = std::stoi(buffer);
    Proxy::pathThrough(data, buffer);
    st.numOfCrashedBlocks = std::stoi(buffer);
    obj->setStatistics(st);

    Platform temp_platform; 
    obj->platform = dynamic_cast<Platform*>(temp_platform.from_string(data));

    return obj; 
}

SaveloadObject* Player::from_json(json& data)
{
    Player* obj = new Player();
    
    Statistics st;
    
    st.userName = data["name"].get<std::string>();
    st.numOfLives = data["num_of_lives"].get<uint64_t>();
    st.score = data["score"].get<uint64_t>();
    st.numOfBonuses = data["num_of_bonuses"].get<uint64_t>();
    st.numOfCrashedBlocks = data["num_of_crashed_blocks"].get<uint64_t>();
    obj->setStatistics(st);

    Platform temp_platform; 
    obj->platform = dynamic_cast<Platform *>(temp_platform.from_json(data["platform"]));

    return obj; 
}

Player::~Player()
{
    if (_CrtIsValidHeapPointer(platform))
        delete platform;
}

/*---------------------------------------------------------------------/*---------------------------------------------------------------------Player*/
Ball::Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c) : MovableObject(radius, p, s_parent, { 0, 0 })
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
    case KID:
        new_speed.x = res.x * BALL_SPEED_KID.x;
        new_speed.y = res.x * BALL_SPEED_KID.y;
        break;
    case IMPOSSIBLE:
        new_speed.x = res.x * BALL_SPEED_IMPOSSIBLE.x;
        new_speed.y = res.x * BALL_SPEED_IMPOSSIBLE.y;
        break;
    default:
        break;
    }
    speed = new_speed;
    selfRect->setFillColor(c);
    initPosition = { BALL_WIDTH_INIT_OFFSET * s_parent.x, BALL_HEIGHT_INIT_OFFSET * s_parent.y };
}

Ball::Ball(sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : MovableObject(radius, p, s_parent, sp)
{
    Difficulty diff = Settings::getDifficulty();
    resolution res = Settings::getResolution();
    selfRect->setFillColor(c);
    initPosition = { BALL_WIDTH_INIT_OFFSET * s_parent.x, BALL_HEIGHT_INIT_OFFSET * s_parent.y };
}

Ball::Ball(EventObject* parent, sf::Vector2f s_parent, float radius, sf::Vector2f p, sf::Color c, sf::Vector2f sp) : Ball(s_parent, radius, p, c, sp)
{
    ptrParent = parent;
}

void Ball::eventsHandler()
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
                    this->move({ 0, -y });
                else
                    this->move({ 0, y });
                changeSpeedDirectionY();
            }
            delete event.data;
            break;
        }
        case FALL_DOWN:
            resetToInitPosition();
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

std::string Ball::to_string()
{
    std::string data = ""; 

    data = data + "position_x:" + " " + std::to_string(position.x)  + "\n";
    data = data + "position_y:" + " " + std::to_string(position.y)  + "\n";
    data = data + "size_x:" + " " + std::to_string(size.x) + "\n";
    data = data + "size_y:" + " " + std::to_string(size.y) + "\n";
    data = data + "speed_x:" + " " + std::to_string(speed.x) + "\n";
    data = data + "speed_y:" + " " + std::to_string(speed.y) + "\n";
    data = data + "visibility:" + " " + std::to_string(visibility) + "\n";
    
    return data; 
}

json Ball::to_json()
{
    json jresult;
    jresult = {
        { "position_x", position.x},
        { "position_y", position.y },
        { "size_x", size.x },
        { "size_y", size.y },
        { "speed_x", speed.x },
        { "speed_y", speed.y },
        { "visibility", visibility}
    };
    return jresult; 
}

SaveloadObject* Ball::from_string(std::stringstream& data)
{
    std::string buffer; 
    sf::Vector2f position, size, speed;
    bool visibility;

    Proxy::pathThrough(data, buffer);
    position.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    position.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    size.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    size.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    speed.x = stof(buffer);
    Proxy::pathThrough(data, buffer);
    speed.y = stof(buffer);

    Proxy::pathThrough(data, buffer);
    visibility = stoi(buffer);

    Ball* obj = new Ball(nullptr, Settings::getResolution(), size.x, position, sf::Color::Black, speed);
    obj->setVisibility(visibility);

    return obj; 
}

SaveloadObject* Ball::from_json(json& data)
{
    sf::Vector2f position, size, speed;
    bool visibility;

    position.x = data["position_x"].get<float>();
    position.y = data["position_y"].get<float>();

    size.x = data["size_x"].get<float>();
    size.y = data["size_y"].get<float>();

    speed.x = data["speed_x"].get<float>();
    speed.y = data["speed_y"].get<float>();

    visibility = data["visibility"].get<float>();

    Ball* obj = new Ball(nullptr, Settings::getResolution(), size.x, position, sf::Color::Black, speed);
    obj->setVisibility(visibility);

    return obj; 
}

Ball::~Ball() = default;
/*---------------------------------------------------------------------/*---------------------------------------------------------------------Ball*/
Balls::Balls() {

}

std::string Balls::to_string()
{
    std::string data = ""; 
    data = data + "balls:" + " " + "num:" + " " + std::to_string(balls.size()) + "\n";
    for (const auto& b : balls)
    {
        data = data + b->to_string();
        data = data + "\n";
    }
    data = data + "\n\n";
    return data; 
}

json Balls::to_json()
{
    json jresult; 
    jresult["num_of_balls"] = balls.size();
    for (const auto& b : balls)
        jresult["balls"].push_back(b->to_json());
    return jresult; 
}

SaveloadObject* Balls::from_string(std::stringstream& data)
{
    std::string buffer; 
    Proxy::simpleRead(data, buffer);

    Balls* obj = new Balls;
    Ball temp;

    Proxy::pathThrough(data, buffer);
    uint64_t numOfBalls = stoi(buffer);
    for (uint64_t i = 0; i < numOfBalls; i++)
    {
        Ball* ball = dynamic_cast<Ball*>(temp.from_string(data));
        obj->balls.push_back(ball);
    }
    return obj;
}

SaveloadObject* Balls::from_json(json& data)
{
    Balls* obj = new Balls;
    Ball temp;

    uint64_t numOfBalls = data["num_of_balls"].get<uint64_t>();
    for (uint64_t i = 0; i < numOfBalls; i++)
    {
        Ball* ball = dynamic_cast<Ball*>(temp.from_json(data["balls"][i]));
        obj->balls.push_back(ball);
    }
    return obj;
}

Balls::~Balls()
{
    for (auto& b : balls)
        delete b;
}

/*--------------------------------------------------------------------/*---------------------------------------------------------------------Players*/
Players::Players() {

}

void Players::eventsHandler()
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
                p->platform->resetToInitPosition(); 
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
                p->getStatistics()->numOfCrashedBlocks++; 
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
        case SCORE_UP:
        {
            Platform* searchPlatform = (Platform*)event.data->at(0);
            Player* p = getPlayer(searchPlatform);
            if (p)
            {
                Bonus* bonus = (Bonus*)event.data->at(1);
                p->getStatistics()->score += bonus->bonusScoreValue;
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

Player* Players::getPlayer(Platform* platform)
{
    for (auto& p : players)
    {
        if (p->platform == platform)
            return p;
    }
    return nullptr;
}

int Players::getPlayerIndex(Platform* platform)
{
    for (int i = 0, n = players.size(); i < n; i++)
    {
        if (players[i]->platform == platform)
            return i;
    }
    return -1;
}

std::string Players::to_string()
{
    std::string data = "";

    data = data + "players:" + " " + "num:" + " " + std::to_string(players.size()) + "\n";
    for (const auto& p : players)
    {
        data = data + p->to_string();
        data = data + "\n";
    }
    data = data + "\n\n";

    return data;
}

json Players::to_json()
{
    json jresult;
    jresult["num_of_players"] = players.size();
    for (const auto& p : players) 
        jresult["players"].push_back(p->to_json());
    return jresult; 
}

SaveloadObject* Players::from_string(std::stringstream& data)
{
    std::string buffer;
    Proxy::simpleRead(data, buffer);

    Players* obj = new Players;
    Player temp;
   
    Proxy::pathThrough(data, buffer);
    uint64_t numOfPlayers = stoi(buffer);
    for (uint64_t i = 0; i < numOfPlayers; i++)
    {
        Player* player = dynamic_cast<Player*>(temp.from_string(data));
        obj->players.push_back(player);
    }
    return obj;
}

SaveloadObject* Players::from_json(json& data)
{
    Players* obj = new Players;
    Player temp;

    uint64_t numOfPlayers = data["num_of_players"].get<uint64_t>();
    for (uint64_t i = 0; i < numOfPlayers; i++)
    {
        Player* player = dynamic_cast<Player*>(temp.from_json(data["players"][i]));
        obj->players.push_back(player);
    }
    return obj;
}

Players::~Players()
{
    clear();
}

/*--------------------------------------------------------------------/*---------------------------------------------------------------------Blocks*/
Blocks::Blocks()
{

}

std::string Blocks::to_string()
{
    std::string data;
    data = data + "blocks:" + " " + "num:" + " " + std::to_string(blocks.size()) + "\n";
    for (const auto& b : blocks)
    {
        data = data + b->to_string(); 
        data = data + "\n";
    }
    return data; 
}

json Blocks::to_json()
{
    json jresult; 

    jresult["num_of_blocks"] = blocks.size();
    for (const auto& b : blocks)
        jresult["blocks"].push_back(b->to_json());

    return jresult; 
}

SaveloadObject* Blocks::from_string(std::stringstream& data)
{
    std::string buffer;
    Proxy::simpleRead(data, buffer);

    Blocks* obj = new Blocks;
    Block temp;

    Proxy::pathThrough(data, buffer);
    uint64_t numOfBlocks = stoi(buffer);
    for (uint64_t i = 0; i < numOfBlocks; i++)
    {
        Block* block = dynamic_cast<Block*>(temp.from_string(data));
        obj->blocks.push_back(block);
    }
    return obj;
}

SaveloadObject* Blocks::from_json(json& data)
{
    Blocks* obj = new Blocks;
    Block temp;

    uint64_t numOfBlocks = data["num_of_blocks"].get<uint64_t>();
    for (uint64_t i = 0; i < numOfBlocks; i++)
    {
        Block* block = dynamic_cast<Block*>(temp.from_json(data["blocks"][i]));
        obj->blocks.push_back(block);
    }
    return obj;
}

Blocks::~Blocks()
{
    for (auto& b : blocks)
        delete b;
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Proxy*/
std::string Proxy::to_string(std::vector <SaveloadObject*>& objectsToSave) {
    std::stringstream strStream;
    for (int i = 0; i < objectsToSave.size(); ++i) 
        strStream << objectsToSave[i]->to_string();
    return strStream.str();
}

std::string Proxy::to_json_string(std::vector <SaveloadObject*>& objectsToSave) {
    json jsonStream;
    for (int i = 0; i < objectsToSave.size(); ++i)
        jsonStream.push_back(objectsToSave[i]->to_json());
    return jsonStream.dump(4);
}

void Proxy::from_string(std::vector <SaveloadObject*>& objectsToLoad, std::stringstream& data) {
    for (int i = 0; i < objectsToLoad.size(); ++i)
        objectsToLoad[i] = objectsToLoad[i]->from_string(data);  
}

void Proxy::from_json(std::vector <SaveloadObject*>& objectsToLoad, json& data) {
    for (int i = 0; i < objectsToLoad.size(); ++i)
        objectsToLoad[i] = objectsToLoad[i]->from_json(data[i]);
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Constants*/

sf::Texture* Constants::TEXTURE_BONUS_PLATFORM_LENGTH_UP;
sf::Texture* Constants::TEXTURE_BONUS_PLATFORM_LENGTH_DOWN;
sf::Texture* Constants::TEXTURE_BONUS_PLATFORM_SPEED_DOWN;
sf::Texture* Constants::TEXTURE_BONUS_PLATFORM_SPEED_UP;
sf::Texture* Constants::TEXTURE_BONUS_BALL_SPEED_UP;
sf::Texture* Constants::TEXTURE_BONUS_BALL_SPEED_DOWN;
sf::Font* Constants::FONT_HEADER;
sf::Font* Constants::FONT_BODY;

void Constants::defineConstants()
{
    try
    {
        TEXTURE_BONUS_PLATFORM_LENGTH_UP = new sf::Texture(); 
        TEXTURE_BONUS_PLATFORM_LENGTH_DOWN = new sf::Texture();
        TEXTURE_BONUS_PLATFORM_SPEED_DOWN = new sf::Texture();
        TEXTURE_BONUS_PLATFORM_SPEED_UP = new sf::Texture();
        TEXTURE_BONUS_BALL_SPEED_UP = new sf::Texture();
        TEXTURE_BONUS_BALL_SPEED_DOWN = new sf::Texture();
        FONT_HEADER = new sf::Font();
        FONT_BODY = new sf::Font();


        TEXTURE_BONUS_PLATFORM_LENGTH_UP->loadFromFile("PlatformLengthUp.png");
        TEXTURE_BONUS_PLATFORM_LENGTH_DOWN->loadFromFile("PlatformLengthDown.png");
        TEXTURE_BONUS_PLATFORM_SPEED_DOWN->loadFromFile("PlatformSpeedDown.png");
        TEXTURE_BONUS_PLATFORM_SPEED_UP->loadFromFile("PlatformSpeedUp.png");
        TEXTURE_BONUS_BALL_SPEED_UP->loadFromFile("BallSpeedUp.png");
        TEXTURE_BONUS_BALL_SPEED_DOWN->loadFromFile("BallSpeedDown.png");
        
        FONT_HEADER->loadFromFile("header.otf");
        FONT_BODY->loadFromFile("simple.ttf");
    }
    catch (...)
    {
        std::cout << "Error occured while defining constants.\n"; 
    }
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Settings*/
Difficulty Settings::diff;
resolution Settings::res;

Settings::Settings()
{
    setDefaultSettings();
}

void Settings::setDefaultSettings()
{
    diff = MEDIUM;
    res = { mResolution[0].x, mResolution[0].y };
}

std::string Settings::to_string()
{
    std::string data = ""; 
    data = data + "settings:" + " " + Settings::resToStr(Settings::getResolution()) + " " + Settings::diffToStr(Settings::getDifficulty()) + "\n";
    data = data + "\n\n\n";
    return data; 
}

json Settings::to_json()
{
    json jresult; 
    jresult["settings"] =
    {
        {"resolution_x", Settings::getResolution().x},
        {"resolution_y", Settings::getResolution().y},
        {"difficulty",   Settings::getDifficulty()}
    };
    return jresult; 
}

SaveloadObject* Settings::from_string(std::stringstream& data)
{
    Settings* obj = new Settings;
    
    std::string buffer = "";
    Proxy::simpleRead(data, buffer);
    
    Proxy::simpleRead(data, buffer);
    std::string res_str(buffer);
    Proxy::simpleRead(data, buffer);
    std::string diff_str(buffer);

    resolution res = Settings::strToRes(res_str);
    Difficulty diff = Settings::strToDiff(diff_str);
    
    obj->res = res;
    obj->diff = diff;

    return obj;
}

SaveloadObject* Settings::from_json(json& data)
{
    Settings* obj = new Settings;

    sf::Vector2f res = { data["settings"]["resolution_x"].get<float>(), data["settings"]["resolution_y"].get<float>() };
    Difficulty   diff = (Difficulty)data["settings"]["difficulty"].get<int>();
    
    obj->res = res;
    obj->diff = diff;
    
    return obj;
}

Settings::~Settings()
{
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Gamefield*/
GameField::GameField(sf::Vector2f s, sf::Vector2f p, sf::Color color) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
    displayObjects.push_back(this);
    initMessageBoxes();
    state = GameState::OFF;

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

GameField::GameField(EventObject* parent, sf::Vector2f s, sf::Vector2f p, sf::Color color) : GameField(s, p, color)
{
    ptrParent = parent;
}

void GameField::initMessageBoxes()
{
    /*Init Start MessageBox*/
    {
        sf::Vector2f parentSize = size;
        sf::Vector2f position_for_window = DisplayObject::getRelativeCenter({ 0, 0, parentSize.x, parentSize.y }, { 0, 0, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y });

        Messagebox* mStart = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, position_for_window, ColorCustomMidPurpleBlueUltraDark);
        mStart->setVisibility(false);

        auto* textField = new TextField();
        textField->setAllProperties(MSTART_TEXT, *Constants::FONT_BODY, sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0, sf::Color::White, ColorCustomPink);
        sf::Vector2f  position_for_text = DisplayObject::getRelativeCenter({ position_for_window.x, position_for_window.y, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y }, { 0, 0, textField->getTextGlobal().width ,  textField->getTextGlobal().height });
        textField->setTextPosition(position_for_text);


        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mStart->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mStart->addTextField(textField); 

        mBoxes.push_back(mStart);

        sf::Vector2f position_for_button;
        position_for_button.x = position_for_window.x - BUTTON_MESSAGE_BOX_SIZE.x * size.x + MBOX_WIDTH * size.x;
        position_for_button.y = position_for_window.y + 1;

        Button* button = new Button(mStart, "X", { BUTTON_MESSAGE_BOX_SIZE.x * size.x, BUTTON_MESSAGE_BOX_SIZE.y * size.y }, position_for_button, EventType::CLOSE_MBOX);
        button->setTextSize(MBOX_FONT_SIZE * size.y);
        button->setScales({ MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y });
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(1);
        mStart->addButton(button);
    }
    /*Init End Box*/
    {
        sf::Vector2f parentSize = size;
        sf::Vector2f position_for_window = DisplayObject::getRelativeCenter({ 0, 0, parentSize.x, parentSize.y }, { 0, 0, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y });

        Messagebox* mEnd = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, position_for_window, ColorCustomMidPurpleBlueUltraDark);
        mEnd->setVisibility(false);

        auto* textField = new TextField(MEND_TEXT, *Constants::FONT_BODY,
            sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0,
            sf::Color::White, ColorCustomPink);

        sf::Vector2f  position_for_text = DisplayObject::getRelativeCenter({ position_for_window.x, position_for_window.y, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y }, { 0, 0, textField->getTextGlobal().width ,  textField->getTextGlobal().height });
        textField->setTextPosition(position_for_text);


        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mEnd->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mEnd->addTextField(textField);

        mBoxes.push_back(mEnd);

        sf::Vector2f position_for_button;
        position_for_button.x = position_for_window.x - BUTTON_MESSAGE_BOX_SIZE.x * size.x + MBOX_WIDTH * size.x;
        position_for_button.y = position_for_window.y + 1;

        Button* button = new Button(mEnd, "X", { BUTTON_MESSAGE_BOX_SIZE.x * size.x, BUTTON_MESSAGE_BOX_SIZE.y * size.y }, position_for_button, EventType::CLOSE_MBOX);
        button->setTextSize(MBOX_FONT_SIZE * size.y);
        button->setScales({ MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y });
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(1);
        mEnd->addButton(button);
    }
    /*Init Win Box*/
    {
        sf::Vector2f parentSize = size;
        sf::Vector2f position_for_window = DisplayObject::getRelativeCenter({ 0, 0, parentSize.x, parentSize.y }, { 0, 0, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y });

        Messagebox* mWin = new Messagebox(this, { MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y }, position_for_window, ColorCustomMidPurpleBlueUltraDark);
        mWin->setVisibility(false);

        auto* textField = new TextField(MWIN_TEXT, *Constants::FONT_BODY,
            sf::Text::Regular, MBOX_TEXT_SIZE * size.y, 0,
            sf::Color::White, ColorCustomPink);

        sf::Vector2f  position_for_text = DisplayObject::getRelativeCenter({ position_for_window.x, position_for_window.y, MBOX_WIDTH * size.x , MBOX_HEIGHT * size.y }, { 0, 0, textField->getTextGlobal().width ,  textField->getTextGlobal().height });
        textField->setTextPosition(position_for_text);


        sf::Vector2f wCenter = DisplayObject::getRelativeCenter(mWin->getGlobalBounds(), textField->getTextGlobal());
        textField->setTextPosition({ wCenter.x, wCenter.y });
        textField->setScales(size);
        mWin->addTextField(textField);
        
        mBoxes.push_back(mWin);

        sf::Vector2f position_for_button;
        position_for_button.x = position_for_window.x - BUTTON_MESSAGE_BOX_SIZE.x * size.x + MBOX_WIDTH * size.x;
        position_for_button.y = position_for_window.y + 1;

        Button* button = new Button(mWin, "X", { BUTTON_MESSAGE_BOX_SIZE.x * size.x, BUTTON_MESSAGE_BOX_SIZE.y * size.y }, position_for_button, EventType::CLOSE_MBOX);
        button->setTextSize(MBOX_FONT_SIZE * size.y);
        button->setScales({ MBOX_WIDTH * size.x, MBOX_HEIGHT * size.y });
        button->setColors(ColorCustomGreen, ColorCustomDarkGreen, ColorCustomDarkGreen, sf::Color::White);
        button->setBorderThickness(1);
        mWin->addButton(button);
    }
}

void GameField::collisionDetection()
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

void GameField::collisionSolution(Collision collision)
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
        if (block->cantBreak == true) break;
        sendEvent(dynamic_cast<EventObject*>(collision.staticObj), EventType::COLLIDE_REACTION);

        std::vector<void*>* initiator_player_information = new std::vector<void*>;
        initiator_player_information->push_back(prevHitInitiator);

        /*if block is crashed*/
        if (block->numOfLives== 1)
        {
            /*victory detection*/
            if (blocks.getNumOfRemainingBlocks() - 1 == 0) sendEvent(this, WIN);

            std::vector<void*>* data = new std::vector<void*>; 
            data->push_back(prevHitInitiator);
            sendEvent(ptrParent, { SCORE_UP_FOR_CRASHING, data });
            /*activate bonus moving*/
            for (int i = 0, n = block->bonuses.size(); i < n; i++)
            {
                block->bonuses[i]->setVisibility(true); 
                this->displayObjects.push_back(block->bonuses[i]);
                this->movableObjects.push_back(block->bonuses[i]);
            }
        }
        break;
    }
    default:
        break;
    }
}

void GameField::display(sf::RenderWindow* window, sf::View* view)
{
    sf::Event event{};
    while (window->pollEvent(event))
    {
        this->statusBar->menuButton->update(event, *window);  //Change button colors depending on the event
        for (auto& m : mBoxes)
        {
            if (m->isVisible())
            {
                for (auto& button : m->messageboxItems)
                    button->update(event, *window);
                m->eventsHandler();
            }
        }
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
            m->draw(window, view);
        }
    }
    //isFirstStart = false;
    this->eventsHandler();
}

void GameField::eventsHandler()
{
    doEvent event;
    while (pollEvent(event))
    {
        switch (event.event)
        {
        case CLOSE_MBOX:
        {
            Messagebox* mbox = (Messagebox*)event.data->at(0);
            if (mbox == mBoxes[1])
            {
                sendEvent(ptrParent, RESET_GAME);
                hide();
            }
            else if (mbox == mBoxes[2])
            {
                sendEvent(ptrParent, OPEN_START_MENU);
                hide();
            }
            delete event.data;
            break;
        }
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
            state = GameState::OFF;
            mBoxes[2]->setVisibility(true);
            break;
        case SHOW_END_MESSAGE:
            clearQueue();
            state = GameState::OFF;
            mBoxes[1]->setVisibility(true);
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

void GameField::addNewBonus(int index, BonusDescriptor bd)
{
    BonusTimer* clock = new BonusTimer;
    clock->Reset();
    bonusesTimers.push_back({ {index, bd}, {BONUS_MAX_TIME, clock} });
}

void GameField::checkBonusTimers()
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

            sendEvent(ptrParent, { APPLY_BONUSES, initiator });

            //delete bonusesTimers[i].second.second; //timer 
        }
        else new_bonusesTimers.push_back(bonusesTimers[i]);
    }
    bonusesTimers = new_bonusesTimers;
}

void GameField::update()
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

std::string GameField::to_string()
{
    std::string data = ""; 

    data = data + balls.to_string(); 
    data = data + blocks.to_string();
    data = data + "bonusesTimers:" + " " + "num:" + " " + std::to_string(bonusesTimers.size()) + "\n";
    for (int i = 0, n = bonusesTimers.size(); i < n; i++)
    {
        data = data + "player_index:" + " " + std::to_string(bonusesTimers[i].first.first) + "\n";
        data = data + "bonus_descriptor:" + " " + std::to_string(bonusesTimers[i].first.second) + "\n";
        float time = bonusesTimers[i].second.second->GetElapsedSeconds(); 
        float saveTime = BONUS_MAX_TIME - bonusesTimers[i].second.second->GetElapsedSeconds(); 
        if (saveTime <= 0 || saveTime >= BONUS_MAX_TIME)
            saveTime = BONUS_MAX_TIME;
        data = data + "max_time:" + " " + std::to_string(saveTime) + "\n";
        data = data + "\n";
    }
    data = data + "\n\n";
    
    return data; 
}

json GameField::to_json()
{
    json jresult; 

    jresult.push_back(balls.to_json());
    jresult.push_back(blocks.to_json());
    
    json jTimers; 
    jTimers["num_of_bonuses_timers"] = bonusesTimers.size();
    for (int i = 0, n = bonusesTimers.size(); i < n; i++)
    {
        float saveTime = BONUS_MAX_TIME - bonusesTimers[i].second.second->GetElapsedSeconds();
        if (saveTime <= 0 || saveTime >= BONUS_MAX_TIME)
            saveTime = BONUS_MAX_TIME;

        jTimers["bonuses_timers"].push_back
        ({
            {"player_index", bonusesTimers[i].first.first},
            {"bonus_descriptor", bonusesTimers[i].first.second},
            {"max_time", saveTime},
        });
    }
    jresult.push_back(jTimers);
    
    return jresult; 
}

SaveloadObject* GameField::from_string(std::stringstream& data)
{ 
    GameField* obj = new GameField({ 0, 0 }, {0, 0}, sf::Color::Black);

    obj->balls = *(dynamic_cast<Balls *>(obj->balls.from_string(data)));
    obj->blocks = *(dynamic_cast<Blocks *>(obj->blocks.from_string(data))); 

    std::string buffer; 

    Proxy::simpleRead(data, buffer); 
    Proxy::pathThrough(data, buffer);
    uint64_t numOfBonusesTimers = stoi(buffer);
    for (uint64_t i = 0; i < numOfBonusesTimers; i++)
    {
        int index;
        BonusDescriptor bd;
        float maxTime;
        BonusTimer* clock = new BonusTimer;

        Proxy::pathThrough(data, buffer);
        index = stoi(buffer);
        Proxy::pathThrough(data, buffer);
        bd = (BonusDescriptor)stoi(buffer);
        Proxy::pathThrough(data, buffer);
        maxTime = stof(buffer);

        clock->Reset();
        clock->Pause();
        obj->bonusesTimers.push_back({ {index, bd}, {maxTime, clock} });
    }
    return obj;
}

SaveloadObject* GameField::from_json(json& data)
{
    GameField* obj = new GameField({ 0, 0 }, { 0, 0 }, sf::Color::Black);

    obj->balls = *(dynamic_cast<Balls*>(obj->balls.from_json(data[0])));
    obj->blocks = *(dynamic_cast<Blocks*>(obj->blocks.from_json(data[1])));

    size_t numOfBonusesTimers = data[2]["num_of_bonuses_timers"].get<size_t>();
    for (size_t i = 0; i < numOfBonusesTimers; i++)
    {
        int index;
        BonusDescriptor bd;
        float maxTime;
        BonusTimer* clock = new BonusTimer;

        index = data[2]["bonuses_timers"][i]["player_index"].get<int>();
        bd = (BonusDescriptor)data[2]["bonuses_timers"][i]["bonus_descriptor"].get<int>();
        maxTime = data[2]["bonuses_timers"][i]["max_time"].get<float>();

        clock->Reset();
        clock->Pause();
        obj->bonusesTimers.push_back({ {index, bd}, {maxTime, clock} });
    }

    return obj; 
}

GameField::~GameField()
{
    balls.~Balls();
    blocks.~Blocks();
    delete statusBar;
    for (auto& m : mBoxes)
        delete m;
}

/*--------------------------------------------------------------------/*--------------------------------------------------------------------Game*/
Game::Game()
{
    Constants::defineConstants(); 
    proxy = new Proxy();
    window = NULL;
    view = NULL;
    nullization();
    init();
    sendEvent(this, OPEN_START_MENU);
    /**/
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    random.seed(ss);
}

void Game::resetStartMenu(resolution res, Difficulty diff)
{
    if (startMenu)
        delete startMenu;
    startMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    startMenu->setVisibility(false);

    auto* textField = new TextField("Arkanoid", *Constants::FONT_HEADER, sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS, sf::Color::White, ColorCustomPink);
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

void Game::resetSettingsMenu(resolution res, Difficulty diff)
{
    if (settingsMenu)
        delete settingsMenu;
    settingsMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    settingsMenu->setVisibility(false);

    auto* textField = new TextField("Settings", *Constants::FONT_HEADER, sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS, sf::Color::White, ColorCustomPink);
    sf::Vector2f wCenter = DisplayObject::getRelativeCenter(settingsMenu->getGlobalBounds(), textField->getTextGlobal());
    textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y });
    textField->setScales(res);
    settingsMenu->addTextField(textField);

    /*Subheaders*/
    auto* changeResolutionField = new TextField("CHANGE THE RESOLUTION", *Constants::FONT_BODY, sf::Text::Bold, SUB_HEADER_TEXT_SIZE * res.y, SUB_HEADER_BORDERTHICKNESS, sf::Color::White, ColorCustomPink);
    auto* changeDifficultyField = new TextField("CHANGE THE DIFFICULTY", *Constants::FONT_BODY, sf::Text::Bold, SUB_HEADER_TEXT_SIZE * res.y, SUB_HEADER_BORDERTHICKNESS, sf::Color::White, ColorCustomPink);

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

void Game::resetWaitMenu(resolution res, Difficulty diff)
{
    if (waitMenu)
        delete waitMenu;
    waitMenu = new Menu(this, res, { 0, 0 }, ColorCustomLightBlue);
    waitMenu->setVisibility(false);

    auto* textField = new TextField("Pause", *Constants::FONT_HEADER, sf::Text::Bold, HEADER_TEXT_SIZE * res.y, HEADER_BORDERTHICKNESS, sf::Color::White, ColorCustomPink);
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

void Game::resetGameField(resolution res, Difficulty diff)
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
    this->gameField->statusBar->data = player->getStatistics();
    this->gameField->statusBar->update();

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
        case KID:
            blockWidth = res.x / BLOCK_NUM_WIDTH_KID;
            blockHeight = (0.4 * res.y - STATUS_BAR_HEIGHT) / BLOCK_NUM_HEIGHT_KID;
            blockRows = BLOCK_NUM_WIDTH_KID;
            blockColumns = BLOCK_NUM_HEIGHT_KID;
            break;
        case IMPOSSIBLE:
            blockWidth = res.x / BLOCK_NUM_WIDTH_IMPOSSIBLE;
            blockHeight = (0.4 * res.y - STATUS_BAR_HEIGHT) / BLOCK_NUM_HEIGHT_IMPOSSIBLE;
            blockRows = BLOCK_NUM_WIDTH_IMPOSSIBLE;
            blockColumns = BLOCK_NUM_HEIGHT_IMPOSSIBLE;
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
            block->initBonuses(random, range_for_ramdom);
            int probability = range_for_ramdom(random);
            if (probability <= 10)
            {
                block->cantBreak = true;
                // block->setColor(ColorCustomPink);
            }
            gameField->blocks.addBlock(block);
            gameField->displayObjects.push_back(block);
        }
    }
    /*Init ball*/
    sf::Vector2f centerPlatformPosition; 
    centerPlatformPosition.x = platform->getPosition().x + (float)platform->getSize().x / 2 - sqrt(BALL_SQUARE * res.x * res.y); 
    centerPlatformPosition.y = platform->getPosition().y - 2 * sqrt(BALL_SQUARE * res.x * res.y); 

    Ball* ball = new Ball(res, sqrt(BALL_SQUARE * res.x * res.y), centerPlatformPosition, sf::Color::Black);
    ball->setInitPosition({ centerPlatformPosition.x, BALL_HEIGHT_INIT_OFFSET * res.y });

    ball->setParent(gameField);
    gameField->balls.addBall(ball);
    gameField->displayObjects.push_back(ball);
    gameField->movableObjects.push_back(ball);

    reInitObjectsToSave(); 
}

void Game::resetMenus(resolution res, Difficulty diff)
{
    resetStartMenu(res, diff);
    resetWaitMenu(res, diff);
    resetSettingsMenu(res, diff);
}

void Game::nullization()
{
    startMenu = settingsMenu = waitMenu = NULL;
    gameField = NULL;
}

void Game::init()
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
    reInitObjectsToSave(); 
}

void Game::reInitObjectsToSave()
{
    objectsToSave.clear();
    objectsToSave.push_back(&settings);
    objectsToSave.push_back(&players);
    objectsToSave.push_back(gameField);
}

void Game::start()
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

void Game::eventsHadler()
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
            settings.setResolution(res);
            settings.setDifficulty(diff);
            if (res.x == 1920 && res.y == 1080)
            {
                window->create(sf::VideoMode((int)res.x, (int)res.y), "Arkanoid", sf::Style::Fullscreen);
                window->setPosition({ 0, 0 });
                view = new sf::View(window->getDefaultView());
                window->setVisible(true);
                window->setVerticalSyncEnabled(true);
            }
            else
            {
                window->create(sf::VideoMode((int)res.x, (int)res.y), "Arkanoid", sf::Style::Titlebar | sf::Style::Close);
                window->setPosition({ 0, 0 });
                view = new sf::View(window->getDefaultView());
                window->setVisible(true);
                window->setVerticalSyncEnabled(true);
            }
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
            if (players.players.size() - 1 > 0)
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
            if (bonus->descriptor == BonusDescriptor::BONUS_SCORE_UP)
            {
                sendEvent(&players, { SCORE_UP, event.data });
                break;
            }
            int* index = new int;
            *index = players.getPlayerIndex(platform);
            BonusDescriptor* bd = new BonusDescriptor;
            *bd = bonus->descriptor;
            event.data->at(0) = index;
            event.data->at(1) = bd;
            gameField->addNewBonus(*index, *bd);
            sendEvent(&players, BONUS_UP);
        }
        case APPLY_BONUSES:
            this->bonusesSolution(event.data);
            break;
        case SAVE_GAME:
            save(); 
            save_json(); 
            waitMenu->setVisibility(true);
            break;
        case LOAD_THE_GAME:
            load();
            //load_json(); 
            gameField->setVisibility(true);
            break;
        default:
            break;
        }
    }
}

void Game::bonusesSolution(std::vector<void*>* eventData)
{
    int index = *((int*)eventData->at(0));
    Platform* initiator = players.players[index]->platform;
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

void Game::reset()
{
    delete startMenu;
    delete settingsMenu;
    delete waitMenu;
    delete gameField;
    players.clear();

    this->nullization();
    this->init();
}

void Game::load() 
{
    std::ifstream inFile;
    std::string filename = TEXT_FILE_GAME_PATH;
    std::stringstream data;
    inFile.open(filename);
    data << inFile.rdbuf();
    inFile.close();

    proxy->from_string(objectsToSave, data);

    /*Initialization*/
    {
        Settings* new_settings = dynamic_cast<Settings*>(objectsToSave[0]);
        Players* new_players = dynamic_cast<Players*>(objectsToSave[1]);
        GameField* new_gameField = dynamic_cast<GameField*>(objectsToSave[2]);

        /*Settings*/
        {
            settings.setDifficulty(new_settings->getDifficulty());
            settings.setResolution(new_settings->getResolution());

            resolution res = Settings::getResolution();

            if (res.x == 1920 && res.y == 1080)
            {
                window->create(sf::VideoMode((int)res.x, (int)res.y), "Arkanoid", sf::Style::Fullscreen);
                window->setPosition({ 0, 0 });
                view = new sf::View(window->getDefaultView());
                window->setVisible(true);
                window->setVerticalSyncEnabled(true);
            }
            resetMenus(Settings::getResolution(), Settings::getDifficulty());
            if (window && view)
            {
                view->reset(sf::FloatRect(0, 0, res.x, res.y));
                window->setSize({ (uint32_t)res.x, (uint32_t)res.y });
            }

            if (gameField) delete gameField;
            gameField = new GameField(this, Settings::getResolution(), { 0, 0 }, ColorCustomLightBlue);
            gameField->setVisibility(false);
            gameField->bonusesTimers = new_gameField->bonusesTimers;
            players.clear();

            /*Players*/
            {
                players = *new_players;
                players.setParent(this);
                for (int i = 0, n = players.players.size(); i < n; i++)
                {
                    players.players[i]->platform->setParent(gameField);
                    gameField->movableObjects.push_back(players.players[i]->platform);
                    gameField->displayObjects.push_back(players.players[i]->platform);
                }
                gameField->statusBar->data = &players.players[0]->statistics;
            }

            /*GameField*/
            {
                gameField->balls = new_gameField->balls; 
                gameField->blocks = new_gameField->blocks; 
                for (int i = 0, n = gameField->balls.balls.size(); i < n; i++)
                {
                    gameField->balls.balls[i]->setParent(gameField);
                    gameField->displayObjects.push_back(gameField->balls.balls[i]);
                    gameField->movableObjects.push_back(gameField->balls.balls[i]);
                }
                for (int i = 0, n = gameField->blocks.blocks.size(); i < n; i++)
                {
                    gameField->displayObjects.push_back(gameField->blocks.blocks[i]);
                    gameField->blocks.blocks[i]->setParent(gameField);
                    for (int j = 0, m = gameField->blocks.blocks[i]->bonuses.size(); j < m; j++)
                    {
                        if (gameField->blocks.blocks[i]->bonuses[j]->isVisible())
                        {
                            gameField->displayObjects.push_back(gameField->blocks.blocks[i]->bonuses[j]);
                            gameField->movableObjects.push_back(gameField->blocks.blocks[i]->bonuses[j]);
                        }
                    }
                }
            }
        }
    }
}

void Game::load_json() 
{
    std::ifstream inFile; 
    std::string filename = JSON_FILE_GAME_PATH; 
    json data{};
    std::stringstream strStream;
    inFile.open(filename);
    strStream << inFile.rdbuf();
    inFile.close();
    std::string str = strStream.str();
    data = json::parse(str);

    proxy->from_json(objectsToSave, data);
    
    /*Initialization*/
    {
        Settings* new_settings = dynamic_cast<Settings*>(objectsToSave[0]);
        Players* new_players = dynamic_cast<Players*>(objectsToSave[1]);
        GameField* new_gameField = dynamic_cast<GameField*>(objectsToSave[2]);

        /*Settings*/
        {
            settings.setDifficulty(new_settings->getDifficulty());
            settings.setResolution(new_settings->getResolution());

            resolution res = Settings::getResolution();

            if (res.x == 1920 && res.y == 1080)
            {
                window->create(sf::VideoMode((int)res.x, (int)res.y), "Arkanoid", sf::Style::Fullscreen);
                window->setPosition({ 0, 0 });
                view = new sf::View(window->getDefaultView());
                window->setVisible(true);
                window->setVerticalSyncEnabled(true);
            }
            resetMenus(Settings::getResolution(), Settings::getDifficulty());
            if (window && view)
            {
                view->reset(sf::FloatRect(0, 0, res.x, res.y));
                window->setSize({ (uint32_t)res.x, (uint32_t)res.y });
            }

            if (gameField) delete gameField;
            gameField = new GameField(this, Settings::getResolution(), { 0, 0 }, ColorCustomLightBlue);
            gameField->setVisibility(false);
            gameField->bonusesTimers = new_gameField->bonusesTimers; 
            players.clear();

            /*Players*/
            {
                players = *new_players;
                players.setParent(this);
                for (int i = 0, n = players.players.size(); i < n; i++)
                {
                    players.players[i]->platform->setParent(gameField);
                    gameField->movableObjects.push_back(players.players[i]->platform);
                    gameField->displayObjects.push_back(players.players[i]->platform);
                }
                gameField->statusBar->data = &players.players[0]->statistics;
            }

            /*GameField*/
            {
                gameField->balls = new_gameField->balls;
                gameField->blocks = new_gameField->blocks;
                for (int i = 0, n = gameField->balls.balls.size(); i < n; i++)
                {
                    gameField->balls.balls[i]->setParent(gameField);
                    gameField->displayObjects.push_back(gameField->balls.balls[i]);
                    gameField->movableObjects.push_back(gameField->balls.balls[i]);
                }
                for (int i = 0, n = gameField->blocks.blocks.size(); i < n; i++)
                {
                    gameField->displayObjects.push_back(gameField->blocks.blocks[i]);
                    gameField->blocks.blocks[i]->setParent(gameField);
                    for (int j = 0, m = gameField->blocks.blocks[i]->bonuses.size(); j < m; j++)
                    {
                        if (gameField->blocks.blocks[i]->bonuses[j]->isVisible())
                        {
                            gameField->displayObjects.push_back(gameField->blocks.blocks[i]->bonuses[j]);
                            gameField->movableObjects.push_back(gameField->blocks.blocks[i]->bonuses[j]);
                        }
                    }
                }
            }
        }
    }
}

void Game::save() 
{
    std::ofstream outFile; 
    std::string filename = TEXT_FILE_GAME_PATH;
    std::string data = proxy->to_string(objectsToSave); 
    outFile.open(filename);
    outFile << data << '\n';
    outFile.close();
}

void Game::save_json() 
{
    std::ofstream outFile;
    std::string filename = JSON_FILE_GAME_PATH;
    std::string data = proxy->to_json_string(objectsToSave);
    outFile.open(filename);
    outFile << std::setw(4) << data << '\n';
    outFile.close();
}

Game::~Game()
{
    delete startMenu;
    delete settingsMenu;
    delete waitMenu;
    delete gameField;
    players.clear();
    delete window;
    delete view;
}

    