#pragma once
#include "Pizarra.h"
#include "Gema.h"
#include <SFML/Graphics.hpp>

class Run; // forward (si necesitas, pero aquí ponemos Run dentro de .cpp)

class Game {
public:
    Game();

    void seedRand();
    void loadGemTextures(); // intenta cargar texturas
    void init();            // inicia tablero y gems
    void startNewGame(int moves, int targetScore);

    void clickAt(const sf::Vector2f& p);
    void update(float dt);
    void draw(sf::RenderWindow& w);

    bool isWin() const;
    bool isLose() const;
    void restart();

    // setters para recursos desde main
    void setGemTexture(int idx, const sf::Texture& tex);
    void setFont(const sf::Font& f);

    // getters
    int getScore() const;
    int getMovesLeft() const;

private:
    // helpers privados
    void cellFromPos(const sf::Vector2f& p, int& outR, int& outC) const;
    bool areAdjacent(int r1, int c1, int r2, int c2) const;
    void attemptSwap(int ar, int ac, int br, int bc);
    void findRunsInternal(/*out*/ class Run horiz[], int& hCount, /*out*/ class Run vert[], int& vCount);
    void processMatchesAndCascade();
    void fillNoInitialMatches();
    bool createsMatchAt(int r, int c);

    Board board;
    Gem gems[8][8];

    int score;
    int movesLeft;
    int target;

    bool selected;
    int selR, selC;

    bool animating;
    int swapPhase; // 0 none, 1 swapped forward, 2 swapped back
    int pendingA_r, pendingA_c, pendingB_r, pendingB_c;

    sf::Texture gemTextures[5];
    bool texLoaded[5];

    sf::Font font;
    bool fontLoaded;
};

