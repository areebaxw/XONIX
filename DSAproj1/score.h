#ifndef SCORE_H
#define SCORE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
using namespace std;

class Stack; // Forward declaration for the Stack class

class ScoreManager {
private:
    // Existing private members
    int player1Score;
    int player2Score;
    bool isSinglePlayer;
    int player1BonusCount;
    int player2BonusCount;
    int player1ContinuousTiles;
    int player2ContinuousTiles;
    bool player1BonusApplied;
    bool player2BonusApplied;
    int player1PowerUpCount;
    int player2PowerUpCount;
    bool player1PowerUpUsed;
    bool player2PowerUpUsed;
    unsigned int player1PowerUpStartTime;
    unsigned int player2PowerUpStartTime;
    int player1LastPowerUpScore;
    int player2LastPowerUpScore;




    // Stacks for tracking game state
    Stack* player1TileStack;
    Stack* player2TileStack;
    Stack* player1PowerUpStack;
    Stack* player2PowerUpStack;

    // Graphics-related members
    sf::Font font;
    sf::Text player1ScoreText;
    sf::Text player2ScoreText;

    // New private helper methods
    std::string sanitizeFilename(const std::string& username);
    std::string getCurrentDateTime();
    int determinePowerUpLevel(int score);

public:
    void setPlayerUsernames(const std::string& p1Username, const std::string& p2Username);
    std::string player1Username;
    std::string player2Username;
    // Constructor and Destructor
    ScoreManager();
    ~ScoreManager();

    // New methods for saving and loading progress
    void saveUserProgress(const std::string& username);
    bool loadUserProgress(const std::string& username);

    // Existing public methods
    bool initializeFont(const std::string& fontPath);
    void resetScores();
    void updateScore(int player1NewTiles, int player2NewTiles);
    bool canActivatePowerUp(bool isPlayer1) const;
    bool tryActivatePowerUp(bool isPlayer1);
    void updatePowerUpTimer(bool isPlayer1);
    void resetPowerUpState(bool isPlayer1);
    int getPowerUpCount(bool isPlayer1) const;
    bool isPowerUpUsed(bool isPlayer1) const;
    int getPlayer1Score() const;
    int getPlayer2Score() const;
    int getPlayer1BonusCount() const;
    int getPlayer2BonusCount() const;
    void drawScores(sf::RenderWindow& window, bool isSinglePlayerMode);
    void setGameMode(bool singlePlayer);
    static int countFilledTiles(int grid[35][55]);
    void recordGameResults(const string& scoreFilePath,
        const string& powerUpFilePath,
        const string& player1Username,
        const string& player2Username,
        bool isSinglePlayer);


    
};

#endif // SCORE_H