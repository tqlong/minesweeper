#ifndef GAME_H
#define GAME_H

#include <vector>
#include <SDL2/SDL.h>

class Game
{
public:
    struct UserInput {
        enum Type { CLICK, QUIT };
        Type type;
        int row;
        int col;
    };

public:
    Game(int nrow_ = 10, int ncol_ = 12);
    virtual ~Game();

    bool initGraphics();
    void releaseGraphics();
    void printMine() const;
    void renderGame() const;
    void renderGameGraphics(int wonOrLost) const;
    UserInput getUserInput();
    void clickOn(int i, int j);
    bool isAllOpen() const;
    bool isMine(int i, int j) const;

protected:
    int nrow = 4, ncol = 5;
    int CELL_WIDTH = 40;
    int CELL_HEIGHT = 40;
    int SCREEN_WIDTH = ncol*CELL_WIDTH+1;
    int SCREEN_HEIGHT = nrow*CELL_HEIGHT+1;
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;
    //The window renderer
    SDL_Renderer* renderer = NULL;
    //The messages
    SDL_Texture* Message[9];
    //The messages
    SDL_Texture* WonOrLostMessage[3];
    //Mine
    SDL_Texture* Mine;

    typedef std::vector<std::vector<int> > Table;
    Table mine, count, isOpen;

private:
    void generateMine();
    void countMine();
    bool initSDL();
    bool initWindow();
    bool initRenderer();
    bool initMessages();
    bool initMineImage();
};

#endif // GAME_H
