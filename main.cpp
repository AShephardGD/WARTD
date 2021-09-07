#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

int main() {
    //Настройка музыки в главном меню
    sf::Music music;
    if (!music.openFromFile("Music/Warcraft II - Orc Briefing.flac")) {
        return 1;
    }
    music.setLoop(true);

    //Настройка окна
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);
    while (window.isOpen()) {

        //Создание фона главного меню
        window.clear(sf::Color::Blue);

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("Textures/ui/Menu_background_without_title.png")) {
            return -1;
        }
        backgroundTexture.setSmooth(true);

        sf::Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        float factorX = ((float) desktopMode.width) / backgroundTexture.getSize().x,
              factorY = ((float) desktopMode.height) / backgroundTexture.getSize().y;
        backgroundSprite.scale(factorX, factorY);
        window.draw(backgroundSprite);

        window.display();

        //Запуск музыки в главном меню
        if (music.getStatus() == sf::SoundSource::Stopped) {
            music.play();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                window.close();
            }
        }


    }
    return 0;
}
