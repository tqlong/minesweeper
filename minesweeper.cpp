#include <iostream>

#include "game.h"

using namespace std;

int main(int argc, char** argv)
{
    typedef Game::UserInput UserInput;
    Game g(10, 6);
    if (!g.initGraphics()) return 1;
    bool stop = false;
    int wonOrLost = 0;
    do {
        g.renderGameGraphics(wonOrLost);
        UserInput input = g.getUserInput();
        switch (input.type) {
        case UserInput::QUIT:
            stop = true;
            break;
        case UserInput::CLICK:
            if (wonOrLost != 0) break; // stop accepting new input after won or lost
            if (g.isMine(input.row, input.col)) {
                cout << "You lost." << endl;
                g.printMine();
                wonOrLost = 1;
            } else {
                g.clickOn(input.row, input.col);
                if (g.isAllOpen()) {
                    cout << "You win." << endl;
                    wonOrLost = 2;
                }
            }
            break;
        }
    } while ( !stop );
    g.releaseGraphics();
    return 0;
}
