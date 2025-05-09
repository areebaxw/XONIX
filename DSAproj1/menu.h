#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "level.h"
#include "auth.h"
#include "leaderboard.h"
#include "matchmaking.h"
#include "friendlist.h"
#include "playerhash.h"
#include "theme.h"
#include "Sprite.h"
#include "player.h"
using namespace sf;
using namespace std;

class Menu {
private:
    RenderWindow& window;
    Font font;

    Leaderboard leaderboard;

    // Main menu text
    Text startText;
    Text selectLevelText;
    Text leaderboardText;
    Text profileText;
	MySprite bgSprite;
    // End menu text
    Text endText;
    Text finalScoreText;

    Level level;  // Reference to level class
    MySprite bgMode;

    void initMainMenu();
    void initEndMenu();

public:
    // Matchmaking and game state variables
    bool isMatchmakingEnabled;
    bool showingMatchmakingQueue;

    std::string matchedPlayer1;  // First player's username (single declaration)
    std::string matchedPlayer2;  // Second player's username (single declaration)
    Matchmaking matchmaker;

    // Player usernames
    std::string player1Username;
    std::string player2Username;

    // Player profile
    PlayerProfile* currentProfile;
    void handleModeSelection();

    // Level and game state
    int currentLevel;
    bool showingMainMenu = true;
    bool showingModeSelection = false;
    bool showingLevelSelection = false;
    bool gameStarted = false;
    bool isSinglePlayerSelected = true;

    // Set current level
    void setCurrentLevel(int level) {
        currentLevel = level;
    }
    // ThemeInventory themeInventory;
    sf::Text themeSelectionText;

    // Menu display functions
    void displayLevelSelection();
    void handleLevelSelection();
    void displayMainMenu();
    void handleMainMenu();
    void selectMode();

    void selectLevel();

    // Leaderboard methods
    void setLeaderboardDisplayMode(int mode);
    void showLeaderboard();

    // Game management methods
    void displayEndMenu(int score, int highScore);
    void handleEndMenu();
    void showProfile();
    void showMatchmakingQueue();
    bool startMatchmaking();
    void handleMatchmakingQueue();

    // Constructor and Destructor
    Menu(RenderWindow& window, const std::string& username, Authentication& authRef);
    ~Menu();

    // Game control methods
    void startGame();
    void restartGame();
    bool isPlayerInQueue(const std::string& username);
    void exitGame();
    int getCurrentLevel();





    // Score tracking
    int player1Score = 0;
    int player2Score = 0;
    bool restartRequested = false;










    // Queue status text
    Text queueStatusText;


    void setSecondPlayerName(const std::string& playerName);
    std::string secondPlayerName;

    std::string promptForSecondPlayerName();


    std::string getValidPlayerName(const std::string& name, const std::string& defaultName);

    void syncPlayerNames();



    //friendlist 
    // 
    Text friendListText;                // Text for Friends List menu option

    // New methods for friend list management
    void displayFriendList();           // Display friend list screen
    void handleFriendListInteractions();// Handle user interactions in friend list
    void sendFriendRequest();           // Open window to send friend request
    void displayFriendRequests();
    void showFriendList();
    bool showingFriendList = false;
    Authentication& auth;
    void sendFriendRequestToUser(const std::string& sender, const std::string& receiver);



    void acceptFriendRequest();  // Function signature for accepting friend requests
    void rejectFriendRequest();



	int themeSelection();





};