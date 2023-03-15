#include "game_classes.h"

sf::RectangleShape t;
sf::RectangleShape p1;

sf::RectangleShape p2;

sf::RectangleShape p3;

sf::RectangleShape p4;
int x = 0;

void Arkanoid::DisplayObject::rotate(float angle, bool isrotating)
{
   sf::FloatRect  r = selfRect->getLocalBounds();
   float xs[3] = {r.left + r.width / 2, r.left,r.width };
   float ys[3] = { r.top, r.height, r.height };
   float x = (xs[0] + xs[1] + xs[2]) / 3, y = (ys[0] + ys[1] + ys[2]) / 3;
   selfRect->setOrigin(x,  y);
        t.setSize( {selfRect->getGlobalBounds().width, selfRect->getGlobalBounds().height} );
        setPosition({ selfRect->getPosition().x + r.width / 2, selfRect->getPosition().y + r.height / 2 } );
        t.setPosition(getPosition());
        t.setFillColor(sf::Color::White);
        selfRect->rotate(angle);
}

 sf::Vector2f Arkanoid::DisplayObject::getCenter(sf::FloatRect parentSize, sf::FloatRect childSize)
{
    return {parentSize.left + parentSize.width/2 - childSize.width/2,parentSize.top  + parentSize.height/2 - childSize.height};
}

bool Arkanoid::EventObject::pollEvent(EventType& event)
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
    std::queue<EventType > empty;
    std::swap( eventsQueue, empty );
}
//--------------------------------------------------------------------->>> TextField

Arkanoid::TextField::TextField(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness,
                               sf::Color normal, sf::Color hovered, sf::Color clicked, sf::Color border)
{
    clrNormal  = normal;
    clrClicked = clicked;
    clrHovered = hovered;
    clrBorder  = border;
    font.loadFromFile( fnt); text.setFont(font);
    text.setString(str = txt);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder);
}

Arkanoid::TextField::TextField()
{
    setDefaultProperties();
}

void Arkanoid::TextField::setDefaultProperties()
{
    //
    clrNormal = clrClicked = clrHovered = sf::Color::White;
    clrBorder = sf::Color(102, 0, 102);
    font.loadFromFile(totalProjectPath + "/fonts/pixel.ttf");
    borderThickness = 0;
    text.setString(str = "Click me!");
    text.setFont(font);
    text.setStyle(textStyle = sf::Text::Bold);
    text.setCharacterSize(charSize = 10);
    text.setFillColor(clrNormal);
}

void Arkanoid::TextField::setAllProperties(const std::string& txt, const std::string& fnt, uint32_t txtStyle, uint32_t size, float thickness,
                      sf::Color normal, sf::Color hovered, sf::Color clicked, sf::Color border)
{
    clrNormal  = normal;
    clrClicked = clicked;
    clrHovered = hovered;
    clrBorder  = border;
    text.setString(str = txt);
    font.loadFromFile( fnt); text.setFont(font);
    text.setStyle(textStyle = txtStyle);
    text.setCharacterSize(charSize = size);
    text.setFillColor(clrNormal);
    text.setOutlineThickness(borderThickness = thickness);
    text.setOutlineColor(clrBorder);
}

Arkanoid::TextField::~TextField() = default;

//--------------------------------------------------------------------->>> Button
void Arkanoid::Button::setDefaultProperties()
{
    clrBgNormal   = sf::Color::Green;
    clrBgClicked  = sf::Color::Red;
    clrBgHovered  = sf::Color::Blue;
    clrBorder     = sf::Color::Yellow;
    selfRect->setFillColor(clrBgNormal);
    selfRect->setOutlineColor(clrBorder);
    state = BtnState::NORMAL;
    textfield = new TextField();
    textfield->setTextStyle(sf::Text::Bold);
}

Arkanoid::Button::Button(const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : DisplayObject(s, p)
{
    setDefaultProperties();
    selfRect->setOutlineThickness(borderThickness = 0.02f*s.x);
    textfield->setCharSize(s.x * 0.08f);
    textfield->setText(name);
    textfield->setTextPosition(DisplayObject::getCenter(selfRect->getGlobalBounds(), textfield->getTextGetGlobal()));
    textfield->setScales(s);
    event = evnt;
}

Arkanoid::Button::Button(EventObject *parent, const std::string& name, sf::Vector2f s, sf::Vector2f p, EventType evnt) : Button(name, s, p, evnt)
{
    ptrParent = parent;
}

Arkanoid::Button::Button(const std::string& name, float radius, uint16_t points, sf::Vector2f p, EventType evnt) : DisplayObject(radius, points, p)
{
    setDefaultProperties();
    selfRect->setOutlineThickness(borderThickness = 0.02f*radius);
    textfield->setCharSize(radius * 0.08f);
    textfield->setText(name);
    textfield->setTextPosition(DisplayObject::getCenter(selfRect->getGlobalBounds(), textfield->getTextGetGlobal()));
    textfield->setScales( { radius, radius });
    event = evnt;
}

Arkanoid::Button::Button(EventObject *parent, const std::string& name, float radius, uint16_t points, sf::Vector2f p, EventType evnt) : Button(name, radius, points, p, evnt)
{
    ptrParent = parent;
}

void Arkanoid::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*selfRect, states);
    textfield->update(size);
    if (!textfield->isEmpty()) target.draw(*textfield, states);
}

void Arkanoid::Button::update(sf::Event &e, sf::RenderWindow &window)
{
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(window);
    bool mouseInButton = mousePosition.x >= selfRect->getPosition().x
                          && mousePosition.x <= selfRect->getPosition().x + selfRect->getGlobalBounds().width
                          && mousePosition.y >= selfRect->getPosition().y
                          && mousePosition.y <= selfRect->getPosition().y + selfRect->getGlobalBounds().height;
    if(e.type == sf::Event::MouseMoved)
    {
        if (mouseInButton)
        {
            state = BtnState::HOVERED;
        }
        else
            state = BtnState::NORMAL;
    }
    else if (e.type == sf::Event::MouseButtonPressed)
    {
        switch(e.mouseButton.button)
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
    else if (e.type == sf::Event::MouseButtonReleased)
    {
        switch(e.mouseButton.button)
        {
            case sf::Mouse::Left:

                if (mouseInButton)
                    state = BtnState::HOVERED;
                else
                    state = BtnState::NORMAL;
                break;
            default:
                break;
        }
    }
    switch(state)
    {
        case BtnState::NORMAL:
            selfRect->setFillColor(clrBgNormal);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorNormal();
            break;
        case BtnState::HOVERED:
            selfRect->setFillColor(clrBgHovered);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorHovered();
            break;
        case BtnState::PRESSED:
            selfRect->setFillColor(clrBgClicked);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorClicked();
            sendEvent(ptrParent, event);
            break;
    }
}

Arkanoid::Button::~Button()
= default;

//--------------------------------------------------------------------->>> Menu
Arkanoid::Menu::Menu()
{

}

Arkanoid::Menu::Menu(sf::Vector2f s, sf::Vector2f p, sf::Color color, const std::string& title) : DisplayObject(s, p)
{
    selfRect->setFillColor(color);
    window = new sf::RenderWindow(sf::VideoMode((int)s.x, (int)s.y), title);
    window->setPosition({(int)p.x, (int)p.y});
    window->setVisible(false);
    view = new sf::View(window->getDefaultView());
}

Arkanoid::Menu::Menu(sf::Vector2f s, sf::Vector2f p, const std::string& background, const std::string& title) : DisplayObject(s, p)
{
    texture.loadFromFile(background);
    texture.setRepeated(false);
    selfRect->setTexture(&texture);
    window = new sf::RenderWindow(sf::VideoMode((int)s.x, (int)s.y), title);
    window->setPosition({(int)p.x, (int)p.y});
    window->setVisible(false);
    view   = new sf::View( selfRect->getGlobalBounds());
}

Arkanoid::Menu::Menu(EventObject *parent, sf::Vector2f s, sf::Vector2f p, sf::Color color, const std::string& title) : Menu(s, p, color, title)
{
    ptrParent = parent;
}

void Arkanoid::Menu::eventsHadler()
{
    EventType event;
    while (pollEvent(event))
    {
        switch (event)
        {
            case OPEN_SETTINGS:
                clearQueue();
                sendEvent(ptrParent, OPEN_SETTINGS);
                window->close();
                break;
            case OPEN_GAME_FIELD:
                clearQueue();
                sendEvent(ptrParent, OPEN_GAME_FIELD);
                window->close();
                break;
            case QUIT:
                clearQueue();
                sendEvent(ptrParent, QUIT);
                window->close();
                break;
            case OPEN_START_MENU:
                clearQueue();
                sendEvent(ptrParent, OPEN_START_MENU);
                window->close();
                break;
            default:
                break;
        }
    }
}

void Arkanoid::Menu::display()
{
    window->setVisible(true);
    window->setVerticalSyncEnabled(true);
    DisplayObject::display();
    while (window->isOpen())
    {
        sf::Event event{};
        while (window->pollEvent(event)) {
            for (auto &o: buttons)
                o.update(event, *window);
            eventsHadler();
            switch (event.type) {
                case sf::Event::Resized:
                    view->reset(sf::FloatRect(0, 0, (float) event.size.width, (float) event.size.height));
                    setSize({(float) event.size.width, (float) event.size.height});
                    for (auto &o: textFields)
                        o.update({(float) event.size.width, (float) event.size.height});
                    for (auto &o: buttons)
                        o.updateSizes({(float) event.size.width, (float) event.size.height});
                    if (buttons.size() == 6)
                    {
                        std::cout << buttons[4].selfRect->getGlobalBounds().width << " x " << buttons[4].selfRect->getGlobalBounds().height << "\n";
                    p1.setSize({buttons[4].selfRect->getGlobalBounds().width,
                               buttons[4].selfRect->getGlobalBounds().height});
                    p1.setPosition(buttons[4].selfRect->getPosition());
                    p1.setFillColor(sf::Color::Yellow);

                    p2.setSize({buttons[1].selfRect->getGlobalBounds().width,
                                buttons[1].selfRect->getGlobalBounds().height});
                    p2.setPosition(buttons[1].selfRect->getPosition());
                    p2.setFillColor(sf::Color::Yellow);

                    p3.setSize({buttons[2].selfRect->getGlobalBounds().width,
                                buttons[2].selfRect->getGlobalBounds().height});
                    p3.setPosition(buttons[2].selfRect->getPosition());
                    p3.setFillColor(sf::Color::Yellow);

                    p4.setSize({buttons[3].selfRect->getGlobalBounds().width,
                                buttons[3].selfRect->getGlobalBounds().height});
                    p4.setPosition(buttons[3].selfRect->getPosition());
                    p4.setFillColor(sf::Color::Yellow);}
                    break;
                case sf::Event::Closed:
                    window->close();
                    break;
                default:
                    break;
            }
        }
        if (x == 0 && buttons.size() == 6)
        {
            buttons[0].rotate(-90, true);
            buttons[1].rotate(90, true);
            buttons[2].rotate(-90, true);
            buttons[3].rotate(90, true);
            x++;
            buttons[0].setScales({ (float)window->getSize().x,  (float)window->getSize().y });
            buttons[1].setScales({ (float)window->getSize().x,  (float)window->getSize().y });
            buttons[2].setScales({ (float)window->getSize().x,  (float)window->getSize().y });
            buttons[3].setScales({ (float)window->getSize().x,  (float)window->getSize().y });
        }

        window->setView( *view );
        window->clear();
        window->draw(*selfRect);
        window->draw(p1);
        window->draw(p2);
        window->draw(p3);
        window->draw(p4);
        for (auto& o : textFields)
            window->draw(o);
        for (const auto& o: buttons)
            window->draw(o);
        window->display();
    }
    window->setVisible(false);
}

Arkanoid::Menu::~Menu()
{}

//--------------------------------------------------------------------->>> MessageBox

Arkanoid::MessageBox::MessageBox() {

}

void Arkanoid::MessageBox::draw(sf::RenderTarget &target, sf::RenderStates states) const {

}

void Arkanoid::MessageBox::eventsHandler(sf::Event &e, sf::RenderWindow &window) {

}

Arkanoid::MessageBox::~MessageBox() {

}

//--------------------------------------------------------------------->>> Statistics

Arkanoid::Statistics::Statistics() {

}

void Arkanoid::Statistics::draw(sf::RenderTarget &target, sf::RenderStates states) const {

}

Arkanoid::Statistics::~Statistics() {

}

//--------------------------------------------------------------------->>> Platform


Arkanoid::Platform::Platform(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp) {

}

void Arkanoid::Platform::move() {

}

void Arkanoid::Platform::draw(sf::RenderTarget &target, sf::RenderStates states) const {

}

void Arkanoid::Platform::randomlyChangeLength() {

}

Arkanoid::Platform::~Platform() {

}


//--------------------------------------------------------------------->>> Bonus

Arkanoid::Bonus::Bonus() {

}

void Arkanoid::Bonus::applyBonus() {

}

Arkanoid::Bonus::~Bonus() {

}

//--------------------------------------------------------------------->>> Block
Arkanoid::Block::Block(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp, bool bonus) {

}

void Arkanoid::Block::draw(sf::RenderTarget &target, sf::RenderStates states) const {

}

void Arkanoid::Block::crashReaction() {

}

void Arkanoid::Block::collideReaction() {

}

Arkanoid::Block::~Block() {

}

//--------------------------------------------------------------------->>> Player


Arkanoid::Player::Player() {

}

void Arkanoid::Player::randomlyChangeScore() {

}

void Arkanoid::Player::randomlyChangeLives() {

}

Arkanoid::Player::~Player() {

}

//--------------------------------------------------------------------->>> Ball

Arkanoid::Ball::Ball(const sf::Vector2f &p, const sf::Vector2f &s, const sf::Color &c, float sp) {

}

void Arkanoid::Ball::move() {

}

void Arkanoid::Ball::draw(sf::RenderTarget &target, sf::RenderStates states) const {

}

Arkanoid::Ball::~Ball() {

}

//--------------------------------------------------------------------->>> Balls

Arkanoid::Balls::Balls() {

}

void Arkanoid::Balls::numBallsUp() {

}

Arkanoid::Balls::~Balls() {

}

//--------------------------------------------------------------------->>> Players

Arkanoid::Players::Players() {

}

Arkanoid::Players::~Players() {

}

//--------------------------------------------------------------------->>> Blocks

Arkanoid::Blocks::Blocks() {

}

void Arkanoid::Blocks::updateBonusPercentage(Difficulty diff) {

}

Arkanoid::Blocks::~Blocks() {

}

//--------------------------------------------------------------------->>> History

Arkanoid::History::History() {

}

void Arkanoid::History::setData(Arkanoid::Statistics st) {

}

void Arkanoid::History::saveHistory() {

}

void Arkanoid::History::importHistory() {

}

void Arkanoid::History::clearHistory() {

}

Arkanoid::History::~History() {

}

//--------------------------------------------------------------------->>> Settings
Arkanoid::Settings::Settings()
{
    setDefaultSettings();
}

void Arkanoid::Settings::saveSettings()
{
}

void Arkanoid::Settings::setDefaultSettings()
{
    diff = MEDIUM;
    res  = { mResolution[0].x, mResolution[0].y };
}

void Arkanoid::Settings::importSettings()
{
}

Arkanoid::Settings::~Settings()
{
}
//--------------------------------------------------------------------->>> GameField
//Delegating constructor initializer

Arkanoid::GameField::GameField(sf::Vector2f s, sf::Vector2f p) : DisplayObject(s, p)
{
    size = s;
    position = p;
}

Arkanoid::GameField::GameField(EventObject *parent, sf::Vector2f s, sf::Vector2f p) : GameField(s, p)
{
    ptrParent = parent;
}

void Arkanoid::GameField::checkCollisions()
{
    DisplayObject::checkCollisions();
}

void Arkanoid::GameField::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

}

void Arkanoid::GameField::update()
{

}

void Arkanoid::GameField::render()
{

}

Arkanoid::GameField::~GameField()
{

}
//--------------------------------------------------------------------->>> Game

Arkanoid::Game::Game() : gameField(sf::Vector2f(), sf::Vector2f())
{
    initMenus();
}

void Arkanoid::Game::initMenus()
{
    sf::Vector2f res = settings.getResolution();
    Difficulty diff = settings.getDifficulty();
    /*set start menu*/
    startMenu = new Menu(this, res, {0, 0}, sf::Color::Black, "ARKANOID");

    auto* textField = new TextField;
    textField->setAllProperties("ARKANOID", totalProjectPath + "/fonts/pixel.ttf",
                                sf::Text::Bold, S_TEXT_SIZE, S_TEXT_BORDERTHICKNESS,
                                sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    sf::Vector2f wCenter = DisplayObject::getCenter(startMenu->getGlobal(), textField->getTextGetGlobal());
    textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y});
    textField->setScales(res);
    startMenu->addTextField(*textField);

    wCenter = DisplayObject::getCenter(startMenu->getGlobal(), {0, 0, S_BUTTON_WIDTH, S_BUTTON_HEIGHT});
    std::vector<Button> btns =
            {
                Button(startMenu, "START",    { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , wCenter.y }, EventType::OPEN_GAME_FIELD),
                Button(startMenu, "SETTINGS", { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , 1.2f*S_BUTTON_HEIGHT + wCenter.y }, EventType::OPEN_SETTINGS),
                Button(startMenu, "QUIT",     { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , 2.4f*S_BUTTON_HEIGHT + wCenter.y }, EventType::QUIT),
            };

    for (auto & o : btns)
    {
        o.display();
        o.setScales(res);
        o.setColors(ColorCustomBlue, ColorCustomDarkBlue, ColorCustomDarkBlue, sf::Color::White);
        o.setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        startMenu->addButton(o);
    }

    /*set settings menu*/
    settings.wSettings = new Menu(this, res, {0,0}, sf::Color::Black, "SETTINGS");

    auto resolutionTextField = new TextField, difficultyTextField = new TextField;
    resolutionTextField->setAllProperties("RESOLUTION", totalProjectPath + "/fonts/pixel.ttf",
                                          sf::Text::Bold, ST_TEXT_SIZE, ST_TEXT_BORDERTHICKNESS,
                                          sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    difficultyTextField->setAllProperties("DIFFICULTY", totalProjectPath + "/fonts/pixel.ttf",
                                          sf::Text::Bold, ST_TEXT_SIZE, ST_TEXT_BORDERTHICKNESS,
                                          sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    wCenter = DisplayObject::getCenter(settings.wSettings->getGlobal(), resolutionTextField->getTextGetGlobal());
    resolutionTextField->setTextPosition({ 0.1f*wCenter.x, 0.2f * wCenter.y});
    resolutionTextField->setScales(res);
    difficultyTextField->setTextPosition({ 0.1f*wCenter.x, 0.6f * wCenter.y});
    difficultyTextField->setScales(res);
    settings.wSettings->addTextField(*resolutionTextField);
    settings.wSettings->addTextField(*difficultyTextField);

    auto resolutionField = new TextField, difficultyField = new TextField;
    resolutionField->setAllProperties( std::to_string((int)res.x) + " x " + std::to_string((int)res.y), totalProjectPath + "/fonts/pixel.ttf",
                                          sf::Text::Bold, ST_TEXT_SIZE - 15, ST_TEXT_BORDERTHICKNESS,
                                          sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    difficultyField->setAllProperties(settings.to_string(diff), totalProjectPath + "/fonts/pixel.ttf",
                                          sf::Text::Bold, ST_TEXT_SIZE - 15, ST_TEXT_BORDERTHICKNESS,
                                          sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    resolutionField->setTextPosition({ 0.3f*wCenter.x, 0.4f * wCenter.y});
    resolutionField->setScales(res);
    difficultyField->setTextPosition({ 0.3f*wCenter.x, 0.8f * wCenter.y});
    difficultyField->setScales(res);
    settings.wSettings->addTextField(*resolutionField);
    settings.wSettings->addTextField(*difficultyField);

    btns.clear();
    btns =
            {
                Button(settings.wSettings, "",     ST_BUTTON_RADIUS, 3, { resolutionTextField->getTextGetGlobal().left, 0.45f * wCenter.y}, EventType::RES_BACKWARD),
                Button(settings.wSettings, "",     ST_BUTTON_RADIUS, 3, { resolutionField->getTextGetGlobal().left + resolutionField->getTextGetGlobal().width +  2*ST_BUTTON_RADIUS, 0.45f * wCenter.y }, EventType::RES_FORWARD),
                Button(settings.wSettings, "",     ST_BUTTON_RADIUS, 3, { resolutionTextField->getTextGetGlobal().left, 0.85f * wCenter.y }, EventType::DIFF_BACKWARD),
                Button(settings.wSettings, "",     ST_BUTTON_RADIUS, 3, { resolutionField->getTextGetGlobal().left + resolutionField->getTextGetGlobal().width +  2*ST_BUTTON_RADIUS , 0.85f * wCenter.y}, EventType::DIFF_FORWARD),
                Button(settings.wSettings, "SAVE",     { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { resolutionTextField->getTextGetGlobal().left + ST_BUTTON_RADIUS, 1.2f*wCenter.y}, EventType::SAVE_SETTINGS),
                Button(settings.wSettings, "QUIT",     { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { resolutionTextField->getTextGetGlobal().left + ST_BUTTON_RADIUS, 1.4f* wCenter.y}, EventType::SAVE_SETTINGS),
            };
   /* btns[0].rotate(-90);
    btns[1].rotate(90);
    btns[2].rotate(-90);*/
 //   btns[3].rotate(90);
    for (auto & o : btns)
    {
        o.display();
        o.setScales(res);
        o.setColors(ColorCustomBlue, ColorCustomDarkBlue, ColorCustomDarkBlue, sf::Color::White);
        o.setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        settings.wSettings->addButton(o);
    }

}

void Arkanoid::Game::eventsHadler()
{
    EventType event;
    while (pollEvent(event))
    {
        switch (event)
        {
            case OPEN_SETTINGS:
                settings.wSettings->display();
                settings.wSettings->hide();
                break;
            case OPEN_GAME_FIELD:
                break;
            case QUIT:
                break;
            case OPEN_START_MENU:
                break;
            default:
                break;
        }
    }
}

void Arkanoid::Game::start()
{
    startMenu->display();
    startMenu->hide();
    eventsHadler();
}

Arkanoid::Game::~Game()
{

}
