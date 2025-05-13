#include "score.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <SFML/Graphics.hpp>

using namespace std;

class Stack
{
private:
    int data[1000];
    int top_index;

public:
    Stack() : top_index(-1)
    {}

    void push(int val) {
        if (top_index < 99) {
            data[++top_index] = val;
        }
    }

    void pop() {
        if (!empty()) {
            top_index--;
        }
    }

    int top() const {
        return empty() ? -1 : data[top_index];
    }

    bool empty() const {
        return top_index == -1;
    }

    int size() const {
        return top_index + 1;
    }

    void clear() {
        top_index = -1;
    }
};

ScoreManager::ScoreManager() :
    player1Score(0),
    player2Score(0),
    isSinglePlayer(true),
    player1BonusCount(0),
    player2BonusCount(0),
    player1ContinuousTiles(0),
    player2ContinuousTiles(0),
    player1BonusApplied(false),
    player2BonusApplied(false),
    player1PowerUpCount(0),
    player2PowerUpCount(0),
    player1PowerUpUsed(false),
    player2PowerUpUsed(false),
    player1PowerUpStartTime(0),
    player2PowerUpStartTime(0),
    player1LastPowerUpScore(0),
    player2LastPowerUpScore(0)
{
    
    player1ScoreText.setCharacterSize(24);
    player1ScoreText.setFillColor(sf::Color::White);
    player1ScoreText.setPosition(10.f, 10.f);

    player2ScoreText.setCharacterSize(24);
    player2ScoreText.setFillColor(sf::Color::White);
    player2ScoreText.setPosition(55.f * 18.f - 150.f, 10.f);

    // Initialize stacks
    player1TileStack = new Stack();
    player2TileStack = new Stack();
    player1PowerUpStack = new Stack();
    player2PowerUpStack = new Stack();
}

ScoreManager::~ScoreManager() {
  
    delete player1TileStack;
    delete player2TileStack;
    delete player1PowerUpStack;
    delete player2PowerUpStack;
}

bool ScoreManager::initializeFont(const string& fontPath) {
 
    if (!font.loadFromFile(fontPath)) {
        cout << "Error loading font: " << fontPath << endl;
        return false;
    }

 
    player1ScoreText.setFont(font);
    player2ScoreText.setFont(font);

    return true;
}

void ScoreManager::resetScores() {
    player1Score = 0;
    player2Score = 0;
    player1BonusCount = 0;
    player2BonusCount = 0;
    player1ContinuousTiles = 0;
    player2ContinuousTiles = 0;
    player1BonusApplied = false;
    player2BonusApplied = false;
    player1PowerUpCount = 0;
    player2PowerUpCount = 0;
    player1PowerUpUsed = false;
    player2PowerUpUsed = false;
    player1PowerUpStartTime = 0;
    player2PowerUpStartTime = 0;
    player1LastPowerUpScore = 0;
    player2LastPowerUpScore = 0;


    player1TileStack->clear();
    player2TileStack->clear();
    player1PowerUpStack->clear();
    player2PowerUpStack->clear();
}

void ScoreManager::updateScore(int player1NewTiles, int player2NewTiles)
{
   



    //-------------------------------------BONUS LOGIC-------------------------------------------------//
    int player1Multiplier = 1;
    bool bonusApplied = false;

    if (player1NewTiles > 0) {
        if (player1BonusCount < 3 && player1NewTiles > 10) {
            player1Multiplier = 2;
            bonusApplied = true;
        }
        else if (player1BonusCount >= 3 && player1BonusCount < 5 && player1NewTiles > 5) {
            player1Multiplier = 2;
            bonusApplied = true;
        }
        else if (player1BonusCount >= 5 && player1NewTiles > 5) {
            player1Multiplier = 4;
            bonusApplied = true;
        }

        if (bonusApplied && player1BonusCount < 5) {
            player1BonusCount++;
        }

        player1Score += player1NewTiles * player1Multiplier;
    }

  
    int player1ScoreIncrease = player1NewTiles * player1Multiplier;
    player1Score += player1ScoreIncrease;



    // -----------------------------------------POWERUP LOGIC---------------------------------------//

    if (player1Score >= 100 && player1LastPowerUpScore < 100) {
        player1PowerUpStack->push(1);
        player1LastPowerUpScore = 100;
    }
    else if (player1Score >= 70 && player1LastPowerUpScore < 70) {
        if (player1PowerUpStack->size() < 2) {
            player1PowerUpStack->push(1);
        }
        player1LastPowerUpScore = 70;
    }
    else if (player1Score >= 50 && player1LastPowerUpScore < 50) {
        player1PowerUpStack->push(1);
        player1LastPowerUpScore = 50;
    }

    //----------------------------------------- Multiplayer logic-------------------------------------------//
    if (!isSinglePlayer) {
        int player2Multiplier = 1;

        if (player2NewTiles > 0) {
            player2TileStack->push(player2NewTiles);

            int continuousTiles = 0;
            Stack* tempStack = new Stack();

            while (!player2TileStack->empty()) {
                int tile = player2TileStack->top();
                continuousTiles += tile;
                tempStack->push(tile);
                player2TileStack->pop();
            }

          
            while (!tempStack->empty()) {
                player2TileStack->push(tempStack->top());
                tempStack->pop();
            }
            delete tempStack;

            //-------------------------------------- Bonus logic for Player 2------------------------------------------//
            if (player2BonusCount >= 5) {
                player2Multiplier = (player2NewTiles > 5) ? 4 : 1;
            }
            else if (player2BonusCount >= 3) {
                if (player2NewTiles > 5) {
                    player2Multiplier = 2;

                    if (!player2BonusApplied) {
                        player2BonusCount++;
                        player2BonusApplied = true;
                    }
                }
            }
            else {
                if (continuousTiles > 10 && !player2BonusApplied) {
                    player2BonusCount++;
                    player2BonusApplied = true;
                }
            }
        }
        else {
            player2TileStack->clear();
            player2BonusApplied = false;
        }

        //--------------------------- score for player 2---------------------------------//
        int player2ScoreIncrease = max(0, player2NewTiles) * player2Multiplier;
        player2Score += player2ScoreIncrease;

		//---------------------------------- POWERUP LOGIC FOR PLAYER2-------------------------------------//
        if (player2Score >= 100 && player2LastPowerUpScore < 100) {
            player2PowerUpStack->push(1);
            player2LastPowerUpScore = 100;
        }

        else if (player2Score >= 70 && player2LastPowerUpScore < 70) {
            if (player2PowerUpStack->size() < 2) {
                player2PowerUpStack->push(1);
            }
            player2LastPowerUpScore = 70;
        }
        else if (player2Score >= 50 && player2LastPowerUpScore < 50) {
            player2PowerUpStack->push(1);
            player2LastPowerUpScore = 50;
        }
    }
}


bool ScoreManager::canActivatePowerUp(bool isPlayer1) const {
    if (isPlayer1) {
        return !player1PowerUpStack->empty();
    }
    else {
        return (!isSinglePlayer && !player2PowerUpStack->empty());
    }
}

bool ScoreManager::tryActivatePowerUp(bool isPlayer1) {
    if (isPlayer1) {

		//------------------ Check if power-up is available for player 1 -----------------//
        if (!player1PowerUpStack->empty()) {
            player1PowerUpStack->pop();
            player1PowerUpUsed = true;
            player1PowerUpStartTime = static_cast<unsigned int>(time(NULL));
            return true;
        }
    }
	//------------------ Check if power-up is available for player 2 -----------------//
    else if (!isSinglePlayer) {
        if (!player2PowerUpStack->empty()) {
            player2PowerUpStack->pop();
            player2PowerUpUsed = true;
            player2PowerUpStartTime = static_cast<unsigned int>(time(NULL));
            return true;
        }
    }
    return false;
}

void ScoreManager::updatePowerUpTimer(bool isPlayer1) {


    unsigned int currentTime = static_cast<unsigned int>(time(NULL));

    
	//------------------ Reset power-up state after 3 seconds if not used -----------------//
    if (isPlayer1) {
        if (player1PowerUpUsed &&
            currentTime - player1PowerUpStartTime >= 3) {
            player1PowerUpUsed = false;
        }
    }
	//------------------ Reset power-up state after 3 seconds if not used -----------------//
    else {
        if (player2PowerUpUsed &&
            currentTime - player2PowerUpStartTime >= 3) {
            player2PowerUpUsed = false;
        }
    }
}

void ScoreManager::resetPowerUpState(bool isPlayer1) {

	//------------------ Reset power-up state for player 1-----------------//
    if (isPlayer1) 
    {
        player1PowerUpUsed = false;
        player1PowerUpStartTime = 0;
    }
	//---------------- Reset power-up state for player 2-----------------//
    else {
        player2PowerUpUsed = false;
        player2PowerUpStartTime = 0;
    }
}

int ScoreManager::getPowerUpCount(bool isPlayer1) const {
    if (isPlayer1) {
        return player1PowerUpStack->size();
    }
    else {
        return player2PowerUpStack->size();
    }
}

bool ScoreManager::isPowerUpUsed(bool isPlayer1) const {
    if (isPlayer1) {
        return player1PowerUpUsed;
    }
    else {
        return player2PowerUpUsed;
    }
}

int ScoreManager::getPlayer1Score() const {
    return player1Score;
}

int ScoreManager::getPlayer2Score() const {
    return player2Score;
}

int ScoreManager::getPlayer1BonusCount() const {
    return player1BonusCount;
}

int ScoreManager::getPlayer2BonusCount() const {
    return player2BonusCount;
}
void ScoreManager::setPlayerUsernames(const string& p1Username, const string& p2Username) {
    player1Username = p1Username;
    player2Username = p2Username;
}

void ScoreManager::drawScores(sf::RenderWindow& window, bool isSinglePlayerMode) {

    isSinglePlayer = isSinglePlayerMode;


    

    if (isSinglePlayerMode) {
        string scoreText = "Score: " + to_string(player1Score);

      
    /*    if (player1BonusCount > 0) {
            if (player1BonusCount < 3) {
                scoreText += " (×2 when >10 tiles)";
            }
            else if (player1BonusCount < 5) {
                scoreText += " (×2 when >5 tiles)";
            }
            else {
                scoreText += " (×4 when >5 tiles)";
            }
            scoreText += " [Bonus Lvl " + to_string(player1BonusCount) + "]";
        }*/

       
        scoreText += " [Power-Ups: " + to_string(player1PowerUpStack->size()) + "]";

       
        if (player1PowerUpUsed) {
            scoreText += " (Active)";
        }

        player1ScoreText.setString(scoreText);

        player1ScoreText.setCharacterSize(27);
        player1ScoreText.setPosition(340, 26);

        window.draw(player1ScoreText);





    }


    else {
        /*      string player1ScoreStr = "Player 1: " + to_string(player1Score);
              string player2ScoreStr = "Player 2: " + to_string(player2Score);*/

        string player1ScoreStr = player1Username + ": " + to_string(player1Score);
        string player2ScoreStr = player2Username + ": " + to_string(player2Score);




        // Add bonus level information
        if (player1BonusCount > 0) {
            player1ScoreStr += " (Bonus Lvl " + to_string(player1BonusCount) + ")";
        }
        if (player2BonusCount > 0) {
            player2ScoreStr += " (Bonus Lvl " + to_string(player2BonusCount) + ")";
        }

        // Add power-up count details
        player1ScoreStr += " [Power-Ups: " + to_string(player1PowerUpStack->size()) + "]";
        player2ScoreStr += " [Power-Ups: " + to_string(player2PowerUpStack->size()) + "]";

        // Show if power-ups are in use
        if (player1PowerUpUsed)
        {
            player1ScoreStr += " (Active)";
        }
        if (player2PowerUpUsed) {
            player2ScoreStr += " (Active)";
        }

        player1ScoreText.setString(player1ScoreStr);
        player2ScoreText.setString(player2ScoreStr);


        player1ScoreText.setPosition(50, 20);
        player2ScoreText.setPosition(600, 20);


        window.draw(player1ScoreText);
        window.draw(player2ScoreText);
    }
}

void ScoreManager::setGameMode(bool singlePlayer) {
    isSinglePlayer = singlePlayer;
}

int ScoreManager::countFilledTiles(int grid[35][55]) {
    int filledTiles = 0;
    const int M = 35;
    const int N = 55;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (grid[i][j] == 2)
                filledTiles++;
    return filledTiles;
}

string ScoreManager::helper(const string& username) {


    string updated = username;
   

    for (char& c : updated) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' ||
            c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    return updated;
}

string ScoreManager::getCurrentDateTime() {
 
    static int currentYear = 2024;
    static int currentMonth = 5;
    static int currentDay = 11;
    static int currentHour = 12;
    static int currentMinute = 30;
    static int currentSecond = 0;


    currentSecond++;

    // Handle second overflow
    if (currentSecond >= 60) {
        currentSecond = 0;
        currentMinute++;

        // Handle minute overflow
        if (currentMinute >= 60) {
            currentMinute = 0;
            currentHour++;

            // Handle hour overflow
            if (currentHour >= 24) {
                currentHour = 0;
                currentDay++;

                // Simple month day count (not accounting for leap years)
                int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

                // Handle day overflow
                if (currentDay > daysInMonth[currentMonth]) {
                    currentDay = 1;
                    currentMonth++;

                    // Handle month overflow
                    if (currentMonth > 12) {
                        currentMonth = 1;
                        currentYear++;
                    }
                }
            }
        }
    }

    // Manual string formatting with leading zeros
    string timestamp =
        (currentYear < 10 ? "0" : "") + to_string(currentYear) + "-" +
        (currentMonth < 10 ? "0" : "") + to_string(currentMonth) + "-" +
        (currentDay < 10 ? "0" : "") + to_string(currentDay) + " " +
        (currentHour < 10 ? "0" : "") + to_string(currentHour) + ":" +
        (currentMinute < 10 ? "0" : "") + to_string(currentMinute) + ":" +
        (currentSecond < 10 ? "0" : "") + to_string(currentSecond);

    return timestamp;
}


void ScoreManager::saveUserProgress(const string& username) {
    
    string safeUsername =helper(username);

   
    string progressFilePath = "user_progress_" + safeUsername + ".txt";


    

    ofstream progressFile(progressFilePath, ios::app);
    if (progressFile.is_open()) {
        string currentTime = getCurrentDateTime();

        
        progressFile << "--- Game Session on " << currentTime << " ---\n";
        if (isSinglePlayer) {
            progressFile << "Final Score: " << player1Score << "\n";
            progressFile << "Bonus Level: " << player1BonusCount << "\n";
            progressFile << "Power-Ups: " << player1PowerUpStack->size() << "\n\n";
        }
        else {
            progressFile << "Player 1 Score: " << player1Score
                << " (Bonus Level: " << player1BonusCount
                << ", Power-Ups: " << player1PowerUpStack->size() << ")\n";
            progressFile << "Player 2 Score: " << player2Score
                << " (Bonus Level: " << player2BonusCount
                << ", Power-Ups: " << player2PowerUpStack->size() << ")\n\n";
        }
        progressFile.close();
    }
}

bool ScoreManager::loadUserProgress(const string& username) 
{
    
    string safeUsername =helper(username);

   
    string progressFilePath = "user_progress_" + safeUsername + ".txt";

    ifstream progressFile(progressFilePath);
    if (progressFile.is_open()) {
    
        progressFile.close();
        return true;
    }
    return false;
}

void ScoreManager::recordGameResults(const string& scoreFilePath, const string& powerUpFilePath,const string& player1Username,  const string& player2Username,
bool isSinglePlayer)
{

    string safePlayer1Username =helper(player1Username);
    string safePlayer2Username =helper(player2Username);

  
    string timestamp = getCurrentDateTime();


    ofstream scoreFile(scoreFilePath, ios::app);

    if (scoreFile.is_open()) {
        scoreFile << "--- Game Score for " << safePlayer1Username << endl;

        if (isSinglePlayer) {
            scoreFile << "Game Mode: Single Player\n";
            scoreFile << "Game Score for " << player1Username << "\n";
            scoreFile << "Final Score: " << player1Score << "\n";
            scoreFile << "Bonus Level: " << player1BonusCount << "\n";
            scoreFile << "Power-Ups Collected: " << player1PowerUpStack->size() << "\n";
            scoreFile << "Power-Ups Used: " << (player1PowerUpUsed ? "Yes" : "No") << "\n\n";
        }
        else {
            scoreFile << "Game Mode: Multiplayer\n";
            scoreFile << "Game Score for " << player1Username << "\n";
            scoreFile << "Player 1 Score: " << player1Score
                << " (Bonus Level: " << player1BonusCount
                << ", Power-Ups: " << player1PowerUpStack->size()
                << ", Used: " << (player1PowerUpUsed ? "Yes" : "No") << ")\n";

            

            
            
            scoreFile << "Game Score for " << player2Username << "\n";

            scoreFile << "Player 2 Score: " << player2Score

                << " (Bonus Level: " << player2BonusCount

                << ", Power-Ups: " << player2PowerUpStack->size()

                << ", Used: " << (player2PowerUpUsed ? "Yes" : "No") << ")\n\n";
        }
        scoreFile.close();
    }
    else {
        cout << "Unable to open score file: " << scoreFilePath << endl;
    }

    saveUserProgress(player1Username);


	//------------------- Record power-up usage -----------------//

    ofstream powerUpFile(powerUpFilePath, ios::app);
    if (powerUpFile.is_open()) {
        powerUpFile << "--- Power-Up Tracking for " << safePlayer1Username
            << " (" << timestamp << ") ---\n";


        
        if (isSinglePlayer) {
            powerUpFile << "Game Mode: Single Player\n";
            powerUpFile << "Total Power-Ups: " << player1PowerUpStack->size() << "\n";
            powerUpFile << "Current Power-Up Status: "
                << (player1PowerUpUsed ? "Active" : "Inactive") << "\n\n";
        }
        else {
            powerUpFile << "Game Mode: Multiplayer\n";

            powerUpFile << "Player 1 - Total Power-Ups: " << player1PowerUpStack->size()

                << " (Status: " << (player1PowerUpUsed ? "Active" : "Inactive") << ")\n";

            powerUpFile << "Player 2 - Total Power-Ups: " << player2PowerUpStack->size()

                << " (Status: " << (player2PowerUpUsed ? "Active" : "Inactive") << ")\n\n";
        }
        powerUpFile.close();
    }
    else {
        cout << "Unable to open power-up file: " << powerUpFilePath << endl;
    }
}