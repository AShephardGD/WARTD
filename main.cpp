#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <fstream>

#include "Vidgets/Button.h"
void playNewGame();
void loadGame();
void briefing();
void playingGame();

enum class Race {
    Human,
    Orc
};

short level = -1;
Race race;

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
sf::Music mainMenu;
sf::Music orcBriefing, orcVictory, orcDefeat, orc1, orc2, orc3, orc4, orc5;
sf::Music humanBriefing, humanVictory, humanDefeat, human1, human2, human3, human4, human5;
sf::Sound buttonPressed;

//Настройка окна
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);

//Фон главного меню
sf::Texture backgroundTexture, briefingTexture;
sf::Sprite backgroundSprite, briefingSprite;

//Шрифт
sf::Font quake;

//Текстуры кнопок
sf::Texture orcPressed, orcUnpressed, humanPressed, humanUnpressed;

//Состояние окна
WindowStatus status = WindowStatus::MainMenu;

//Тексты брифингов
std::string orcText1;
std::string humanText1;

void MainMenu() {
    if (mainMenu.getStatus() == sf::SoundSource::Stopped) {
        mainMenu.play();
    }

    //Отрисовка объектов на экране
    window.clear(sf::Color::Blue);

    window.draw(backgroundSprite);

    buttonText = "Warcraft Tower Defence";
    sf::Text text;
    text.setFont(quake);
    text.setString(buttonText);
    text.setCharacterSize(desktopMode.height / 10);
    buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 10;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

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
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::NewGame;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + buttonHeight + 3) && mousePressedY < (buttonPosY + 2 * buttonHeight + 3)) {
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::LoadGame;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 2 * buttonHeight + 6) && mousePressedY < (buttonPosY + 3 * buttonHeight + 6)) {
                buttonPressed.play();
                window.close();
            }
            mousePressedX = -1;
            mousePressedY = -1;
        }
    }
}

void playNewGame() {
    if (mainMenu.getStatus() == sf::SoundSource::Stopped) {
        mainMenu.play();
    }

    window.clear(sf::Color::Blue);

    window.draw(backgroundSprite);

    buttonText = "Warcraft Tower Defence";
    sf::Text text;
    text.setFont(quake);
    text.setString(buttonText);
    text.setCharacterSize(desktopMode.height / 10);
    buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 10;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

    buttonText = "Choose Your Race";
    text.setString(buttonText);
    text.setCharacterSize(desktopMode.height / 25);
    buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 2 - desktopMode.height / 25 - 3;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

    buttonPosX = 3 * desktopMode.width / 8;
    buttonPosY = desktopMode.height / 2;
    buttonWidth = desktopMode.width / 4;
    buttonHeight = desktopMode.height / 25;
    buttonText = "Orc";
    createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                 buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                 mousePressedX, mousePressedY);

    buttonPosY += buttonHeight + 3;
    buttonText = "Human";
    createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                 buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                 mousePressedX, mousePressedY);

    buttonPosY += 2 * buttonHeight + 6;
    buttonText = "Back to Main Menu";
    createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                 buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                 mousePressedX, mousePressedY);

    window.display();

    buttonPosY -= 3 * buttonHeight + 9;
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
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                race = Race::Orc;
                level = 1;
                status = WindowStatus::Briefing;
                mainMenu.stop();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + buttonHeight + 3) && mousePressedY < (buttonPosY + 2 * buttonHeight + 3)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                race = Race::Human;
                level = 1;
                status = WindowStatus::Briefing;
                mainMenu.stop();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 3 * buttonHeight + 9) && mousePressedY < (buttonPosY + 4 * buttonHeight + 9)) {
                buttonPressed.play();
                status = WindowStatus::MainMenu;
                break;
            }
            mousePressedX = -1;
            mousePressedY = -1;
        }
    }
}

void loadGame() {
    if (mainMenu.getStatus() == sf::SoundSource::Stopped) {
        mainMenu.play();
    }

    window.clear(sf::Color::Blue);

    window.draw(backgroundSprite);

    buttonText = "Warcraft Tower Defence";
    sf::Text text;
    text.setFont(quake);
    text.setString(buttonText);
    text.setCharacterSize(desktopMode.height / 10);
    buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 10;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

    buttonPosX = 3 * desktopMode.width / 8;
    buttonPosY = desktopMode.height / 2;
    buttonWidth = desktopMode.width / 4;
    buttonHeight = desktopMode.height / 25;
    std::string buttonsText[7] {"", "", "", "", "", "", "Back To Main Menu"};
    for (short i = 0; i < 7; ++i) {
        buttonText = buttonsText[i];
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
        buttonPosY += buttonHeight + 3;
    }

    buttonPosY = desktopMode.height / 2;
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
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + buttonHeight + 3) && mousePressedY < (buttonPosY + 2 * buttonHeight + 3)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 2 * buttonHeight + 6) && mousePressedY < (buttonPosY + 3 * buttonHeight + 6)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 3 * buttonHeight + 9) && mousePressedY < (buttonPosY + 4 * buttonHeight + 9)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &
                     mousePressedY > (buttonPosY + 4 * buttonHeight + 12) && mousePressedY < (buttonPosY + 5 * buttonHeight + 12)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 5 * buttonHeight + 15) && mousePressedY < (buttonPosY + 6 * buttonHeight + 15)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                     mousePressedY > (buttonPosY + 6 * buttonHeight + 18) && mousePressedY < (buttonPosY + 7 * buttonHeight + 18)) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                status = WindowStatus::MainMenu;
                break;
            }
            mousePressedX = -1;
            mousePressedY = -1;
        }
    }
}

void briefing() {
    window.clear(sf::Color::Blue);

    window.draw(briefingSprite);

    sf::Text text;
    text.setFont(quake);
    if (race == Race::Orc) {
        if (orcBriefing.getStatus() == sf::SoundSource::Stopped) {
            orcBriefing.play();
        }
        text.setColor(sf::Color::Yellow);
        switch (level) {
            case 1:
                text.setString(orcText1);
                break;
        }
    }
    else {
        if (humanBriefing.getStatus() == sf::SoundSource::Stopped) {
            humanBriefing.play();
        }
        text.setColor(sf::Color::White);
        switch (level) {
            case 1:
                text.setString(humanText1);
                break;
        }
    }
    text.setCharacterSize(desktopMode.height / 30);
    buttonPosX = desktopMode.width / 10;
    buttonPosY = desktopMode.height / 10;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

    buttonWidth = desktopMode.width / 9;
    buttonHeight = desktopMode.height / 25;
    buttonPosX = desktopMode.width - 2 * buttonWidth;
    buttonPosY = desktopMode.height - buttonHeight;
    buttonText = "Continue";
    if (race == Race::Orc) {
        createButton(window, quake, orcUnpressed, orcPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
    }
    else {
        createButton(window, quake, humanUnpressed, humanPressed, buttonText,
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
    }

    window.display();
    sf::Event event;
    while(window.pollEvent(event)) {
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
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::MainMenu;
                buttonPressed.play();
                if (race == Race::Orc) {
                    orcBriefing.stop();
                }
                else {
                    humanBriefing.stop();
                }
                break;
            }
            mousePressedX = -1;
            mousePressedY = -1;
        }
    }
}

void playingGame() {

}

int main() {
    //Загрузка музыки
    if (!mainMenu.openFromFile("Music/Warcraft II - Orc Briefing.flac")) {
        return -1;
    }
    mainMenu.setLoop(true);
    if (!orcBriefing.openFromFile("Music/Warcraft II - Orc Briefing.flac")) {
        return -1;
    }
    orcBriefing.setLoop(true);
    if (!orcVictory.openFromFile("Music/Warcraft II - Orc Victory.flac")) {
        return -1;
    }
    orcVictory.setLoop(true);
    if (!orcDefeat.openFromFile("Music/Warcraft II - Orc Defeat.flac")) {
        return -1;
    }
    orcDefeat.setLoop(true);
    if (!orc1.openFromFile("Music/Warcraft II - Orc BGM 1.flac")) {
        return -1;
    }
    orc1.setLoop(true);
    if (!orc2.openFromFile("Music/Warcraft II - Orc BGM 2.flac")) {
        return -1;
    }
    orc2.setLoop(true);
    if (!orc3.openFromFile("Music/Warcraft II - Orc BGM 3.flac")) {
        return -1;
    }
    orc3.setLoop(true);
    if (!orc4.openFromFile("Music/Warcraft II - Orc BGM 4.flac")) {
        return -1;
    }
    orc4.setLoop(true);
    if (!orc5.openFromFile("Music/Warcraft II - Orc BGM 5.flac")) {
        return -1;
    }
    orc5.setLoop(true);
    if (!humanBriefing.openFromFile("Music/Warcraft II - Human Briefing.flac")) {
        return -1;
    }
    humanBriefing.setLoop(true);
    if (!humanVictory.openFromFile("Music/Warcraft II - Human Victory.flac")) {
        return -1;
    }
    humanVictory.setLoop(true);
    if (!humanDefeat.openFromFile("Music/Warcraft II - Human Defeat.flac")) {
        return -1;
    }
    humanDefeat.setLoop(true);
    if (!human1.openFromFile("Music/Warcraft II - Human BGM 1.flac")) {
        return -1;
    }
    human1.setLoop(true);
    if (!human2.openFromFile("Music/Warcraft II - Human BGM 2.flac")) {
        return -1;
    }
    human2.setLoop(true);
    if (!human3.openFromFile("Music/Warcraft II - Human BGM 3.flac")) {
        return -1;
    }
    human3.setLoop(true);
    if (!human4.openFromFile("Music/Warcraft II - Human BGM 4.flac")) {
        return -1;
    }
    human4.setLoop(true);
    if (!human5.openFromFile("Music/Warcraft II - Human BGM 5.flac")) {
        return -1;
    }
    human5.setLoop(true);

    //Загрузка звуков
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

    if (!briefingTexture.loadFromFile("Textures/ui/Briefing_background.png")) {
        return -1;
    }
    briefingTexture.setSmooth(true);

    briefingSprite.setTexture(briefingTexture);
    factorX = ((float) desktopMode.width) / briefingTexture.getSize().x;
    factorY = ((float) desktopMode.height) / briefingTexture.getSize().y;
    briefingSprite.scale(factorX, factorY);

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

    //Загрузка текстов брифингов
    std::ifstream briefingFile("Briefings/orc1.txt");
    std::string readingString;
    if (!briefingFile.is_open()) {
        return -1;
    }
    while (std::getline(briefingFile, readingString)) {
        orcText1 += readingString + "\n";
    }
    briefingFile.close();
    briefingFile.open("Briefings/human1.txt");
    if (!briefingFile.is_open()) {
        return -1;
    }
    while (std::getline(briefingFile, readingString)) {
        humanText1 += readingString + "\n";
    }
    briefingFile.close();

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
    }
    return 0;
}
