#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {  
private:
    sf::RectangleShape shape;
    sf::Text label;

public:
    Button();
    void setPosition(float x, float y);
    void setText(const std::string& s, const sf::Font& f, unsigned int size = 18);
    void draw(sf::RenderWindow& w) const;
    bool contains(sf::Vector2f p) const;
    void setFillColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
};
