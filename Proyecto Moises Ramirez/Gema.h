#pragma once
#include <SFML/Graphics.hpp>

class Gem {
private:
    float animTime, animDuration;
    bool hasTexture;

public:
    Gem();
    sf::CircleShape circle;
    sf::Sprite sprite;

    void init(int t, int r, int c);
    void setTexture(const sf::Texture& tex);
    void clearTexture();
    void updateImage();
    void setTarget(float nx, float ny, float dur = 0.18f);
    void update(float dt);
    void draw(sf::RenderWindow& w) const;
    void setPositionImmediate(float nx, float ny);
    void forceSetType(int t, const sf::Texture* tex);

    int type;
    int gridR, gridC;
    float x, y, tx, ty;
    bool moving;
};
