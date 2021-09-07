#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

enum class Style {
    Human,
    Orc
};

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
    if (mouseX > posX && mouseX < (posX + width)
    button.setTexture(texture);
    button.setPosition()
}

#endif // BUTTON_H_INCLUDED
