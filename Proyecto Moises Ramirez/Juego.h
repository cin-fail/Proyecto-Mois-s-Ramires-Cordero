#pragma once
#include "Pizarra.h"
#include "Gema.h"
#include <SFML/Graphics.hpp>

class Run;

class Game {
private:
    void cellFromPos(const sf::Vector2f& p, int& outR, int& outC) const;
    bool areAdjacent(int r1, int c1, int r2, int c2) const;
    void attemptSwap(int ar, int ac, int br, int bc);
    void findRunsInternal(class Run horiz[], int& hCount, class Run vert[], int& vCount);
    void processMatchesAndCascade();
    void fillNoInitialMatches();
    bool createsMatchAt(int r, int c);

    void syncGemsWithBoardImmediate();
    Board board;
    Gem gems[8][8];

    int score;
    int movesLeft;
    int target;

    bool selected;
    int selR, selC;

    bool animating;
    int swapPhase;
    int pendingA_r, pendingA_c, pendingB_r, pendingB_c;

    sf::Texture gemTextures[5];
    bool texLoaded[5];

    sf::Font font;
    bool fontLoaded;

public:
    Game();

    void seedRand();
    void loadGemTextures();
    void init();
    void startNewGame(int moves, int targetScore);

    void clickAt(const sf::Vector2f& p);
    void update(float dt);
    void draw(sf::RenderWindow& w);

    bool isWin() const;
    bool isLose() const;
    void restart();

    void setGemTexture(int idx, const sf::Texture& tex);
    void setFont(const sf::Font& f);

    int getScore() const;
    int getMovesLeft() const;

};

