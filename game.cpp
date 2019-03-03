#include "game.h"

#include "macro.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

using namespace std;

const int di[] = {-1, -1, -1, 0,  0, 1,  1, 1};
const int dj[] = {-1,  0,  1, -1, 1, -1, 0, 1};

Game::Game(int nrow_, int ncol_)
    : nrow(nrow_), ncol(ncol_),
      mine(nrow, vector<int>(ncol, 0)),
      count(nrow, vector<int>(ncol, 0)),
      isOpen(nrow, vector<int>(ncol, 0))
{
    CELL_WIDTH = 40;
    CELL_HEIGHT = 40;
    SCREEN_WIDTH = ncol*CELL_WIDTH+1;
    SCREEN_HEIGHT = nrow*CELL_HEIGHT+1;
    generateMine();
    countMine();
}

Game::~Game()
{
    //dtor
}

bool Game::initSDL()
{
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
}

bool Game::initWindow()
{
    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
}

bool Game::initRenderer()
{
    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL ) {
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    return true;
}

bool Game::initMessages()
{
    if( TTF_Init() == -1 ) {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        return false;
    }

    TTF_Font* Sans = TTF_OpenFont("arial.ttf", 24);
    if (Sans == NULL) {
        printf( "Font could not be loaded: %s\n", TTF_GetError() );
        return false;
    }

    SDL_Color Color[] = { { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 },
                          { 255, 255, 0 }, { 255, 0, 255 }, { 0, 255, 255 },
                          { 255, 255, 63 }, { 255, 63, 255 }, { 63, 255, 255 } };
    ifor(k, 9) {
        stringstream ss;
        ss << k;
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, ss.str().c_str(), Color[k]);
        numberMessage[k] = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        SDL_FreeSurface(surfaceMessage);
    }
    SDL_Surface* surfaceMessage;
    wonOrLostMessage[0] = NULL;
    surfaceMessage = TTF_RenderText_Solid(Sans, "You lost", { 255, 0, 0 });
    wonOrLostMessage[1] = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    surfaceMessage = TTF_RenderText_Solid(Sans, "You win", { 20, 255, 0 });
    wonOrLostMessage[2] = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    TTF_CloseFont( Sans );
    return true;
}

bool Game::initMineImage()
{
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return false;
    }

    SDL_Surface* loadedSurface = IMG_Load( "minesweeper.png" );
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", "minesweeper.png", IMG_GetError() );
        return false;
    }
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
    //Create texture from surface pixels
    mineMessage = SDL_CreateTextureFromSurface( renderer, loadedSurface );
    if( mineMessage == NULL ) {
        printf( "Unable to create texture from %s! SDL Error: %s\n", "minesweeper.png", SDL_GetError() );
        return false;
    }
    SDL_FreeSurface(loadedSurface);

    return true;
}

bool Game::initGraphics()
{
    if (!initSDL()) return false;
    if (!initWindow()) return false;
    if (!initRenderer()) return false;
    if (!initMessages()) return false;
    if (!initMineImage()) return false;
    return true;
}

void Game::releaseGraphics()
{
    ifor(i, 9) SDL_DestroyTexture(numberMessage[i]);
    ifor(i, 3) SDL_DestroyTexture(wonOrLostMessage[i]);
    SDL_DestroyTexture(mineMessage);
    //Destroy window
    SDL_FreeSurface(screenSurface);
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Game::generateMine()
{
    ijfor(i, j, nrow, ncol) {
        if (rand() % 100 < 20) mine[i][j] = 1;
        else mine[i][j] = 0;
    }
}

void Game::printMine() const
{
    cout << "      ";
    ifor(j, ncol) cout << setw(4) << j;
    cout << endl << "      ";
    ifor(j, ncol) cout << "----";
    cout << endl;
    ifor(i, nrow) {
        cout << setw(4) << i << " |";
        ifor(j, ncol) {
            cout << setw(4) << mine[i][j];
        }
        cout << endl;
    }
}

void Game::countMine()
{
    count = vector<vector<int> >(nrow, vector<int>(ncol, 0));

    ijfor(i, j, nrow, ncol) {
        ifor(k, 8)
            if ( inside(i + di[k], j + dj[k], nrow, ncol) )
                count[i][j] += mine[i+di[k]][j+dj[k]];
    }
}

void Game::renderGame() const
{
    ifor(i, 40) cout << "\n";
    // in 2 dong dau tien (chi so cot)
    cout << "      ";
    ifor(j, ncol) cout << setw(4) << j;
    cout << endl << "      ";
    ifor(j, ncol) cout << "----";
    cout << endl;

    // in cac dong
    ifor(i, nrow) {
        cout << setw(4) << i << " |";
        ifor(j, ncol) {
            if (isOpen[i][j]) {
                cout << setw(4) << count[i][j];
            } else {
                cout << "   *";
            }
        }
        cout << endl;
    }
}

void Game::clickOn(int i, int j)
{
    if (!inside(i, j, nrow, ncol)) return;
    if (isOpen[i][j] == 1) return;
    isOpen[i][j] = 1;
    if (count[i][j] == 0) {
        ifor(k, 8)
            if ( inside(i + di[k], j + dj[k], nrow, ncol) && mine[i+di[k]][j+dj[k]] == 0)
                clickOn(i+di[k], j+dj[k]);
    }
}

bool Game::isAllOpen() const
{
    ijfor(i, j, nrow, ncol)
        if (mine[i][j] == 0 && isOpen[i][j] == 0) return false;
    return true;
}

Game::UserInput Game::getUserInput() const
{
    SDL_Event e;
    UserInput input;
    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if( e.type == SDL_QUIT ) {
            input.type = UserInput::QUIT;
        } else if (e.type == SDL_MOUSEBUTTONUP) {
            input.type = UserInput::CLICK;
            int x, y;
            SDL_GetMouseState(&x, &y);
            input.row = y / CELL_HEIGHT;
            input.col = x / CELL_WIDTH;
            cout << "up: " << input.row << " " << input.col << endl;
        }
    }
    return input;
}

SDL_Rect getCenteredRect(int tx, int ty, int tw, int th, SDL_Texture* texture)
{
    SDL_Rect Message_rect; //create a rect
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    double scalew = (double)tw / w, scaleh = (double)th / h;
    double scale = scalew < scaleh ? scalew : scaleh;
    Message_rect.x = tx + tw/2 - (int)(w*scale/2);  //controls the rect's x coordinate
    Message_rect.y = ty + th/2 - (int)(h*scale/2); // controls the rect's y coordinte
    Message_rect.w = (int)(w*scale); // controls the width of the rect
    Message_rect.h = (int)(h*scale); // controls the height of the rect
    return Message_rect;
}

void renderCenteredTexture(SDL_Renderer* renderer, int tx, int ty, int tw, int th, SDL_Texture* texture)
{
    SDL_Rect rect = getCenteredRect(tx, ty, tw, th, texture);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Game::renderGameGraphics(int wonOrLost) const
{
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( renderer );

    ifor(i, nrow+1) {
        SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0xFF, 0xFF );
        SDL_RenderDrawLine( renderer, 0, i*CELL_HEIGHT, ncol*CELL_WIDTH, i*CELL_HEIGHT );
    }

    ifor(j, ncol+1) {
        SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0xFF, 0xFF );
        SDL_RenderDrawLine( renderer, j*CELL_WIDTH, 0, j*CELL_WIDTH, nrow*CELL_HEIGHT );
    }

    ijfor(i, j, nrow, ncol) {
        if (isOpen[i][j] > 0) {
            SDL_Rect fillRect = { j*CELL_WIDTH+1, i*CELL_HEIGHT+1, CELL_WIDTH-1, CELL_HEIGHT-1 };
            SDL_SetRenderDrawColor( renderer, 0xA0, 0xA0, 0xA0, 0xFF );
            SDL_RenderFillRect( renderer, &fillRect );
            if (mine[i][j] == 0) {
                if (count[i][j] > 0) {
                    renderCenteredTexture(renderer, j*CELL_WIDTH, i*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, numberMessage[count[i][j]]);
                }
            }
        }
        if (wonOrLost > 0 && mine[i][j] > 0) {
            renderCenteredTexture(renderer, j*CELL_WIDTH, i*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, mineMessage);
        }
    }

    if (wonOrLost > 0) {
        renderCenteredTexture(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, wonOrLostMessage[wonOrLost]);
    }

    SDL_RenderPresent( renderer );
    SDL_Delay(25);
}

bool Game::isMine(int i, int j) const
{
    if (!inside(i, j, nrow, ncol)) return false;
    return mine[i][j] > 0;
}
