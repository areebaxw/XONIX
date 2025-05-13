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

    Authentication& auth;

    PlayerHashTable playerHashTable;
    Level level;

	//-------------------MENU TEXTS-------------------//
    Text startText;
    Text selectLevelText;
    Text leaderboardText;
    Text profileText;
    Text endText;
    Text finalScoreText;
    Text themeSelectionText;
    Text friendListText;
    Text queueStatusText;


	//-------------------MENU SPRITES-------------------//
    MySprite bgMode;
    MySprite bgSprite;
    MySprite bgLevel;
    MySprite bgLeaderboard;
    MySprite bgFriends;
    MySprite bgPlayer;

    void initMainMenu();
    void initEndMenu();

public:
   

    //----------------------------------MATCHMAKING STUFF--------------------//
    bool isMatchmakingEnabled;
    bool showingMatchmakingQueue;

    string matchedPlayer1;  
    string matchedPlayer2; 
    Matchmaking matchmaker;


	//------------------ PLAYER USERNAMES--------------------//
    string player1Username;
    string player2Username;

     //----------------------PLAYER PROFILE-----------------------------//
    PlayerProfile* currentProfile;
    void handleModeSelection();

    //--------------------------LEVEL AND GAME STATE------------------------------//
    int currentLevel;
    bool showingMainMenu = true;
    bool showingModeSelection = false;
    bool showingLevelSelection = false;
    bool gameStarted = false;
    bool isSinglePlayerSelected = true;


    void setCurrentLevel(int level) {
        currentLevel = level;
    }
   
   

    //------------------------ MENU DISPLAY FUNCTIONS-----------------------//
    void displayLevelSelection();
    void handleLevelSelection();
    void displayMainMenu();
    void handleMainMenu();
    void selectMode();
    void selectLevel();


    //------------------------------LEADERBOARD METHODS-------------------------//
    void setLeaderboardDisplayMode(int mode);
    void showLeaderboard();

    //----------------------------GAME MANAGMENT METHODS--------------------------//
    void displayEndMenu(int score, int highScore);
    void handleEndMenu();
    void showProfile();
    void showMatchmakingQueue();
    bool startMatchmaking();
    void handleMatchmakingQueue();


	//----------------------------GAME MANAGEMENT METHODS--------------------------//
    void startGame();
    void restartGame();
    bool isPlayerInQueue(const string& username);
    void exitGame();
    int getCurrentLevel();


	//------------------------GAME STATE VARIABLES--------------------------//
    int player1Score = 0;
    int player2Score = 0;
    bool restartRequested = false;

  
//------------------------SECOND PLAYER NAME STUFF------------------------------//
    void setSecondPlayerName(const string& playerName);
    string secondPlayerName;
    string promptForSecondPlayerName();
    string getValidPlayerName(const string& name, const string& defaultName);
    void syncPlayerNames();


	//------------------------FRIENDLIST METHODS--------------------------//
    void sendFriendRequest();           
    void displayFriendRequests();
    void showFriendList();
    bool showingFriendList = false;
    void sendFriendRequestToUser(const string& sender, const string& receiver);
    void acceptFriendRequest();  
    void rejectFriendRequest();
    void showFriendsForMultiplayer();



	//------------------------THEME SELECTION METHODS--------------------------//
	int themeSelection();



	//----------------------------CONSTRUCTOR AND DESTRUCTOR--------------------------//
    Menu(RenderWindow& window, const string& username, Authentication& authRef);
    ~Menu();


};