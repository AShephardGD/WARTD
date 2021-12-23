#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


void createButton(sf::RenderWindow& window, sf::Font& font,
                  sf::Texture& unpressed, sf::Texture& pressed,
                  const std::string& str,
                  size_t posX, size_t posY,
                  float width, float height,
                  size_t mouseX, size_t mouseY) {
    //Отрисовка текстуры кнопки
    sf::Sprite button;
    if (mouseX > posX && mouseX < (posX + width) &&
        mouseY > posY && mouseY < (posY + height)) {
        button.setTexture(pressed);
    }
    else {
        button.setTexture(unpressed);
    }
    button.setPosition(posX, posY);
    float scaleX = width / button.getGlobalBounds().width,
          scaleY = height / button.getGlobalBounds().height;
    button.scale(scaleX, scaleY);
    window.draw(button);

    //Отрисовка текста на кнопке
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setColor(sf::Color::Yellow);
    text.setCharacterSize(height * 4 / 5);
    size_t textX = posX + (width / 2)  - (text.getGlobalBounds().width / 2), textY = posY;
    text.setPosition(textX, textY);
    window.draw(text);
}

#endif // BUTTON_H_INCLUDED
