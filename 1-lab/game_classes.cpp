#include "game_classes.h"

 sf::Vector2f Arkanoid::DisplayObject::getCenter(sf::FloatRect parentSize, sf::FloatRect childSize)
{
    return {parentSize.left + parentSize.width/2 - childSize.width/2,parentSize.top  + parentSize.height/2 - childSize.height};
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
    font.loadFromFile(totalProjectPath + "/fonts/arial.ttf");
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
    onClick       = nullptr;
}

Arkanoid::Button::Button(const std::string& name, sf::Vector2f s, sf::Vector2f p) : DisplayObject(s, p)
{
    setDefaultProperties();

    selfRect->setFillColor(clrBgNormal);
    selfRect->setOutlineColor(clrBorder);
    selfRect->setOutlineThickness(borderThickness = 0.02f*s.x);

    state = btnState::NORMAL;
    sf::Font font;
    font.loadFromFile(totalProjectPath + "/fonts/pixel.ttf");
    textfield = new TextField();
    textfield->setTextStyle(sf::Text::Bold);
    textfield->setCharSize(s.x * 0.08f);
    textfield->setText(name);
    textfield->setTextPosition(DisplayObject::getCenter(selfRect->getGlobalBounds(), textfield->getTextGetGlobal()));
    textfield->setScales(s);
}

void Arkanoid::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*selfRect, states);
    textfield->update(size);
    target.draw(*textfield, states);
}

void Arkanoid::Button::eventsHandler(sf::Event &e, sf::RenderWindow &window) {
    sf::Vector2f mousePosition = (sf::Vector2f)sf::Mouse::getPosition(window);
    bool mouseInButton = mousePosition.x >= selfRect->getPosition().x
                          && mousePosition.x <= selfRect->getPosition().x + selfRect->getGlobalBounds().width
                          && mousePosition.y >= selfRect->getPosition().y
                          && mousePosition.y <= selfRect->getPosition().y + selfRect->getGlobalBounds().height;

    if(e.type == sf::Event::MouseMoved)
    {
        if (mouseInButton)
        {
            state = btnState::HOVERED;
        }
        else
            state = btnState::NORMAL;
    }
    else if (e.type == sf::Event::MouseButtonPressed)
    {
        switch(e.mouseButton.button)
        {
            case sf::Mouse::Left:
                if (mouseInButton)
                    state = btnState::PRESSED;
                else
                    state = btnState::NORMAL;
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
                    state = btnState::HOVERED;
                else
                    state = btnState::NORMAL;
                break;
            default:
                break;
        }
    }
    switch(state)
    {
        case btnState::NORMAL:
            selfRect->setFillColor(clrBgNormal);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorNormal();
            break;
        case btnState::HOVERED:
            selfRect->setFillColor(clrBgHovered);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorHovered();
            break;
        case btnState::PRESSED:
            selfRect->setFillColor(clrBgClicked);
            selfRect->setOutlineColor(clrBorder);
            textfield->setCurrentColorClicked();
            //onClick(window);
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
    textField = new TextField;
    textField->setText("");
    selfRect->setFillColor(color);
    window = new sf::RenderWindow(sf::VideoMode((int)s.x, (int)s.y), title);
    view = new sf::View(window->getDefaultView());
}

Arkanoid::Menu::Menu(sf::Vector2f s, sf::Vector2f p, const std::string& background, const std::string& title) : DisplayObject(s, p)
{
    textField = new TextField;
    textField->setText("");
    texture.loadFromFile(background);
    texture.setRepeated(false);
    selfRect->setTexture(&texture);
    window = new sf::RenderWindow(sf::VideoMode((int)s.x, (int)s.y), title);
    view   = new sf::View( selfRect->getGlobalBounds());
}

void Arkanoid::Menu::display()
{
    window->setVerticalSyncEnabled(true);
    DisplayObject::display();
}

void Arkanoid::Menu::eventsHandler()
{
    while (window->isOpen())
    {
        sf::Event event{};
        while (window->pollEvent(event))
        {
            for (auto& o : buttons)
                o.eventsHandler(event, *window);
            switch (event.type)
            {
                case sf::Event::Resized:
                    view->reset(sf::FloatRect(0, 0,(float )event.size.width, (float )event.size.height ));
                    setSize( {(float )event.size.width, (float )event.size.height} );
                    textField->update({(float )event.size.width, (float )event.size.height});
                    for (auto& o : buttons)
                        o.updateSizes({(float )event.size.width, (float )event.size.height} );
                    break;
                case sf::Event::Closed:
                    window->close();
                    break;
                default:
                    break;
            }
        }
        window->setView( *view );
        window->clear();
        window->draw(*selfRect);
        window->draw(*textField);
        for (const auto& o: buttons)
            window->draw(o);
        window->display();
    }
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

void Arkanoid::Blocks::updateBonusPercentage(difficulty diff) {

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

Arkanoid::GameField::GameField(sf::Vector2f s, sf::Vector2f p) : DisplayObject(s, p)
{

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

void Arkanoid::Game::initMenus()
{
    sf::Vector2f res = settings.getResolution();
    startMenu = new Menu(res, {0, 0}, sf::Color::Black, "ARKANOID");
    startMenu->textField->setAllProperties("ARKANOID", totalProjectPath + "/fonts/pixel.ttf",
                                         sf::Text::Bold, S_MAINTEXT_SIZE, S_MAINTEXT_BORDERTHICKNESS,
                                         sf::Color::White, sf::Color::White, sf::Color::White, ColorCustomBlue);
    sf::Vector2f wCenter = DisplayObject::getCenter(startMenu->getGlobal(), startMenu->textField->getTextGetGlobal());
    startMenu->textField->setTextPosition({ wCenter.x, 0.25f * wCenter.y});
    startMenu->textField->setScales(res);

    wCenter = DisplayObject::getCenter(startMenu->getGlobal(), {0, 0, S_BUTTON_WIDTH, S_BUTTON_HEIGHT});
    std::vector<Button> btns =
            {
                Button("START",    { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , wCenter.y }),
                Button("SETTINGS", { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , 1.2f*S_BUTTON_HEIGHT + wCenter.y }),
                Button("QUIT",     { S_BUTTON_WIDTH, S_BUTTON_HEIGHT }, { wCenter.x , 2.4f*S_BUTTON_HEIGHT + wCenter.y }),
            };
    for (auto & o : btns)
    {
        o.display();
        o.setScales(res);
        o.setColors(ColorCustomBlue, ColorCustomDarkBlue, ColorCustomDarkBlue, sf::Color::White);
        o.setBorderThickness(S_BUTTON_BORDERTHICKNESS);
        startMenu->addButton(o);
    }
}

Arkanoid::Game::Game() : gameField(sf::Vector2f(), sf::Vector2f())
{
    initMenus();
}

void Arkanoid::Game::start()
{
    startMenu->display();
    startMenu->eventsHandler();
    startMenu->hide();
}


Arkanoid::Game::~Game()
{

}

