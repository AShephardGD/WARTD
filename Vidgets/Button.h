#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

void createButton(sf::RenderWindow& window, sf::Font& font,
                  sf::Texture& unpressed, sf::Texture& pressed,
                  std::string& str,
                  size_t posX, size_t posY,
                  size_t width, size_t height,
                  size_t mouseX, size_t mouseY) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    sf::Sprite button;
    if (mouseX > posX && mouseX < (posX + width) &&
        mouseY > posY && mouseY < (posY + height)) {
        button.setTexture(pressed);
    }
    else {
        button.setTexture(unpressed);
    }
    button.setPosition(posX, posY);
}

#endif // BUTTON_H_INCLUDED
