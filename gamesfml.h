#ifndef GAMESFML_H
#define GAMESFML_H

#include "game.h"
#include <SFML/Graphics.hpp>

class GameSFML : public Game
{
public:
    GameSFML(int nrow, int ncol);
    virtual ~GameSFML();

    bool initGraphics();
    void releaseGraphics();
    UserInput getUserInput();
    bool isWindowOpen() const { return window.isOpen(); }
    void renderGameGraphics(int wonOrLost);
protected:
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture mineTexture;
    sf::Sprite mineSprite;
    sf::Text numbers[9];
    sf::Text wonOrLostMessage[3];
private:
};

#endif // GAMESFML_H
