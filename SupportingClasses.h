#ifndef SUPPORTINGCLASSES_H_INCLUDED
#define SUPPORTINGCLASSES_H_INCLUDED

#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

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
    unsigned int _index = 0;
    unsigned int _speed = 40;
    unsigned int _animationSpeed = 600;
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
public:
    Unit(short x, short y,  sf::Sprite sprite, short health, unsigned char def,
         unsigned char boxWidth, unsigned char boxHeight,
         unsigned char movingBoxStart, unsigned char movingBoxCount, unsigned char dyingBoxStart, unsigned char dyingBoxCount,
         unsigned char upBox, unsigned char downBox, unsigned char leftBox, unsigned char rightBox) {
        _x = x;
        _y = y;
        _boxWidth = boxWidth;
        _boxHeight = boxHeight;
        _movingBoxStart = movingBoxStart;
        _movingBoxCount = movingBoxCount;
        _dyingBoxStart = dyingBoxStart;
        _dyingBoxCount = dyingBoxCount;
        _upBox = upBox;
        _downBox = downBox;
        _leftBox = leftBox;
        _rightBox = rightBox;
        _health = health;
        _def = def;
    }

    void move(Moving dir) {
        if (!(_index % _speed)){
            switch (dir) {
                case Moving::Down:
                    ++_y;
                    break;
                case Moving::Left:
                    ++_x;
                    break;
                case Moving::Right:
                    --_x;
                    break;
                case Moving::Up:
                    --_y;
                    break;
            }
            _sprite.setPosition(_x, _y);
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
        char curBox = (_index % (_animationSpeed * _speed)) / ((_animationSpeed * _speed) / _dyingBoxCount);
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
        _index = _index % (_animationSpeed * _speed);
    }

    void damaged(unsigned short damage) {
        _health -= (damage - _def);
        if (_health < 0) {
            _isDead = true;
        }
    }
};

#endif // SUPPORTINGCLASSES_H_INCLUDED
