#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <queue>
#include <ctime>

#include "Vidgets/Button.h"
#include "SupportingClasses.h"

void playNewGame();
void loadGame();
void briefing();
void playingGame();
void gameOver();


enum class WindowStatus {
    MainMenu,
    NewGame,
    LoadGame,
    Game,
    Briefing,
    End
};

enum class Terrarian {
    Summer,
    Swamp,
    Wasteland,
    Winter
};

//Настройка окна
sf::RenderWindow window(desktopMode, "WARTD", sf::Style::Fullscreen);

//Начальные настройки
unsigned short playingGameIcons = 0;
const unsigned char MaxLevel = 1, MaxHealth = 3, MaxWaves = 1;
const unsigned char MainMenuButtons = 3, PlayNewGameButtons = 3, LoadGameButtons = 7, PlayingGameButtons = 6, TempGameButtons = 7;
std::string saves[6] = {"", "", "", "", "", ""};

//Human
const unsigned short HMapsLeft[MaxLevel] {106 * windowScaleWidth};
const unsigned short HMapsTop[MaxLevel] {0};
const unsigned short HMapsWidth[MaxLevel] {50};
const unsigned short HMapsHeight[MaxLevel] {34};
const unsigned short HMapsEntry[3 * MaxLevel] {0, 14, 6}; // ширина, высота, длина входа
const unsigned short HHallsLeft[MaxLevel] {46};
const unsigned short HHallsTop[MaxLevel] {12};
const unsigned short HLevelWaves[MaxLevel] {1};
const std::vector<std::vector<std::vector<std::pair<short, short>>>> HWaves = {{{std::make_pair(10, 0)}}};

//Orcs
const unsigned short OMapsLeft[MaxLevel] {0};
const unsigned short OMapsTop[MaxLevel] {184 * windowScaleHeight};
const unsigned short OMapsWidth[MaxLevel] {53};
const unsigned short OMapsHeight[MaxLevel] {28};
const unsigned short OMapsEntry[3 * MaxLevel] {22, 0, 5}; //ширина, высота, длина входа
const unsigned short OHallsLeft[MaxLevel] {0};
const unsigned short OHallsTop[MaxLevel] {24};
const unsigned short OLevelWaves[MaxLevel] {1};
const std::vector<std::vector<std::vector<std::pair<short, short>>>> OWaves = {{{std::make_pair(10, 0)}}};

const size_t startedGold[MaxLevel] {100};

//Состояние игры
unsigned short level = -1, lastMusic = -1, music = -1, currentHealth = 3, currentWave = -1;
size_t currentGold = 0, currentScore = 0;
bool isPaused = false, cantBuild = false, notEnoughGold = false, isAttack = false, saving = true, win = false;
unsigned short isBuilding = 0, enemyDistance = 1000 * speed, enemyReleased = 0, defeatedEnemies = 0, waveEnemies = -1;
std::vector<std::vector<unsigned short>> map;
std::vector<Tower> playingTowers;
std::vector<Unit> currentEnemies;
std::vector<Ammo> ammoSprites;
std::vector<std::pair<short, short>> enemyPath;
unsigned short gameIndex = 0;

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
sf::Sound buttonPressed, construction, error, bowHit;
sf::Sound orcUnderAttack;
sf::Sound humanUnderAttack;

//Фоны
sf::Texture backgroundTexture, briefingTexture;
sf::Texture hGameTextures[MaxLevel], oGameTextures[MaxLevel];
sf::Texture hTowerIconTextures[Towers], oTowerIconTextures[Towers];
sf::Texture hTowerTextures[Towers], oTowerTextures[Towers];
sf::Texture hEnemyTextures[Units], oEnemyTextures[Units];
sf::Texture hHallTexture, oHallTexture;
sf::Texture hInfoTexture, oInfoTexture;
sf::Texture hVictoryTexture, oVictoryTexture;
sf::Texture hDefeatTexture, oDefeatTexture;
sf::Texture goldTexture, buildingTexture, healthTexture;
sf::Texture ammoTextures[Ammos];
sf::Texture smallFireTexture, bigFireTexture;
sf::Sprite backgroundSprite, briefingSprite;
sf::Sprite hGameSprites[MaxLevel], oGameSprites[MaxLevel];
sf::Sprite hTowerIconSprites[Towers], oTowerIconSprites[Towers];
sf::Sprite hTowerSprites[Towers], oTowerSprites[Towers];
sf::Sprite hHallSprite, oHallSprite;
sf::Sprite hInfoSprite, oInfoSprite;
sf::Sprite hVictorySprite, oVictorySprite;
sf::Sprite hDefeatSprite, oDefeatSprite;
sf::Sprite hSaveSprite, oSaveSprite;
sf::Sprite hEndSprite, oEndSprite;
sf::Sprite goldSprite, buildingSprite, healthSprite;
sf::Sprite smallFireSprite, bigFireSprite;

//Шрифт
sf::Font quake;

//Текстуры кнопок
sf::Texture orcPressed, orcUnpressed, humanPressed, humanUnpressed;

//Состояние окна
WindowStatus status = WindowStatus::MainMenu;

//Тексты брифингов
std::string orcTexts[MaxLevel];
std::string humanTexts[MaxLevel];

std::vector<std::pair<short, short>> bfs() {
    std::vector<std::vector<unsigned short>> copyMap = map;
    std::queue<Path> q;
    if (race == Race::Orc) {
        q.push(Path(OMapsEntry[3 * level], OMapsEntry[3 * level + 1], std::vector<std::pair<short, short>>()));
    }
    else {
        q.push(Path(HMapsEntry[3 * level], HMapsEntry[3 * level + 1], std::vector<std::pair<short, short>>()));
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

void save(unsigned char slot) {
    std::string saveName;
    std::ofstream file(std::to_string(slot));
    if (race == Race::Orc) {
        file << "Orc\n";
        saveName = "Orc";
    }
    else {
        file << "Human\n";
        saveName = "Human";
    }
    saves[slot] = saveName + ", " + std::to_string(level + 1);
    file << level << '\n';
    file << currentHealth << '\n';
    file << currentWave << '\n';
    file << currentGold << '\n';
    file << currentScore << '\n';
    file << isAttack << '\n';
    file << enemyReleased << '\n';
    file << defeatedEnemies << '\n';
    file << waveEnemies << '\n';
    file << map.size() << '\n';
    file << map[0].size() << '\n';
    for (unsigned char i = 0; i < map.size(); ++i) {
        for (unsigned char j = 0; j < map[i].size(); ++j) {
            file << map[i][j] << '\n';
        }
    }
    file << playingTowers.size() << '\n';
    for (unsigned short i = 0; i < playingTowers.size(); ++i) {
        file << playingTowers[i].save();
    }
    file << currentEnemies.size() << '\n';
    for (unsigned short i = 0; i < currentEnemies.size(); ++i) {
        file << currentEnemies[i].save();
    }
    file << ammoSprites.size() << '\n';
    for (unsigned short i = 0; i < ammoSprites.size(); ++i) {
        file << ammoSprites[i].save();
    }
    file.close();
}

bool load(unsigned char slot) {
    size_t temp;
    std::ifstream file(std::to_string(slot));
    if (!file.is_open()) {
        return false;
    }
    std::string readingString;
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    if (readingString == "Orc") {
        race = Race::Orc;
    }
    else {
        race = Race::Human;
    }

    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    level = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    currentHealth = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    currentWave = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    currentGold = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    currentScore = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    isAttack = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    enemyReleased = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    defeatedEnemies = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    waveEnemies = std::stoi(readingString);
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    map = std::vector<std::vector<unsigned short>>(std::stoi(readingString));
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    temp = std::stoi(readingString);
    for (unsigned char i = 0; i < map.size(); ++i) {
        map[i] = std::vector<unsigned short>(temp);
        for (unsigned char j = 0; j < temp; ++j) {
            std::getline(file, readingString);
            if (readingString == "") {
                return false;
            }
            map[i][j] = std::stoi(readingString);
        }
    }
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    playingTowers = std::vector<Tower>(std::stoi(readingString));
    for (unsigned short i = 0; i < playingTowers.size(); ++i) {
        std::getline(file, readingString);
        if (readingString == "") {
            return false;
        }
        playingTowers[i] = Tower(readingString);
        if (race == Race::Orc) {
            playingTowers[i].setTexture(oTowerTextures[playingTowers[i].code()]);
        }
        else {
            playingTowers[i].setTexture(hTowerTextures[playingTowers[i].code()]);
        }
    }
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    currentEnemies = std::vector<Unit>(std::stoi(readingString));
    for (unsigned short i = 0; i < currentEnemies.size(); ++i) {
        std::getline(file, readingString);
        if (readingString == "") {
            return false;
        }
        currentEnemies[i] = Unit(readingString);
        if (race == Race::Orc) {
            currentEnemies[i].setTexture(oEnemyTextures[currentEnemies[i].unit()]);
        }
        else {
            currentEnemies[i].setTexture(hEnemyTextures[currentEnemies[i].unit()]);
        }
    }
    std::getline(file, readingString);
    if (readingString == "") {
        return false;
    }
    ammoSprites = std::vector<Ammo>(std::stoi(readingString));
    for (unsigned short i = 0; i < ammoSprites.size(); ++i) {
        std::getline(file, readingString);
        if (readingString == "") {
            return false;
        }
        ammoSprites[i] = Ammo(readingString);
        ammoSprites[i].setTexture(ammoTextures[ammoSprites[i].code()]);
    }
    enemyPath = bfs();
    isPaused = false;
    win = false;
    saving = false;
    file.close();
    return true;
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
                status = WindowStatus::NewGame;
                buttonPressed.play();
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                status = WindowStatus::LoadGame;
                buttonPressed.play();
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
                buttonPressed.play();
                race = Race::Orc;
                level = 0;
                status = WindowStatus::Briefing;
                mainMenu.stop();
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                buttonPressed.play();
                race = Race::Human;
                level = 0;
                status = WindowStatus::Briefing;
                mainMenu.stop();
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
                buttonPressed.play();
                status = WindowStatus::MainMenu;
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

    for (short i = 0; i < 6; ++i) {
        buttonsWidth[i] = desktopMode.width / 4;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = 3 * desktopMode.width / 8;
        buttonsPosY[i] = desktopMode.height / 2 + i * (buttonsHeight[i] + 3);
        createButton(window, quake, orcUnpressed, orcPressed, saves[i],
                     buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                     mousePressedX, mousePressedY);
    }

    buttonsWidth[6] = desktopMode.width / 4;
    buttonsHeight[6] = desktopMode.height / 25;
    buttonsPosX[6] = 3 * desktopMode.width / 8;
    buttonsPosY[6] = desktopMode.height / 2 + 6 * (buttonsHeight[6] + 3);
    createButton(window, quake, orcUnpressed, orcPressed, "Back To Main Menu",
                 buttonsPosX[6], buttonsPosY[6], buttonsWidth[6], buttonsHeight[6],
                 mousePressedX, mousePressedY);

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
                buttonPressed.play();
                if (load(0)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                buttonPressed.play();
                if (load(1)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
                buttonPressed.play();
                if (load(2)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[3] && mousePressedX < (buttonsPosX[3] + buttonsWidth[3]) &&
                     mousePressedY > buttonsPosY[3] && mousePressedY < (buttonsPosY[3] + buttonsHeight[3])) {
                buttonPressed.play();
                if (load(3)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[4] && mousePressedX < (buttonsPosX[4] + buttonsWidth[4]) &&
                     mousePressedY > buttonsPosY[4] && mousePressedY < (buttonsPosY[4] + buttonsHeight[4])) {
                buttonPressed.play();
                if (load(4)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[5] && mousePressedX < (buttonsPosX[5] + buttonsWidth[5]) &&
                     mousePressedY > buttonsPosY[5] && mousePressedY < (buttonsPosY[5] + buttonsHeight[5])) {
                buttonPressed.play();
                if (load(5)) {
                    mainMenu.stop();
                    status = WindowStatus::Game;
                }
            }
            else if (mousePressedX > buttonsPosX[6] && mousePressedX < (buttonsPosX[6] + buttonsWidth[6]) &&
                     mousePressedY > buttonsPosY[6] && mousePressedY < (buttonsPosY[6] + buttonsHeight[6])) {
                buttonPressed.play();
                status = WindowStatus::MainMenu;
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
        text.setString(orcTexts[level]);
    }
    else {
        if (humanBriefing.getStatus() == sf::SoundSource::Stopped) {
            humanBriefing.play();
        }
        text.setColor(sf::Color::White);
        text.setString(humanTexts[level]);
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
                status = WindowStatus::Game;
                buttonPressed.play();
                if (race == Race::Orc) {
                    orcBriefing.stop();
                    map = std::vector<std::vector<unsigned short>>(OMapsHeight[level], std::vector<unsigned short>(OMapsWidth[level], 0));
                    for (unsigned short i = 0; i < 4; ++i) {
                        for (unsigned short j = 0; j < 4; ++j) {
                            map[OHallsTop[level] + i][OHallsLeft[level] + j] = 2;
                        }
                    }
                    for (unsigned short i = 0; i < OMapsEntry[3 * level + 2]; ++i) {
                        if (!OMapsEntry[level]) {
                            map[OMapsEntry[3 * level + 1] + i][0] = 3;
                        }
                        else {
                            map[0][OMapsEntry[3 * level] + i] = 3;
                        }
                    }
                    buttonPosX = OMapsLeft[level] + (OHallsLeft[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = OMapsTop[level] + (OHallsTop[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().height / 2;
                    smallFireSprite.setPosition(buttonPosX, buttonPosY);
                    buttonPosX = OMapsLeft[level] + (OHallsLeft[level] + 2) * boxSize - bigFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = OMapsTop[level] + (OHallsTop[level] + 2) * boxSize - bigFireSprite.getGlobalBounds().height / 2;
                    bigFireSprite.setPosition(buttonPosX, buttonPosY);
                }
                else {
                    humanBriefing.stop();
                    map = std::vector<std::vector<unsigned short>>(HMapsHeight[level], std::vector<unsigned short>(HMapsWidth[level], 0));
                    for (unsigned short i = 0; i < 4; ++i) {
                        for (unsigned short j = 0; j < 4; ++j) {
                            map[HHallsTop[level] + i][HHallsLeft[level] + j] = 2;
                        }
                    }
                    for (unsigned short i = 0; i < HMapsEntry[3 * level + 2]; ++i) {
                        if (!HMapsEntry[level]) {
                            map[HMapsEntry[3 * level + 1] + i][0] = 3;
                        }
                        else {
                            map[0][HMapsEntry[3 * level] + i] = 3;
                        }
                    }
                    buttonPosX = HMapsLeft[level] + (HHallsLeft[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = HMapsTop[level] + (HHallsTop[level] + 2) * boxSize - smallFireSprite.getGlobalBounds().height / 2;
                    smallFireSprite.setPosition(buttonPosX, buttonPosY);
                    buttonPosX = HMapsLeft[level] + (HHallsLeft[level] + 2) * boxSize - bigFireSprite.getGlobalBounds().width / 2;
                    buttonPosY = HMapsTop[level] + (HHallsTop[level] + 2) * boxSize - bigFireSprite.getGlobalBounds().height / 2;
                    bigFireSprite.setPosition(buttonPosX, buttonPosY);
                }
                currentWave = 0;
                currentGold = startedGold[level];
                playingGameIcons = 1;
                playingTowers.clear();
                currentEnemies.clear();
                ammoSprites.clear();
                isBuilding = 0;
                currentScore = 0;
                defeatedEnemies = 0;
                isPaused = false;
                isAttack = false;
                win = false;
                saving = false;
                currentHealth = MaxHealth;
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

    if (!isPaused) {
        ++gameIndex;
    }

    size_t buttonsPosX[PlayingGameButtons], buttonsPosY[PlayingGameButtons],
           buttonsWidth[PlayingGameButtons], buttonsHeight[PlayingGameButtons],
           tempButtonsPosX[TempGameButtons], tempButtonsPosY[TempGameButtons],
           tempButtonsWidth[TempGameButtons], tempButtonsHeight[TempGameButtons];

    sf::Text towerCharacteristics[3]; //Название, урон, скорострельность

    std::string buttonsText[PlayingGameButtons] {"Exit", "Save", "Pause", "Next", "-", "+"};

    for (short i = 0; i < PlayingGameButtons; ++i) {
        buttonsWidth[i] = desktopMode.width / 9;
        buttonsHeight[i] = desktopMode.height / 25;
        buttonsPosX[i] = desktopMode.width - (buttonsWidth[0] + 1);
        buttonsPosY[i] = desktopMode.height - (i + 1) * (buttonsHeight[0] + 1);
    }

    buttonsWidth[4] = desktopMode.height / 25;
    buttonsHeight[4] = desktopMode.height / 25;
    buttonsPosX[4] = desktopMode.width - (buttonsWidth[0] + 1);
    buttonsPosY[4] = buttonsPosY[3] - (buttonsHeight[0] + 1);
    buttonsWidth[5] = buttonsWidth[4];
    buttonsHeight[5] = buttonsHeight[4];
    buttonsPosX[5] = desktopMode.width - buttonsWidth[5];
    buttonsPosY[5] = buttonsPosY[4];

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

        window.draw(oGameSprites[level]);
        window.draw(oHallSprite);

        for (unsigned short i = 0; i < playingTowers.size(); ++i) {
            window.draw(playingTowers[i].sprite());
        }

        if (currentHealth < 2) {
            if (!isPaused) {
                gameIndex = gameIndex % (250 * speed);
                bigFireSprite.setTextureRect(sf::IntRect(48 * (gameIndex % (125 * speed) / (25 * speed)), 48 * (gameIndex / (125 * speed)), 48, 48));
            }
            window.draw(bigFireSprite);
        }
        else if (currentHealth < 3) {
            if (!isPaused) {
                gameIndex = gameIndex % (150 * speed);
                smallFireSprite.setTextureRect(sf::IntRect(32 * (gameIndex % (125 * speed) / (25 * speed)), 16 + 48 * (gameIndex / (125 * speed)), 32, 32));
            }
            window.draw(smallFireSprite);
        }

        if (isAttack && !isPaused) {
            if (!(gameIndex % enemyDistance)) {
                char currentUnit = enemyReleased;
                for (unsigned char i = 0; i < OWaves[level][currentWave].size(); ++i) {
                    currentUnit -= OWaves[level][currentWave][i].first;
                    if (currentUnit < 0) {
                        currentUnit = OWaves[level][currentWave][i].second;
                        currentEnemies.push_back(Unit(currentUnit, oEnemyTextures[currentUnit]));
                        ++enemyReleased;
                        break;
                    }
                }

            }
        }


        for (auto it = currentEnemies.begin(); it != currentEnemies.end(); ) {
            window.draw((*it).sprite());
            if (!isPaused) {
                std::pair<short, short> enemyDestination = enemyPath[(*it).partOfPath];
                enemyDestination.first *= boxSize;
                enemyDestination.second *= boxSize;
                enemyDestination.first += OMapsLeft[level];
                enemyDestination.second += OMapsTop[level];
                if ((*it).x() > enemyDestination.first && !(*it).isDead()) {
                    (*it).move(Moving::Left);
                }
                else if ((*it).x() < enemyDestination.first && !(*it).isDead()) {
                    (*it).move(Moving::Right);
                }
                else if ((*it).y() > enemyDestination.second && !(*it).isDead()) {
                    (*it).move(Moving::Up);
                }
                else if ((*it).y() < enemyDestination.second && !(*it).isDead()) {
                    (*it).move(Moving::Down);
                }
                else if (!(*it).isDead()){
                    ++((*it).partOfPath);
                }
                if ((*it).partOfPath == enemyPath.size() - 1) {
                    --currentHealth;
                    for (auto iter = ammoSprites.begin(); iter != ammoSprites.end();) {
                        if ((*iter).aim() == (currentEnemies.begin() - it)) {
                            iter = ammoSprites.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                    it = currentEnemies.erase(it);
                    ++defeatedEnemies;
                }
                else if ((*it).toDelete()) {
                    for (auto iter = ammoSprites.begin(); iter != ammoSprites.end();) {
                        if ((*iter).aim() == (currentEnemies.begin() - it)) {
                            iter = ammoSprites.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                    currentGold += (*it).gold();
                    currentScore += (*it).score();
                    ++defeatedEnemies;
                    it = currentEnemies.erase(it);
                }
                else if ((*it).isDead()) {
                    (*it).die();
                    ++it;
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }


        for (auto it = ammoSprites.begin(); it != ammoSprites.end();) {
            window.draw((*it).sprite());
            if (!isPaused) {
                (*it).setDestination(currentEnemies[(*it).aim()].x(), currentEnemies[(*it).aim()].y());
                (*it).move();
                if ((*it).isHit()) {
                    unsigned short damage = (*it).minDmg() + std::rand() % ((*it).maxDmg() - (*it).minDmg());
                    currentEnemies[(*it).aim()].damaged(damage);
                    it = ammoSprites.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }

        for (unsigned char i = 0; i < playingTowers.size(); ++i) {
            if (playingTowers[i].code() == 0 || isPaused) {
                continue;
            }
            unsigned short minDistance = -1;
            unsigned char nearestEnemy = -1;
            for (unsigned char j = 0; j < currentEnemies.size(); ++j) {
                short dx = playingTowers[i].x() - currentEnemies[j].x(), dy = playingTowers[i].y() - currentEnemies[j].y();
                unsigned short distance = std::sqrt(dx * dx + dy * dy);
                if (distance < minDistance && !currentEnemies[j].isDead()) {
                    minDistance = distance;
                    nearestEnemy = j;
                }
            }
            if (nearestEnemy < currentEnemies.size() && minDistance < playingTowers[i].range() && currentEnemies[nearestEnemy].x() > 0 && currentEnemies[nearestEnemy].y() > 0) {
                if (playingTowers[i].towerShot()) {
                    ammoSprites.push_back(Ammo(ammoTextures[playingTowers[i].ammo()], playingTowers[i].x(), playingTowers[i].y(),
                                               currentEnemies[nearestEnemy].x(), currentEnemies[nearestEnemy].y(), nearestEnemy,
                                               playingTowers[i].code(), playingTowers[i].ammo()));
                }
            }
            playingTowers[i].idle();
        }


        if (isBuilding) {
            towerX = OMapsLeft[level] + boxSize * windowScaleWidth * ((sf::Mouse::getPosition(window).x - OMapsLeft[level]) / boxSize);
            towerY = OMapsTop[level] + boxSize * windowScaleHeight * ((sf::Mouse::getPosition(window).y - OMapsTop[level]) / boxSize);
            xBox = (sf::Mouse::getPosition(window).x - OMapsLeft[level]) / boxSize;
            yBox = (sf::Mouse::getPosition(window).y - OMapsTop[level]) / boxSize;
            if (towerX < OMapsLeft[level]) {
                towerX = OMapsLeft[level];
                xBox = 0;
            }
            else if (towerX > OMapsLeft[level] + (OMapsWidth[level] - dist) * boxSize * windowScaleWidth) {
                towerX = OMapsLeft[level] + (OMapsWidth[level] - dist) * boxSize * windowScaleWidth;
                xBox = (OMapsWidth[level] - dist);
            }
            if (towerY < OMapsTop[level]) {
                towerY = OMapsTop[level];
                yBox = 0;
            }
            else if (towerY > OMapsTop[level] + (OMapsHeight[level] - dist) * boxSize * windowScaleHeight) {
                towerY = OMapsTop[level] + (OMapsHeight[level] - dist) * boxSize * windowScaleHeight;
                yBox = (OMapsHeight[level] - dist);
            }
            oTowerSprites[isBuilding - 1].setPosition(towerX, towerY);
            window.draw(oTowerSprites[isBuilding - 1]);
        }

        window.draw(oInfoSprite);
        for (short i = 0; i < Towers; ++i) {
            window.draw(oTowerIconSprites[i]);
        }
        for (short i = 0; i < PlayingGameButtons; ++i) {
            createButton(window, quake, orcUnpressed, orcPressed, buttonsText[i],
                         buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                         mousePressedX, mousePressedY);
        }

        buttonText = std::to_string(currentGold);
        buttonPosX = 8 * desktopMode.width / 9;
        for (short i = 0; i < 3; ++i) {
            buttonPosY = 1 + 2 * goldSprite.getGlobalBounds().height + i * desktopMode.height / 40;
            towerCharacteristics[i].setFont(quake);
            towerCharacteristics[i].setCharacterSize(desktopMode.height / 50);
            towerCharacteristics[i].setPosition(buttonPosX, buttonPosY);
            towerCharacteristics[i].setColor(sf::Color::Yellow);
        }
        for (unsigned char i = 0; i < Towers; ++i) {
            if ((sf::Mouse::getPosition(window).x > hTowerIconSprites[i].getGlobalBounds().left &&
                 sf::Mouse::getPosition(window).x < (hTowerIconSprites[i].getGlobalBounds().left + hTowerIconSprites[i].getGlobalBounds().width) &&
                 sf::Mouse::getPosition(window).y > hTowerIconSprites[i].getGlobalBounds().top &&
                 sf::Mouse::getPosition(window).y < (hTowerIconSprites[i].getGlobalBounds().top + hTowerIconSprites[i].getGlobalBounds().height) && !isBuilding) ||
                 isBuilding == i + 1) {
                buttonText += '-' + std::to_string(OTowersCost[i]);
                towerCharacteristics[0].setString(OTowersNames[i]);
                if (OTowersDamage[2 * i + 1]) {
                    towerCharacteristics[1].setString("Damage: " + std::to_string(OTowersDamage[2 * i]) + '-' + std::to_string(OTowersDamage[2 * i + 1]));
                }
                else {
                    towerCharacteristics[1].setString("Damage: -");
                }
                if (OTowersRate[i]) {
                    std::string s = std::to_string(OTowersRate[i]);
                    s = s.substr(0, s.find('.') + 3);
                    towerCharacteristics[2].setString("Rate: " + s);
                }
                else {
                    towerCharacteristics[2].setString("Rate: -");
                }
                for (unsigned char j = 0; j < 3; ++j) {
                        window.draw(towerCharacteristics[j]);
                }
            }
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

        window.draw(hGameSprites[level]);
        window.draw(hHallSprite);

        for (unsigned short i = 0; i < playingTowers.size(); ++i) {
            window.draw(playingTowers[i].sprite());
        }

        if (currentHealth < 2) {
            if (!isPaused) {
                gameIndex = gameIndex % (250 * speed);
                bigFireSprite.setTextureRect(sf::IntRect(48 * (gameIndex % (125 * speed) / (25 * speed)), 48 * (gameIndex / (125 * speed)), 48, 48));
            }
            window.draw(bigFireSprite);
        }
        else if (currentHealth < 3) {
            if (!isPaused) {
                gameIndex = gameIndex % (150 * speed);
                smallFireSprite.setTextureRect(sf::IntRect(32 * (gameIndex % (125 * speed) / (25 * speed)), 16 + 48 * (gameIndex / (125 * speed)), 32, 32));
            }
            window.draw(smallFireSprite);
        }

        if (isAttack && !isPaused) {
            if (!(gameIndex % enemyDistance)) {
                char currentUnit = enemyReleased;
                for (unsigned char i = 0; i < HWaves[level][currentWave].size(); ++i) {
                    currentUnit -= HWaves[level][currentWave][i].first;
                    if (currentUnit < 0) {
                        currentUnit = HWaves[level][currentWave][i].second;
                        currentEnemies.push_back(Unit(currentUnit, hEnemyTextures[currentUnit]));
                        ++enemyReleased;
                        break;
                    }
                }

            }
        }


        for (auto it = currentEnemies.begin(); it != currentEnemies.end(); ) {
            window.draw((*it).sprite());
            if (!isPaused) {
                std::pair<short, short> enemyDestination = enemyPath[(*it).partOfPath];
                enemyDestination.first *= boxSize;
                enemyDestination.second *= boxSize;
                enemyDestination.first += HMapsLeft[level];
                enemyDestination.second += HMapsTop[level];
                if ((*it).x() > enemyDestination.first && !(*it).isDead()) {
                    (*it).move(Moving::Left);
                }
                else if ((*it).x() < enemyDestination.first && !(*it).isDead()) {
                    (*it).move(Moving::Right);
                }
                else if ((*it).y() > enemyDestination.second && !(*it).isDead()) {
                    (*it).move(Moving::Up);
                }
                else if ((*it).y() < enemyDestination.second && !(*it).isDead()) {
                    (*it).move(Moving::Down);
                }
                else if (!(*it).isDead()){
                    ++((*it).partOfPath);
                }
                if ((*it).partOfPath == enemyPath.size() - 1) {
                    --currentHealth;
                    for (auto iter = ammoSprites.begin(); iter != ammoSprites.end();) {
                        if ((*iter).aim() == (currentEnemies.begin() - it)) {
                            iter = ammoSprites.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                    ++defeatedEnemies;
                    it = currentEnemies.erase(it);
                }
                else if ((*it).toDelete()) {
                    for (auto iter = ammoSprites.begin(); iter != ammoSprites.end();) {
                        if ((*iter).aim() == (currentEnemies.begin() - it)) {
                            iter = ammoSprites.erase(iter);
                        }
                        else {
                            ++iter;
                        }
                    }
                    currentGold += (*it).gold();
                    currentScore += (*it).score();
                    ++defeatedEnemies;
                    it = currentEnemies.erase(it);
                }
                else if ((*it).isDead()) {
                    (*it).die();
                    ++it;
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }

        for (auto it = ammoSprites.begin(); it != ammoSprites.end();) {
            window.draw((*it).sprite());
            if (!isPaused) {
                (*it).setDestination(currentEnemies[(*it).aim()].x(), currentEnemies[(*it).aim()].y());
                (*it).move();
                if ((*it).isHit()) {
                    unsigned short damage = (*it).minDmg() + std::rand() % ((*it).maxDmg() - (*it).minDmg());
                    currentEnemies[(*it).aim()].damaged(damage);
                    it = ammoSprites.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                ++it;
            }
        }


        for (unsigned char i = 0; i < playingTowers.size(); ++i) {
            if (playingTowers[i].code() == 0 || isPaused) {
                continue;
            }
            unsigned short minDistance = -1;
            unsigned char nearestEnemy = -1;
            for (unsigned char j = 0; j < currentEnemies.size(); ++j) {
                short dx = playingTowers[i].x() - currentEnemies[j].x(), dy = playingTowers[i].y() - currentEnemies[j].y();
                unsigned short distance = std::sqrt(dx * dx + dy * dy);
                if (distance < minDistance && !currentEnemies[j].isDead()) {
                    minDistance = distance;
                    nearestEnemy = j;
                }
            }
            if (nearestEnemy < currentEnemies.size() && minDistance < playingTowers[i].range() && currentEnemies[nearestEnemy].x() > 0 && currentEnemies[nearestEnemy].y() > 0) {
                if (playingTowers[i].towerShot()) {
                    ammoSprites.push_back(Ammo(ammoTextures[playingTowers[i].ammo()], playingTowers[i].x(), playingTowers[i].y(),
                                               currentEnemies[nearestEnemy].x(), currentEnemies[nearestEnemy].y(), nearestEnemy,
                                               playingTowers[i].code(), playingTowers[i].ammo()));
                }
            }
            playingTowers[i].idle();
        }

        if (isBuilding) {
            towerX = HMapsLeft[level] + boxSize * windowScaleWidth * ((sf::Mouse::getPosition(window).x - HMapsLeft[level]) / boxSize);
            towerY = HMapsTop[level] + boxSize * windowScaleHeight * ((sf::Mouse::getPosition(window).y - HMapsTop[level]) / boxSize);
            xBox = (sf::Mouse::getPosition(window).x - HMapsLeft[level]) / boxSize;
            yBox = (sf::Mouse::getPosition(window).y - HMapsTop[level]) / boxSize;
            if (towerX < HMapsLeft[level]) {
                towerX = HMapsLeft[level];
                xBox = 0;
            }
            else if (towerX > HMapsLeft[level] + (HMapsWidth[level] - dist) * boxSize * windowScaleWidth) {
                towerX = HMapsLeft[level] + (HMapsWidth[level] - dist) * boxSize * windowScaleWidth;
                xBox = (HMapsWidth[level] - dist);
            }
            if (towerY < HMapsTop[level]) {
                towerY = HMapsTop[level];
                yBox = 0;
            }
            else if (towerY > HMapsTop[level] + (HMapsHeight[level] - dist) * boxSize * windowScaleHeight) {
                towerY = HMapsTop[level] + (HMapsHeight[level] - dist) * boxSize * windowScaleHeight;
                yBox = (HMapsHeight[level] - dist);
            }
            hTowerSprites[isBuilding - 1].setPosition(towerX, towerY);
            window.draw(hTowerSprites[isBuilding - 1]);
        }

        window.draw(hInfoSprite);
        for (short i = 0; i < Towers; ++i) {
            window.draw(hTowerIconSprites[i]);
        }
        for (short i = 0; i < PlayingGameButtons; ++i) {
            createButton(window, quake, humanUnpressed, humanPressed, buttonsText[i],
                         buttonsPosX[i], buttonsPosY[i], buttonsWidth[i], buttonsHeight[i],
                         mousePressedX, mousePressedY);
        }

        buttonText = std::to_string(currentGold);
        buttonPosX = 8 * desktopMode.width / 9;
        for (short i = 0; i < 3; ++i) {
            buttonPosY = 1 + 2 * goldSprite.getGlobalBounds().height + i * desktopMode.height / 40;
            towerCharacteristics[i].setFont(quake);
            towerCharacteristics[i].setCharacterSize(desktopMode.height / 50);
            towerCharacteristics[i].setPosition(buttonPosX, buttonPosY);
            towerCharacteristics[i].setColor(sf::Color::Yellow);
        }
        for (unsigned char i = 0; i < Towers; ++i) {
            if ((sf::Mouse::getPosition(window).x > hTowerIconSprites[i].getGlobalBounds().left &&
                 sf::Mouse::getPosition(window).x < (hTowerIconSprites[i].getGlobalBounds().left + hTowerIconSprites[i].getGlobalBounds().width) &&
                 sf::Mouse::getPosition(window).y > hTowerIconSprites[i].getGlobalBounds().top &&
                 sf::Mouse::getPosition(window).y < (hTowerIconSprites[i].getGlobalBounds().top + hTowerIconSprites[i].getGlobalBounds().height) && !isBuilding) ||
                 isBuilding == i + 1) {
                buttonText += '-' + std::to_string(HTowersCost[i]);
                towerCharacteristics[0].setString(HTowersNames[i]);
                if (HTowersAttack[2 * i + 1]) {
                    towerCharacteristics[1].setString("Damage: " + std::to_string(HTowersAttack[2 * i]) + '-' + std::to_string(HTowersAttack[2 * i + 1]));
                }
                else {
                    towerCharacteristics[1].setString("Damage: -");
                }
                if (HTowersRate[i]) {
                    std::string s = std::to_string(HTowersRate[i]);
                    s = s.substr(0, s.find('.') + 3);
                    towerCharacteristics[2].setString("Rate: " + s);
                }
                else {
                    towerCharacteristics[2].setString("Rate: -");
                }
                for (unsigned char j = 0; j < 3; ++j) {
                        window.draw(towerCharacteristics[j]);
                }
            }
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

    sf::Text pause;
    pause.setFont(quake);
    pause.setCharacterSize(desktopMode.height / 40);
    pause.setPosition(0, 0);
    pause.setColor(sf::Color::Yellow);
    pause.setString("Paused");

    sf::Text speedNum;
    speedNum.setFont(quake);
    speedNum.setCharacterSize(desktopMode.height / 30);
    speedNum.setString(std::to_string(10 - speed));
    speedNum.setPosition(17 * desktopMode.width / 18 - speedNum.getGlobalBounds().width / 2, buttonsPosY[5]);
    speedNum.setColor(sf::Color::Yellow);

    sf::Text gameSpeed;
    gameSpeed.setFont(quake);
    gameSpeed.setCharacterSize(desktopMode.height / 30);
    gameSpeed.setString("Speed");
    gameSpeed.setPosition(17 * desktopMode.width / 18 - gameSpeed.getGlobalBounds().width / 2, buttonsPosY[4] - (buttonsHeight[0] + 1));
    gameSpeed.setColor(sf::Color::Yellow);

    sf::Text victory;
    victory.setFont(quake);
    victory.setCharacterSize(desktopMode.height / 25);
    victory.setColor(sf::Color::Yellow);

    if (cantBuild) {
        window.draw(cantBuildText);
    }

    if (notEnoughGold) {
        window.draw(notEnough);
    }

    if (isBuilding) {
        window.draw(buildingSprite);
    }

    if (isPaused && currentHealth > 0 && defeatedEnemies != waveEnemies && !saving) {
        window.draw(pause);
    }

    window.draw(speedNum);
    window.draw(gameSpeed);

    if (defeatedEnemies == waveEnemies && currentHealth > 0) {
        isPaused = true;
        isBuilding = 0;
        cantBuild = false;
        notEnoughGold = false;
        win = true;
        tempButtonsWidth[0] = desktopMode.width / 4;
        tempButtonsHeight[0] = desktopMode.height / 25;
        tempButtonsPosX[0] = hEndSprite.getGlobalBounds().left;
        tempButtonsPosY[0] = hEndSprite.getGlobalBounds().top + hEndSprite.getGlobalBounds().height - tempButtonsHeight[0] - 1;
        if (race == Race::Orc) {
            window.draw(oEndSprite);
            createButton(window, quake, orcUnpressed, orcPressed, "Victory",
                         tempButtonsPosX[0], tempButtonsPosY[0], tempButtonsWidth[0], tempButtonsHeight[0],
                         mousePressedX, mousePressedY);
        }
        else {
            window.draw(hEndSprite);
            createButton(window, quake, humanUnpressed, humanPressed, "Victory",
                         tempButtonsPosX[0], tempButtonsPosY[0], tempButtonsWidth[0], tempButtonsHeight[0],
                         mousePressedX, mousePressedY);
        }
        victory.setString("Victory");
        victory.setPosition((desktopMode.width - victory.getGlobalBounds().width) / 2, hEndSprite.getGlobalBounds().top);
        window.draw(victory);

    }

    if (currentHealth <= 0 && !gameIndex) {
        isPaused = true;
        isBuilding = 0;
        cantBuild = false;
        notEnoughGold = false;
        tempButtonsWidth[0] = desktopMode.width / 4;
        tempButtonsHeight[0] = desktopMode.height / 25;
        tempButtonsPosX[0] = hEndSprite.getGlobalBounds().left;
        tempButtonsPosY[0] = hEndSprite.getGlobalBounds().top + hEndSprite.getGlobalBounds().height - tempButtonsHeight[0] - 1;
        if (race == Race::Orc) {
            window.draw(oEndSprite);
            createButton(window, quake, orcUnpressed, orcPressed, "Defeat",
                         tempButtonsPosX[0], tempButtonsPosY[0], tempButtonsWidth[0], tempButtonsHeight[0],
                         mousePressedX, mousePressedY);
        }
        else {
            window.draw(hEndSprite);
            createButton(window, quake, humanUnpressed, humanPressed, "Defeat",
                         tempButtonsPosX[0], tempButtonsPosY[0], tempButtonsWidth[0], tempButtonsHeight[0],
                         mousePressedX, mousePressedY);
        }
        victory.setString("Defeat");
        victory.setPosition((desktopMode.width - victory.getGlobalBounds().width) / 2, hEndSprite.getGlobalBounds().top);
        window.draw(victory);
    }

    if (saving) {
        if (race == Race::Orc) {
            window.draw(oSaveSprite);
            for (unsigned char i = 0; i < TempGameButtons - 1; ++i) {
                createButton(window, quake, orcUnpressed, orcPressed, saves[i],
                             tempButtonsPosX[i], tempButtonsPosY[i], tempButtonsWidth[i], tempButtonsHeight[i],
                             mousePressedX, mousePressedY);
            }
            createButton(window, quake, orcUnpressed, orcPressed, "Back To Game",
                         tempButtonsPosX[6], tempButtonsPosY[6], tempButtonsWidth[6], tempButtonsHeight[6],
                         mousePressedX, mousePressedY);
        }
        else {
            window.draw(hSaveSprite);
            for (unsigned char i = 0; i < TempGameButtons - 1; ++i) {
                createButton(window, quake, humanUnpressed, humanPressed, saves[i],
                             tempButtonsPosX[i], tempButtonsPosY[i], tempButtonsWidth[i], tempButtonsHeight[i],
                             mousePressedX, mousePressedY);
            }
            createButton(window, quake, humanUnpressed, humanPressed, "Back To Game",
                         tempButtonsPosX[6], tempButtonsPosY[6], tempButtonsWidth[6], tempButtonsHeight[6],
                         mousePressedX, mousePressedY);
        }
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
            if (mousePressedX > tempButtonsPosX[0] && mousePressedX < (tempButtonsPosX[0] + tempButtonsWidth[0]) &&
                mousePressedY > tempButtonsPosY[0] && mousePressedY < (tempButtonsPosY[0] + tempButtonsHeight[0]) &&
                !gameIndex && currentHealth <= 0) {
                buttonPressed.play();
                status = WindowStatus::End;
                orcMusic[music].stop();
                humanMusic[music].stop();
            }
            else if (mousePressedX > tempButtonsPosX[0] && mousePressedX < (tempButtonsPosX[0] + tempButtonsWidth[0]) &&
                     mousePressedY > tempButtonsPosY[0] && mousePressedY < (tempButtonsPosY[0] + tempButtonsHeight[0]) &&
                     defeatedEnemies == waveEnemies && currentHealth > 0) {
                buttonPressed.play();
                status = WindowStatus::End;
                orcMusic[music].stop();
                humanMusic[music].stop();
            }
            else if (mousePressedX > tempButtonsPosX[0] && mousePressedX < (tempButtonsPosX[0] + tempButtonsWidth[0]) &&
                     mousePressedY > tempButtonsPosY[0] && mousePressedY < (tempButtonsPosY[0] + tempButtonsHeight[0]) &&
                     saving) {
                buttonPressed.play();
                save(0);
            }
            else if (mousePressedX > tempButtonsPosX[1] && mousePressedX < (tempButtonsPosX[1] + tempButtonsWidth[1]) &&
                     mousePressedY > tempButtonsPosY[1] && mousePressedY < (tempButtonsPosY[1] + tempButtonsHeight[1]) &&
                     saving) {
                buttonPressed.play();
                save(1);
            }
            else if (mousePressedX > tempButtonsPosX[2] && mousePressedX < (tempButtonsPosX[2] + tempButtonsWidth[2]) &&
                     mousePressedY > tempButtonsPosY[2] && mousePressedY < (tempButtonsPosY[2] + tempButtonsHeight[2]) &&
                     saving) {
                buttonPressed.play();
                save(2);
            }
            else if (mousePressedX > tempButtonsPosX[3] && mousePressedX < (tempButtonsPosX[3] + tempButtonsWidth[3]) &&
                     mousePressedY > tempButtonsPosY[3] && mousePressedY < (tempButtonsPosY[3] + tempButtonsHeight[3]) &&
                     saving) {
                buttonPressed.play();
                save(3);
            }
            else if (mousePressedX > tempButtonsPosX[4] && mousePressedX < (tempButtonsPosX[4] + tempButtonsWidth[4]) &&
                     mousePressedY > tempButtonsPosY[4] && mousePressedY < (tempButtonsPosY[4] + tempButtonsHeight[4]) &&
                     saving) {
                buttonPressed.play();
                save(4);
            }
            else if (mousePressedX > tempButtonsPosX[5] && mousePressedX < (tempButtonsPosX[5] + tempButtonsWidth[5]) &&
                     mousePressedY > tempButtonsPosY[5] && mousePressedY < (tempButtonsPosY[5] + tempButtonsHeight[5]) &&
                     saving) {
                buttonPressed.play();
                save(5);
            }
            else if (mousePressedX > tempButtonsPosX[6] && mousePressedX < (tempButtonsPosX[6] + tempButtonsWidth[6]) &&
                     mousePressedY > tempButtonsPosY[6] && mousePressedY < (tempButtonsPosY[6] + tempButtonsHeight[6]) &&
                     saving) {
                buttonPressed.play();
                saving = false;
                isPaused = false;
            }
            else if (mousePressedX > buttonsPosX[0] && mousePressedX < (buttonsPosX[0] + buttonsWidth[0]) &&
                     mousePressedY > buttonsPosY[0] && mousePressedY < (buttonsPosY[0] + buttonsHeight[0])) {
                status = WindowStatus::MainMenu;
                buttonPressed.play();
                orcMusic[music].stop();
                humanMusic[music].stop();
            }
            else if (mousePressedX > buttonsPosX[1] && mousePressedX < (buttonsPosX[1] + buttonsWidth[1]) &&
                     mousePressedY > buttonsPosY[1] && mousePressedY < (buttonsPosY[1] + buttonsHeight[1])) {
                buttonPressed.play();
                if (!saving) {
                    isPaused = true;
                    isBuilding = 0;
                    cantBuild = false;
                    notEnoughGold = false;
                    for (unsigned char i = 0; i < TempGameButtons; ++i) {
                        tempButtonsWidth[i] = desktopMode.width / 4;
                        tempButtonsHeight[i] = desktopMode.height / 25;
                        tempButtonsPosX[i] = hSaveSprite.getGlobalBounds().left;
                        tempButtonsPosY[i] = hSaveSprite.getGlobalBounds().top + i * desktopMode.height / 25;
                    }
                }
                else {
                    isPaused = false;
                }
                saving = !saving;
            }
            else if (mousePressedX > buttonsPosX[2] && mousePressedX < (buttonsPosX[2] + buttonsWidth[2]) &&
                     mousePressedY > buttonsPosY[2] && mousePressedY < (buttonsPosY[2] + buttonsHeight[2])) {
                if (currentHealth > 0) {
                    isPaused = !isPaused;
                }
                buttonPressed.play();
            }
            else if (mousePressedX > buttonsPosX[3] && mousePressedX < (buttonsPosX[3] + buttonsWidth[3]) &&
                     mousePressedY > buttonsPosY[3] && mousePressedY < (buttonsPosY[3] + buttonsHeight[3])) {
                if (!isAttack) {
                    isAttack = true;
                    gameIndex = 0;
                    enemyReleased = 0;
                    defeatedEnemies = 0;
                    enemyPath = std::move(bfs());
                    waveEnemies = 0;
                    if (race == Race::Orc) {
                        orcUnderAttack.play();
                        for (unsigned char i = 0; i < OWaves[level][currentWave].size(); ++i) {
                            waveEnemies += OWaves[level][currentWave][i].first;
                        }
                    }
                    else {
                        humanUnderAttack.play();
                        for (unsigned char i = 0; i < HWaves[level][currentWave].size(); ++i) {
                            waveEnemies += HWaves[level][currentWave][i].first;
                        }
                    }
                }
                buttonPressed.play();
            }
            else if (mousePressedX > buttonsPosX[4] && mousePressedX < (buttonsPosX[4] + buttonsWidth[4]) &&
                     mousePressedY > buttonsPosY[4] && mousePressedY < (buttonsPosY[4] + buttonsHeight[4])) {
                if (speed < 9) {
                    ++speed;
                }
                buttonPressed.play();
            }
            else if (mousePressedX > buttonsPosX[5] && mousePressedX < (buttonsPosX[5] + buttonsWidth[5]) &&
                     mousePressedY > buttonsPosY[5] && mousePressedY < (buttonsPosY[5] + buttonsHeight[5])) {
                if (speed > 1) {
                    --speed;
                }
                buttonPressed.play();
            }
            else if (mousePressedX > buildingSprite.getGlobalBounds().left && mousePressedX < (buildingSprite.getGlobalBounds().left + buildingSprite.getGlobalBounds().width) &&
                     mousePressedY > buildingSprite.getGlobalBounds().top && mousePressedY < (buildingSprite.getGlobalBounds().top + buildingSprite.getGlobalBounds().height) &&
                     isBuilding) {
                buttonPressed.play();
                isBuilding = 0;
            }
            else if (mousePressedX > hTowerIconSprites[0].getGlobalBounds().left) {
                for (short i = 0; i < Towers; ++i) {
                    if (mousePressedX > hTowerIconSprites[i].getGlobalBounds().left && mousePressedX < (hTowerIconSprites[i].getGlobalBounds().left + hTowerIconSprites[i].getGlobalBounds().width) &&
                        mousePressedY > hTowerIconSprites[i].getGlobalBounds().top && mousePressedY < (hTowerIconSprites[i].getGlobalBounds().top + hTowerIconSprites[i].getGlobalBounds().height)) {
                        buttonPressed.play();
                        isBuilding = i + 1;
                        break;
                    }
                }
            }
            else if (isBuilding && !isAttack) {
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
                if (currentGold < OTowersCost[isBuilding - 1] && race == Race::Orc) {
                    error.play();
                    notEnoughGold = true;
                    break;
                }
                else if (currentGold < HTowersCost[isBuilding - 1] && race == Race::Human) {
                    error.play();
                    notEnoughGold = true;
                    break;
                }
                for (unsigned short i = 0; i < dist; ++i) {
                    for (unsigned short j = 0; j < dist; ++j) {
                        map[yBox + i][xBox + j] = 1;
                    }
                }
                std::vector<std::pair<short, short>> tempPath = bfs();
                if (!tempPath.size()) {
                    error.play();
                    cantBuild = true;
                    for (unsigned short i = 0; i < dist; ++i) {
                        for (unsigned short j = 0; j < dist; ++j) {
                            map[yBox + i][xBox + j] = 0;
                        }
                    }
                    enemyPath = std::move(tempPath);
                    break;
                }
                construction.play();
                if (race == Race::Orc) {
                    playingTowers.push_back(Tower(oTowerTextures[isBuilding - 1], towerX, towerY, isBuilding - 1));
                }
                else {
                    playingTowers.push_back(Tower(hTowerTextures[isBuilding - 1], towerX, towerY, isBuilding - 1));
                }
                if (race == Race::Orc) {
                    currentGold -= OTowersCost[isBuilding - 1];
                }
                else {
                    currentGold -= HTowersCost[isBuilding - 1];
                }
            }
            mousePressedX = -1;
            mousePressedY = -1;
            cantBuild = false;
            notEnoughGold = false;
        }
    }
    gameIndex = gameIndex % 10000;

    window.display();
}

void gameOver() {
    window.clear(sf::Color::Blue);

    ++gameIndex;

    buttonWidth = desktopMode.width / 9;
    buttonHeight = desktopMode.height / 25;
    buttonPosX = desktopMode.width - 2 * buttonWidth;
    buttonPosY = desktopMode.height - buttonHeight;

    size_t totalScore = currentScore + 10 * currentGold + 1000 * currentHealth;

    if (race == Race::Orc) {
        if (win) {
            if (orcVictory.getStatus() == sf::SoundSource::Stopped) {
                orcVictory.play();
            }
            window.draw(oVictorySprite);
            buttonText = "Victory";
            totalScore += 5000;
        }
        else {
            if (orcDefeat.getStatus() == sf::SoundSource::Stopped) {
                orcDefeat.play();
            }
            window.draw(oDefeatSprite);
            buttonText = "Defeat";
        }
        createButton(window, quake, orcUnpressed, orcPressed, "Continue",
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
    }
    else {
        if (win) {
            if (humanVictory.getStatus() == sf::SoundSource::Stopped) {
                humanVictory.play();
            }
            window.draw(hVictorySprite);
            buttonText = "Victory";
            totalScore += 5000;
        }
        else {
            if (humanDefeat.getStatus() == sf::SoundSource::Stopped) {
                humanDefeat.play();
            }
            window.draw(hDefeatSprite);
            buttonText = "Defeat";
        }
        createButton(window, quake, humanUnpressed, humanPressed, "Continue",
                     buttonPosX, buttonPosY, buttonWidth, buttonHeight,
                     mousePressedX, mousePressedY);
    }

    sf::Text text;
    text.setFont(quake);
    text.setString(buttonText);
    text.setCharacterSize(desktopMode.height / 10);
    text.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - text.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 20;
    text.setPosition(buttonPosX, buttonPosY);
    window.draw(text);

    sf::Text enemyScore;
    enemyScore.setFont(quake);
    enemyScore.setString("Killed: " + std::to_string(currentScore));
    enemyScore.setCharacterSize(desktopMode.height / 25);
    enemyScore.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - enemyScore.getGlobalBounds().width) / 2;
    buttonPosY = desktopMode.height / 2;
    enemyScore.setPosition(buttonPosX, buttonPosY);

    sf::Text lives;
    lives.setFont(quake);
    lives.setString("Saved: " + std::to_string(currentHealth * 1000));
    lives.setCharacterSize(desktopMode.height / 25);
    lives.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - lives.getGlobalBounds().width) / 2;
    buttonPosY = enemyScore.getGlobalBounds().top + enemyScore.getGlobalBounds().height + 1;
    lives.setPosition(buttonPosX, buttonPosY);

    sf::Text gold;
    gold.setFont(quake);
    gold.setString("Gold: " + std::to_string(currentGold * 10));
    gold.setCharacterSize(desktopMode.height / 25);
    gold.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - gold.getGlobalBounds().width) / 2;
    buttonPosY = lives.getGlobalBounds().top + lives.getGlobalBounds().height + 1;
    gold.setPosition(buttonPosX, buttonPosY);

    sf::Text victory;
    victory.setFont(quake);
    victory.setString("Victory: 5000");
    victory.setCharacterSize(desktopMode.height / 25);
    victory.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - victory.getGlobalBounds().width) / 2;
    buttonPosY = gold.getGlobalBounds().top + gold.getGlobalBounds().height + 1;
    victory.setPosition(buttonPosX, buttonPosY);

    sf::Text total;
    total.setFont(quake);
    total.setString("Total: " + std::to_string(totalScore));
    total.setCharacterSize(desktopMode.height / 25);
    total.setColor(sf::Color::Yellow);
    buttonPosX = (desktopMode.width - total.getGlobalBounds().width) / 2;
    buttonPosY = victory.getGlobalBounds().top + victory.getGlobalBounds().height + 1;
    total.setPosition(buttonPosX, buttonPosY);

    buttonWidth = desktopMode.width / 9;
    buttonHeight = desktopMode.height / 25;
    buttonPosX = desktopMode.width - 2 * buttonWidth;
    buttonPosY = desktopMode.height - buttonHeight;

    switch (gameIndex / 10000) {
        default:
            --gameIndex;
            window.draw(total);
        case 3:
            if (win) {
                window.draw(victory);
            }
        case 2:
            window.draw(gold);
        case 1:
            window.draw(lives);
        case 0:
            window.draw(enemyScore);
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
            if (mousePressedX > buttonPosX && mousePressedX < (buttonPosX + buttonWidth) &&
                mousePressedY > buttonPosY && mousePressedY < (buttonPosY + buttonHeight)) {
                mousePressedX = -1;
                mousePressedY = -1;
                status = WindowStatus::MainMenu;
                buttonPressed.play();
                humanVictory.stop();
                humanDefeat.stop();
                orcVictory.stop();
                orcDefeat.stop();
            }
        }
    }

    window.display();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    for (unsigned char i = 0; i < 6; ++i) {
        std::ifstream file(std::to_string(i));
        if (!file.is_open()) {
            continue;
        }
        std::string readingString, saveName;
        std::getline(file, readingString);
        if (readingString == "") {
            continue;
        }
        if (readingString == "Orc") {
            saveName = "Orc, ";
        }
        else {
            saveName = "Human, ";
        }
        std::getline(file, readingString);
        if (readingString == "") {
            continue;
        }
        saves[i] = saveName + std::to_string(1 + std::stoi(readingString));
        file.close();
    }

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
    sf::SoundBuffer pressing, constructing, cantDoThis, fire, hit;
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
    if (!fire.loadFromFile("SFX/ui/Bowfire.wav")) {
        return -1;
    }
    shot.setBuffer(fire);
    if (!hit.loadFromFile("SFX/ui/Bowhit.wav")) {
        return -1;
    }
    bowHit.setBuffer(hit);

    sf::SoundBuffer bufferOrcUnderAttack, bufferOrcDead;
    if (!bufferOrcUnderAttack.loadFromFile("SFX/Orc/Ohelp.wav")) {
        return -1;
    }
    orcUnderAttack.setBuffer(bufferOrcUnderAttack);
    if (!bufferOrcDead.loadFromFile("SFX/Orc/Odead.wav")) {
        return -1;
    }
    orcDead.setBuffer(bufferOrcDead);

    sf::SoundBuffer bufferHumanUnderAttack, bufferHumanDead;
    if (!bufferHumanUnderAttack.loadFromFile("SFX/Human/Hhelp.wav")) {
        return -1;
    }
    humanUnderAttack.setBuffer(bufferHumanUnderAttack);
    if (!bufferHumanDead.loadFromFile("SFX/Human/Hdead.wav")) {
        return -1;
    }
    humanDead.setBuffer(bufferHumanDead);

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

    if (!hVictoryTexture.loadFromFile("Textures/ui/Human/victory.png")) {
        return -1;
    }
    hVictoryTexture.setSmooth(true);
    hVictorySprite.setTexture(hVictoryTexture);
    factorX = ((float) desktopMode.width) / hVictoryTexture.getSize().x;
    factorY = ((float) desktopMode.height) / hVictoryTexture.getSize().y;
    hVictorySprite.scale(factorX, factorY);

    if (!hDefeatTexture.loadFromFile("Textures/ui/Human/defeat.png")) {
        return -1;
    }
    hDefeatTexture.setSmooth(true);
    hDefeatSprite.setTexture(hDefeatTexture);
    factorX = ((float) desktopMode.width) / hDefeatTexture.getSize().x;
    factorY = ((float) desktopMode.height) / hDefeatTexture.getSize().y;
    hDefeatSprite.scale(factorX, factorY);

    if (!oVictoryTexture.loadFromFile("Textures/ui/Orc/victory.png")) {
        return -1;
    }
    oVictoryTexture.setSmooth(true);
    oVictorySprite.setTexture(oVictoryTexture);
    factorX = ((float) desktopMode.width) / oVictoryTexture.getSize().x;
    factorY = ((float) desktopMode.height) / oVictoryTexture.getSize().y;
    oVictorySprite.scale(factorX, factorY);

    if (!oDefeatTexture.loadFromFile("Textures/ui/Orc/defeat.png")) {
        return -1;
    }
    oDefeatTexture.setSmooth(true);
    oDefeatSprite.setTexture(oDefeatTexture);
    factorX = ((float) desktopMode.width) / oDefeatTexture.getSize().x;
    factorY = ((float) desktopMode.height) / oDefeatTexture.getSize().y;
    oDefeatSprite.scale(factorX, factorY);

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
    oSaveSprite.setTexture(oInfoTexture);
    oSaveSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 4, desktopMode.height / 5));
    oSaveSprite.setPosition((desktopMode.width - oSaveSprite.getGlobalBounds().width) / 2, (desktopMode.height - oSaveSprite.getGlobalBounds().height) / 2);
    oEndSprite.setTexture(oInfoTexture);
    oEndSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 4, 4 * desktopMode.height / 25));
    oEndSprite.setPosition((desktopMode.width - oEndSprite.getGlobalBounds().width) / 2, (desktopMode.height - oEndSprite.getGlobalBounds().height) / 2);

    if (!hInfoTexture.loadFromFile("Textures/ui/Human/infopanel.png")) {
        return -1;
    }
    hInfoTexture.setRepeated(true);
    hInfoSprite.setTexture(hInfoTexture);
    hInfoSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 8, desktopMode.height));
    hInfoSprite.setPosition(8 * desktopMode.width / 9, 0);
    hSaveSprite.setTexture(hInfoTexture);
    hSaveSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 4, desktopMode.height / 5));
    hSaveSprite.setPosition((desktopMode.width - hSaveSprite.getGlobalBounds().width) / 2, (desktopMode.height - hSaveSprite.getGlobalBounds().height) / 2);
    hEndSprite.setTexture(hInfoTexture);
    hEndSprite.setTextureRect(sf::IntRect(0, 0, desktopMode.width / 4, 4 * desktopMode.height / 25));
    hEndSprite.setPosition((desktopMode.width - hEndSprite.getGlobalBounds().width) / 2, (desktopMode.height - hEndSprite.getGlobalBounds().height) / 2);

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
    for (unsigned short i = 0; i < Towers; ++i) {
        if (!oTowerTextures[i].loadFromFile("Textures/Tilesets/Orc/" + strings[i] + ".png")) {
            return -1;
        }
        oTowerTextures[i].setSmooth(true);
        oTowerSprites[i].setTexture(oTowerTextures[i]);
        oTowerSprites[i].setScale(windowScaleWidth, windowScaleHeight);
        if (!hTowerTextures[i].loadFromFile("Textures/Tilesets/Human/" + strings[i + Towers] + ".png")) {
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
    for (unsigned short i = 0; i < Ammos; ++i) {
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
    for (short i = 0; i < Towers; ++i) {
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
    smallFireSprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    smallFireSprite.setScale(windowScaleWidth, windowScaleHeight);
    if (!bigFireTexture.loadFromFile("Textures/Tilesets/big_fire.png")) {
        return -1;
    }
    bigFireTexture.setSmooth(true);
    bigFireSprite.setTexture(bigFireTexture);
    bigFireSprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
    bigFireSprite.setScale(windowScaleWidth, windowScaleHeight);

    //Загрузка карт уровней
    for (short i = 0; i < MaxLevel; ++i) {
        if (!hGameTextures[i].loadFromFile("Textures/Maps/human" + std::to_string(i + 1) + ".png")) {
            return -1;
        }
        if (!oGameTextures[i].loadFromFile("Textures/Maps/orc" + std::to_string(i + 1) + ".png")) {
            return -1;
        }
        hGameTextures[i].setSmooth(true);
        oGameTextures[i].setSmooth(true);
        hGameSprites[i].setTexture(hGameTextures[i]);
        oGameSprites[i].setTexture(oGameTextures[i]);
        factorX = ((float) desktopMode.width) / hGameTextures[i].getSize().x * 8 / 9;
        factorY = ((float) desktopMode.height) / hGameTextures[i].getSize().y;
        hGameSprites[i].scale(factorX, factorY);
        factorX = ((float) desktopMode.width) / oGameTextures[i].getSize().x * 8 / 9;
        factorY = ((float) desktopMode.height) / oGameTextures[i].getSize().y;
        oGameSprites[i].scale(factorX, factorY);
    }

    //Загрузка текстов брифингов
    for (short i = 0; i < MaxLevel; ++i) {
        std::ifstream briefingFile("Briefings/orc" + std::to_string(i + 1) + ".txt");
        std::string readingString;
        if (!briefingFile.is_open()) {
            return -1;
        }
        while (std::getline(briefingFile, readingString)) {
            orcTexts[i] += readingString + "\n";
        }
        briefingFile.close();
        briefingFile.open("Briefings/human" + std::to_string(i + 1) + ".txt");
        if (!briefingFile.is_open()) {
            return -1;
        }
        while (std::getline(briefingFile, readingString)) {
            humanTexts[i] += readingString + "\n";
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
            case WindowStatus::End:
                gameOver();
                break;
        }
    }
    return 0;
}
