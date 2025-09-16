#include <SFML/Graphics.hpp>
#include <iostream>
#include "Juego.h"
#include "Botón.h"

int showEndPopup(const std::string& title, const std::string& message, sf::Font& font) {
    // 0 = salir, 1 = reiniciar
    sf::RenderWindow popup(sf::VideoMode(360, 200), title, sf::Style::Close);
    popup.setFramerateLimit(60);
    sf::Text txt(message, font, 20);
    sf::FloatRect tb = txt.getLocalBounds();
    txt.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    txt.setPosition(180.f, 60.f);
    Button bQuit, bRestart;
    bQuit.setPosition(60.f, 120.f);
    bRestart.setPosition(200.f, 120.f);
    bQuit.setText("Salir", font);
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
        popup.clear(sf::Color(50, 50, 50));
        popup.draw(txt);
        bQuit.draw(popup);
        bRestart.draw(popup);
        popup.display();
    }
    return 0;
}

int main() {
    // ventana principal 800x600
    sf::RenderWindow window(sf::VideoMode(800, 600), "Candy OOP - 8x8", sf::Style::Close);
    window.setFramerateLimit(60);

    Game game;
    game.seedRand();

    // cargar texturas (opcional) y fuente
    for (int i = 0; i < 5; ++i) {
        sf::Texture tex;
        std::string p1 = "Project/gema" + std::to_string(i + 1) + ".png";
        std::string p2 = "Project/gem" + std::to_string(i + 1) + ".png";
        std::string p3 = "resources/gema" + std::to_string(i + 1) + ".png";
        if (tex.loadFromFile(p1)) game.setGemTexture(i, tex);
        else if (tex.loadFromFile(p2)) game.setGemTexture(i, tex);
        else if (tex.loadFromFile(p3)) game.setGemTexture(i, tex);
    }

    sf::Font font;
    if (font.loadFromFile("C:\\Windows\\Fonts\\times.ttf") ||
        font.loadFromFile("C:\\Windows\\Fonts\\timesnewroman.ttf") ||
        font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf") ||
        font.loadFromFile("resources/times.ttf")) {
        game.setFont(font);
    }

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

        window.clear(sf::Color(20, 20, 20));
        game.draw(window);
        window.display();

        if (game.isWin()) {
            int action = showEndPopup("Has ganado", "Has ganado!", font);
            if (action == 0) { window.close(); break; }
            else { game.restart(); continue; }
        }
        else if (game.isLose()) {
            int action = showEndPopup("Has perdido", "Has perdido :(", font);
            if (action == 0) { window.close(); break; }
            else { game.restart(); continue; }
        }
    }

    return 0;
}