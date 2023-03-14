#include "game_classes.h"

using namespace std;

void closeWindow(sf::RenderWindow& w)
{
    w.close();
}


int main()
{
    Arkanoid::Game game;
    game.start();
    return 0;
}