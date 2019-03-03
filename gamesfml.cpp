#include "gamesfml.h"
#include "macro.h"
#include <iostream>
#include <sstream>

using namespace std;

GameSFML::GameSFML(int nrow, int ncol)
    : Game(nrow, ncol),
      window(sf::VideoMode(SCREEN_WIDTH+1, SCREEN_HEIGHT+1), "SFML works!")
{
}

GameSFML::~GameSFML()
{
    //dtor
}

template<typename VECTOR2>
float getScale(VECTOR2 size, int width, int height)
{
    float scale = min((float)width / size.x, (float)width/size.y);
    return scale;
}

bool GameSFML::initGraphics()
{
    if (!font.loadFromFile("arial.ttf")) return false;
    if (!mineTexture.loadFromFile("minesweeper.png")) return false;
    mineSprite.setTexture(mineTexture);
    float scale = getScale(mineTexture.getSize(), CELL_WIDTH, CELL_HEIGHT);
    mineSprite.setScale( scale, scale );

    sf::Color Color[] = { { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 },
                          { 255, 255, 0 }, { 255, 0, 255 }, { 0, 255, 255 },
                          { 255, 255, 63 }, { 255, 63, 255 }, { 63, 255, 255 } };
    ifor(k, 9) {
        sf::Text& text = numbers[k];
        text.setFont(font);
        stringstream ss;
        ss << k;
        text.setString(ss.str());
        text.setCharacterSize(64);
        text.setFillColor(Color[k]);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
        float scale = getScale(sf::Vector2f{textRect.width, textRect.height}, CELL_WIDTH, CELL_HEIGHT);
        text.setScale(scale, scale);
    }
    ifor(k, 3) {
        if (k == 0) continue;
        sf::Text& text = wonOrLostMessage[k];
        text.setFont(font);
        text.setString(k == 1 ? "You lost" : "You win");
        text.setCharacterSize(64);
        text.setFillColor(k == 1 ? sf::Color::Red : sf::Color::Green);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
        float scale = getScale(sf::Vector2f{textRect.width, textRect.height}, SCREEN_WIDTH, SCREEN_HEIGHT);
        text.setScale(scale, scale);
        text.setPosition({0.5f*SCREEN_WIDTH, 0.5f*SCREEN_HEIGHT});
    }
    return true;
}

void GameSFML::releaseGraphics()
{

}

GameSFML::UserInput GameSFML::getUserInput()
{
    UserInput input;
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) {
            window.close();
            input.type = UserInput::QUIT;
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                input.type = UserInput::CLICK;
                input.row = event.mouseButton.y / CELL_HEIGHT;
                input.col = event.mouseButton.x / CELL_WIDTH;
                std::cout << "mouse x: " << event.mouseButton.x << " " << input.col << std::endl;
                std::cout << "mouse y: " << event.mouseButton.y << " " << input.row << std::endl;
            }
        }
    }

    return input;
}

void GameSFML::renderGameGraphics(int wonOrLost)
{
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::RectangleShape cell({1.0f*CELL_WIDTH, 1.0f*CELL_HEIGHT});
    cell.setOutlineThickness(1.f);
    cell.setOutlineColor(sf::Color::Blue);

    window.clear({0xFF, 0xFF, 0xFF, 0xFF});
    window.draw(shape);
    ijfor(i, j, nrow, ncol) {
        cell.setFillColor(isOpen[i][j] > 0 ? sf::Color({0xA0, 0xA0, 0xA0, 0xFF}) : sf::Color::White);
        cell.setPosition({j*CELL_WIDTH*1.f+1, i*CELL_HEIGHT*1.f+1});
        window.draw(cell);
        if (isOpen[i][j] && mine[i][j] == 0 && count[i][j] > 0) {
            int k = count[i][j];
            numbers[k].setPosition({j*CELL_WIDTH+CELL_WIDTH*0.5f, i*CELL_HEIGHT*1.f+CELL_HEIGHT*0.5f});
            window.draw(numbers[k]);
        }
        if (wonOrLost > 0 && mine[i][j] > 0) {
            mineSprite.setPosition({j*CELL_WIDTH*1.f+1, i*CELL_HEIGHT*1.f+1});
            window.draw(mineSprite);
        }
    }
    if (wonOrLost > 0)
        window.draw(wonOrLostMessage[wonOrLost]);

    window.display();
    sf::sleep(sf::milliseconds(25));
}
