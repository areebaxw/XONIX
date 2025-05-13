#include "menu.h"
#include "level.h"
#include "player.h"
#include "auth.h"
#include "theme.h"
#include "Sprite.h"
#include <iostream>
using namespace std;
using namespace sf;


Menu::Menu(RenderWindow& window, const string& username, Authentication& authRef)
    : window(window), auth(authRef)
{
    font.loadFromFile("arial.ttf"); 


    currentProfile = new PlayerProfile(username);


    //player names
    matchedPlayer1 = username;
    matchedPlayer2 = "";

    initMainMenu();
    initEndMenu();
    leaderboard.loadLeaderboard();

    //state variables
    showingMainMenu = true;
    showingModeSelection = false;
    showingLevelSelection = false;
    gameStarted = false;
    isSinglePlayerSelected = true;
    restartRequested = false;

    // Initialize scores
    player1Score = 0;
    player2Score = 0;





    //frindlist
    showingFriendList = false;
    currentProfile = new PlayerProfile(username);
}





//-------------------- Menu Logic ------------------//

void Menu::initMainMenu()
{
    startText.setFont(font);
    startText.setString("Start");
    startText.setPosition(470, 124);
    startText.setCharacterSize(30);
    startText.setFillColor(Color::White);

    selectLevelText.setFont(font);
    selectLevelText.setString("Levels");
    selectLevelText.setPosition(470, 190);
    selectLevelText.setCharacterSize(30);
    selectLevelText.setFillColor(Color::White);

    leaderboardText.setFont(font);
    leaderboardText.setString("Leaderboard");
    leaderboardText.setPosition(448, 245);
    leaderboardText.setCharacterSize(25);
    leaderboardText.setFillColor(Color::White);

    profileText.setFont(font);
    profileText.setString("Player");
    profileText.setPosition(785, 37);
    profileText.setCharacterSize(30);

    profileText.setFillColor(Color::White);  
    profileText.setOutlineColor(Color::Black);
    profileText.setOutlineThickness(2);


    //theme

    themeSelectionText.setFont(font);
    themeSelectionText.setString("Theme");
    themeSelectionText.setPosition(470, 305);
    themeSelectionText.setCharacterSize(30);
    themeSelectionText.setFillColor(Color::White);


    //friendlist 

    // Add Friends List menu option
    friendListText.setFont(font);
    friendListText.setString("Friends");
    friendListText.setPosition(470, 360); 
    friendListText.setCharacterSize(30);
    friendListText.setFillColor(Color::White);

}


string Menu::getValidPlayerName(const string& name, const string& defaultName) {
    if (name.empty()) {
        return defaultName;
    }
    return name;
}

//--------------------- End Menu Logic ------------------//


void Menu::initEndMenu()
{
    endText.setFont(font);
   ///* endText.setString("Game Over");*/
    endText.setPosition(200, 100);
    endText.setCharacterSize(30);
    endText.setFillColor(Color::White);

    finalScoreText.setFont(font);
    finalScoreText.setString("Final Score: 1000"); 
    finalScoreText.setPosition(200, 150);
    finalScoreText.setCharacterSize(30);
    finalScoreText.setFillColor(Color::White);
}


//---------------- Menu Display Logic ------------------//
void Menu::displayMainMenu()
{
    window.clear();


   Texture texture;
    if (!texture.loadFromFile("images/mm.jpg")) {  
        cout<< "Error loading texture" << endl;
    }


   Sprite sprite(texture);


    window.draw(sprite);


	bgSprite.Draw(window);
    window.draw(startText);
    window.draw(selectLevelText);
    window.draw(leaderboardText);
    window.draw(profileText);
    window.draw(friendListText);
    window.draw(themeSelectionText);

    window.display();

}



void Menu::setSecondPlayerName(const string& playerName) {
    secondPlayerName = playerName;
    matchedPlayer2 = playerName;
}


//----------------- Menu Event Handling ------------------//
void Menu::handleMainMenu()
{
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();

        if (e.type == Event::KeyPressed)
        {
            if (e.key.code == Keyboard::Enter) {
                startGame();  
            }
            else if (e.key.code == Keyboard::Down) {
                selectLevel(); 
            }
            else if (e.key.code == Keyboard::Up) {
                showLeaderboard(); 
            }
            else if (e.key.code == Keyboard::Right) {
                showProfile();  
            }

        }

        //-----------------------------------MOUSE HANDLE------------------------------------//
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);
            Vector2f mousePosF = window.mapPixelToCoords(mousePosition);
           

            if (startText.getGlobalBounds().contains(mousePosF)) {

                string player1 = currentProfile->getUsername();  
                string player2 = leaderboard.getTopScorer(); 

                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  
                    matchedPlayer2 = player2; 

                    //---------------- Switch to multiplayer mode----------------//
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;

                    cout << "Matchmaking: Player 1 (" << matchedPlayer1 << ") vs Player 2 (" << matchedPlayer2 << ")" << endl;
                }
                else {
                    cout << "No players in queue or unable to match. Try again later!" << endl;
                }

                startGame();  
            }

			//------------------ Select Level ----------------//
            else if (selectLevelText.getGlobalBounds().contains(mousePosF)) {
                selectLevel();  
            }
        
			//--------------- Show Leaderboard ----------------//
            else if (leaderboardText.getGlobalBounds().contains(mousePosF)) {
                showLeaderboard();
            }

			//---------------- Show Profile ----------------//
else if (profileText.getGlobalBounds().contains(mousePosF)) {
    showProfile(); 
}
            
			//------------------ Show Friends List ----------------//
else if (friendListText.getGlobalBounds().contains(mousePosF)) {
                showingMainMenu = false;
                showFriendList();
            }

			//------------------ Show Theme Selection ----------------//

			else if (themeSelectionText.getGlobalBounds().contains(mousePosF))
            {
                int themeId = -1;
				themeId = themeSelection();
				cout << "Theme ID selected: " << themeId << endl;

                
                if (themeId > 0 && themeId < 4)
                {

                    showingMainMenu = true;
                    
                    AVLTree themeSearching;
                    AVLNode* themeNode = themeSearching.search(themeId);

					//------------------ Set Theme ----------------//
                    if(themeNode)
                    {
                        MySprite tempSprite(themeNode->themepaths[0], 0, 0);
                        MySprite tempSprite2(themeNode->themepaths[1], 0, 0);
                        MySprite tempSprite3(themeNode->themepaths[2], 0, 0);
                        MySprite tempSprite4(themeNode->themepaths[3], 0, 0);
                        MySprite tempSprite5(themeNode->themepaths[4], 0, 0);
                        MySprite tempSprite6(themeNode->themepaths[5], 0, 0);

						cout << endl << themeNode->themepaths[0] << endl;
						this->bgSprite.setTexture(tempSprite.getPath());
                        this->bgMode.setTexture(tempSprite2.getPath());
                        this->bgLevel.setTexture(tempSprite3.getPath());
                        this->bgLeaderboard.setTexture(tempSprite4.getPath());
                        this->bgFriends.setTexture(tempSprite5.getPath());
                        this->bgPlayer.setTexture(tempSprite6.getPath());
                    }
                }
			}

        }
    }
}




void Menu::selectMode()
{
    window.clear();





   Texture texture;
    if (!texture.loadFromFile("images/auth.jpg")) {  
        cout<< "Error loading texture" << endl;
        
    }


   Sprite sprite(texture);


    window.draw(sprite);

	//------------------ Mode Selection Text ----------------//
    Text modeSelectionText;
    modeSelectionText.setFont(font);
    modeSelectionText.setString("Select Mode:");
    modeSelectionText.setCharacterSize(30);
    modeSelectionText.setFillColor(Color::White);
    modeSelectionText.setPosition(410, 150);

    Text singlePlayerText;
    singlePlayerText.setFont(font);
    singlePlayerText.setString("1. Single Player");
    singlePlayerText.setCharacterSize(30);
    singlePlayerText.setFillColor(Color::White);
    singlePlayerText.setPosition(380, 200);

    Text multiplayerText;
    multiplayerText.setFont(font);
    multiplayerText.setString("2. Multiplayer");
    multiplayerText.setCharacterSize(30);
    multiplayerText.setFillColor(Color::White);
    multiplayerText.setPosition(380, 250);

    Text friendMultiplayerText;
    friendMultiplayerText.setFont(font);
    friendMultiplayerText.setString("3. Play with Friends");
    friendMultiplayerText.setCharacterSize(30);
    friendMultiplayerText.setFillColor(Color::White);
    friendMultiplayerText.setPosition(380, 300);

    Text matchmakingText;
    matchmakingText.setFont(font);
    matchmakingText.setString("4. Matchmaking");
    matchmakingText.setCharacterSize(30);
    matchmakingText.setFillColor(Color::White);
    matchmakingText.setPosition(380, 350);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(560, 410);

    bgMode.Draw(window);

    window.draw(modeSelectionText);
    window.draw(singlePlayerText);
    window.draw(multiplayerText);
    window.draw(friendMultiplayerText);
    window.draw(matchmakingText);
    window.draw(backText);
    window.display();
}



//----------------- MODE SELECTION ----------------//
void Menu::handleModeSelection()
{
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();


        
        if (e.type == Event::KeyPressed)
        {
			//----------------- Single Player Mode ----------------//
            if (e.key.code == Keyboard::Num1 || e.key.code == Keyboard::Numpad1) {
                isSinglePlayerSelected = true;
                showingModeSelection = false;
                gameStarted = true;
                cout << "Single Player Mode Selected!" << endl;
            }


			//----------------- Multiplayer Mode ----------------//
		
            else if (e.key.code == Keyboard::Num2 || e.key.code == Keyboard::Numpad2) {
            
                RenderWindow secondPlayerWindow(VideoMode(400, 200), "Enter Second Player Name");
                Font font;
                if (!font.loadFromFile("arial.ttf")) {
                    cout << "Error loading font" << endl;
                    return;
                }

                Text promptText;
                promptText.setFont(font);
                promptText.setString("Enter Second Player Name:");
                promptText.setCharacterSize(20);
                promptText.setFillColor(Color::White);
                promptText.setPosition(50, 50);

                Text inputText;
                inputText.setFont(font);
                inputText.setCharacterSize(20);
                inputText.setFillColor(Color::White);
                inputText.setPosition(50, 100);

                string inputString;

				//----------------- Input Handling for Second Player Name ----------------//
                while (secondPlayerWindow.isOpen())
                {
                    Event inputEvent;
                    while (secondPlayerWindow.pollEvent(inputEvent))
                    {
                        if (inputEvent.type == Event::Closed)
                            secondPlayerWindow.close();

                        if (inputEvent.type == Event::TextEntered)
                        {
                            if (inputEvent.text.unicode == '\b' && !inputString.empty()) {
                                inputString.pop_back();
                            }
                            else if (inputEvent.text.unicode < 128 &&
                                inputEvent.text.unicode != '\b' &&
                                inputEvent.text.unicode != '\r')
                            {
                                inputString += static_cast<char>(inputEvent.text.unicode);
                            }
                            inputText.setString(inputString);
                        }

						//----------------- Check for Enter Key Press ----------------//
                        if (inputEvent.type == Event::KeyPressed)
                        {
                            if (inputEvent.key.code == Keyboard::Return && !inputString.empty()) {
                                matchedPlayer2 = inputString;
                                secondPlayerWindow.close();
                            }
                        }
                    }

                    secondPlayerWindow.clear();
                    secondPlayerWindow.draw(promptText);
                    secondPlayerWindow.draw(inputText);
                    secondPlayerWindow.display();
                }

               
                if (!matchedPlayer2.empty()) {
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;
                   
                    syncPlayerNames();
                    cout << "Multiplayer Mode Selected with Player 2: " << matchedPlayer2 << endl;
                }
            }
			//---------------- Show Friends List for Multiplayer ----------------//
            else if (e.key.code == Keyboard::Num3 || e.key.code == Keyboard::Numpad3) {
              
                showFriendsForMultiplayer();
            }

            else if (e.key.code == Keyboard::Num4 || e.key.code == Keyboard::Numpad4) {
               
                isMatchmakingEnabled = true;
                showingModeSelection = false;
                showingMatchmakingQueue = true;

                
				//----------------- Matchmaking Logic ----------------//
                string player1 = currentProfile->getUsername();  
                string player2 = leaderboard.getTopScorer(); 
                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  
                    matchedPlayer2 = player2;  
                }
                else {
                 
                    matchedPlayer2 = "Player2";
                }

                cout << "Matchmaking Mode Selected!" << endl;
            }
            else if (e.key.code == Keyboard::Escape) {
               
                showingModeSelection = false;
                showingMainMenu = true;
            }
        }

      
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);

           

			//----------------- Single Player Mode ----------------//
            if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 150 && mousePosition.y <= 180) {
                isSinglePlayerSelected = true;
                showingModeSelection = false;
                gameStarted = true;
                cout << "Single Player Mode Selected!" << endl;
            }
            

			//----------------- Multiplayer Mode ----------------//
			
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 200 && mousePosition.y <= 230) {
               
                RenderWindow secondPlayerWindow(VideoMode(400, 200), "Enter Second Player Name");
                Font font;
                if (!font.loadFromFile("arial.ttf")) {
                    cout << "Error loading font" << endl;
                    return;
                }

                Text promptText;
                promptText.setFont(font);
                promptText.setString("Enter Second Player Name:");
                promptText.setCharacterSize(20);
                promptText.setFillColor(Color::White);
                promptText.setPosition(50, 50);

                Text inputText;
                inputText.setFont(font);
                inputText.setCharacterSize(20);
                inputText.setFillColor(Color::White);
                inputText.setPosition(50, 100);

                string inputString;

				//----------------- Input Handling for Second Player Name ----------------//
                while (secondPlayerWindow.isOpen())
                {
                    Event inputEvent;
                    while (secondPlayerWindow.pollEvent(inputEvent))
                    {
                        if (inputEvent.type == Event::Closed)
                            secondPlayerWindow.close();

                        if (inputEvent.type == Event::TextEntered)
                        {
                            if (inputEvent.text.unicode == '\b' && !inputString.empty()) {
                                inputString.pop_back();
                            }
                            else if (inputEvent.text.unicode < 128 &&
                                inputEvent.text.unicode != '\b' &&
                                inputEvent.text.unicode != '\r')
                            {
                                inputString += static_cast<char>(inputEvent.text.unicode);
                            }
                            inputText.setString(inputString);
                        }

                        if (inputEvent.type == Event::KeyPressed)
                        {
                            if (inputEvent.key.code == Keyboard::Return && !inputString.empty()) {
                                matchedPlayer2 = inputString;
                                secondPlayerWindow.close();
                            }
                        }
                    }

                    secondPlayerWindow.clear();
                    secondPlayerWindow.draw(promptText);
                    secondPlayerWindow.draw(inputText);
                    secondPlayerWindow.display();
                }

                
                if (!matchedPlayer2.empty()) {
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;
                    cout << "Multiplayer Mode Selected with Player 2: " << matchedPlayer2 << endl;
                }
            }
         
			//---------------- Show Friends List for Multiplayer ----------------// 
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 250 && mousePosition.y <= 280) {
               
                showFriendsForMultiplayer();
            }
           
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 300 && mousePosition.y <= 330) {
               
                isMatchmakingEnabled = true;
                showingModeSelection = false;
                showingMatchmakingQueue = true;

             
                string player1 = currentProfile->getUsername();  
                string player2 = leaderboard.getTopScorer();  

                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  
                    matchedPlayer2 = player2;  
                }
                else {
                   
                    matchedPlayer2 = "Player2";
                }

                cout << "Matchmaking Mode Selected!" << endl;
            }
        }
    }
}


void Menu::showMatchmakingQueue() {
    window.clear();



   Font font;
    if (!font.loadFromFile("Courier Prime Bold.ttf")) {
        cout << "Error loading font" << endl;

    }

   
    Text matchmakingTitleText;
    matchmakingTitleText.setFont(font);
    matchmakingTitleText.setString("Matchmaking Queue");
    matchmakingTitleText.setCharacterSize(50);
    matchmakingTitleText.setFillColor(Color::White);
    matchmakingTitleText.setPosition(200, 100);

   
    string currentUsername = currentProfile->getUsername();
    string topPlayer = matchmaker.getTopPlayerFromLeaderboard();




    if (matchmaker.getQueueSize() < 2) {

    
        if (!topPlayer.empty() && topPlayer != currentUsername) {
  
            string nextTopPlayer = matchmaker.getNextTopPlayerFromLeaderboard(topPlayer);  


            if (!nextTopPlayer.empty()) {
               
                queueStatusText.setString("Ready to match with: " + nextTopPlayer);
            }
            else {
               
                queueStatusText.setString("Not enough players in queue.");
            }
        }
        else {
        
            queueStatusText.setString("Waiting for other players...");
        }
    }
    else {
       
        if (!topPlayer.empty() && topPlayer != currentUsername) 
        {
            queueStatusText.setString("Ready to match with: " + topPlayer); 
        }
        else {
           
            queueStatusText.setString("Players in Queue: " + to_string(matchmaker.getQueueSize()));
        }
    }

   
    queueStatusText.setCharacterSize(30);
    queueStatusText.setFillColor(Color::White);
    queueStatusText.setPosition(200, 150);

    
    Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("Press ENTER to start matchmaking\nPress ESC to cancel");
    instructionText.setCharacterSize(30);
    instructionText.setFillColor(Color::White);
    instructionText.setPosition(200, 250);

    
    window.draw(matchmakingTitleText);
    window.draw(queueStatusText);
    window.draw(instructionText);

   
    window.display();
}


string Menu::promptForSecondPlayerName() {
    string secondPlayerName;
    cout << "Enter the name of the second player: ";
    cin >> secondPlayerName;
    return secondPlayerName;
}


bool Menu::startMatchmaking() {
    
    string currentUsername = currentProfile->getUsername();
    matchmaker.populateQueueFromLeaderboard(currentUsername);

    cout << "Current Queue Size: " << matchmaker.getQueueSize() << endl;
   // matchmaker.debugPrintQueue();  


    string topPlayerUsername = matchmaker.getTopPlayerFromLeaderboard();
    cout << "Top player from leaderboard: " << topPlayerUsername << endl;




    matchedPlayer1 = currentUsername;

    //------------------- If the current player is the top player, trying to match with other players----------------------//
    if (topPlayerUsername.empty() || topPlayerUsername == currentUsername)
    {
        cout << "Adding " << currentUsername << " to matchmaking queue..." << endl;
        int currentScore = matchmaker.readUserHighestScore(currentUsername);

      
        matchmaker.enterQueue(currentUsername, currentScore);

        //---------------------- If the queue has only one player, trying to match with the next top player from the leaderboard--------//
        if (matchmaker.getQueueSize() < 2) 
        {
            string nextTopPlayer = matchmaker.getNextTopPlayerFromLeaderboard(topPlayerUsername);

            if (!nextTopPlayer.empty() && nextTopPlayer != currentUsername)
            {
                matchedPlayer2 = nextTopPlayer;  

              
                secondPlayerName = matchedPlayer2;

                cout << "Only one player in the queue. Matching "
                    << matchedPlayer1 << " vs " << matchedPlayer2 << endl;

                ////
                //ofstream logFile("matchmaking_log.txt", ios_base::app);
                //if (logFile.is_open()) {
                //    logFile << "Matched players: " << matchedPlayer1
                //        << " vs " << matchedPlayer2
                //        << " (Next top leaderboard player)" << endl;
                //    logFile.close();
                //}

                // Proceed to start the game
                isSinglePlayerSelected = false;
                gameStarted = true;
                showingMatchmakingQueue = false;
                return true;
            }
            else 
            {
                cout << "Not enough players to match. Current queue size: "
                    << matchmaker.getQueueSize() << endl;

              
                if (matchedPlayer2.empty()) {
                    matchedPlayer2 = "Player2";
                    secondPlayerName = matchedPlayer2;
                }

                return false;
            }
        }

        //----------------------- If there are more than one player, continue the regular matchmaking------------------------//
        if (matchmaker.matchPlayers(matchedPlayer1, matchedPlayer2)) 
        {
            secondPlayerName = matchedPlayer2;

       
            isSinglePlayerSelected = false;
            gameStarted = true;
            showingMatchmakingQueue = false;



            cout << "Match found: " << matchedPlayer1 << " vs " << matchedPlayer2 << endl;
            return true;
        }
        else {
            cout << "Matchmaking failed. Try again later!" << endl;

            if (matchedPlayer2.empty()) {
                matchedPlayer2 = "Player2";
                secondPlayerName = matchedPlayer2;
            }

            return false;
        }
    }
    else {
        //------------------ If the top player isn't the same as the current player, match with them-----------------------------//
        matchedPlayer2 = topPlayerUsername;  

        secondPlayerName = matchedPlayer2;

        cout << "Match found: " << matchedPlayer1 << " vs " << matchedPlayer2 << endl;





    //    
    //    string logMessage = "Matched players: " + matchedPlayer1 +
    //        " vs " + matchedPlayer2 +
    //        " (Top leaderboard player)";


    ///*    ofstream logFile("matchmaking_log.txt", ios_base::app);
    //    if (logFile.is_open()) {
    //        logFile << logMessage << endl;
    //        logFile.close();
    //    }*/

       


        isSinglePlayerSelected = false;
        gameStarted = true;
        showingMatchmakingQueue = false;
        return true;
    }
}


void Menu::handleMatchmakingQueue()
{
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();

        if (e.type == Event::KeyPressed)
        {
            if (e.key.code == Keyboard::Return) {
                // Attempt to start matchmaking
                if (startMatchmaking()) {
                    cout << "Match found: " << matchedPlayer1 << " vs " << matchedPlayer2 << endl;
                    gameStarted = true;
                }
                else {
                    cout << "Not enough players in queue, try again later!" << endl;
                }
            }
            else if (e.key.code == Keyboard::Escape) {
                // Cancel matchmaking
                showingMatchmakingQueue = false;
                showingModeSelection = true;
                isMatchmakingEnabled = false;
            }
        }
    }
}


int currentLevel;
Level level;  

void Menu::selectLevel()
{
    showingMainMenu = false;
    showingLevelSelection = true;

    window.clear();




   



    Text levelSelectionText;
    levelSelectionText.setFont(font);
    levelSelectionText.setString("Select Level:");
    levelSelectionText.setCharacterSize(30);
    levelSelectionText.setFillColor(Color::White);
    levelSelectionText.setPosition(400, 100);

    Text level1Text;
    level1Text.setFont(font);
    level1Text.setString("1. Level 1 (Easy)");
    level1Text.setCharacterSize(30);
    level1Text.setFillColor(Color::White);
    level1Text.setPosition(400, 150);

    Text level2Text;
    level2Text.setFont(font);
    level2Text.setString("2. Level 2 (Medium)");
    level2Text.setCharacterSize(30);
    level2Text.setFillColor(Color::White);
    level2Text.setPosition(400, 200);

    Text level3Text;
    level3Text.setFont(font);
    level3Text.setString("3. Level 3 (Hard)");
    level3Text.setCharacterSize(30);
    level3Text.setFillColor(Color::White);
    level3Text.setPosition(400, 250);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(400, 300);



    bgLevel.Draw(window);


    window.draw(levelSelectionText);
    window.draw(level1Text);
    window.draw(level2Text);
    window.draw(level3Text);
    window.draw(backText);
    window.display();

    // Handle level selection input
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();

        if (e.type == Event::KeyPressed)
        {
            if (e.key.code == Keyboard::Num1 || e.key.code == Keyboard::Numpad1) {
                currentLevel = 0;  // Level 1
                level.setLevel(currentLevel);  // Update level settings
                cout << "Level 1 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            else if (e.key.code == Keyboard::Num2 || e.key.code == Keyboard::Numpad2) {
                currentLevel = 1;  // Level 2
                level.setLevel(currentLevel);  // Update level settings
                cout << "Level 2 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            else if (e.key.code == Keyboard::Num3 || e.key.code == Keyboard::Numpad3) {
                currentLevel = 2;  // Level 3
                level.setLevel(currentLevel);  // Update level settings
                cout << "Level 3 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            else if (e.key.code == Keyboard::Escape) {
                // Go back to main menu without changing level
                showingLevelSelection = false;
                showingMainMenu = true;
            }
        }
    }
}


int Menu::getCurrentLevel() {
    return currentLevel;
}
void Menu::setLeaderboardDisplayMode(int mode) {
    leaderboard.setDisplayMode(mode);
}

void Menu::showLeaderboard()
{

    showingMainMenu = false;


    // 0 - Single Player
    // 1 - Multiplayer
    // 2 - Total Score
    int currentMode = 0;

    while (true) {
      
        window.clear();

       Texture texture;
        if (!texture.loadFromFile("images/bb.jpg")) {  
            cout<< "Error loading texture" << endl;
           
        }


       Sprite sprite(texture);


        window.draw(sprite);
        bgLeaderboard.Draw(window);


        Text leaderboardTitle;
        leaderboardTitle.setFont(font);
        leaderboardTitle.setString("LEADERBOARD");
        leaderboardTitle.setCharacterSize(40);
        leaderboardTitle.setFillColor(Color::White);
        leaderboardTitle.setPosition(400, 50);



        // Mode selection text
        Text modeText;
        modeText.setFont(font);
        string modeString;



		//------------------ Display Mode -----------------//
        switch (currentMode) 
        {
        case 0:
            leaderboard.setDisplayMode(0);  // Single Player
            modeString = "Mode: Single Player Scores";
            break;
        case 1:
            leaderboard.setDisplayMode(1);  // Multiplayer
            modeString = "Mode: Multiplayer Matches";
            break;

            //not efficient
        case 2:
            leaderboard.setDisplayMode(0);  // Default
            modeString = "Mode: Combined Scores";
            break;
        }



        modeText.setString(modeString);
        modeText.setCharacterSize(20);
        modeText.setFillColor(Color::Yellow);
        modeText.setPosition(395, 100);

   
        window.draw(leaderboardTitle);
        window.draw(modeText);


		//------------------ Sort and Display Leaderboard -----------------//
      
        LeaderboardEntry sortedLeaderboard[HEAP_SIZE];
        leaderboard.sortForDisplay(sortedLeaderboard);

       
        for (int i = 0; i < 10; i++) {
            Text playerText;
            playerText.setFont(font);

         
            if (i < leaderboard.heapSize) {
                string displayText;



                switch (currentMode) {

					//------------------- Single Player -----------------//
                case 0:
                   
                    if (sortedLeaderboard[i].isMultiplayer) {
                        playerText.setString(to_string(i + 1) + ". ---");
                    }
                    else {
                        displayText = to_string(i + 1) + ". " +
                            sortedLeaderboard[i].username +
                            " - Score: " +
                            to_string(sortedLeaderboard[i].player1Score);
                        playerText.setString(displayText);
                    }
                    break;

                case 1: 
					//------------------- Multiplayer -----------------//
                    if (!sortedLeaderboard[i].isMultiplayer) {
                        playerText.setString(to_string(i + 1) + ". ---");
                    }
                    else {
                        displayText = to_string(i + 1) + ". " +
                            sortedLeaderboard[i].player1Name + ": " +
                            to_string(sortedLeaderboard[i].player1Score) +
                            " vs " +
                            sortedLeaderboard[i].player2Name + ": " +
                            to_string(sortedLeaderboard[i].player2Score);

                        playerText.setString(displayText);
                    }
                    break;

					//------------------- Combined -----------------//
                case 2:
                    displayText = to_string(i + 1) + ". " +
                        sortedLeaderboard[i].username +
                        " - Score: " +
                        to_string(sortedLeaderboard[i].score);
                    playerText.setString(displayText);
                    break;
                }
            }
            else {
                playerText.setString(to_string(i + 1) + ". ---");
            }

            playerText.setCharacterSize(25);
            playerText.setFillColor(Color::White);
            playerText.setPosition(400, 150 + i * 40);

            window.draw(playerText);
        }

		//------------------ Instruction Text -----------------//
        Text instructionText;
        instructionText.setFont(font);
        instructionText.setString("Press SPACE to change view\nPress ESC to go back");
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::Yellow);
        instructionText.setPosition(400, 550);

        window.draw(instructionText);
        window.display();


		//------------------ Event Handling for leaderboard -----------------//
        Event e;
        bool exitLeaderboard = false;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) 
                {
                    showingMainMenu = true;
                    exitLeaderboard = true;
                    break;
                }

                if (e.key.code == Keyboard::Space) 
                {
                  
                    currentMode = (currentMode + 1) % 3;
                    break;  
                }
            }
        }

        // Exit leaderboard if requested
        if (exitLeaderboard) {
            break;
        }
    }
}
void Menu::displayEndMenu(int score1, int score2)
{
    window.clear();

   Texture texture;
    if (!texture.loadFromFile("images/end1.jpg")) {  
        cout << "Error loading texture" << endl;
     
    }
   Sprite sprite(texture);
    window.draw(sprite);


   Font font;
    if (!font.loadFromFile("Courier Prime Bold.ttf")) {
        cout << "Error loading font" << endl;
    }

    string player1DisplayName = matchedPlayer1.empty() ? currentProfile->getUsername() : matchedPlayer1;
    string player2DisplayName = matchedPlayer2.empty() ? "Player2" : matchedPlayer2;


    
   Text winnerText;
    
   //------------------ winner Text -----------------//

    if (!isSinglePlayerSelected) {
        if (score1 > score2) {
            winnerText.setString(player1DisplayName + " Wins!");
            winnerText.setFillColor(Color::White);
        }
        else if (score2 > score1) {
            winnerText.setString(player2DisplayName + " Wins!");
            winnerText.setFillColor(Color::White);
        }
        else {
            winnerText.setString("It's a Tie!");
            winnerText.setFillColor(Color::White);
        }
    }
    else {
       // endText.setString("Game Over");
    
    }

  
    endText.setCharacterSize(40);
    endText.setPosition(300, 200);


    winnerText.setFont(font);
    winnerText.setCharacterSize(50);
   winnerText.setPosition(350, 350);


   //------------------- Final Score Text -----------------//
    if (!isSinglePlayerSelected) {
        finalScoreText.setString(
            player1DisplayName + " Score: " + to_string(score1) + "\n" +
            player2DisplayName + " Score: " + to_string(score2)
        );
    }
    else {
        finalScoreText.setString("Your Score: " + to_string(score1));
    }
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(30);
    finalScoreText.setFillColor(Color::White);
    finalScoreText.setPosition(380, 200);

   
	//------------------- Record Game Result -----------------//
    if (!isSinglePlayerSelected) {
        leaderboard.recordGameResult(player1DisplayName,score1,player2DisplayName,score2,true,currentLevel);
    }
    else {
       
        string currentUsername = currentProfile->getUsername();
        leaderboard.recordGameResult(currentUsername, score1, "", 0, false, currentLevel);
    }

  
	//------------------- Restart and Exit Text -----------------//
  
    Text restartText;
    restartText.setFont(font);
    restartText.setString("Press R to restart");
    restartText.setCharacterSize(20);
    restartText.setFillColor(Color::White);
    restartText.setPosition(400, 470);

    Text exitText;
    exitText.setFont(font);
    exitText.setString("Press ESC to exit");
    exitText.setCharacterSize(20);
    exitText.setFillColor(Color::White);
    exitText.setPosition(400, 500);


    window.draw(endText);
    window.draw(finalScoreText);
    window.draw(winnerText);
    window.draw(restartText);
    window.draw(exitText);

    window.display();
}
void Menu::startGame()
{
    cout << "Starting game process!" << endl;
    showingMainMenu = false;
    showingModeSelection = true;
   
}


void Menu::restartGame()
{
    cout << "Game Restarted" << endl;
    gameStarted = false;
    showingMainMenu = true;
    restartRequested = true;  
    player1Score = 0;
    player2Score = 0;
}


void Menu::exitGame()
{
    cout << "Exiting Game" << endl;
    window.close();
}


void Menu::displayLevelSelection()
{
    window.clear();


   Texture texture;
    if (!texture.loadFromFile("images/lvl.jpg")) {  
        cout<< "Error loading texture" << endl;
     
    }


   Sprite sprite(texture);


    window.draw(sprite);



	//------------------ Level Selection Text -----------------//

    Text levelSelectionText;
    levelSelectionText.setFont(font);
    levelSelectionText.setString("Select Level:");
    levelSelectionText.setCharacterSize(30);
    levelSelectionText.setFillColor(Color::White);
    levelSelectionText.setPosition(350, 150);

    Text level1Text;
    level1Text.setFont(font);
    level1Text.setString("1. Level 1 (Easy)");
    level1Text.setCharacterSize(30);
    level1Text.setFillColor(Color::White);
    level1Text.setPosition(350, 200);

    Text level2Text;
    level2Text.setFont(font);
    level2Text.setString("2. Level 2 (Medium)");
    level2Text.setCharacterSize(30);
    level2Text.setFillColor(Color::White);
    level2Text.setPosition(350, 250);

    Text level3Text;
    level3Text.setFont(font);
    level3Text.setString("3. Level 3 (Hard)");
    level3Text.setCharacterSize(30);
    level3Text.setFillColor(Color::White);
    level3Text.setPosition(350, 300);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(350, 350);

	bgLevel.Draw(window);  
    window.draw(levelSelectionText);
    window.draw(level1Text);
    window.draw(level2Text);
    window.draw(level3Text);
    window.draw(backText);
    window.display();
}


//------------------- Handle Level Selection Input -----------------//

void Menu::handleLevelSelection()
{
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();

		//---------------- Keyboard Input Handling -----------------//
        if (e.type == Event::KeyPressed)
        {
			//---------------- Level 1 Selection -----------------//
            if (e.key.code == Keyboard::Num1 || e.key.code == Keyboard::Numpad1) {
                currentLevel = 0; 
                level.setLevel(currentLevel);  
                cout << "Level 1 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
			//---------------- Level 2 Selection -----------------//
            else if (e.key.code == Keyboard::Num2 || e.key.code == Keyboard::Numpad2) {
                currentLevel = 1;  
                level.setLevel(currentLevel);  
                cout << "Level 2 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;


            }
			//---------------- Level 3 Selection -----------------//
            else if (e.key.code == Keyboard::Num3 || e.key.code == Keyboard::Numpad3) {
                currentLevel = 2; 
                level.setLevel(currentLevel); 
                cout << "Level 3 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            else if (e.key.code == Keyboard::Escape) 
            {
                
                showingLevelSelection = false;
                showingMainMenu = true;
            }
        }

		//---------------- Mouse Click Handling -----------------//
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);

			//---------------- Check if Level 1 option is clicked -----------------//
            if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 150 && mousePosition.y <= 180) {
                currentLevel = 0;
                level.setLevel(currentLevel);
                cout << "Level 1 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }



			//---------------- Check if Level 2 option is clicked -----------------//
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 200 && mousePosition.y <= 230) {
                currentLevel = 1;
                level.setLevel(currentLevel);
                cout << "Level 2 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }


			//---------------- Check if Level 3 option is clicked -----------------//
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 250 && mousePosition.y <= 280) {
                currentLevel = 2;
                level.setLevel(currentLevel);
                cout << "Level 3 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
        }
    }
}
void Menu::showProfile()
{
    
    showingMainMenu = false;

   
    window.clear();

    bgPlayer.Draw(window);

    
    currentProfile->displayProfile(window,bgPlayer);

	//------------------ Profile Text -----------------//
    Event e;
    bool inProfileView = true;
    while (inProfileView) {
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();


            if (!currentProfile->handleProfileInput(e)) {

                inProfileView = false;
                showingMainMenu = true;
                break;
            }
        }


        if (!inProfileView)
            break;
    }

    if (showingMainMenu) {
        displayMainMenu();
    }
}


Menu::~Menu()
{
    if (currentProfile) {
        delete currentProfile;
    }
}

//------------------ Send Friend Request -----------------//

void Menu::sendFriendRequestToUser(const string& sender, const string& receiver) {

    string requestFileName = receiver + "_friend_requests.txt";


    ofstream requestFile(requestFileName, ios_base::app);

    if (requestFile.is_open()) {

        requestFile << sender << endl;
        requestFile.close();

        cout << sender << " sent a friend request to " << receiver << endl;
    }
    else {
        cout << "Error: Could not save friend request" << endl;
    }
}

void Menu::showFriendList() {
   
    showingMainMenu = false;
    showingFriendList = true;

    string currentUsername = currentProfile->getUsername();

   
	//------------------- Load Friend List -----------------//
    FriendList* friendList = playerHashTable.getFriendList(currentUsername);

	//------------------- If friend list doesn't exist, create one -----------------//
    if (!friendList) {
      
        playerHashTable.addPlayer(currentUsername);
        friendList = playerHashTable.getFriendList(currentUsername);
    }

  
    string friendsFilename = currentUsername + "_friends.txt";
    string requestsFilename = currentUsername + "_friend_requests.txt";


	//-------------------- Load Friends and Requests from Files -----------------//
    friendList->loadFriendsFromFile(currentUsername);
    friendList->loadFriendRequestsFromFile(currentUsername);


	//------------------ Background and Textures -----------------//
    while (showingFriendList) 
    {
        window.clear();  


       Texture texture;
        if (!texture.loadFromFile("images/frndz.jpg")) 
        {  
            cout<< "Error loading texture" << endl;
            
        }


       Sprite sprite(texture);


        window.draw(sprite);

        bgFriends.Draw(window);


		
   
        Text titleText;
        titleText.setFont(font);
        titleText.setString("Friends List");
        titleText.setCharacterSize(40);
        titleText.setFillColor(Color(214, 46, 155));
        titleText.setPosition(400, 50);

        Text friendsHeaderText;
        friendsHeaderText.setFont(font);
        friendsHeaderText.setString("Friends:");
        friendsHeaderText.setCharacterSize(30);
        friendsHeaderText.setFillColor(Color::Black);
        friendsHeaderText.setPosition(400, 120);

      
        Text friendsListText;
        friendsListText.setFont(font);
        friendsListText.setCharacterSize(20);
        friendsListText.setFillColor(Color(0, 0, 139));
        friendsListText.setPosition(400, 160);

        

		//------------------- Get Friends List -----------------//
        string friends[100];
        int friendCount = 0;
        friendList->getFriendsList(friends, friendCount);

        string friendsDisplay;
        for (int i = 0; i < friendCount; ++i) {
            friendsDisplay += friends[i] + "\n";
        }

        if (friendsDisplay.empty()) {
            friendsDisplay = "No friends yet.\n";
        }
        friendsListText.setString(friendsDisplay);

		//------------------ Friend Requests Header -----------------//
     
        Text requestHeaderText;
        requestHeaderText.setFont(font);
        requestHeaderText.setString("Friend Requests:");
        requestHeaderText.setCharacterSize(30);
        requestHeaderText.setFillColor(Color::Black);
        requestHeaderText.setPosition(400, 300);

        Text requestsListText;
        requestsListText.setFont(font);
        requestsListText.setCharacterSize(20);
        requestsListText.setFillColor(Color(0, 0, 139));
        requestsListText.setPosition(400, 340);

        
		//------------------- Get Friend Requests List -----------------//
        string requests[100];
        int requestCount = 0;
        friendList->getFriendRequestsList(requests, requestCount);

        string requestsDisplay;
        for (int i = 0; i < requestCount; ++i) {
            requestsDisplay += requests[i] + "\n";
        }

        if (requestsDisplay.empty()) {
            requestsDisplay = "No friend requests.\n";
        }
        requestsListText.setString(requestsDisplay);

     
        Text instructionText;
        instructionText.setFont(font);
        instructionText.setString("S: Send Friend Request\nA: Accept Request\nR: Reject Request\nESC: Back to Menu");
            instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::Red);
        instructionText.setPosition(400, 435);

       
        window.draw(titleText);
        window.draw(friendsHeaderText);
        window.draw(friendsListText);
        window.draw(requestHeaderText);
        window.draw(requestsListText);
        window.draw(instructionText);

        window.display();

        
		//------------------ Event Handling for Friend List -----------------//
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

           
            if (e.type == Event::KeyPressed) {
                switch (e.key.code) 
                {
					//------------------ Escape Key -----------------//
                case Keyboard::Escape:
                  
                    showingFriendList = false;
                    showingMainMenu = true;
                    return;

					//------------------ Send Friend Request -----------------//
                case Keyboard::S:
                    
                    sendFriendRequest();
                    friendList->loadFriendsFromFile(currentUsername);
                    friendList->loadFriendRequestsFromFile(currentUsername);
                    break;

					//------------------ Accept Friend Request -----------------//
                case Keyboard::A:
                 
                {
                    ifstream requestFile(requestsFilename);
                    string firstRequest;
                    if (getline(requestFile, firstRequest)) 
                    {
                        requestFile.close();

                       
						//----- Temporary file for rewriting requests-----//
                        string tempFilename = currentUsername + "_temp_requests.txt";
                        ofstream tempFile(tempFilename);

                       
                        ifstream oldRequestFile(requestsFilename);
                        string tempRequest;
                        bool firstSkipped = false;

						//-- Copy all requests except the first one
                        while (getline(oldRequestFile, tempRequest)) 
                        {
                            if (!firstSkipped) {
                                firstSkipped = true;
                                continue;
                            }
                            tempFile << tempRequest << endl;
                        }

                        oldRequestFile.close();
                        tempFile.close();

						//-- Replace original requests file
                        remove(requestsFilename.c_str());
                        rename(tempFilename.c_str(), requestsFilename.c_str());

                       
						//-------------- Accept the friend request-----------------------//
                        bool accepted = friendList->acceptFriendRequestWithFile(currentUsername, firstRequest);

                        if (accepted) {
                            // Update PlayerHashTable
                            FriendList* requesterFriendList = playerHashTable.getFriendList(firstRequest);
                            if (requesterFriendList) {
                                requesterFriendList->addFriend(currentUsername);
                                requesterFriendList->saveFriendsToFile(firstRequest);
                            }

							//-------------RELOAD FRIENDS AND REQUESTS-----------------//
                            friendList->loadFriendsFromFile(currentUsername);
                            friendList->loadFriendRequestsFromFile(currentUsername);

                            cout << "Accepted friend request from: " << firstRequest << endl;
                        }
                    }
                }
                break;

				//------------------ Reject Friend Request -----------------//
                case Keyboard::R:
                   
                {
         
                    ifstream requestFile(requestsFilename);
                    string firstRequest;
					//-- Read the first request
                    if (getline(requestFile, firstRequest)) 
                    {
                        requestFile.close();

                        
                        string tempFilename = currentUsername + "_temp_requests.txt";
                        ofstream tempFile(tempFilename);

                     
                        ifstream oldRequestFile(requestsFilename);
                        string tempRequest;
                        bool firstSkipped = false;

                     
						//-- Copy all requests except the first one
                        while (getline(oldRequestFile, tempRequest)) {
                            if (!firstSkipped) {
                                firstSkipped = true;
                                continue;
                            }
                            tempFile << tempRequest << endl;
                        }

                        oldRequestFile.close();
                        tempFile.close();

                        remove(requestsFilename.c_str());
                        rename(tempFilename.c_str(), requestsFilename.c_str());

                       
						//-------------- Reject the friend request-----------------------//
                        friendList->loadFriendRequestsFromFile(currentUsername);

                        cout << "Rejected friend request from: " << firstRequest << endl;
                    }
                }
                break;
                }
            }
        }
    }
}

void Menu::sendFriendRequest() {
    RenderWindow usersWindow(VideoMode(600, 500), "Send Friend Request");

   
    Font windowFont;
    if (!windowFont.loadFromFile("arial.ttf")) {
        cout<< "Error loading font" << endl;
        return;
    }

    Text titleText;
    titleText.setFont(windowFont);
    titleText.setString("Select a User to Send Friend Request");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50, 50);

  
	//------------------ Get current user and valid usernames -----------------//
    string currentUsername = currentProfile->getUsername();
    string validUsernames[100];  
    int userCount = 0;

    

	//------------------ Get the list of users from the authentication system -----------------//

    UserNode* current = auth.getUserListHead();
    while (current != NULL) 
    {
       
        if (current->username != currentUsername) {
           
			//-------- Check if the user is already a friend or has sent/received requests -----------------//
            FriendList* currentUserFriendList = playerHashTable.getFriendList(currentUsername);


            if (currentUserFriendList) {
                
                bool isFriend = false;
                string friendsFilename = currentUsername + "_friends.txt";
                ifstream friendsFile(friendsFilename);
                string friendLine;

                while (getline(friendsFile, friendLine)) {
                    if (friendLine == current->username) {
                        isFriend = true;
                        break;
                    }
                }
                friendsFile.close();

				//-------Check if the user is already a friend or has sent/received requests-----------------//
                if (!isFriend) {
                  
                    bool outgoingRequestExists = false;
                    string outgoingRequestsFilename = currentUsername + "_friend_requests.txt";
                    ifstream outgoingRequestsFile(outgoingRequestsFilename);
                    string outgoingRequestLine;

                    while (getline(outgoingRequestsFile, outgoingRequestLine)) 
                    {
                        if (outgoingRequestLine == current->username) 
                        {
                            outgoingRequestExists = true;
                            break;
                        }
                    }
                    outgoingRequestsFile.close();

                    
					//------------------ Check if the user has sent a request to the current user -----------------//
                    bool incomingRequestExists = false;
                    string incomingRequestsFilename = current->username + "_friend_requests.txt";
                    ifstream incomingRequestsFile(incomingRequestsFilename);
                    string incomingRequestLine;

                    while (getline(incomingRequestsFile, incomingRequestLine)) {
                        if (incomingRequestLine == currentUsername) {
                            incomingRequestExists = true;
                            break;
                        }
                    }
                    incomingRequestsFile.close();

					//------------------ If the user is not a friend and has not sent/received requests, add to valid usernames -----------------//
                    if (!outgoingRequestExists && !incomingRequestExists) {
                        validUsernames[userCount++] = current->username;
                    }
                }
            }
        }
        current = current->next;
    }


	//------------------ If no valid users, show message -----------------//

    if (userCount == 0) 
    {
       
        Text noUsersText;
        noUsersText.setFont(windowFont);
        noUsersText.setString("No users available to send friend request.");
        noUsersText.setCharacterSize(20);
        noUsersText.setFillColor(Color::Red);
        noUsersText.setPosition(50, 150);

        usersWindow.clear(Color(50, 50, 50));
        usersWindow.draw(titleText);
        usersWindow.draw(noUsersText);
        usersWindow.display();

      
       sleep(milliseconds(2000));
        return;
    }

   
    Text* userTexts = new Text[userCount];

  
	//------------------ Create text objects for each valid username -----------------//
    for (int index = 0; index < userCount; ++index) {
        
        userTexts[index].setFont(windowFont);
        userTexts[index].setString("\t" + validUsernames[index] + " (SEND)");
        userTexts[index].setCharacterSize(25);
        userTexts[index].setFillColor(Color::White);
        userTexts[index].setPosition(50, 150 + index * 40);
    }

 
    int selectedIndex = 0;

	//------------------ Main loop for user selection -----------------//
    while (usersWindow.isOpen()) {
        Event e;
        while (usersWindow.pollEvent(e)) {
            if (e.type == Event::Closed)
                usersWindow.close();

   
            if (e.type == Event::KeyPressed) 
            {
				//------------------ Move selection down -----------------//
                if (e.key.code == Keyboard::Down) {
                    
                    selectedIndex = (selectedIndex + 1) % userCount;
                }
				//------------------ Move selection up -----------------//
                else if (e.key.code == Keyboard::Up) {
                    // Move selection up
                    selectedIndex = (selectedIndex - 1 + userCount) % userCount;
                }

				//------------------ Send friend request -----------------//
                else if (e.key.code == Keyboard::Return) 
                {
                    
                    string selectedUser = validUsernames[selectedIndex];

                    sendFriendRequestToUser(currentUsername, selectedUser);

					//------------------ Update the friend list of the current user -----------------//
                    FriendList* currentUserFriendList = playerHashTable.getFriendList(currentUsername);
                    if (currentUserFriendList) {
                        currentUserFriendList->sendFriendRequest(selectedUser);
                    }

                    
					//------------------ Update the friend list of the selected user -----------------//

                    Text confirmText;
                    confirmText.setFont(windowFont);
                    confirmText.setString("Friend request sent to " + selectedUser);
                    confirmText.setCharacterSize(20);
                    confirmText.setFillColor(Color::Green);
                    confirmText.setPosition(50, 450);

                    usersWindow.clear(Color(50, 50, 50));
                    usersWindow.draw(titleText);


					//------------------ Draw usernames with highlighting -----------------//
                    for (int i = 0; i < userCount; ++i) 
                    {
                        usersWindow.draw(userTexts[i]);
                    }
                    usersWindow.draw(confirmText);
                    usersWindow.display();

             
                   sleep(milliseconds(2000));

                   
                    usersWindow.close();
                }
                else if (e.key.code == Keyboard::Escape) {
                    
                    usersWindow.close();
                }
            }
        }

       
        usersWindow.clear(Color(50, 50, 50));

    
        usersWindow.draw(titleText);

		//------------------ Draw usernames with highlighting -----------------//

        for (int i = 0; i < userCount; ++i) {
           
            if (i == selectedIndex) {
                userTexts[i].setFillColor(Color::Yellow);
            }
            else {
                userTexts[i].setFillColor(Color::White);
            }
            usersWindow.draw(userTexts[i]);
        }

      
        usersWindow.display();
    }

    delete[] userTexts;
}

void Menu::displayFriendRequests() {
   
    //-----------------------VARIABLES--------------------------//
    string currentUsername = currentProfile->getUsername();
    FriendList friendList;
    string requestsFilename = currentUsername + "_friend_requests.txt";
    ifstream requestsFile(requestsFilename);
    string* requestsList = new string[100];  
    int requestCount = 0;
    string request;


	//------------------ Read friend requests from file -----------------//
    while (getline(requestsFile, request) && requestCount < 100) {
        requestsList[requestCount++] = request;
    }
    requestsFile.close();

    RenderWindow requestsWindow(VideoMode(400, 300), "Friend Requests");

 
    Font windowFont;
    if (!windowFont.loadFromFile("arial.ttf")) 
    {
        cout<< "Error loading font" << endl;
        delete[] requestsList;
        return;
    }

    //
    Text titleText, instructionText;
    titleText.setFont(windowFont);
    instructionText.setFont(windowFont);

    titleText.setString("Incoming Friend Requests");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50, 50);

    instructionText.setString("Use Arrow Keys to Navigate\nEnter to Accept\nBackspace to Reject");
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(Color::White);
    instructionText.setPosition(50, 250);

  

	//------------------ Highlight selected request -----------------//
    int selectedIndex = 0;

    while (requestsWindow.isOpen()) {
        Event e;
        while (requestsWindow.pollEvent(e)) 
        {
            if (e.type == Event::Closed)
                requestsWindow.close();
            
            if (e.type == Event::KeyPressed) {
                if (requestCount > 0) {
                    if (e.key.code == Keyboard::Down) {
                        
                        selectedIndex = (selectedIndex + 1) % requestCount;
                    }
                    else if (e.key.code == Keyboard::Up) {
                       
                        selectedIndex = (selectedIndex - 1 + requestCount) % requestCount;
                    }

					//---------------- Accept friend request -----------------//
                    else if (e.key.code == Keyboard::Return) {
                      

                        
                        string selectedUser = requestsList[selectedIndex];
                        bool accepted = friendList.acceptFriendRequestWithFile(currentUsername, selectedUser);

                        if (accepted) 
                        {
                            
                            for (int i = selectedIndex; i < requestCount - 1; i++) {
                                requestsList[i] = requestsList[i + 1];
                            }
                            requestCount--;

							//------------------ Update the friend list of the selected user -----------------//
                            if (selectedIndex >= requestCount) {
                                selectedIndex = requestCount > 0 ? requestCount - 1 : 0;
                            }

                            
                            cout << "Accepted friend request from: " << selectedUser << endl;
                        }
                    }

					//---------------- Reject friend request -----------------//

                    else if (e.key.code == Keyboard::Backspace) {
                        
                        string selectedUser = requestsList[selectedIndex];

                        string requestsFilename = currentUsername + "_friend_requests.txt";

                      
                        ifstream inputFile(requestsFilename);
                        ofstream tempFile("temp_requests.txt");

                        string line;
                        while (getline(inputFile, line)) {
                            if (line != selectedUser) {
                                tempFile << line << endl;
                            }
                        }

                        inputFile.close();
                        tempFile.close();

          
                        remove(requestsFilename.c_str());
                        rename("temp_requests.txt", requestsFilename.c_str());

                       

						//------------------ Update the friend list of the selected user -----------------//

                        for (int i = selectedIndex; i < requestCount - 1; i++) 
                        {
                            requestsList[i] = requestsList[i + 1];
                        }
                        requestCount--;

                      
						//------------------ Update selected index -----------------//
                        if (selectedIndex >= requestCount) {
                            selectedIndex = requestCount > 0 ? requestCount - 1 : 0;
                        }

                      
                        cout << "Rejected friend request from: " << selectedUser << endl;
                    }
                }

               
                if (e.key.code == Keyboard::Escape) {
                    requestsWindow.close();
                }
            }
        }

      
        requestsWindow.clear(Color(50, 50, 50));
        requestsWindow.draw(titleText);
        requestsWindow.draw(instructionText);


		//------------------ Draw friend requests header -----------------//
        for (int i = 0; i < requestCount; i++) {
            Text requestText;
            requestText.setFont(windowFont);
            requestText.setString(requestsList[i]);
            requestText.setCharacterSize(25);

			//------------------ Highlight selected request -----------------//
            if (i == selectedIndex) 
            {
                requestText.setFillColor(Color::Yellow);
            }
            else 
            {
                requestText.setFillColor(Color::White);
            }

            requestText.setPosition(50, 100 + i * 40);
            requestsWindow.draw(requestText);
        }

        
        requestsWindow.display();
    }

    delete[] requestsList;
}



void Menu::acceptFriendRequest() {
    string currentUsername = currentProfile->getUsername();
    string requestsFileName = currentUsername + "_friend_requests.txt";
    string friendsFileName = currentUsername + "_friends.txt";

    
    ifstream requestsFile(requestsFileName);
    string requester;

    if (getline(requestsFile, requester)) 
    {
        requestsFile.close();

        
		//------------------- Add requester to friends file -----------------//
        ofstream friendsFile(friendsFileName, ios_base::app);
        friendsFile << requester << endl;
        friendsFile.close();

      
		//------------------ Remove the request from requests file -----------------//
        ifstream inputFile(requestsFileName);
        ofstream tempFile("temp_requests.txt");
        string line;
        bool firstLineSkipped = false;

        while (getline(inputFile, line)) {
            if (!firstLineSkipped) {
                firstLineSkipped = true;
                continue;
            }
            tempFile << line << endl;
        }

        inputFile.close();
        tempFile.close();

        remove(requestsFileName.c_str());
        rename("temp_requests.txt", requestsFileName.c_str());

        cout << "Accepted friend request from: " << requester << endl;
    }
}

void Menu::rejectFriendRequest() 
{
    string currentUsername = currentProfile->getUsername();
    string requestsFileName = currentUsername + "_friend_requests.txt";

    
    ifstream requestsFile(requestsFileName);
    string requester;

	// ------------------ Read the first request -----------------//
    if (getline(requestsFile, requester)) {
        requestsFile.close();

        
        ifstream inputFile(requestsFileName);
        ofstream tempFile("temp_requests.txt");
        string line;
        bool firstLineSkipped = false;



		//------------------ Remove the request from requests file -----------------//
        while (getline(inputFile, line)) {
            if (!firstLineSkipped) {
                firstLineSkipped = true;
                continue;
            }
            tempFile << line << endl;
        }

        inputFile.close();
        tempFile.close();

       
        remove(requestsFileName.c_str());
        rename("temp_requests.txt", requestsFileName.c_str());

        cout << "Rejected friend request from: " << requester << endl;
    }
}



void Menu::syncPlayerNames() 
{
    
    if (matchedPlayer1.empty()) {
        matchedPlayer1 = currentProfile->getUsername();
    }
    if (!isSinglePlayerSelected && matchedPlayer2.empty()) {
        matchedPlayer2 = "Player2";
    }
    secondPlayerName = matchedPlayer2;
}

int Menu::themeSelection()
{
    showingMainMenu = false;
    Text titleText, theme1Text, theme2Text, theme3Text;


	//------------------ Load font -----------------//
    titleText.setFont(font);
    titleText.setString("Select Theme");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(400, 100);

    theme1Text.setFont(font);
    theme1Text.setString("1. Ocean Theme");
    theme1Text.setCharacterSize(20);
    theme1Text.setFillColor(Color::White);
    theme1Text.setPosition(410, 180);

    theme2Text.setFont(font);
    theme2Text.setString("2. Forest Theme");
    theme2Text.setCharacterSize(20);
    theme2Text.setFillColor(Color::White);
    theme2Text.setPosition(410, 250);

    theme3Text.setFont(font);
    theme3Text.setString("3.Spooky Theme");
    theme3Text.setCharacterSize(20);
    theme3Text.setFillColor(Color::White);
    theme3Text.setPosition(410, 320);

    while (window.isOpen()) {


        window.clear();
       Texture texture;
        if (!texture.loadFromFile("images/mm.jpg")) {  
            cout<< "Error loading texture" << endl;
        }


       Sprite sprite(texture);


        window.draw(sprite);
        window.draw(titleText);
        window.draw(theme1Text);
        window.draw(theme2Text);
        window.draw(theme3Text);
        window.display();

        Event e;
		//------------------ Event Handling -----------------//
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    showingMainMenu = true;
                    return -1;
                }
            }

			//------------------ Mouse Click Handling -----------------//
            if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left) {
               Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (theme1Text.getGlobalBounds().contains(mousePos)) 
                    return 1;
                if (theme2Text.getGlobalBounds().contains(mousePos))
                    return 2;
                if (theme3Text.getGlobalBounds().contains(mousePos)) 
                    return 3;
            }
        }
    }

    return -1; 
}


void Menu::showFriendsForMultiplayer() {
    
   Font font;
    if (!font.loadFromFile("Courier Prime Bold.ttf")) {
        cout<< "Error loading font" << endl;
     
    }


    string currentUsername = currentProfile->getUsername();

    FriendList* friendList = playerHashTable.getFriendList(currentUsername);
	//------------------- If friend list doesn't exist, create one -----------------//
    if (!friendList) 
    {
        playerHashTable.addPlayer(currentUsername);
        friendList = playerHashTable.getFriendList(currentUsername);
    }
    friendList->loadFriendsFromFile(currentUsername);



    string friendsList[100];
    int friendCount = 0;
    friendList->getFriendsList(friendsList, friendCount);

   
    RenderWindow friendsWindow(VideoMode(600, 500), "Select Friend for Multiplayer");

    //// Font setup
    //Font font;
    //if (!font.loadFromFile("arial.ttf")) {
    //    cout<< "Error loading font" << endl;
    //    return;
    //}

 
	//------------------ Background and Textures -----------------//
    Text titleText;
    titleText.setFont(font);
    titleText.setString("Select a Friend to Play With");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50, 50);

    
    Text noFriendsText;
    noFriendsText.setFont(font);
    noFriendsText.setString("No friends found. Add friends first!");
    noFriendsText.setCharacterSize(20);
    noFriendsText.setFillColor(Color::Red);
    noFriendsText.setPosition(50, 150);




	//------------------ Create text objects for each friend -----------------//
    Text friendTexts[100];

    // Populate text objects
    for (int index = 0; index < friendCount; ++index) {
        friendTexts[index].setFont(font);
        friendTexts[index].setString(to_string(index + 1) + ". " + friendsList[index]);
        friendTexts[index].setCharacterSize(25);
        friendTexts[index].setFillColor(Color::White);
        friendTexts[index].setPosition(50, 150 + index * 40);
    }

    
    int selectedIndex = 0;

    //// Instructions text
    //Text instructionText;
    //instructionText.setFont(font);
    //instructionText.setString("Use Arrow Keys to Navigate\nENTER to Select\nESC to Cancel");
    //instructionText.setCharacterSize(20);
    //instructionText.setFillColor(Color::Yellow);
    //instructionText.setPosition(50, 450);

    while (friendsWindow.isOpen()) {
        Event e;


        while (friendsWindow.pollEvent(e)) {
            if (e.type == Event::Closed)
                friendsWindow.close();

            
            if (e.type == Event::KeyPressed) {
                if (friendCount > 0) {

                   
                    if (e.key.code == Keyboard::Down) {
                        
						//------------------ Move selection down -----------------//
                        selectedIndex = (selectedIndex + 1) % friendCount;
                    }
                    else if (e.key.code == Keyboard::Up) 
                    {
						//------------------ Move selection up -----------------//
                        selectedIndex = (selectedIndex - 1 + friendCount) % friendCount;
                    }
					//------------------ Select friend for multiplayer -----------------//
                    else if (e.key.code == Keyboard::Return) {
                        
						//------------------ Check if the selected friend is valid -----------------//
                        if (friendCount > 0) {
                            matchedPlayer1 = currentUsername;
                            matchedPlayer2 = friendsList[selectedIndex];

                           

							//------------------ Set multiplayer mode and start game -----------------//
                            isSinglePlayerSelected = false;
                            showingModeSelection = false;
                            gameStarted = true;

                            cout << "Multiplayer with friend: " << matchedPlayer2 << endl;

                            
                            friendsWindow.close();
                            return;
                        }
                    }
                }

   
                if (e.key.code == Keyboard::Escape) 
                {
                    showingModeSelection = true;
                    friendsWindow.close();
                    return;
                }
            }
        }

       
        friendsWindow.clear(Color::Black);

    
        friendsWindow.draw(titleText);

		
        
        if (friendCount > 0) {
           
			//------------------ Draw friend list header -----------------//
            for (int i = 0; i < friendCount; ++i) {
              
                if (i == selectedIndex) 
                {
                    friendTexts[i].setFillColor(Color::Magenta);
                }
                else 
                {
                    friendTexts[i].setFillColor(Color::White);
                }
                friendsWindow.draw(friendTexts[i]);
            }
        }
        else 
        {
            friendsWindow.draw(noFriendsText);
        }

        // Draw instructions
     /*   friendsWindow.draw(instructionText);*/

       
        friendsWindow.display();
    }
}