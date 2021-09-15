#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Vidgets/Button.h"

int main() {
    //Для подсчёта атрибутов кнопки в отдельные переменные
    size_t buttonPosX, buttonPosY, buttonWidth, buttonHeight;
    std::string buttonText;

    //Позиция клика
    size_t mousePressedX = -1, mousePressedY = -1;

    //Настройка музыки в главном меню
    sf::Music music;
    if (!music.openFromFile("Music/Warcraft II - Orc Briefing.flac")) {
        return 1;
    }
    music.setLoop(true);

    //Настройка окна
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);

    //Создание фона главного меню
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

    //Загрузка шрифта
    sf::Font quake;
    if (!quake.loadFromFile("Fonts/Quake Cyr.ttf")) {
        return -1;
    }

    //Загрузка текстур кнопок
    sf::Texture orcPressed, orcUnpressed, humanPressed, humanUnpressed;
    if (!orcPressed.loadFromFile("Textures/ui/Orc/button-large-pressed.png")) {
        return -1;
    }
    if (!orcUnpressed.loadFromFile("Textures/ui/Orc/button-large-normal.png")) {
        return -1;
    }
    if (!humanPressed.loadFromFile("Textures/ui/Human/button-large-pressed.png")) {
        return -1;
    }
    if (!humanUnpressed.loadFromFile("Textures/ui/Human/button-large-normal.png")) {
        return -1;
    }
    orcPressed.setSmooth(true);
    orcUnpressed.setSmooth(true);
    humanPressed.setSmooth(true);
    humanUnpressed.setSmooth(true);

    while (window.isOpen()) {

        //Отрисовка объектов на экране
        window.clear(sf::Color::Blue);

        buttonPosX = desktopMode.width / 3;
        buttonPosY = desktopMode.height / 2;
        buttonWidth = orcPressed.getSize().x;
        buttonHeight = orcPressed.getSize().y;
        buttonText = "New Game";
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
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
