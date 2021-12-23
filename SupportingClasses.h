#ifndef SUPPORTINGCLASSES_H_INCLUDED
#define SUPPORTINGCLASSES_H_INCLUDED

#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

sf::Sound shot, humanDead, orcDead;

enum class Race {
    Human,
    Orc
};

const unsigned char Units = 1, Towers = 2, Ammos = 1;
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
const unsigned short boxSize = 32;
const float windowScaleWidth = desktopMode.width / 1920, windowScaleHeight = desktopMode.height / 1080;

Race race;
unsigned char speed = 3;

//Grunt
const short HEnemiesStartX[Units] {-106 * windowScaleWidth};
const short HEnemiesStartY[Units] {14 * boxSize * windowScaleHeight};
const short HEnemiesGold[Units] {5};
const short HEnemiesScore[Units] {100};
const unsigned short HEnemiesHealth[Units] {250};
const unsigned char HEnemiesDefense[Units] {0};
const unsigned char HEnemiesBoxWidth[Units] {72};
const unsigned char HEnemiesBoxHeight[Units] {72};
const unsigned char HEnemiesMovingStart[Units] {0};
const unsigned char HEnemiesMovingCount[Units] {5};
const unsigned char HEnemiesDyingStart[Units] {8};
const unsigned char HEnemiesDyingCount[Units] {3};
const unsigned char HEnemiesUp[Units] {0};
const unsigned char HEnemiesDown[Units] {4};
const unsigned char HEnemiesLeft[Units] {6};
const unsigned char HEnemiesRight[Units] {2};
//Footman
const short OEnemiesStartX[Units] {22 * boxSize * windowScaleWidth};
const short OEnemiesStartY[Units] {-184 * windowScaleHeight};
const short OEnemiesGold[Units] {5};
const short OEnemiesScore[Units] {100};
const unsigned short OEnemiesHealth[Units] {100};
const unsigned char OEnemiesDefense[Units] {5};
const unsigned char OEnemiesBoxWidth[Units] {72};
const unsigned char OEnemiesBoxHeight[Units] {72};
const unsigned char OEnemiesMovingStart[Units] {0};
const unsigned char OEnemiesMovingCount[Units] {5};
const unsigned char OEnemiesDyingStart[Units] {8};
const unsigned char OEnemiesDyingCount[Units] {3};
const unsigned char OEnemiesUp[Units] {0};
const unsigned char OEnemiesDown[Units] {4};
const unsigned char OEnemiesLeft[Units] {6};
const unsigned char OEnemiesRight[Units] {2};

const size_t OTowersCost[Towers] {1, 10};
const size_t OTowersDamage[2 * Towers] {0, 0, 14, 16};
const float OTowersRate[Towers] {0, 0.25};
const unsigned short OTowersRange[Towers] {0, 450};
const unsigned short OTowersAmmo[Towers] {0, 0};
const std::string OTowersNames[Towers] {"Wall", "Watch Tower"};

const size_t HTowersCost[Towers] {1, 10};
const size_t HTowersAttack[2 * Towers] {0, 0, 14, 16};
const float HTowersRate[Towers] {0, 0.25};
const unsigned short HTowersRange[Towers] {0, 450};
const unsigned short HTowersAmmo[Towers] {0, 0};
const std::string HTowersNames[Towers] {"Wall", "Scout Tower"};

enum class Moving {
    Up,
    Down,
    Left,
    Right
};

class Path {
public:
    std::vector<std::pair<short, short>> path;
    short x;
    short y;
    Path(short gotX, short gotY, std::vector<std::pair<short, short>> vector) {
        path = vector;
        path.push_back(std::make_pair(gotX, gotY));
        x = gotX;
        y = gotY;
    }
};

class Unit {
    short _x;
    short _y;
    short _health;
    unsigned char _def;
    unsigned short _gold;
    unsigned short _score;
    unsigned int _index = 0;
    sf::Sprite _sprite;
    unsigned char _boxWidth;
    unsigned char _boxHeight;
    unsigned char _movingBoxStart;
    unsigned char _movingBoxCount;
    unsigned char _dyingBoxStart;
    unsigned char _dyingBoxCount;
    unsigned char _upBox;
    unsigned char _downBox;
    unsigned char _leftBox;
    unsigned char _rightBox;
    bool _isDead = false;
    bool _toDelete = false;
    Moving lastDir;
    unsigned char _unit;

    void load(const std::string& info) {
        std::string temp = info;
        size_t index = temp.find('|');
        _x = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _y = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _health = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _isDead = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _toDelete = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        partOfPath = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _unit = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        if (temp.substr(0, index) == "Down") {
            lastDir = Moving::Down;
        }
        else if (temp.substr(0, index) == "Left") {
            lastDir = Moving::Left;
        }
        else if (temp.substr(0, index) == "Right") {
            lastDir = Moving::Right;
        }
        else {
            lastDir = Moving::Up;
        }
        if (race == Race::Orc) {
            _def = OEnemiesDefense[_unit];
            _gold = OEnemiesGold[_unit];
            _score = OEnemiesScore[_unit];
            _boxWidth = OEnemiesBoxWidth[_unit];
            _boxHeight = OEnemiesBoxHeight[_unit];
            _movingBoxStart = OEnemiesMovingStart[_unit];
            _movingBoxCount = OEnemiesMovingCount[_unit];
            _dyingBoxStart = OEnemiesDyingStart[_unit];
            _dyingBoxCount = OEnemiesDyingCount[_unit];
            _upBox = OEnemiesUp[_unit];
            _downBox = OEnemiesDown[_unit];
            _leftBox = OEnemiesLeft[_unit];
            _rightBox = OEnemiesRight[_unit];
        }
        else {
            _def = HEnemiesDefense[_unit];
            _gold = HEnemiesGold[_unit];
            _score = HEnemiesScore[_unit];
            _boxWidth = HEnemiesBoxWidth[_unit];
            _boxHeight = HEnemiesBoxHeight[_unit];
            _movingBoxStart = HEnemiesMovingStart[_unit];
            _movingBoxCount = HEnemiesMovingCount[_unit];
            _dyingBoxStart = HEnemiesDyingStart[_unit];
            _dyingBoxCount = HEnemiesDyingCount[_unit];
            _upBox = HEnemiesUp[_unit];
            _downBox = HEnemiesDown[_unit];
            _leftBox = HEnemiesLeft[_unit];
            _rightBox = HEnemiesRight[_unit];
        }
    }

public:
    unsigned short partOfPath = 0;

    Unit() {}

    Unit(const std::string& info) {
        load(info);
    }

    Unit(unsigned char unit, sf::Texture& texture) {
        _unit = unit;
        if (race == Race::Orc) {
            _x = OEnemiesStartX[unit];
            _y = OEnemiesStartY[unit];
            _health = OEnemiesHealth[unit];
            _def = OEnemiesDefense[unit];
            _gold = OEnemiesGold[unit];
            _score = OEnemiesScore[unit];
            _boxWidth = OEnemiesBoxWidth[unit];
            _boxHeight = OEnemiesBoxHeight[unit];
            _movingBoxStart = OEnemiesMovingStart[unit];
            _movingBoxCount = OEnemiesMovingCount[unit];
            _dyingBoxStart = OEnemiesDyingStart[unit];
            _dyingBoxCount = OEnemiesDyingCount[unit];
            _upBox = OEnemiesUp[unit];
            _downBox = OEnemiesDown[unit];
            _leftBox = OEnemiesLeft[unit];
            _rightBox = OEnemiesRight[unit];
        }
        else {
            _x = HEnemiesStartX[unit];
            _y = HEnemiesStartY[unit];
            _health = HEnemiesHealth[unit];
            _def = HEnemiesDefense[unit];
            _gold = HEnemiesGold[unit];
            _score = HEnemiesScore[unit];
            _boxWidth = HEnemiesBoxWidth[unit];
            _boxHeight = HEnemiesBoxHeight[unit];
            _movingBoxStart = HEnemiesMovingStart[unit];
            _movingBoxCount = HEnemiesMovingCount[unit];
            _dyingBoxStart = HEnemiesDyingStart[unit];
            _dyingBoxCount = HEnemiesDyingCount[unit];
            _upBox = HEnemiesUp[unit];
            _downBox = HEnemiesDown[unit];
            _leftBox = HEnemiesLeft[unit];
            _rightBox = HEnemiesRight[unit];
        }
        _sprite.setTexture(texture);
        _sprite.setPosition(_x - boxSize / 2, _y - boxSize / 2);
        _sprite.setTextureRect(sf::IntRect(0, 0, _boxWidth, _boxHeight));
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
    }

    void move(Moving dir) {
        unsigned short _speed = 10 * speed;
        unsigned short _animationSpeed = 350 * speed;
        if (!(_index % _speed)){
            switch (dir) {
                case Moving::Down:
                    ++_y;
                    break;
                case Moving::Left:
                    --_x;
                    break;
                case Moving::Right:
                    ++_x;
                    break;
                case Moving::Up:
                    --_y;
                    break;
            }
            _sprite.setPosition(_x - boxSize / 2, _y - boxSize / 2);
        }
        char curBox = (_index % _animationSpeed) / (_animationSpeed / _movingBoxCount);
        switch (dir) {
            case Moving::Down:
                _sprite.setTextureRect(sf::IntRect(_downBox * _boxWidth, _boxHeight * (curBox + _movingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Left:
                _sprite.setTextureRect(sf::IntRect(_leftBox * _boxWidth, _boxHeight * (curBox + _movingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Right:
                _sprite.setTextureRect(sf::IntRect(_rightBox * _boxWidth, _boxHeight * (curBox + _movingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Up:
                _sprite.setTextureRect(sf::IntRect(_upBox * _boxWidth, _boxHeight * (curBox + _movingBoxStart), _boxWidth, _boxHeight));
                break;
        }
        lastDir = dir;
        ++_index;
        _index = _index % (_animationSpeed * _speed);
    }

    void die() {
        unsigned short _speed = 10 * speed;
        unsigned short _animationSpeed = 350 * speed;
        char curBox = (_index % _animationSpeed) / (_animationSpeed / _dyingBoxCount);
        switch (lastDir) {
            case Moving::Down:
                _sprite.setTextureRect(sf::IntRect(_downBox * _boxWidth, _boxHeight * (curBox + _dyingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Left:
                _sprite.setTextureRect(sf::IntRect(_leftBox * _boxWidth, _boxHeight * (curBox + _dyingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Right:
                _sprite.setTextureRect(sf::IntRect(_rightBox * _boxWidth, _boxHeight * (curBox + _dyingBoxStart), _boxWidth, _boxHeight));
                break;
            case Moving::Up:
                _sprite.setTextureRect(sf::IntRect(_upBox * _boxWidth, _boxHeight * (curBox + _dyingBoxStart), _boxWidth, _boxHeight));
                break;
        }
        if (!_index) {
            _toDelete = true;
        }
        ++_index;
        _index = _index % _animationSpeed;
    }

    void damaged(unsigned short damage) {
        _health -= (damage - _def);
        if (_health < 0) {
            if (race == Race::Orc) {
                humanDead.play();
            }
            else {
                orcDead.play();
            }
            _isDead = true;
            die();

        }
    }

    sf::Sprite& sprite() {
        return _sprite;
    }

    short x() {
        return _x;
    }

    short y() {
        return _y;
    }

    bool isDead() {
        return _isDead;
    }

    bool toDelete() {
        return _toDelete;
    }

    unsigned short gold() {
        return _gold;
    }

    unsigned short score() {
        return _score;
    }

    unsigned char unit() {
        return _unit;
    }

    void setTexture(sf::Texture& texture) {
        _sprite.setTexture(texture);
        _sprite.setPosition(_x - boxSize / 2, _y - boxSize / 2);
        _sprite.setTextureRect(sf::IntRect(0, 0, _boxWidth, _boxHeight));
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
    }

    std::string save() {
        std::string res = "";
        res += std::to_string(_x) + "|";
        res += std::to_string(_y) + "|";
        res += std::to_string(_health) + "|";
        res += std::to_string(_isDead) + "|";
        res += std::to_string(_toDelete) + "|";
        res += std::to_string(partOfPath) + "|";
        res += std::to_string(_unit) + "|";
        switch (lastDir) {
            case Moving::Down:
                res += "Down\n";
                break;
            case Moving::Left:
                res += "Left\n";
                break;
            case Moving::Right:
                res += "Right\n";
                break;
            case Moving::Up:
                res += "Up\n";
                break;
        }
        return res;
    }
};

class Tower {
    sf::Sprite _sprite;
    unsigned short _x;
    unsigned short _y;
    unsigned short _index = 0;
    float _rate;
    unsigned char _code;

    void load(const std::string& info) {
        std::string temp = info;
        size_t index = temp.find('|');
        _x = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _y = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _rate = std::stof(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _code = std::stoi(temp.substr(0, index));
    }

public:
    Tower() {}

    Tower(std::string& info) {
        load(info);
    }

    Tower(sf::Texture& texture, unsigned short x, unsigned short y, unsigned char code) {
        if (race == Race::Orc) {
            _rate = OTowersRate[code];
        }
        else {
            _rate = HTowersRate[code];
        }
        _code = code;
        _sprite.setTexture(texture);
        _sprite.setPosition(x, y);
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
        _x = x + _sprite.getGlobalBounds().width / 2;
        _y = y + _sprite.getGlobalBounds().height / 2;
    }

    bool towerShot() {
        if (!(_index % (int)(_rate * 10000))) {
            ++_index;
            shot.play();
            return true;
        }
        _index = _index % (int)(_rate * 10000);
        return false;
    }

    void idle() {
        if ((_index % (int)(_rate * 10000))) {
            ++_index;
        }
    }

    unsigned short& index() {
        return _index;
    }

    sf::Sprite& sprite() {
        return _sprite;
    }

    unsigned short x() {
        return _x;
    }

    unsigned short y() {
        return _y;
    }

    unsigned char code() {
        return _code;
    }

    unsigned char ammo() {
        if (race == Race::Orc) {
            return OTowersAmmo[_code];
        }
        else {
            return HTowersAmmo[_code];
        }
    }

    void setTexture(sf::Texture& texture) {
        _sprite.setTexture(texture);
        _sprite.setPosition(_x - _sprite.getGlobalBounds().width / 2, _y - _sprite.getGlobalBounds().height / 2);
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
    }

    unsigned short range() {
        if (race == Race::Orc) {
            return OTowersRange[_code];
        }
        else {
            return HTowersRange[_code];
        }
    }

    std::string save() {
        std::string res = "";
        res += std::to_string(_x) + "|";
        res += std::to_string(_y) + "|";
        res += std::to_string(_rate) + "|";
        res += std::to_string(_code) + "\n";
        return res;
    }
};

class Ammo {
    sf::Sprite _sprite;
    short _x;
    short _y;
    short _destX;
    short _destY;
    unsigned char _aim;
    unsigned char _code;
    unsigned char _tower;

    void load(std::string& info) {
        std::string temp = info;
        size_t index = temp.find('|');
        _x = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _y = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _destX = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _destY = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _aim = std::stoi(temp.substr(0, index));
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _code = std::stoi(temp);
        temp = temp.substr(index + 1);
        index = temp.find('|');
        _tower = std::stoi(temp);
    }

public:
    Ammo() {}

    Ammo(std::string& info) {
        load(info);
    }

    Ammo(sf::Texture& texture, short x, short y, short destX, short destY, unsigned char aim, unsigned char tower, unsigned char code) {
        _x = x;
        _y = y;
        _destX = destX;
        _destY = destY;
        _aim = aim;
        _code = code;
        _tower = tower;
        _sprite.setTexture(texture);
        _sprite.setTextureRect(sf::IntRect(0, 0, 39, 39));
        _sprite.setPosition(_x - _sprite.getGlobalBounds().width / 2, _y - _sprite.getGlobalBounds().height / 2);
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
    }

    void move() {
        short dx = _destX - _x, dy = _destY - _y;
        float cos = ((float) dx) / std::sqrt(dx * dx + dy * dy);
        float sin = ((float) dy) / std::sqrt(dx * dx + dy * dy);
        if (sin < -std::sqrt(3) / 2) {
            _sprite.setTextureRect(sf::IntRect(39 * 0, 0, 39, 39));
        }
        else if (cos > 1/2 && cos < std::sqrt(3) / 2 && sin < 0) {
            _sprite.setTextureRect(sf::IntRect(39 * 1, 0, 39, 39));
        }
        else if (cos > std::sqrt(3) / 2) {
            _sprite.setTextureRect(sf::IntRect(39 * 2, 0, 39, 39));
        }
        else if (cos > 1/2 && cos < std::sqrt(3) && sin > 0) {
            _sprite.setTextureRect(sf::IntRect(39 * 3, 0, 39, 39));
        }
        else if (sin > std::sqrt(3) / 2) {
            _sprite.setTextureRect(sf::IntRect(39 * 4, 0, 39, 39));
        }
        else if (cos > -std::sqrt(3) / 2 && cos < - 1/2 && sin > 0) {
            _sprite.setTextureRect(sf::IntRect(39 * 5, 0, 39, 39));
        }
        else if (cos < -std::sqrt(3) / 2) {
            _sprite.setTextureRect(sf::IntRect(39 * 6, 0, 39, 39));
        }
        else if (cos > -(std::sqrt(3) / 2) && cos < -1/2 && sin < 0) {
            _sprite.setTextureRect(sf::IntRect(39 * 7, 0, 39, 39));
        }
        _x += cos * speed;
        _y += sin * speed;
        _sprite.setPosition(_x - _sprite.getGlobalBounds().width / 2, _y - _sprite.getGlobalBounds().height / 2);
    }

    void setDestination(short x, short y) {
        _destX = x;
        _destY = y;
    }

    bool isHit() {
        short dx = std::abs(_x - _destX), dy = std::abs(_y - _destY);
        return (dx <= 10 && dy <= 10);
    }

    unsigned char aim() {
        return _aim;
    }

    unsigned short minDmg() {
        if (race == Race::Orc) {
            return OTowersDamage[2 * _tower];
        }
        else {
            return HTowersAttack[2 * _tower];
        }
    }

    unsigned short maxDmg() {
        if (race == Race::Orc) {
            return OTowersDamage[2 * _tower + 1];
        }
        else {
            return HTowersAttack[2 * _tower + 1];
        }
    }

    sf::Sprite& sprite() {
        return _sprite;
    }

    unsigned char code() {
        return _code;
    }

    void setTexture(sf::Texture& texture) {
        _sprite.setTexture(texture);
        _sprite.setTextureRect(sf::IntRect(0, 0, 39, 39));
        _sprite.setPosition(_x - _sprite.getGlobalBounds().width / 2, _y - _sprite.getGlobalBounds().height / 2);
        _sprite.setScale(windowScaleWidth, windowScaleHeight);
    }

    std::string save() {
        std::string res = "";
        res += std::to_string(_x) + '|';
        res += std::to_string(_y) + '|';
        res += std::to_string(_destX) + '|';
        res += std::to_string(_destY) + '|';
        res += std::to_string(_aim) + '|';
        res += std::to_string(_code) + '|';
        res += std::to_string(_tower) + '\n';
        return res;
    }
};

#endif // SUPPORTINGCLASSES_H_INCLUDED
