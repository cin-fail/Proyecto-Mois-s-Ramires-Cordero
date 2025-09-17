#include <SFML/Graphics.hpp>
#include <iostream>
#include "Juego.h"
#include "Botón.h"

int showEndPopup(const std::string& title, const std::string& message, sf::Font& font) {
    sf::RenderWindow popup(sf::VideoMode(400, 200), title, sf::Style::Close);
    popup.setFramerateLimit(60);
    sf::Text txt(message, font, 20);
    sf::FloatRect tb = txt.getLocalBounds();
    txt.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    txt.setPosition(180.f, 60.f);
    Button bQuit, bRestart;
    bQuit.setPosition(60.f, 120.f);
    bQuit.setText("Salir", font, 14);
    bRestart.setPosition(200.f, 120.f);
    bRestart.setText("Jugar de nuevo", font, 14);

    while (popup.isOpen()) {
        sf::Event e;
        while (popup.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { popup.close(); return 0; }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mp((float)e.mouseButton.x, (float)e.mouseButton.y);
                if (bQuit.contains(mp)) { popup.close(); return 0; }
                if (bRestart.contains(mp)) { popup.close(); return 1; }
            }
        }
        sf::Texture fondo3;
        fondo3.loadFromFile("Project/FONDO3.jpg");

        sf::Sprite spriteFondo;
        spriteFondo.setTexture(fondo3);
        popup.draw(spriteFondo);
        popup.draw(txt);
        bQuit.draw(popup);
        bRestart.draw(popup);
        popup.display();
    }
    return 0;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Proyecto Moisés", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontOK = false;
    if (font.loadFromFile("C:\\Windows\\Fonts\\times.ttf")) fontOK = true;

    Button bStart, bExit;
    bStart.setPosition(320.f, 240.f);
    bExit.setPosition(320.f, 320.f);
    if (fontOK) {
        bStart.setFillColor(sf::Color(50, 80, 120, 100));
        bStart.setTextColor(sf::Color(230, 230, 230));
        bStart.setText("Iniciar juego", font, 20);

        bExit.setFillColor(sf::Color(180, 140, 80, 180));
        bExit.setTextColor(sf::Color(255, 255, 220));
        bExit.setText("Salir", font, 20);
    }

    bool startPressed = false;
    while (window.isOpen() && !startPressed) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); break; }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
                if (bStart.contains(mp)) {
                    startPressed = true;
                    break;
                }
                if (bExit.contains(mp)) {
                    window.close();
                    break;
                }
            }
        }

        if (!window.isOpen()) break;

        sf::Texture fondo1;
        fondo1.loadFromFile("Project/FONDO1.jpg");

        sf::Sprite spriteFondo;
        spriteFondo.setTexture(fondo1);
        window.draw(spriteFondo);

        if (fontOK) {
            sf::Text title("Horror Match-3", font, 40);
            title.setFillColor(sf::Color::White);
            sf::FloatRect tb = title.getLocalBounds();
            title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
            title.setPosition(400.f, 120.f);
            window.draw(title);

            sf::Text subtitle("Un juego con temática de terror.", font, 19);
            subtitle.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect sb = subtitle.getLocalBounds();
            subtitle.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
            subtitle.setPosition(400.f, 170.f);
            window.draw(subtitle);
        }
        bStart.draw(window);
        bExit.draw(window);
        window.display();
    }

    if (!window.isOpen()) return 0;

    Game game;
    game.seedRand();

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("Project/FONDO2.jpg");
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    for (int i = 0; i < 5; ++i) {
        sf::Texture tex;
        std::string p1 = "Project/gema" + std::to_string(i + 1) + ".png";
        std::string p2 = "Project/gem" + std::to_string(i + 1) + ".png";
        std::string p3 = "resources/gema" + std::to_string(i + 1) + ".png";
        if (tex.loadFromFile(p1)) game.setGemTexture(i, tex);
        else if (tex.loadFromFile(p2)) game.setGemTexture(i, tex);
        else if (tex.loadFromFile(p3)) game.setGemTexture(i, tex);
    }

    if (fontOK) game.setFont(font);

    game.init();

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); break; }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
                game.clickAt(mp);
            }
        }

        game.update(dt);

        window.clear();
        window.draw(backgroundSprite);
        game.draw(window);
        window.display();

        if (game.isWin()) {
            int action = showEndPopup("Has ganado", "Has ganado!", font);
            if (action == 0) { window.close(); break; }
            else {
                game.restart();
                continue;
            }
        }
        else if (game.isLose()) {
            int action = showEndPopup("Has perdido", "Has perdido :(", font);
            if (action == 0) { window.close(); break; }
            else { game.restart(); continue; }
        }
    }

    return 0;
}