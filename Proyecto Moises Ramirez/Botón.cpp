#include "Botón.h"

Button::Button() {
    shape.setSize({ 160.f,42.f });
    shape.setFillColor(sf::Color(100, 100, 100));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    // label repositioned when setText is called
}

void Button::setText(const std::string& s, const sf::Font& f, unsigned int size) {
    label.setFont(f);
    label.setString(s);
    label.setCharacterSize(size);
    label.setFillColor(sf::Color::White);
    sf::FloatRect tb = label.getLocalBounds();
    label.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    label.setPosition(shape.getPosition().x + shape.getSize().x / 2.f, shape.getPosition().y + shape.getSize().y / 2.f);
}

void Button::draw(sf::RenderWindow& w) const {
    w.draw(shape);
    w.draw(label);
}

bool Button::contains(sf::Vector2f p) const {
    return shape.getGlobalBounds().contains(p);
}
