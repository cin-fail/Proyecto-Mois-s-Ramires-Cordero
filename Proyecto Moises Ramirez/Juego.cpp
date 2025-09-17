#include "Juego.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

class Run {
public:
    Run() { length = 0; horizontal = true; }
    void add(int r, int c) { if (length < 8 * 8) { rr[length] = r; cc[length] = c; ++length; } }
    int length;
    int rr[8 * 8];
    int cc[8 * 8];
    bool horizontal;
};

static const int ROWS_LOCAL = 8;
static const int COLS_LOCAL = 8;
static const int GEM_TYPES_LOCAL = 5;
static const float CELL_SIZE_LOCAL = 56.f;

const int SCORE_MATCH_3 = 100; 
const int SCORE_MATCH_4 = 150; 
const int SCORE_MATCH_5 = 200; 
const int SCORE_MATCH_L = 200;


Game::Game()
    : score(0), movesLeft(20), target(5000),
    selected(false), selR(-1), selC(-1),
    animating(false), swapPhase(0),
    pendingA_r(-1), pendingA_c(-1), pendingB_r(-1), pendingB_c(-1),
    fontLoaded(false)
{
    for (int i = 0; i < GEM_TYPES_LOCAL; ++i) texLoaded[i] = false;
}

void Game::seedRand() {
    std::srand((unsigned)std::time(nullptr));
}

void Game::loadGemTextures() {
    for (int i = 0; i < GEM_TYPES_LOCAL; ++i) {
        texLoaded[i] = false;
        std::string p1 = "Project/gema" + std::to_string(i + 1) + ".png";
        std::string p2 = "Project/gem" + std::to_string(i + 1) + ".png";
        std::string p3 = "resources/gema" + std::to_string(i + 1) + ".png";
        std::string tries[3] = { p1,p2,p3 };
        for (int k = 0; k < 3; ++k) {
            if (gemTextures[i].loadFromFile(tries[k])) {
                texLoaded[i] = true;
                std::cout << "Loaded gem texture: " << tries[k] << "\n";
                break;
            }
        }
    }
}

void Game::init() {
    board.clear();
    loadGemTextures();
    fillNoInitialMatches();
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) {
        int t = board.get(r, c);
        auto p = Board::cellCenter(r, c);
        gems[r][c].init(t, r, c);
        gems[r][c].x = p.x; gems[r][c].y = p.y; gems[r][c].tx = p.x; gems[r][c].ty = p.y;
        gems[r][c].moving = false;
        if (t >= 0 && t < GEM_TYPES_LOCAL && texLoaded[t]) {
            gems[r][c].setTexture(gemTextures[t]);
            gems[r][c].sprite.setPosition(p);
        }
        else {
            gems[r][c].clearTexture();
            gems[r][c].circle.setPosition(p);
            gems[r][c].updateColor();
        }
    }
    score = 0;
    movesLeft = 20;
    target = 5000;
    selected = false; selR = selC = -1;
    animating = false; swapPhase = 0;
}

void Game::syncGemsWithBoardImmediate() {
    for (int r = 0; r < ROWS_LOCAL; ++r) {
        for (int c = 0; c < COLS_LOCAL; ++c) {
            int t = board.get(r, c);
            if (t >= 0 && t < GEM_TYPES_LOCAL && texLoaded[t]) {
                gems[r][c].forceSetType(t, &gemTextures[t]);
            }
            else {
                gems[r][c].forceSetType(t, nullptr);
            }
            auto p = Board::cellCenter(r, c);
            gems[r][c].setPositionImmediate(p.x, p.y);
        }
    }
}

void Game::startNewGame(int moves, int targetScore) {
    movesLeft = moves; target = targetScore; score = 0;
    init();
}

void Game::clickAt(const sf::Vector2f& p) {
    if (animating) return;
    int r, c; cellFromPos(p, r, c);
    if (r < 0) return;
    if (!selected) { selected = true; selR = r; selC = c; }
    else {
        if (r == selR && c == selC) { selected = false; selR = selC = -1; return; }
        if (areAdjacent(r, c, selR, selC)) {
            attemptSwap(selR, selC, r, c);
        }
        else {
            selR = r; selC = c;
        }
    }
}

void Game::update(float dt) {
    bool anyMoving = false;
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) {
        gems[r][c].update(dt);
        if (gems[r][c].moving) anyMoving = true;
    }
    animating = anyMoving;
    if (anyMoving) return;

    if (swapPhase == 1) {
        Run h[ROWS_LOCAL * COLS_LOCAL]; Run v[ROWS_LOCAL * COLS_LOCAL]; int hC = 0, vC = 0;
        findRunsInternal(h, hC, v, vC);
        if (hC == 0 && vC == 0) {
            int ar = pendingA_r, ac = pendingA_c, br = pendingB_r, bc = pendingB_c;
            int tA = board.get(ar, ac);
            int tB = board.get(br, bc);
            board.set(ar, ac, tB);
            board.set(br, bc, tA);
            int curA_type = board.get(ar, ac);
            int curB_type = board.get(br, bc);
            if (curA_type >= 0 && curA_type < GEM_TYPES_LOCAL && texLoaded[curA_type])
                gems[ar][ac].forceSetType(curA_type, &gemTextures[curA_type]);
            else gems[ar][ac].forceSetType(curA_type, nullptr);

            if (curB_type >= 0 && curB_type < GEM_TYPES_LOCAL && texLoaded[curB_type])
                gems[br][bc].forceSetType(curB_type, &gemTextures[curB_type]);
            else gems[br][bc].forceSetType(curB_type, nullptr);

            auto pa = Board::cellCenter(ar, ac);
            auto pb = Board::cellCenter(br, bc);
            gems[ar][ac].setTarget(pb.x, pb.y, 0.12f);
            gems[br][bc].setTarget(pa.x, pa.y, 0.12f);

            animating = true; swapPhase = 2;
        }
        else {
            processMatchesAndCascade();
            movesLeft = std::max(0, movesLeft - 1);
            swapPhase = 0;
            pendingA_r = pendingA_c = pendingB_r = pendingB_c = -1;
        }
        return;
    }


    if (swapPhase == 2) {
        swapPhase = 0;
        animating = false;
        pendingA_r = pendingA_c = pendingB_r = pendingB_c = -1;
        syncGemsWithBoardImmediate();
        return;
    }

    if (!animating && swapPhase == 0) {
        Run htmp[ROWS_LOCAL * COLS_LOCAL]; Run vtmp[ROWS_LOCAL * COLS_LOCAL]; int hC = 0, vC = 0;
        findRunsInternal(htmp, hC, vtmp, vC);
        if (hC > 0 || vC > 0) {
            processMatchesAndCascade();
        }
        else {
            animating = false;
            syncGemsWithBoardImmediate();
        }
    }
}

void Game::draw(sf::RenderWindow& w) {
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) {
        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE_LOCAL - 6.f, CELL_SIZE_LOCAL - 6.f));
        cell.setPosition(40.f + c * CELL_SIZE_LOCAL + 3.f, 40.f + r * CELL_SIZE_LOCAL + 3.f);
        cell.setFillColor(sf::Color(30, 30, 30));
        cell.setOutlineThickness(2.f);
        cell.setOutlineColor(sf::Color(60, 60, 60));
        w.draw(cell);
    }
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) {
        if (selected && selR == r && selC == c) {
            sf::CircleShape outline(CELL_SIZE_LOCAL * 0.4f + 6.f);
            outline.setOrigin(outline.getRadius(), outline.getRadius());
            outline.setPosition(gems[r][c].x, gems[r][c].y);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor(sf::Color::White);
            outline.setOutlineThickness(3.f);
            w.draw(outline);
        }
        gems[r][c].draw(w);
    }
    if (fontLoaded) {
        sf::Text info;
        info.setFont(font);
        info.setCharacterSize(18);
        info.setFillColor(sf::Color::White);
        info.setPosition(40.f + COLS_LOCAL * CELL_SIZE_LOCAL + 20.f, 40.f);
        info.setString(std::string("Score: ") + std::to_string(score) +
            std::string("\nMoves left: ") + std::to_string(movesLeft) +
            std::string("\nTarget: ") + std::to_string(target));
        w.draw(info);
    }
}

bool Game::isWin() const { return score >= target; }
bool Game::isLose() const { return movesLeft <= 0 && score < target; }
void Game::restart() { init(); }

void Game::setGemTexture(int idx, const sf::Texture& tex) {
    if (idx < 0 || idx >= GEM_TYPES_LOCAL) return;
    gemTextures[idx] = tex;
    texLoaded[idx] = true;
}

void Game::setFont(const sf::Font& f) {
    font = f; fontLoaded = true;
}

int Game::getScore() const { return score; }
int Game::getMovesLeft() const { return movesLeft; }

void Game::cellFromPos(const sf::Vector2f& p, int& outR, int& outC) const {
    int c = (int)((p.x - 40.f) / CELL_SIZE_LOCAL);
    int r = (int)((p.y - 40.f) / CELL_SIZE_LOCAL);
    if (r < 0 || r >= ROWS_LOCAL || c < 0 || c >= COLS_LOCAL) { outR = -1; outC = -1; }
    else { outR = r; outC = c; }
}

bool Game::areAdjacent(int r1, int c1, int r2, int c2) const {
    int dr = r1 - r2; if (dr < 0) dr = -dr;
    int dc = c1 - c2; if (dc < 0) dc = -dc;
    return (dr + dc) == 1;
}

void Game::attemptSwap(int ar, int ac, int br, int bc) {
    int ta = board.get(ar, ac);
    int tb = board.get(br, bc);
    board.set(ar, ac, tb);
    board.set(br, bc, ta);

    int newA_type = board.get(ar, ac);
    int newB_type = board.get(br, bc);

    if (newA_type >= 0 && newA_type < GEM_TYPES_LOCAL && texLoaded[newA_type]) {
        gems[ar][ac].forceSetType(newA_type, &gemTextures[newA_type]);
    }
    else {
        gems[ar][ac].forceSetType(newA_type, nullptr);
    }

    if (newB_type >= 0 && newB_type < GEM_TYPES_LOCAL && texLoaded[newB_type]) {
        gems[br][bc].forceSetType(newB_type, &gemTextures[newB_type]);
    }
    else {
        gems[br][bc].forceSetType(newB_type, nullptr);
    }

    auto pa = Board::cellCenter(ar, ac);
    auto pb = Board::cellCenter(br, bc);
    gems[ar][ac].setTarget(pb.x, pb.y);
    gems[br][bc].setTarget(pa.x, pa.y);

    pendingA_r = ar; pendingA_c = ac; pendingB_r = br; pendingB_c = bc;
    animating = true; swapPhase = 1;
    selected = false; selR = selC = -1;
}

void Game::findRunsInternal(Run horiz[], int& hCount, Run vert[], int& vCount) {
    hCount = 0; vCount = 0;

    for (int r = 0; r < ROWS_LOCAL; ++r) {
        int c = 0;
        while (c < COLS_LOCAL) {
            int start = c;
            int t = board.get(r, c);
            if (t < 0) { ++c; continue; }
            int end = c;
            while (end + 1 < COLS_LOCAL && board.get(r, end + 1) == t) ++end;
            int len = end - start + 1;
            if (len >= 3) {
                Run& run = horiz[hCount++];
                run.length = 0; run.horizontal = true;
                for (int x = start; x <= end; ++x) run.add(r, x);
            }
            c = end + 1;
        }
    }
    for (int c = 0; c < COLS_LOCAL; ++c) {
        int r = 0;
        while (r < ROWS_LOCAL) {
            int start = r;
            int t = board.get(r, c);
            if (t < 0) { ++r; continue; }
            int end = r;
            while (end + 1 < ROWS_LOCAL && board.get(end + 1, c) == t) ++end;
            int len = end - start + 1;
            if (len >= 3) {
                Run& run = vert[vCount++];
                run.length = 0; run.horizontal = false;
                for (int y = start; y <= end; ++y) run.add(y, c);
            }
            r = end + 1;
        }
    }
}

void Game::processMatchesAndCascade() {
    Run horiz[ROWS_LOCAL * COLS_LOCAL]; Run vert[ROWS_LOCAL * COLS_LOCAL];
    int hCount = 0, vCount = 0;
    findRunsInternal(horiz, hCount, vert, vCount);
    if (hCount == 0 && vCount == 0) return;

    bool toClear[ROWS_LOCAL][COLS_LOCAL];
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) toClear[r][c] = false;

    int totalScoreAdd = 0;
    int horizScoreArr[ROWS_LOCAL * COLS_LOCAL] = { 0 };
    int vertScoreArr[ROWS_LOCAL * COLS_LOCAL] = { 0 };

    for (int i = 0; i < hCount; ++i) {
        int len = horiz[i].length;
        int sc = (len >= 5) ? SCORE_MATCH_5 : (len == 4 ? SCORE_MATCH_4 : SCORE_MATCH_3);
        horizScoreArr[i] = sc;
        totalScoreAdd += sc;
        for (int k = 0; k < len; ++k) {
            int rr = horiz[i].rr[k];
            int cc = horiz[i].cc[k];
            toClear[rr][cc] = true;
        }
    }
    for (int i = 0; i < vCount; ++i) {
        int len = vert[i].length;
        int sc = (len >= 5) ? SCORE_MATCH_5 : (len == 4 ? SCORE_MATCH_4 : SCORE_MATCH_3);
        vertScoreArr[i] = sc;
        totalScoreAdd += sc;
        for (int k = 0; k < len; ++k) {
            int rr = vert[i].rr[k];
            int cc = vert[i].cc[k];
            toClear[rr][cc] = true;
        }
    }

    bool horizCounted[ROWS_LOCAL * COLS_LOCAL] = { 0 }, vertCounted[ROWS_LOCAL * COLS_LOCAL] = { 0 };
    for (int hi = 0; hi < hCount; ++hi) {
        for (int vi = 0; vi < vCount; ++vi) {
            bool inter = false;
            for (int a = 0; a < horiz[hi].length && !inter; ++a)
                for (int b = 0; b < vert[vi].length && !inter; ++b)
                    if (horiz[hi].rr[a] == vert[vi].rr[b] && horiz[hi].cc[a] == vert[vi].cc[b]) inter = true;
            if (inter && !horizCounted[hi] && !vertCounted[vi]) {
                totalScoreAdd -= horizScoreArr[hi];
                totalScoreAdd -= vertScoreArr[vi];
                totalScoreAdd += SCORE_MATCH_L;
                horizCounted[hi] = true;
                vertCounted[vi] = true;
            }
        }
    }

    score += totalScoreAdd;

    for (int c = 0; c < COLS_LOCAL; ++c) {
        int write = ROWS_LOCAL - 1;
        for (int r = ROWS_LOCAL - 1; r >= 0; --r) {
            if (!toClear[r][c]) {
                board.set(write, c, board.get(r, c));
                --write;
            }
        }
        for (int r = write; r >= 0; --r) board.set(r, c, rand() % GEM_TYPES_LOCAL);
    }

    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) {
        int t = board.get(r, c);
        gems[r][c].type = t;
        gems[r][c].updateColor();
        auto p = Board::cellCenter(r, c);
        gems[r][c].setTarget(p.x, p.y, 0.18f);
        if (t >= 0 && t < GEM_TYPES_LOCAL && texLoaded[t]) gems[r][c].setTexture(gemTextures[t]);
        else gems[r][c].clearTexture();
    }

    animating = true;
}

void Game::fillNoInitialMatches() {
    for (int r = 0; r < ROWS_LOCAL; ++r) for (int c = 0; c < COLS_LOCAL; ++c) board.set(r, c, 0);
    for (int r = 0; r < ROWS_LOCAL; ++r) {
        for (int c = 0; c < COLS_LOCAL; ++c) {
            int tries = 0;
            while (true) {
                int t = rand() % GEM_TYPES_LOCAL;
                board.set(r, c, t);
                if (createsMatchAt(r, c) && tries < 20) { ++tries; continue; }
                break;
            }
        }
    }
}

bool Game::createsMatchAt(int r, int c) {
    int t = board.get(r, c);
    if (t < 0) return false;
    int cL = c - 1, countL = 0, cR = c + 1, countR = 0, rU = r - 1, countU = 0, rD = r + 1, countD = 0;
    while (cL >= 0 && board.get(r, cL) == t) {
        ++countL; --cL; 
    }
    while (cR < COLS_LOCAL && board.get(r, cR) == t) {
        ++countR; ++cR; 
    }
    if (countL + 1 + countR >= 3) {
        return true;
    }
    while (rU >= 0 && board.get(rU, c) == t) { 
        ++countU; --rU; 
    }
    while (rD < ROWS_LOCAL && board.get(rD, c) == t) { 
        ++countD; ++rD; 
    }
    if (countU + 1 + countD >= 3) return true;
    return false;
}
