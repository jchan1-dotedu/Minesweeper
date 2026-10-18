#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <algorithm>

class Tile
{
    int xpos;
    int ypos;
    int state; // 0, hidden, 1, revealed, 2, flagged
    int knownSurrounding;
    bool isMine;

    sf::Sprite sprite;
    sf::Sprite flagSprite;
    sf::Sprite mineSprite;
    sf::Sprite numSprite;

public:
    Tile() : xpos(0), ypos(0), state(0), isMine(false), knownSurrounding(0)
    {
    }

    Tile(int x, int y, int s, bool m, sf::Texture* texture, sf::Texture* flag, sf::Texture *mine) : xpos(x), ypos(y), state(s), isMine(m)
    {
        sprite.setPosition(x, y);
        sprite.setTexture(*texture);

        flagSprite.setPosition(x, y);
        flagSprite.setTexture(*flag);

        mineSprite.setPosition(x, y);
        mineSprite.setTexture(*mine);

        numSprite.setPosition(x, y);

        knownSurrounding = 0;
    }

    void setKnownSurrounding(int in)
    {
        knownSurrounding = in;
    }

    int getKnownSurrounding()
    {
        return knownSurrounding;
    }

    void setNumTexture(sf::Texture* numTexture)
    {
        numSprite.setTexture(*numTexture);
    }

    void updateTexture(sf::Texture* texture)
    {
        sprite.setTexture(*texture);
    }

    sf::Sprite getSprite()
    {
        return sprite;
    }

    sf::Sprite getFlag()
    {
        return flagSprite;
    }

    sf::Sprite getMineSprite()
    {
        return mineSprite;
    }

    sf::Sprite getNumSprite()
    {
        return numSprite;
    }

    int getState()
    {
        return state;
    }

    void setState(int in)
    {
        state = in;
    }

    bool getMine()
    {
        return isMine;
    }

    void setMine(bool mine)
    {
        this->isMine = mine;
    }
};

class Game
{
    const int neighborOffsets[2][8] = {
        { -1, 0, 1, -1, 1, -1, 0, 1},
        { -1, -1, -1, 0, 0, 1, 1, 1}
    };

    std::string** leaderBoard;

    std::string usernm;
    int width;
    int height;
    int mines;
    int flags;
    int revealed;
    int rows;
    int cols;
    Tile*** Board;

    bool inGame;
    bool paused;
    bool inLB;
    bool isWon;
    bool isLoss;
    bool showMines;

public:
    sf::Texture* tileHidden;
    sf::Texture* tileRevealed;
    sf::Texture* flag;
    sf::Texture* mine;
    sf::Texture** numTextures;
    sf::Texture* faceHappy;
    sf::Texture* faceLose;
    sf::Texture* faceWin;
    sf::Texture* debug;
    sf::Texture* pause;
    sf::Texture* play;
    sf::Texture* leaderBoardT;
    sf::Texture* numStrip;

    Game(std::string u, int w, int h, int m) : usernm(u), width(w), height(h), mines(m), revealed(0), showMines(false), isWon(false), isLoss(false), flags(m), paused(false), inLB(false)
    {
        inGame = true;

        rows = ((height - 100) / 32);
        cols = (width / 32);

        tileHidden = new sf::Texture();
        if (!tileHidden->loadFromFile("../files/images/tile_hidden.png"))
            std::cerr << "Error: Failed to tile_hidden.png" << std::endl;

        tileRevealed = new sf::Texture();
        if (!tileRevealed->loadFromFile("../files/images/tile_revealed.png"))
            std::cerr << "Error: Failed to tile_revealed.png" << std::endl;

        flag = new sf::Texture();
        if (!flag->loadFromFile("../files/images/flag.png"))
            std::cerr << "Error: Failed to flag.png" << std::endl;

        mine = new sf::Texture();
        if (!mine->loadFromFile("../files/images/mine.png"))
            std::cerr << "Error: Failed to mine.png" << std::endl;

        numTextures = new sf::Texture*[8];
        for (int i = 0; i < 8; i++)
        {
            numTextures[i] = new sf::Texture();
            if (!numTextures[i]->loadFromFile("../files/images/number_" + std::to_string(i + 1) + ".png"))
                std::cerr << "Error: Failed to number_" + std::to_string(i + 1) + ".png" << std::endl;
        }        

        faceHappy = new sf::Texture();
        if (!faceHappy->loadFromFile("../files/images/face_happy.png"))
            std::cerr << "Error: Failed to face_happy.png" << std::endl;

        faceLose = new sf::Texture();
        if (!faceLose->loadFromFile("../files/images/face_lose.png"))
            std::cerr << "Error: Failed to face_lose.png" << std::endl;

        faceWin = new sf::Texture();
        if (!faceWin->loadFromFile("../files/images/face_win.png"))
            std::cerr << "Error: Failed to face_win.png" << std::endl;

        debug = new sf::Texture();
        if (!debug->loadFromFile("../files/images/debug.png"))
            std::cerr << "Error: Failed to debug.png" << std::endl;

        pause = new sf::Texture();
        if (!pause->loadFromFile("../files/images/pause.png"))
            std::cerr << "Error: Failed to pause.png" << std::endl;

        play = new sf::Texture();
        if (!play->loadFromFile("../files/images/play.png"))
            std::cerr << "Error: Failed to play.png" << std::endl;

        leaderBoardT = new sf::Texture();
        if (!leaderBoardT->loadFromFile("../files/images/leaderboard.png"))
            std::cerr << "Error: Failed to leaderboard.png" << std::endl;

        numStrip = new sf::Texture();
        if (!numStrip->loadFromFile("../files/images/digits.png"))
            std::cerr << "Error: Failed to digits.png" << std::endl;
    }

    /*
    int checkForMines(Tile*** board, Tile** neighbors, int centerR, int centerC, int n)
    {
        int numOfMines = 0;
        for (int i = 0; i < n; i++)
        {
            if (neighbors[i]->getMine())
                numOfMines++;
            else 
        }

        if (numOfMines > 0)
            return numOfMines;
        
        Tile** toBeTested = new Tile*[8];
        int* centerRs = new int[8];
        int* centerCs = new int[8];

        int ni = 0;
        for (int i = 0; i < 8; i++)
        {
            int tempR = centerR + neighborOffsets[1][i];
            int tempC = centerC + neighborOffsets[0][i];
            Tile* temp = board[ tempR ] [ tempC ];
            if (temp->getMine() && !temp->getState())
            {
                toBeTested[ni] = temp;
                centerRs[ni] = tempR;
                centerCs[ni] = tempC;
                ni++;
            }
        }
        board[centerR][centerC]->updateTexture(tileRevealed);
        board[centerR][centerC]->setState(1);
        checkForMines();

        delete[] toBeTested;
        delete[] centerRs;
        delete[] centerCs;
    }*/

    void checkRevealed()
    {
        revealed = 0;

        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (Board[r][c]->getState() == 1)
                {
                    revealed++;
                    std::cout << revealed << std::endl;
                }
            }
        }
    }

    void checkForMinesMark2(Tile*** board, int centerR, int centerC)
    {
        int numMines = 0;

        int* nextRs = new int[9];
        int* nextCs = new int[9];
        int nexts = 0;

        for (int i = 0; i < 8; i++)
        {
            int tempR = centerR + neighborOffsets[1][i];
            int tempC = centerC + neighborOffsets[0][i];
            if (tempR >= 0 && tempR < rows && tempC >= 0 && tempC < cols)
            {
                Tile* temp = board[tempR][tempC];
                if (temp->getState() != 1)
                {
                    if (temp->getMine())
                        numMines++;
                    else
                    {
                        if (numMines == 0 && !temp->getState())
                        {
                            nextRs[nexts] = tempR;
                            nextCs[nexts] = tempC;
                            nexts++;
                        }
                    }
                }
            }
        }

        if (numMines > 0)
        {
            board[centerR][centerC]->updateTexture(tileRevealed);
            board[centerR][centerC]->setState(1);
            board[centerR][centerC]->setNumTexture(numTextures[numMines - 1]);
            board[centerR][centerC]->setKnownSurrounding(numMines);
        }
        else
        {
            for (int i = 0; i < nexts; i++)
            {
                board[nextRs[i]][nextCs[i]]->updateTexture(tileRevealed);
                board[nextRs[i]][nextCs[i]]->setState(1);
                checkForMinesMark2(board, nextRs[i], nextCs[i]);
            }
        }

        delete[] nextRs;
        delete[] nextCs;
        return;
    }

    void genBoard()
    {
        isWon = false;
        showMines = false;
        inGame = true;
        paused = false;
        isLoss = false;
        flags = mines;
        Board = new Tile**[rows];

        int x = 0, y = 0;

        for (int r = 0; r < rows; r++) 
        {
            Board[r] = new Tile*[cols]; // Allocate each row
                
            int x = 0;
            for (int c = 0; c < cols; c++)
            {
                // std::cout << "x: " << x << ", y: " << y << std::endl;

                Board[r][c] = new Tile(x, y, 0, false, tileHidden, flag, mine);

                x += 32;
            }

            y += 32;
        }

        for (int i = 0; i < mines; i++)
        {
            int rMine = 0;
            int cMine = 0;

            while(true)
            {
                rMine = rand() % rows;
                cMine = rand() % cols;

                if (!Board[rMine][cMine]->getMine())
                {
                    Board[rMine][cMine]->setMine(true); 
                    break;
                }
            }
        }
    }

    void updateCounter(sf::Sprite* arr)
    {
        std::cout << flags << std::endl;

        int temp = abs(flags);
        for (int i = 2; i >= 0; i--)
        {
            arr[i].setTextureRect(sf::IntRect(21 * (temp % 10), 0, 21, 32));
            temp /= 10;
        }
    }

    void setText(sf::Text &text, float x, float y)
    {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
        textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    void captiveLeaderBoard(unsigned long long int time)
    {
        //========== FILE IO AND LEADERBOARD LOGIC ==========//
        std::ifstream file;

        leaderBoard = new std::string*[5];
        for(int i = 0; i < 5; i++)
        {
            leaderBoard[i] = new std::string[3];
        }

        file.open("../files/leaderboard.txt");

        for (int i = 0; i < 5; i++)
        {
            std::string line;
            std::getline(file, line);
            
            if (file.fail())
                break;

            std::cout << "loadingfile..." << std::endl;

            size_t colonI = line.find(":");

            leaderBoard[i][0] = line.substr(0, colonI);

            size_t commaI = line.find(",");
            
            leaderBoard[i][1] = line.substr(colonI + 1, commaI - (colonI + 1));
            leaderBoard[i][2] = line.substr(commaI + 2);
        }

        std::cout << "loaded file!" << std::endl;

        file.close();

        int updatedIndex = -1;

        for (int i = 0; i < 5; i++)
        {
            unsigned int seconds = time / 1000;
            if (isWon && (seconds) < (std::stoi(leaderBoard[i][0]) * 60) + std::stoi(leaderBoard[i][1]))
            {
                std::string* stat = new std::string[3];
                unsigned int minutes = seconds / 60; 
                seconds %= 60;
                stat[0] = std::to_string(minutes);
                stat[1] = std::to_string(seconds);
                stat[2] = usernm;

                std::swap(stat, leaderBoard[4]);
                
                for (int j = 3; j >= i; j--)
                {
                    std::swap(leaderBoard[j + 1], leaderBoard[j]);
                }

                updatedIndex = i;

                delete[] stat;

                break;
            }
        }

        if (updatedIndex > -1)
        {
            std::ofstream oFile("../files/leaderboard.txt", std::ios::out | std::ios::trunc);
            if (!oFile.is_open())
            {
                std::cerr << "Error: Could not open leaderboard.txt!" << std::endl;
            }

            for (int i = 0; i < 5; i++)
            {
                oFile << leaderBoard[i][0] << ":" << leaderBoard[i][1] << ", " << leaderBoard[i][2] << std::endl;
            }

            oFile.close();
        }

        //========== GRAPHICS ==========//
        int widthLB = 416, heightLB = 320;

        sf::RenderWindow window(sf::VideoMode(widthLB, heightLB), "Minesweeper", sf::Style::Close);

        sf::Font font;
        if (!font.loadFromFile("../files/font.ttf"))
        {
            std::cerr << "Error while loading font!" << std::endl;
        }

        sf::Text title("LEADERBOARD", font, 20);
        title.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(title, widthLB / 2, (heightLB / 2) - 120);

        std::string text = "";

        for (int i = 0; i < 5; i++)
        {
            text = ( text + 
                std::to_string(i + 1) + ".\t" + 
                leaderBoard[i][0] + ":" + leaderBoard[i][1] + "\t" +
                leaderBoard[i][2] + ((i == updatedIndex) ? "*" : "") + "\n\n"
            );
        }

        sf::Text content(text, font, 18);
        content.setStyle(sf::Text::Bold);
        setText(content, widthLB / 2, (heightLB / 2) + 20);

        while(window.isOpen()) 
        {
            //========== USER INTERACTION ==========//
            sf::Event event;
            
            while(window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed) {
                    inLB = false;
                    window.close();
                }
            }

            //========== FRAME RENDERING ==========//
            window.clear(sf::Color::Blue);
            window.draw(title);
            window.draw(content);
            window.display();
        }
    }

    void hideBoard(sf::RenderWindow* w)
    {
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                Board[r][c]->updateTexture(tileHidden);
                w->draw(Board[r][c]->getSprite());
            }
        }
        w->display();
    }

    void updateBoard(sf::RenderWindow* w)
    {
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (Board[r][c]->getState() == 1)
                    Board[r][c]->updateTexture(tileRevealed);
                w->draw(Board[r][c]->getSprite());
            }
        }
        w->display();
    }

    int mainGame() 
    {
        sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
        
        //========== TIME MANAGEMENT ==========//
        sf::Clock clock;
        unsigned long long int milliSecondsPassed = 0;
        unsigned long long int milliSecondsPaused = 0;

        //========== BOARD GENERATION ==========//
        genBoard();

        //========== UI ==========//
        sf::Sprite face;
        face.setPosition(((cols / 2) * 32 - 32), (32 * (rows + 0.5)));
        face.setTexture(*faceHappy);

        sf::Sprite debugButton;
        debugButton.setPosition((cols * 32 - 304), (32 * (rows + 0.5)));
        debugButton.setTexture(*debug);

        sf::Sprite pauseButton;
        pauseButton.setPosition((cols * 32 - 240), (32 * (rows + 0.5)));
        pauseButton.setTexture(*pause);

        sf::Sprite lbButton;
        lbButton.setPosition((cols * 32 - 176), (32 * (rows + 0.5)));
        lbButton.setTexture(*leaderBoardT);

        sf::Sprite* minesLeftDigits = new sf::Sprite[3];
        for (int i = 0; i < 3; i++)
        {
            minesLeftDigits[i].setPosition(33 + (i * 21), (32 * (rows + 0.5) + 16));
            minesLeftDigits[i].setTexture(*numStrip);
            minesLeftDigits[i].setTextureRect(sf::IntRect(0, 0, 21, 32));
        }
        sf::Sprite negative;
        negative.setPosition(12, (32 * (rows + 0.5) + 16));
        negative.setTexture(*numStrip);
        negative.setTextureRect(sf::IntRect(210, 0, 21, 32));

        updateCounter(minesLeftDigits);

        // minutes: Start drawing two digits from ((number of columns × 32) − 97, 32 × (number of rows + 0.5) + 16
        // Start drawing two digits from ((number of columns × 32) − 54, 32 × (number of rows + 0.5) + 16)All digits are 21px wide, draw the next digit sprite after 21px

        sf::Sprite* timer = new sf::Sprite[4];
        for (int i = 0; i < 2; i++)
        {
            timer[i].setPosition((cols * 32) - 97 + (i * 21), (32 * (rows + 0.5) + 16));
            timer[i].setTexture(*numStrip);
            timer[i].setTextureRect(sf::IntRect(0, 0, 21, 32));
        }
        for (int i = 2; i < 4; i++)
        {
            timer[i].setPosition((cols * 32) - 54 + ((i - 2) * 21), (32 * (rows + 0.5) + 16));
            timer[i].setTexture(*numStrip);
            timer[i].setTextureRect(sf::IntRect(0, 0, 21, 32));
        }

        while(window.isOpen()) 
        {
            //========== CLOCK =========//
            unsigned int seconds = static_cast<unsigned int>(milliSecondsPassed / 1000);
            unsigned int minutes = seconds / 60;
            seconds %= 60;

            // std::cout << "minutes: " << minutes << " seconds: " << seconds << std::endl;

            timer[0].setTextureRect(sf::IntRect((minutes / 10) * 21, 0, 21, 32));
            timer[1].setTextureRect(sf::IntRect((minutes % 10) * 21, 0, 21, 32));
            timer[2].setTextureRect(sf::IntRect((seconds / 10) * 21, 0, 21, 32));
            timer[3].setTextureRect(sf::IntRect((seconds % 10) * 21, 0, 21, 32));

            //========== LOGIC ==========//
            if ((cols * rows) - mines == revealed)
            {
                if (face.getTexture() != faceWin)
                    face.setTexture(*faceWin);
                inGame = false;

                isWon = true;
                // won
            }

            if (isLoss)
            {
                    if (face.getTexture() != faceLose)
                        face.setTexture(*faceLose);
            }
            else if (inGame)
            {
                if (face.getTexture() != faceHappy)
                        face.setTexture(*faceHappy);
            }

            if (!inGame)
                milliSecondsPaused = clock.getElapsedTime().asMilliseconds() - milliSecondsPassed;
            else
                milliSecondsPassed = clock.getElapsedTime().asMilliseconds() - milliSecondsPaused;
            
            //========== USER INTERACTION ==========//
            sf::Event event;
            
            while(window.pollEvent(event)) 
            {
                if (!inLB)
                {
                    if (event.type == sf::Event::MouseButtonPressed) 
                    {
                        int row = 0;
                        int col = 0;
                
                        if (event.mouseButton.button == sf::Mouse::Left) 
                        {
                            if (inGame)
                            {
                                row = ((event.mouseButton.y) / 32);
                                col = (event.mouseButton.x / 32);

                                std::cout << "Left mouse pressed at coords ("<< col << ", " << row << ")" << std::endl;

                                if (row < rows && row >= 0 && col < cols && col >= 0)
                                {
                                    if (!Board[row][col]->getState())
                                    {
                                        Board[row][col]->updateTexture(tileRevealed);
                                        Board[row][col]->setState(1);

                                        if (Board[row][col]->getMine())
                                        {
                                            inGame = false;
                                            showMines = true;
                                            isLoss = true;
                                        }
                                        else 
                                        {
                                            // std::cout << "Enter" << std::endl;
                                            checkForMinesMark2(Board, row, col);    
                                        }
                                    }
                                }

                                checkRevealed();
                            }

                            int mx = event.mouseButton.x, my = event.mouseButton.y;

                            int centerX = ((cols / 2) * 32 - 32);
                            int centerY = (32 * (rows + 0.5));
                            if (mx >= centerX && mx <= centerX + 64 && my >= centerY && my <= centerY + 64)
                            {
                                std::cout << "reset" << std::endl;

                                for (int r = 0; r < rows; r++)
                                {
                                    for (int c = 0; c < cols; c++)
                                    {
                                        delete Board[r][c];
                                    }
                                    delete[] Board[r];
                                }
                                delete[] Board;
                                inGame = true;
                                revealed = 0;
                                genBoard();
                                milliSecondsPassed = 0;
                                paused = false;
                                milliSecondsPaused = 0;
                                clock.restart();
                                updateCounter(minesLeftDigits);
                                pauseButton.setTexture(*pause);
                            }

                            
                            int debugX = (cols * 32 - 304), debugY = (32 * (rows + 0.5));
                            if (mx >= debugX && mx <= debugX + 64 && my >= debugY && my <= debugY + 64)
                            {
                                std::cout << "debug" << std::endl;
                                showMines = !showMines;
                            }

                            int pauseX = (cols * 32 - 240), pauseY = (32 * (rows + 0.5));
                            if (mx >= pauseX && mx <= pauseX + 64 && my >= pauseY && my <= pauseY + 64 && !isLoss && !isWon)
                            {
                                std::cout << "pause" << std::endl;
                                inGame = !inGame;

                                if (!inGame)
                                {
                                    paused = true;
                                    hideBoard(&window);
                                    pauseButton.setTexture(*play);
                                }
                                else
                                {
                                    paused = false;
                                    updateBoard(&window);
                                    pauseButton.setTexture(*pause);
                                }

                            }

                            int lbX = (cols * 32 - 176), lbY = (32 * (rows + 0.5));
                            if (mx >= lbX && mx <= lbX + 64 && my >= lbY && my <= lbY + 64)
                            {
                                std::cout << "leaderboard" << std::endl;
                                inGame = false;
                                inLB = true;
                                pauseButton.setTexture(*play);
                                hideBoard(&window);
                                window.draw(pauseButton);
                                window.display();
                                paused = true;
                                captiveLeaderBoard(milliSecondsPassed);
                                sf::Event tempE;
                                while (window.pollEvent(tempE)) {  }
                            }

                        }   
                        else if (event.mouseButton.button == sf::Mouse::Right) 
                        {
                            if (inGame)
                            {
                                row = ((event.mouseButton.y) / 32);
                                col = (event.mouseButton.x / 32);

                                std::cout << "Right mouse pressed at coords ("<< col << ", " << row << ")" << std::endl;

                                if (row < rows && row >= 0 && col < cols && col >= 0)
                                {
                                    if (Board[row][col]->getState() == 2)
                                    {
                                        Board[row][col]->setState(0);
                                        flags++;
                                        updateCounter(minesLeftDigits);
                                    }
                                    else if (Board[row][col]->getState() == 0)
                                    {
                                        Board[row][col]->setState(2);
                                        flags--;
                                        updateCounter(minesLeftDigits);
                                    }
                                }
                            }
                        }
                    }

                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                }
            }

            //========== FRAME RENDERING ==========//
            window.clear(sf::Color::White);
            
            for (int r = 0; r < rows; r++)
            {
                for (int c = 0; c < cols; c++)
                {
                        window.draw(Board[r][c]->getSprite());
                        if ((Board[r][c]->getState() == 2 && !paused) || (isWon && !Board[r][c]->getState()))
                        {
                            window.draw(Board[r][c]->getFlag());
                        }
                        if (Board[r][c]->getKnownSurrounding() && !paused)
                        {
                        window.draw(Board[r][c]->getNumSprite());
                        }
                        if (showMines && Board[r][c]->getMine())
                        {
                            window.draw(Board[r][c]->getMineSprite());
                        }
                }
            }

            window.draw(face);
            window.draw(debugButton);
            window.draw(pauseButton);
            window.draw(lbButton);

            for (int i = 0; i < 3; i++)
            {
                window.draw(minesLeftDigits[i]);
            }
            if (flags < 0)
            {
                window.draw(negative);
            }

            for (int i = 0; i < 4; i++)
            {
                window.draw(timer[i]);
            }

            window.display();
        }
        return 0;
    }
};
