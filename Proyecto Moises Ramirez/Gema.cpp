#include "Gema.h"

static const float CELL_SIZE_LOCAL = 56.f; // usado para inicializar círculo (coincide con CONFIG en main)
static float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

Gem::Gem()
    : type(-1), gridR(0), gridC(0), x(0), y(0), tx(0), ty(0),
    moving(false), animTime(0.f), animDuration(0.18f), hasTexture(false)
{
    circle.setRadius(CELL_SIZE_LOCAL * 0.4f);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
}

void Gem::init(int t, int r, int c) {
    type = t; gridR = r; gridC = c;
    // Posición por defecto (main define offsets). Estos valores son actualizados por Game inmediatamente.
    x = tx = 0.f; y = ty = 0.f;
    moving = false; animTime = 0.f;
    updateColor();
    if (hasTexture) sprite.setPosition(x, y);
}

void Gem::setTexture(const sf::Texture& tex) {
    hasTexture = true;
    sprite.setTexture(tex);
    sf::Vector2u ts = tex.getSize();
    if (ts.x > 0 && ts.y > 0) {
        sprite.setOrigin(ts.x * 0.5f, ts.y * 0.5f);
        float desired = CELL_SIZE_LOCAL * 0.8f;
        sprite.setScale(desired / ts.x, desired / ts.y);
    }
}

void Gem::clearTexture() {
    hasTexture = false;
}

void Gem::updateColor() {
    sf::Texture GEMA1;
    sf::Texture GEMA2;
    sf::Texture GEMA3;
    sf::Texture GEMA4;
    sf::Texture GEMA5;
    if (type == 0) GEMA1.loadFromFile("Project/GEMA1.png");
    else if (type == 1) GEMA2.loadFromFile("Project/GEMA2.png");
    else if (type == 2) GEMA3.loadFromFile("Project/GEMA3.png");
    else if (type == 3) GEMA4.loadFromFile("Project/GEMA4.png");
    else GEMA5.loadFromFile("Project/GEMA5.png");
}

void Gem::setTarget(float nx, float ny, float dur) {
    tx = nx; ty = ny; animTime = 0.f; animDuration = dur; moving = true;
}

void Gem::update(float dt) {
    if (!moving) return;
    animTime += dt;
    float t = animTime / animDuration;
    if (t >= 1.f) {
        x = tx; y = ty; moving = false;
    }
    else {
        x = lerp_f(x, tx, t);
        y = lerp_f(y, ty, t);
    }
    circle.setPosition(x, y);
    if (hasTexture) sprite.setPosition(x, y);
}

void Gem::draw(sf::RenderWindow& w) const {
    if (hasTexture) w.draw(sprite);
    else w.draw(circle);
}