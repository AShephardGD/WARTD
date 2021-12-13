#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>
#include <ctime>

#include "Vidgets/Button.h"
#include "SupportingClasses.h"

void playNewGame();
void loadGame();
void briefing();
void playingGame();

enum class Race {
    Human,
    Orc
};

enum class WindowStatus {
    MainMenu,
    NewGame,
    LoadGame,
    Game,
    Briefing
};

enum class Terrarian {
    Summer,
    Swamp,
    Wasteland,
    Winter
};

//Настройка окна
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);

//Состояние игры
short level = -1, lastMusic = -1, music = -1, currentHealth = 20;
size_t currentGold = 0;
short PlayingGameIcons = 0;
const unsigned char maxLevel = 1, MaxHealth = 20, Tower = 2, Units = 1, Ammo = 1, MaxWaves = 1;
const unsigned short MainMenuButtons = 3, PlayNewGameButtons = 3, LoadGameButtons = 7, PlayingGameButtons = 4;
const unsigned short boxSize = 32, windowScaleWidth = desktopMode.width / 1920, windowScaleHeight = desktopMode.height / 1080;

const unsigned short HMapLeft[maxLevel] {106 * windowScaleWidth};
const unsigned short HMapTop[maxLevel] {0};
const unsigned short HMapWidth[maxLevel] {50};
const unsigned short HMapHeight[maxLevel] {34};
const unsigned short HMapEntry[3 * maxLevel] {0, 14, 6}; // ширина, высота, длина входа
const unsigned short HHallLeft[maxLevel] {46};
const unsigned short HHallTop[maxLevel] {12};
const unsigned short HLevelWaves[maxLevel] {1};
//Grunt
const unsigned short HEnemyHealth[Units] {250};
const unsigned char HEnemyDefense[Units] {0};
const unsigned char HEnemyBoxWidth[Units] {72};
const unsigned char HEnemyBoxHeight[Units] {72};
const unsigned char HEnemyMovingBoxStart[Units] {0};
const unsigned char HEnemyMovingBoxCount[Units] {5};
const unsigned char HEnemyDyingBoxStart[Units] {8};
const unsigned char HEnemyDyingBoxCount[Units] {3};
const unsigned char HEnemyUpBox[Units] {0};
const unsigned char HEnemyDownBox[Units] {4};
const unsigned char HEnemyLeftBox[Units] {2};
const unsigned char HEnemyRightBox[Units] {6};

const unsigned short OMapLeft[maxLevel] {0};
const unsigned short OMapTop[maxLevel] {184 * windowScaleHeight};
const unsigned short OMapWidth[maxLevel] {53};
const unsigned short OMapHeight[maxLevel] {28};
const unsigned short OMapEntry[3 * maxLevel] {22, 0, 5}; //ширина, высота, длина входа
const unsigned short OHallLeft[maxLevel] {0};
const unsigned short OHallTop[maxLevel] {24};
const unsigned short OLevelWaves[maxLevel] {1};
//Footman
const size_t startedGold[maxLevel] {100};
const std::pair<short, short> Waves[maxLevel][MaxWaves] = {{std::make_pair(10, 0)}};


bool isPaused = false, cantBuild = false, notEnoughGold = false;
unsigned short isBuilding = 0;
Race race;
Terrarian terrarian;
std::vector<std::vector<char>> map;
std::vector<sf::Sprite> playingTowers;
std::vector<Unit> currentWave;
std::vector<sf::Sprite> ammoSprites;
unsigned short fireIndex = 0;

size_t towerCost[Tower] {1, 10};
size_t towerAttack[2 * Tower] {0, 0, 14, 16};
float towerReload[Tower] {0, 0.5};
std::string towerNames[Tower] {"Wall", "Scout Tower"};

//Для подсчёта атрибутов кнопки в отдельные переменные
size_t buttonPosX = 0, buttonPosY = 0;
float buttonWidth = 0, buttonHeight = 0;
std::string buttonText;

//Позиция клика
size_t mousePressedX = -1, mousePressedY = -1;

//Музыка и звуки
sf::Music mainMenu;
sf::Music orcBriefing, orcVictory, orcDefeat;
sf::Music humanBriefing, humanVictory, humanDefeat;
sf::Music orcMusic[5];
sf::Music humanMusic[5];
sf::Sound buttonPressed;
sf::Sound construction;
sf::Sound error;

//Фоны
sf::Texture backgroundTexture, briefingTexture;
sf::Texture hGameTexture[maxLevel], oGameTexture[maxLevel];
sf::Texture hInfoTexture, oInfoTexture;
sf::Texture goldTexture, buildingTexture, healthTexture;
sf::Texture oTowerIconTextures[Tower], hTowerIconTextures[Tower];
sf::Texture oTowerTextures[Tower], hTowerTextures[Tower];
sf::Texture hHallTexture, oHallTexture;
sf::Texture hEnemyTextures[Units], oEnemyTextures[Units];
sf::Texture ammoTextures[Ammo];
sf::Texture smallFireTexture, bigFireTexture;
sf::Sprite backgroundSprite, briefingSprite;
sf::Sprite hGameSprite[maxLevel];
sf::Sprite oGameSprite[maxLevel];
sf::Sprite hInfoSprite, oInfoSprite;
sf::Sprite goldSprite, buildingSprite, healthSprite;
sf::Sprite oTowerIconSprites[Tower], hTowerIconSprites[Tower];
sf::Sprite oTowerSprites[Tower], hTowerSprites[Tower];
sf::Sprite hHallSprite, oHallSprite;
sf::Sprite smallFireSprite, bigFireSprite;

//Шрифт
sf::Font quake;

//Текстуры кнопок
sf::Texture orcPressed, orcUnpressed, humanPressed, humanUnpressed;

//Состояние окна
WindowStatus status = WindowStatus::MainMenu;

//Тексты брифингов
std::string orcText[maxLevel];
std::string humanText[maxLevel];

std::vector<std::pair<short, short>> bfs() {
    std::vector<std::vector<char>> copyMap = map;
    std::queue<Path> q;
    if (race == Race::Orc) {
        q.push(Path(OMapEntry[3 * level], OMapEntry[3 * level + 1], std::vector<std::pair<short, short>>()));
    }
    else {
        q.push(Path(HMapEntry[3 * level], HMapEntry[3 * level + 1], std::vector<std::pair<short, short>>()));
    }
    copyMap[q.front().y][q.front().x] = 9;
    while (!q.empty()) {
        Path p = q.front();
        q.pop();
        if (p.x >= 1) {
            if (copyMap[p.y][p.x - 1] == 2) {
                p.path.push_back(std::make_pair(p.x - 1, p.y));
                return p.path;
            }
            if (!copyMap[p.y][p.x - 1]) {
                copyMap[p.y][p.x - 1] = 9;
                q.push(Path(p.x - 1, p.y, p.path));
            }
        }
        if (p.x < copyMap[p.y].size() - 1) {
            if (copyMap[p.y][p.x + 1] == 2) {
                p.path.push_back(std::make_pair(p.x - 1, p.y));
                return p.path;
            }
            if (!copyMap[p.y][p.x + 1]) {
                copyMap[p.y][p.x + 1] = 9;
                q.push(Path(p.x + 1, p.y, p.path));
            }
        }
        if (p.y >= 1) {
            if (copyMap[p.y - 1][p.x] == 2) {
                p.path.push_back(std::make_pair(p.x, p.y - 1));
                return p.path;
            }
            if (!copyMap[p.y - 1][p.x]) {
                copyMap[p.y - 1][p.x] = 9;
                q.push(Path(p.x, p.y - 1, p.path));
            }
        }
        if (p.y < copyMap.size() - 1) {
            if (copyMap[p.y + 1][p.x] == 2) {
                p.path.push_back(std::make_pair(p.x, p.y + 1));
                return p.path;
            }
            if (!copyMap[p.y + 1][p.x]) {
                copyMap[p.y + 1][p.x] = 9;
                q.push(Path(p.x, p.y + 1, p.path));
            }
        }
    }
    return std::vector<std::pair<short, short>>(0);
}

void MainMenu() {
    if (mainMenu.getStatus() == sf::SoundSource::Stopped) {
        mainMenu.play();
    }

    size_t buttonsPosX[MainMenuButtons], buttonsPosY[MainMenuButtons],
           buttonsWidth[MainMenuButtons], buttonsHeight[MainMenuButtons];

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

    std::string buttonsText[MainMenuButtons] {"New Game", "Load Game", "Exit Game"};
    for (short i = 0; i < MainMenuButtons; ++i) {
        buttonsWidth[i] = desktopMode.width / 4;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = 3 * desktopMode.width / 8;
        buttonsPosY[i] = desktopMode.height / 2 + i * (buttonsHeight[i] + 3);
        createButton(window, quake, orcUnpressed, orcPressed, buttonsText[i],
                     buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                     mousePressedX, mousePressedY);
    }

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
            if (mousePressedX > buttonsPosX[0] && mousePressedX < (buttonsPosX[0] + buttonsWidth[0]) &&
                mousePressedY > buttonsPosY[0] && mousePressedY < (buttonsPosY[0] + buttonsHeight[0])) {
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::NewGame;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::LoadGame;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
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

    size_t buttonsPosX[PlayNewGameButtons], buttonsPosY[PlayNewGameButtons],
           buttonsWidth[PlayNewGameButtons], buttonsHeight[PlayNewGameButtons];

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

    std::string buttonsText[PlayNewGameButtons] {"Orc", "Human", "Back to Main Menu"};
    for (short i = 0; i < PlayNewGameButtons; ++i) {
        buttonsWidth[i] = desktopMode.width / 4;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = 3 * desktopMode.width / 8;
        buttonsPosY[i] = desktopMode.height / 2 + i * (buttonsHeight[i] + 3);
        createButton(window, quake, orcUnpressed, orcPressed, buttonsText[i],
                     buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                     mousePressedX, mousePressedY);
    }

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
            if (mousePressedX > buttonsPosX[0] && mousePressedX < (buttonsPosX[0] + buttonsWidth[0]) &&
                mousePressedY > buttonsPosY[0] && mousePressedY < (buttonsPosY[0] + buttonsHeight[0])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                race = Race::Orc;
                terrarian = Terrarian::Swamp;
                level = 0;
                status = WindowStatus::Briefing;
                mainMenu.stop();
                break;
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                race = Race::Human;
                terrarian = Terrarian::Summer;
                level = 0;
                status = WindowStatus::Briefing;
                mainMenu.stop();
                break;
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
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

    size_t buttonsPosX[LoadGameButtons], buttonsPosY[LoadGameButtons],
           buttonsWidth[LoadGameButtons], buttonsHeight[LoadGameButtons];

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

    std::string buttonsText[7] {"", "", "", "", "", "", "Back To Main Menu"};
    for (short i = 0; i < 7; ++i) {
        buttonsWidth[i] = desktopMode.width / 4;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = 3 * desktopMode.width / 8;
        buttonsPosY[i] = desktopMode.height / 2 + i * (buttonsHeight[i] + 3);
        createButton(window, quake, orcUnpressed, orcPressed, buttonsText[i],
                     buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                     mousePressedX, mousePressedY);
    }

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
            if (mousePressedX > buttonsPosX[0] && mousePressedX < (buttonsPosX[0] + buttonsWidth[0]) &&
                mousePressedY > buttonsPosY[0] && mousePressedY < (buttonsPosY[0] + buttonsHeight[0])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[3] && mousePressedX < (buttonsPosX[3] + buttonsWidth[3]) &&
                     mousePressedY > buttonsPosY[3] && mousePressedY < (buttonsPosY[3] + buttonsHeight[3])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[4] && mousePressedX < (buttonsPosX[4] + buttonsWidth[4]) &
                     mousePressedY > buttonsPosY[4] && mousePressedY < (buttonsPosY[4] + buttonsHeight[4])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[5] && mousePressedX < (buttonsPosX[5] + buttonsWidth[5]) &&
                     mousePressedY > buttonsPosY[5] && mousePressedY < (buttonsPosY[5] + buttonsHeight[5])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[6] && mousePressedX < (buttonsPosX[6] + buttonsWidth[6]) &&
                     mousePressedY > buttonsPosY[6] && mousePressedY < (buttonsPosY[6] + buttonsHeight[6])) {
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
        text.setString(orcText[level]);
    }
    else {
        if (humanBriefing.getStatus() == sf::SoundSource::Stopped) {
            humanBriefing.play();
        }
        text.setColor(sf::Color::White);
        text.setString(humanText[level]);
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
                status = WindowStatus::Game;
                buttonPressed.play();
                if (race == Race::Orc) {
                    orcBriefing.stop();
                    map = std::vector<std::vector<char>>(OMapHeight[level], std::vector<char>(OMapWidth[level], 0));
                    for (unsigned short i = 0; i < 4; ++i) {
                        for (unsigned short j = 0; j < 4; ++j) {
                            map[OHallTop[level] + i][OHallLeft[level] + j] = 2;
                        }
                    }
                    for (unsigned short i = 0; i < OMapEntry[3 * level + 2]; ++i) {
                        if (!OMapEntry[level]) {
                            map[OMapEntry[3 * level + 1] + i][0] = 3;
                        }
                        else {
                            map[0][OMapEntry[3 * level] + i] = 3;
                        }
                    }
                    buttonPosX = OMapLeft[level] + (OHallLeft[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = OMapTop[level] + (OHallTop[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().height / 2;
                    smallFireSprite.setPosition(buttonPosX, buttonPosY);
                    bigFireSprite.setPosition(buttonPosX, buttonPosY);
                }
                else {
                    humanBriefing.stop();
                    map = std::vector<std::vector<char>>(HMapHeight[level], std::vector<char>(HMapWidth[level], 0));
                    for (unsigned short i = 0; i < 4; ++i) {
                        for (unsigned short j = 0; j < 4; ++j) {
                            map[HHallTop[level] + i][HHallLeft[level] + j] = 2;
                        }
                    }
                    for (unsigned short i = 0; i < HMapEntry[3 * level + 2]; ++i) {
                        if (!HMapEntry[level]) {
                            map[HMapEntry[3 * level + 1] + i][0] = 3;
                        }
                        else {
                            map[0][HMapEntry[3 * level] + i] = 3;
                        }
                    }
                    buttonPosX = HMapLeft[level] + (HHallLeft[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = HMapTop[level] + (HHallTop[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().height / 2;
                    smallFireSprite.setPosition(buttonPosX, buttonPosY);
                    bigFireSprite.setPosition(buttonPosX, buttonPosY);
                }
                currentGold = startedGold[level];
                PlayingGameIcons = 1;
                playingTowers = std::vector<sf::Sprite>();
                isBuilding = 0;
                currentHealth = MaxHealth;
                break;
            }
            mousePressedX = -1;
            mousePressedY = -1;
        }
    }
}

void playingGame() {
    window.clear(sf::Color::Blue);

    short dist = 1;
    switch (isBuilding) {
        case 2:
            dist = 2;
            break;
    }

    size_t buttonsPosX[PlayingGameButtons], buttonsPosY[PlayingGameButtons],
           buttonsWidth[PlayingGameButtons], buttonsHeight[PlayingGameButtons],
           iconsPosX[PlayingGameIcons], iconsPosY[PlayingGameIcons],
           iconsWidth[PlayingGameIcons], iconsHeight[PlayingGameIcons];

    std::string buttonsText[PlayingGameButtons] {"Exit", "Save", "Pause", "Next"};

    for (short i = 0; i < PlayingGameButtons; ++i) {
        buttonsWidth[i] = desktopMode.width / 9;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = desktopMode.width - (buttonsWidth[0] + 1);
        buttonsPosY[i] = desktopMode.height - (i + 1) * (buttonsHeight[0] + 1);
    }

    short towerX, towerY, xBox, yBox;
    if (race == Race::Orc) {
        if (orcMusic[0].getStatus() == sf::SoundSource::Stopped &&
            orcMusic[1].getStatus() == sf::SoundSource::Stopped &&
            orcMusic[2].getStatus() == sf::SoundSource::Stopped &&
            orcMusic[3].getStatus() == sf::SoundSource::Stopped &&
            orcMusic[4].getStatus() == sf::SoundSource::Stopped) {
            lastMusic = music;
            while (music == lastMusic) {
                music = std::rand() % 5;
            }
            orcMusic[music].play();
        }

        window.draw(oGameSprite[level]);
        window.draw(oHallSprite);

        for (unsigned short i = 0; i < playingTowers.size(); ++i) {
            window.draw(playingTowers[i]);
        }

        if (currentHealth < 5) {
            fireIndex = fireIndex % 250;
            bigFireSprite.setTextureRect(sf::IntRect(48 * (fireIndex % 125 / 25), 48 * (fireIndex / 125), 48, 48));
            window.draw(bigFireSprite);
            ++fireIndex;
        }
        else if (currentHealth < 15) {
            fireIndex = fireIndex % 150;
            smallFireSprite.setTextureRect(sf::IntRect(32 * (fireIndex % 125 / 25), 16 + 48 * (fireIndex / 125), 32, 32));
            window.draw(smallFireSprite);
            ++fireIndex;
        }

        if (isBuilding) {
            towerX = OMapLeft[level] + boxSize * windowScaleWidth * ((sf::Mouse::getPosition(window).x - OMapLeft[level]) / boxSize);
            towerY = OMapTop[level] + boxSize * windowScaleHeight * ((sf::Mouse::getPosition(window).y - OMapTop[level]) / boxSize);
            xBox = (sf::Mouse::getPosition(window).x - OMapLeft[level]) / boxSize;
            yBox = (sf::Mouse::getPosition(window).y - OMapTop[level]) / boxSize;
            if (towerX < OMapLeft[level]) {
                towerX = OMapLeft[level];
                xBox = 0;
            }
            else if (towerX > OMapLeft[level] + (OMapWidth[level] - dist) * boxSize * windowScaleWidth) {
                towerX = OMapLeft[level] + (OMapWidth[level] - dist) * boxSize * windowScaleWidth;
                xBox = (OMapWidth[level] - dist);
            }
            if (towerY < OMapTop[level]) {
                towerY = OMapTop[level];
                yBox = 0;
            }
            else if (towerY > OMapTop[level] + (OMapHeight[level] - dist) * boxSize * windowScaleHeight) {
                towerY = OMapTop[level] + (OMapHeight[level] - dist) * boxSize * windowScaleHeight;
                yBox = (OMapHeight[level] - dist);
            }
            oTowerSprites[isBuilding - 1].setPosition(towerX, towerY);
            window.draw(oTowerSprites[isBuilding - 1]);
        }

        window.draw(oInfoSprite);
        for (short i = 0; i < Tower; ++i) {
            window.draw(oTowerIconSprites[i]);
        }
        for (short i = 0; i < PlayingGameButtons; ++i) {
            createButton(window, quake, orcUnpressed, orcPressed, buttonsText[i],
                         buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                         mousePressedX, mousePressedY);
        }
    }
    else {
        if (humanMusic[0].getStatus() == sf::SoundSource::Stopped &&
            humanMusic[1].getStatus() == sf::SoundSource::Stopped &&
            humanMusic[2].getStatus() == sf::SoundSource::Stopped &&
            humanMusic[3].getStatus() == sf::SoundSource::Stopped &&
            humanMusic[4].getStatus() == sf::SoundSource::Stopped) {
            lastMusic = music;
            while (music == lastMusic) {
                music = std::rand() % 5;
            }
            humanMusic[music].play();
        }

        window.draw(hGameSprite[level]);
        window.draw(hHallSprite);

        for (unsigned short i = 0; i < playingTowers.size(); ++i) {
            window.draw(playingTowers[i]);
        }

        if (currentHealth < 15) {
            smallFireSprite.setTextureRect(sf::IntRect(32 * (fireIndex % 125 / 25), 16 + 48 * (fireIndex / 125), 32, 32));
            window.draw(smallFireSprite);
            ++fireIndex;
            fireIndex = fireIndex % 150;
        }

        if (isBuilding) {
            towerX = HMapLeft[level] + boxSize * windowScaleWidth * ((sf::Mouse::getPosition(window).x - HMapLeft[level]) / boxSize);
            towerY = HMapTop[level] + boxSize * windowScaleHeight * ((sf::Mouse::getPosition(window).y - HMapTop[level]) / boxSize);
            xBox = (sf::Mouse::getPosition(window).x - HMapLeft[level]) / boxSize;
            yBox = (sf::Mouse::getPosition(window).y - HMapTop[level]) / boxSize;
            if (towerX < HMapLeft[level]) {
                towerX = HMapLeft[level];
                xBox = 0;
            }
            else if (towerX > HMapLeft[level] + (HMapWidth[level] - dist) * boxSize * windowScaleWidth) {
                towerX = HMapLeft[level] + (HMapWidth[level] - dist) * boxSize * windowScaleWidth;
                xBox = (HMapWidth[level] - dist);
            }
            if (towerY < HMapTop[level]) {
                towerY = HMapTop[level];
                yBox = 0;
            }
            else if (towerY > HMapTop[level] + (HMapHeight[level] - dist) * boxSize * windowScaleHeight) {
                towerY = HMapTop[level] + (HMapHeight[level] - dist) * boxSize * windowScaleHeight;
                yBox = (HMapHeight[level] - dist);
            }
            hTowerSprites[isBuilding - 1].setPosition(towerX, towerY);
            window.draw(hTowerSprites[isBuilding - 1]);
        }

        window.draw(hInfoSprite);
        for (short i = 0; i < Tower; ++i) {
            window.draw(hTowerIconSprites[i]);
        }
        for (short i = 0; i < PlayingGameButtons; ++i) {
            createButton(window, quake, humanUnpressed, humanPressed, buttonsText[i],
                         buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                         mousePressedX, mousePressedY);
        }
    }

    buttonText = std::to_string(currentGold);
    buttonPosX = 8 * desktopMode.width / 9;
    sf::Text towerCharacteristics[3]; //Название, урон, скорострельность
    for (short i = 0; i < 3; ++i) {
        buttonPosY = 1 + 2 * goldSprite.getGlobalBounds().height + i * desktopMode.height / 40;
        towerCharacteristics[i].setFont(quake);
        towerCharacteristics[i].setCharacterSize(desktopMode.height / 50);
        towerCharacteristics[i].setPosition(buttonPosX, buttonPosY);
        towerCharacteristics[i].setColor(sf::Color::Yellow);
    }
    if ((sf::Mouse::getPosition(window).x > hTowerIconSprites[0].getGlobalBounds().left &&
        sf::Mouse::getPosition(window).x < (hTowerIconSprites[0].getGlobalBounds().left + hTowerIconSprites[0].getGlobalBounds().width) &&
        sf::Mouse::getPosition(window).y > hTowerIconSprites[0].getGlobalBounds().top &&
        sf::Mouse::getPosition(window).y < (hTowerIconSprites[0].getGlobalBounds().top + hTowerIconSprites[0].getGlobalBounds().height)) ||
        isBuilding == 1) {
        buttonText += '-' + std::to_string(towerCost[0]);
        towerCharacteristics[0].setString(towerNames[0]);
        towerCharacteristics[1].setString("Damage: -");
        towerCharacteristics[2].setString("Rate: -");
        for (short i = 0; i < 3; ++i) {
            window.draw(towerCharacteristics[i]);
        }

    }
    else if ((sf::Mouse::getPosition(window).x > hTowerIconSprites[1].getGlobalBounds().left &&
             sf::Mouse::getPosition(window).x < (hTowerIconSprites[1].getGlobalBounds().left + hTowerIconSprites[1].getGlobalBounds().width) &&
             sf::Mouse::getPosition(window).y > hTowerIconSprites[1].getGlobalBounds().top &&
             sf::Mouse::getPosition(window).y < (hTowerIconSprites[1].getGlobalBounds().top + hTowerIconSprites[1].getGlobalBounds().height)) ||
             isBuilding == 2) {
        buttonText += '-' + std::to_string(towerCost[1]);
        towerCharacteristics[0].setString(towerNames[1]);
        towerCharacteristics[1].setString("Damage: " + std::to_string(towerAttack[2]) + '-' + std::to_string(towerAttack[3]));
        std::string s = std::to_string(towerReload[1]);
        s = s.substr(0, s.find('.') + 3);
        towerCharacteristics[2].setString("Rate: " + s);
        for (short i = 0; i < 3; ++i) {
            window.draw(towerCharacteristics[i]);
        }
    }
    buttonPosX = 8 * desktopMode.width / 9 + goldSprite.getGlobalBounds().width;
    buttonPosY = 0;
    sf::Text goldText;
    goldText.setFont(quake);
    goldText.setCharacterSize(goldSprite.getGlobalBounds().height - 2);
    goldText.setPosition(buttonPosX, buttonPosY);
    goldText.setColor(sf::Color::Yellow);
    goldText.setString(buttonText);
    window.draw(goldText);
    window.draw(goldSprite);

    buttonPosY = goldSprite.getGlobalBounds().height;
    sf::Text healthText;
    healthText.setFont(quake);
    healthText.setCharacterSize(healthSprite.getGlobalBounds().height - 2);
    healthText.setPosition(buttonPosX, buttonPosY);
    healthText.setColor(sf::Color::Red);
    healthText.setString(std::to_string(currentHealth));
    window.draw(healthSprite);
    window.draw(healthText);

    buttonPosX = desktopMode.width / 100;
    buttonPosY = 23 * desktopMode.height / 25;
    sf::Text cantBuildText;
    cantBuildText.setFont(quake);
    cantBuildText.setCharacterSize(desktopMode.height / 40);
    cantBuildText.setPosition(buttonPosX, buttonPosY);
    cantBuildText.setColor(sf::Color::Yellow);
    cantBuildText.setString("You can't build there.");

    sf::Text notEnough;
    notEnough.setFont(quake);
    notEnough.setCharacterSize(desktopMode.height / 40);
    notEnough.setPosition(buttonPosX, buttonPosY);
    notEnough.setColor(sf::Color::Yellow);
    notEnough.setString("Not enough gold for building tower.");

    if (cantBuild) {
        window.draw(cantBuildText);
    }
    if (notEnoughGold) {
        window.draw(notEnough);
    }

    if (isBuilding) {
        window.draw(buildingSprite);
    }

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
            if (mousePressedX > buttonsPosX[0] && mousePressedX < (buttonsPosX[0] + buttonsWidth[0]) &&
                mousePressedY > buttonsPosY[0] && mousePressedY < (buttonsPosY[0] + buttonsHeight[0])) {
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::MainMenu;
                buttonPressed.play();
                if (race == Race::Orc) {
                    orcMusic[music].stop();
                }
                else {
                    humanMusic[music].stop();
                }
                break;
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                break;
            }
            else if (mousePressedX > buildingSprite.getGlobalBounds().left && mousePressedX < (buildingSprite.getGlobalBounds().left + buildingSprite.getGlobalBounds().width) &&
                     mousePressedY > buildingSprite.getGlobalBounds().top && mousePressedY < (buildingSprite.getGlobalBounds().top + buildingSprite.getGlobalBounds().height) &&
                     isBuilding) {
                mousePressedX = -1;
                mousePressedY = -1;
                buttonPressed.play();
                isBuilding = 0;
                break;
            }
            else if (mousePressedX > hTowerIconSprites[0].getGlobalBounds().left) {
                for (short i = 0; i < Tower; ++i) {
                    if (mousePressedX > hTowerIconSprites[i].getGlobalBounds().left && mousePressedX < (hTowerIconSprites[i].getGlobalBounds().left + hTowerIconSprites[i].getGlobalBounds().width) &&
                        mousePressedY > hTowerIconSprites[i].getGlobalBounds().top && mousePressedY < (hTowerIconSprites[i].getGlobalBounds().top + hTowerIconSprites[i].getGlobalBounds().height)) {
                        buttonPressed.play();
                        isBuilding = i + 1;
                        break;
                    }
                }
                mousePressedX = -1;
                mousePressedY = -1;
                break;
            }
            else if (isBuilding) {
                cantBuild = false;
                notEnoughGold = false;
                for (unsigned short i = 0; i < dist; ++i) {
                    for (unsigned short j = 0; j < dist; ++j) {
                        if (map[yBox + i][xBox + j]) {
                            error.play();
                            cantBuild = true;
                            break;
                        }
                    }
                }
                if (cantBuild) {
                    break;
                }
                if (currentGold < towerCost[isBuilding - 1]) {
                    error.play();
                    notEnoughGold = true;
                    break;
                }
                for (unsigned short i = 0; i < dist; ++i) {
                    for (unsigned short j = 0; j < dist; ++j) {
                        map[yBox + i][xBox + j] = 1;
                    }
                }
                if (!bfs().size()) {
                    error.play();
                    cantBuild = true;
                    for (unsigned short i = 0; i < dist; ++i) {
                        for (unsigned short j = 0; j < dist; ++j) {
                            map[yBox + i][xBox + j] = 0;
                        }
                    }
                    break;
                }
                construction.play();
                if (race == Race::Orc) {
                    playingTowers.push_back(sf::Sprite(oTowerTextures[isBuilding - 1]));
                }
                else {
                    playingTowers.push_back(sf::Sprite(hTowerTextures[isBuilding - 1]));
                }
                playingTowers[playingTowers.size() - 1].setPosition(towerX, towerY);
                playingTowers[playingTowers.size() - 1].setScale(windowScaleWidth, windowScaleHeight);
                mousePressedX = -1;
                mousePressedY = -1;
                currentGold -= towerCost[isBuilding - 1];
                break;
            }
            mousePressedX = -1;
            mousePressedY = -1;
            cantBuild = false;
            notEnoughGold = false;
        }
    }

    window.display();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

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
    for (short i = 0; i < 5; ++i) {
        if (!orcMusic[i].openFromFile("Music/Warcraft II - Orc BGM " + std::to_string(i + 1) + ".flac")) {
            return -1;
        }
    }
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
    for (short i = 0; i < 5; ++i) {
        if (!humanMusic[i].openFromFile("Music/Warcraft II - Human BGM " + std::to_string(i + 1) + ".flac")) {
            return -1;
        }
    }

    //Загрузка звуков
    sf::SoundBuffer pressing, constructing, cantDoThis;
    if (!pressing.loadFromFile("SFX/ui/buttonPressed.wav")) {
        return -1;
    }
    buttonPressed.setBuffer(pressing);
    if (!constructing.loadFromFile("SFX/ui/Constrct.wav")) {
        return -1;
    }
    construction.setBuffer(constructing);
    if (!cantDoThis.loadFromFile("SFX/ui/Error.wav")) {
        return -1;
    }
    error.setBuffer(cantDoThis);

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

    //Загрузка текстур
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

    if (!buildingTexture.loadFromFile("Textures/Tilesets/Building.png")) {
        return -1;
    }
    buildingTexture.setSmooth(true);
    factorX = ((float) desktopMode.width) / buildingTexture.getSize().x / 25;
    factorY = ((float) desktopMode.height) / buildingTexture.getSize().y / 25;
    buildingSprite.setTexture(buildingTexture);
    buildingSprite.scale(factorX, factorY);
    buttonPosX = desktopMode.width - buildingSprite.getGlobalBounds().width - 1;
    buttonPosY = 1;
    buildingSprite.setPosition(buttonPosX, buttonPosY);

    if (!oInfoTexture.loadFromFile("Textures/ui/Orc/infopanel.png")) {
        return -1;
    }
    oInfoTexture.setRepeated(true);
    oInfoSprite.setTexture(oInfoTexture);
    oInfoSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 8, desktopMode.height));
    oInfoSprite.setPosition(8 * desktopMode.width / 9, 0);
    if (!hInfoTexture.loadFromFile("Textures/ui/Human/infopanel.png")) {
        return -1;
    }
    hInfoTexture.setRepeated(true);
    hInfoSprite.setTexture(hInfoTexture);
    hInfoSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 8, desktopMode.height));
    hInfoSprite.setPosition(8 * desktopMode.width / 9, 0);

    if (!oHallTexture.loadFromFile("Textures/Tilesets/Orc/great_hall.png")) {
        return -1;
    }
    oHallTexture.setSmooth(true);
    oHallSprite.setTexture(oHallTexture);
    oHallSprite.setScale(windowScaleWidth, windowScaleHeight);
    oHallSprite.setPosition(0, desktopMode.height - oHallSprite.getGlobalBounds().height);
    if (!hHallTexture.loadFromFile("Textures/Tilesets/Human/town_hall.png")) {
        return -1;
    }
    hHallTexture.setSmooth(true);
    hHallSprite.setTexture(hHallTexture);
    hHallSprite.setScale(windowScaleWidth, windowScaleHeight);
    hHallSprite.setPosition(8 * desktopMode.width / 9 - hHallSprite.getGlobalBounds().width,
                            3 * hHallSprite.getGlobalBounds().height);

    std::vector<std::string> strings = {"wall", "watch_tower", "wall", "scout_tower"};
    for (unsigned short i = 0; i < Tower; ++i) {
        if (!oTowerTextures[i].loadFromFile("Textures/Tilesets/Orc/" + strings[i] + ".png")) {
            return -1;
        }
        oTowerTextures[i].setSmooth(true);
        oTowerSprites[i].setTexture(oTowerTextures[i]);
        oTowerSprites[i].setScale(windowScaleWidth, windowScaleHeight);
        if (!hTowerTextures[i].loadFromFile("Textures/Tilesets/Human/" + strings[i + Tower] + ".png")) {
            return -1;
        }
        hTowerTextures[i].setSmooth(true);
        hTowerSprites[i].setTexture(hTowerTextures[i]);
        hTowerSprites[i].setScale(windowScaleWidth, windowScaleHeight);
    }

    strings = {"footman", "grunt"};
    for (unsigned short i = 0; i < Units; ++i) {
        if (!oEnemyTextures[i].loadFromFile("Textures/Tilesets/Human/" + strings[i] + ".png")) {
            return -1;
        }
        oEnemyTextures[i].setSmooth(true);
        if (!hEnemyTextures[i].loadFromFile("Textures/Tilesets/Orc/" + strings[i + Units] + ".png")) {
            return -1;
        }
        hEnemyTextures[i].setSmooth(true);
    }

    strings = {"arrow"};
    for (unsigned short i = 0; i < Ammo; ++i) {
        if (!ammoTextures[i].loadFromFile("Textures/Tilesets/" + strings[i] + ".png")) {
            return -1;
        }
        ammoTextures[i].setSmooth(true);
    }



    factorX = ((float) desktopMode.width) / 1024;
    if (!goldTexture.loadFromFile("Textures/ui/gold.png")) {
        return -1;
    }
    goldSprite.setTexture(goldTexture);
    goldSprite.scale(factorX, factorX);
    goldSprite.setPosition(8 * desktopMode.width / 9, 1);
    if (!healthTexture.loadFromFile("Textures/ui/Health.png")) {
        return -1;
    }
    healthSprite.setTexture(healthTexture);
    healthSprite.scale(factorX, factorX);
    healthSprite.setPosition(8 * desktopMode.width / 9, 1 + goldSprite.getGlobalBounds().height);

    if (!oTowerIconTextures[0].loadFromFile("Textures/Tilesets/Orc/OrcWall.png")) {
        return -1;
    }
    if (!oTowerIconTextures[1].loadFromFile("Textures/Tilesets/Orc/SwampOrcScoutTower.png")) {
        return -1;
    }
    if (!hTowerIconTextures[0].loadFromFile("Textures/Tilesets/Human/HumanWall.png")) {
        return -1;
    }
    if (!hTowerIconTextures[1].loadFromFile("Textures/Tilesets/Human/SummerHumanScoutTower.png")) {
        return -1;
    }
    factorX = ((float) desktopMode.width) / buildingTexture.getSize().x / 18;
    factorY = ((float) desktopMode.height) / buildingTexture.getSize().y / 18;
    for (short i = 0; i < Tower; ++i) {
        hTowerIconTextures[i].setSmooth(true);
        oTowerIconTextures[i].setSmooth(true);
        hTowerIconSprites[i].setTexture(hTowerIconTextures[i]);
        oTowerIconSprites[i].setTexture(oTowerIconTextures[i]);
        hTowerIconSprites[i].scale(factorX, factorY);
        oTowerIconSprites[i].scale(factorX, factorY);
        buttonPosX = 8 * desktopMode.width / 9 + (i % 2) * (hTowerIconSprites[i].getGlobalBounds().width + 1);
        buttonPosY = (25 - PlayingGameButtons) * desktopMode.height / 25 - (1 + i / 2) * (hTowerIconSprites[i].getGlobalBounds().height + 1);
        hTowerIconSprites[i].setPosition(buttonPosX, buttonPosY);
        oTowerIconSprites[i].setPosition(buttonPosX, buttonPosY);
    }

    if (!smallFireTexture.loadFromFile("Textures/Tilesets/small_fire.png")) {
        return -1;
    }
    smallFireTexture.setSmooth(true);
    smallFireSprite.setTexture(smallFireTexture);
    smallFireSprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
    smallFireSprite.setScale(windowScaleWidth, windowScaleHeight);
    if (!bigFireTexture.loadFromFile("Textures/Tilesets/big_fire.png")) {
        return -1;
    }
    bigFireTexture.setSmooth(true);
    bigFireSprite.setTexture(bigFireTexture);
    bigFireSprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
    bigFireSprite.setScale(windowScaleWidth, windowScaleHeight);

    //Загрузка карт уровней
    for (short i = 0; i < maxLevel; ++i) {
        if (!hGameTexture[i].loadFromFile("Textures/Maps/human" + std::to_string(i + 1) + ".png")) {
            return -1;
        }
        if (!oGameTexture[i].loadFromFile("Textures/Maps/orc" + std::to_string(i + 1) + ".png")) {
            return -1;
        }
        hGameTexture[i].setSmooth(true);
        oGameTexture[i].setSmooth(true);
        hGameSprite[i].setTexture(hGameTexture[i]);
        oGameSprite[i].setTexture(oGameTexture[i]);
        factorX = ((float) desktopMode.width) / hGameTexture[i].getSize().x * 8 / 9;
        factorY = ((float) desktopMode.height) / hGameTexture[i].getSize().y;
        hGameSprite[i].scale(factorX, factorY);
        factorX = ((float) desktopMode.width) / oGameTexture[i].getSize().x * 8 / 9;
        factorY = ((float) desktopMode.height) / oGameTexture[i].getSize().y;
        oGameSprite[i].scale(factorX, factorY);
    }

    //Загрузка текстов брифингов
    for (short i = 0; i < maxLevel; ++i) {
        std::ifstream briefingFile("Briefings/orc" + std::to_string(i + 1) + ".txt");
        std::string readingString;
        if (!briefingFile.is_open()) {
            return -1;
        }
        while (std::getline(briefingFile, readingString)) {
            orcText[i] += readingString + "\n";
        }
        briefingFile.close();
        briefingFile.open("Briefings/human" + std::to_string(i + 1) + ".txt");
        if (!briefingFile.is_open()) {
            return -1;
        }
        while (std::getline(briefingFile, readingString)) {
            humanText[i] += readingString + "\n";
        }
        briefingFile.close();
    }


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
