#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "game.hpp"

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
    textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

int main() {
    int* config = new int[3];

    std::ifstream file("../files/config.cfg");

    std::string line;

    if (file.is_open()) 
    {
        int i = 0;
        while (getline(file, line)) 
        {
            config[i] = std::stoi(line);
            i++;
        }
        file.close();
    }
    else 
    {
        std::cerr << "Unable to open file!" << std::endl;
    }

    const int width = config[0] * 32;
    const int height = (config[1] * 32) + 100;
    std::string usernm = "";

    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
    
    sf::Font font;
    if (!font.loadFromFile("../files/font.ttf"))
    {
        std::cerr << "Error while loading font!" << std::endl;
        return -1;
    }

    sf::Text title("WELCOME TO MINESWEEPER!", font, 24);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(title, width / 2, (height / 2) - 150);

    sf::Text prompt("Enter your name: ", font, 20);
    prompt.setStyle(sf::Text::Bold);
    setText(prompt, width / 2, (height / 2) - 75);

    sf::Text inputNm("|", font, 18);
    inputNm.setStyle(sf::Text::Bold);
    inputNm.setFillColor(sf::Color::Yellow);
    setText(inputNm, width / 2, (height / 2) - 45);

    bool cursorShowing = false;

    while(window.isOpen()) 
    {
        //========== USER INTERACTION ==========//
        sf::Event event;
        
        while(window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered)
            {
                if (
                        std::isalpha(static_cast<char>(event.text.unicode)) && 
                        usernm.length() <= 10
                    )
                {
                    usernm += static_cast<char>(event.text.unicode);
                    
                    // format string casing
                    usernm[0] = std::toupper(usernm[0]);
                    for (int i = 1; i < usernm.length(); i++)
                    {
                        usernm[i] = std::tolower(usernm[i]);
                    }

                    inputNm.setString(usernm + "|");
                    setText(inputNm, width / 2, (height / 2) - 45); // recenter
                }

                else if (event.text.unicode == 8)
                {
                    usernm = usernm.substr(0, usernm.length() - 1);
                
                    inputNm.setString(usernm + "|");
                    setText(inputNm, width / 2, (height / 2) - 45); // recenter
                }

                else if ((event.text.unicode == 13 || event.text.unicode == 10) && usernm.length() > 0)
                {
                    window.close();
                    Game* gm = new Game(usernm, width, height, config[2]);
                    gm->mainGame();
                }
            }
        }

        //========== FRAME RENDERING ==========//
        window.clear(sf::Color::Blue);
        window.draw(title);
        window.draw(prompt);
        window.draw(inputNm);
        window.display();
    }
    delete[] config;

    return 0;
}
