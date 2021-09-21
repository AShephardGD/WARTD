#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Vidgets/Button.h"
void playNewGame();
void loadGame();
void briefing();
void playingGame();

enum class Race {
    Human,
    Orc
};

short Level = -1;

enum class WindowStatus {
    MainMenu,
    NewGame,
    LoadGame,
    Game,
    Briefing
};

//Для подсчёта атрибутов кнопки в отдельные переменные
size_t buttonPosX = 0, buttonPosY = 0;
float buttonWidth = 0, buttonHeight = 0;
std::string buttonText;

//Позиция клика
size_t mousePressedX = -1, mousePressedY = -1;

//Музыка и звуки
sf::Music music;
sf::Sound buttonPressed;

//Настройка окна
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);

//Фон главного меню
sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;

//Шрифт
sf::Font quake;

//Текстуры кнопок
sf::Texture orcPressed, orcUnpressed, humanPressed, humanUnpressed;

//Состояние окна
WindowStatus status = WindowStatus::MainMenu;

void MainMenu() {
        //Отрисовка объектов на экране
        window.clear(sf::Color::Blue);

        buttonText = "Warcraft Tower Defence";
        sf::Text text;
        text.setFont(quake);
        text.setString(buttonText);
        buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
        buttonPosY = desktopMode.height / 10;
        text.setPosition(buttonPosX, buttonPosY);
        window.draw(text);

        window.draw(backgroundSprite);

        buttonPosX = 3 * desktopMode.width / 8;
        buttonPosY = desktopMode.height / 2;
        buttonWidth = desktopMode.width / 4;
        buttonHeight = desktopMode.height / 25;
        buttonText = "New Game";
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);

        buttonPosY += buttonHeight + 3;
        buttonText = "Load Game";
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);

        buttonPosY += buttonHeight + 3;
        buttonText = "Exit Game";
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);

        buttonPosY -= 2 * (buttonHeight + 3);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                mousePressedX = sf::Mouse::getPosition().x;
                mousePressedY = sf::Mouse::getPosition().y;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                    mousePressedY > buttonPosY && mousePressedY < (buttonPosY + buttonHeight)) {
                    playNewGame();
                    buttonPressed.play();
                }
                else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                         mousePressedY > (buttonPosY + buttonHeight + 3) && mousePressedY < (buttonPosY + 2 * buttonHeight + 3)) {
                    loadGame();
                    buttonPressed.play();
                }
                else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                         mousePressedY > (buttonPosY + 2 * buttonHeight + 6) && mousePressedY < (buttonPosY + 3 * buttonHeight + 6)) {
                    window.close();
                    buttonPressed.play();
                }
                mousePressedX = -1;
                mousePressedY = -1;
            }
        }
}

void playNewGame() {

}

void loadGame() {

}

void briefing() {

}

void playingGame() {

}

int main() {

    //Настройка музыки в главном меню
    if (!music.openFromFile("Music/Warcraft II - Orc Briefing.flac")) {
        return -1;
    }
    music.setLoop(true);
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("SFX/ui/buttonPressed.wav")) {
        return -1;
    }
    buttonPressed.setBuffer(buffer);

    //Создание фона главного меню
    if (!backgroundTexture.loadFromFile("Textures/ui/Menu_background_without_title.png")) {
        return -1;
    }
    backgroundTexture.setSmooth(true);

    backgroundSprite.setTexture(backgroundTexture);
    float factorX = ((float) desktopMode.width) / backgroundTexture.getSize().x,
          factorY = ((float) desktopMode.height) / backgroundTexture.getSize().y;
    backgroundSprite.scale(factorX, factorY);

    //Загрузка шрифта
    if (!quake.loadFromFile("Fonts/Quake Cyr.ttf")) {
        return -1;
    }

    //Загрузка текстур кнопок
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

        switch (status) {
            case WindowStatus::MainMenu:
                MainMenu();
                break;
            case WindowStatus::NewGame:
                playNewGame();
                break;
            case WindowStatus::LoadGame:
                loadGame();
                break;
            case WindowStatus::Briefing:
                briefing();
                break;
            case WindowStatus::Game:
                playingGame();
                break;
        }


        //Запуск музыки в главном меню
        if (music.getStatus() == sf::SoundSource::Stopped) {
            music.play();
        }


    }
    return 0;
}
