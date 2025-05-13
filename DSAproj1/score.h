#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
using namespace std;
using namespace sf;

class Stack; 


class ScoreManager {
public:
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

    // ---------------Stacks for tracking game state---------------//
    Stack* player1TileStack;
    Stack* player2TileStack;
    Stack* player1PowerUpStack;
    Stack* player2PowerUpStack;

    // ---------------TEXT AND FONT---------------//
    Font font;
    Text player1ScoreText;
    Text player2ScoreText;

    // ---------------Private helper methods---------------//
    string helper(const string& username);
    string getCurrentDateTime();
    int determinePowerUpLevel(int score);


    // --------------For managing player usernames---------------//
    void setPlayerUsernames(const string& p1Username, const string& p2Username);
    string player1Username;
    string player2Username;

    // ---------------Constructor and Destructor---------------//
    ScoreManager();
    ~ScoreManager();



    // ---------------Methods for saving and loading progress---------------//
    void saveUserProgress(const string& username);
    bool loadUserProgress(const string& username);


    // ---------------For score updates and power-ups---------------//
    bool initializeFont(const string& fontPath);
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


    // ---------------Methods for drawing scores and game mode settings---------------//
    void drawScores(RenderWindow& window, bool isSinglePlayerMode);
    void setGameMode(bool singlePlayer);

    // ---------------Static method for counting filled tiles in a grid---------------//
    static int countFilledTiles(int grid[35][55]);

    // ---------------Method for recording game results to files---------------//
    void recordGameResults(const string& scoreFilePath,const string& powerUpFilePath,const string& player1Username,const string& player2Username,bool isSinglePlayer);
};

