#include "menu.h"
#include "level.h"
#include "player.h"
#include "theme.h"
#include "Sprite.h"


Menu::Menu(RenderWindow& window, const std::string& username, Authentication& authRef)
    : window(window), auth(authRef)
{
    font.loadFromFile("arial.ttf");  // Load font

    // Create player profile
    currentProfile = new PlayerProfile(username);
    // Initialize player names
    matchedPlayer1 = username;
    matchedPlayer2 = "";

    initMainMenu();
    initEndMenu();
    leaderboard.loadLeaderboard();

    // Initialize state variables
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






// Main Menu Logic
void Menu::initMainMenu()
{
    startText.setFont(font);
    startText.setString("Start Game");
    startText.setPosition(200, 100);
    startText.setCharacterSize(30);
    startText.setFillColor(Color::White);

    selectLevelText.setFont(font);
    selectLevelText.setString("Select Level");
    selectLevelText.setPosition(200, 150);
    selectLevelText.setCharacterSize(30);
    selectLevelText.setFillColor(Color::White);

    leaderboardText.setFont(font);
    leaderboardText.setString("Leaderboard");
    leaderboardText.setPosition(200, 200);
    leaderboardText.setCharacterSize(30);
    leaderboardText.setFillColor(Color::White);

    profileText.setFont(font);
    profileText.setString("Player Profile");
    profileText.setPosition(200, 250);
    profileText.setCharacterSize(30);
    profileText.setFillColor(Color::White);

    //theme




    themeSelectionText.setFont(font);
    themeSelectionText.setString("Theme Selection");
    themeSelectionText.setPosition(200, 350);
    themeSelectionText.setCharacterSize(30);
    themeSelectionText.setFillColor(sf::Color::White);









    //friendlist 

    // Add Friends List menu option
    friendListText.setFont(font);
    friendListText.setString("Friends List");
    friendListText.setPosition(200, 300);  // Position after Profile
    friendListText.setCharacterSize(30);
    friendListText.setFillColor(Color::White);

}

std::string Menu::getValidPlayerName(const std::string& name, const std::string& defaultName) {
    if (name.empty()) {
        return defaultName;
    }
    return name;
}

// End Menu Logic
void Menu::initEndMenu()
{
    endText.setFont(font);
    endText.setString("Game Over");
    endText.setPosition(200, 100);
    endText.setCharacterSize(30);
    endText.setFillColor(Color::White);

    finalScoreText.setFont(font);
    finalScoreText.setString("Final Score: 1000");  // Update dynamically
    finalScoreText.setPosition(200, 150);
    finalScoreText.setCharacterSize(30);
    finalScoreText.setFillColor(Color::White);
}

// Display Main Menu
void Menu::displayMainMenu()
{
    window.clear();
	bgSprite.Draw(window);;
    window.draw(startText);
    window.draw(selectLevelText);
    window.draw(leaderboardText);
    window.draw(profileText);
    window.draw(friendListText);
    window.draw(themeSelectionText);
	// Draw background sprite
    window.display();

}



void Menu::setSecondPlayerName(const std::string& playerName) {
    secondPlayerName = playerName;
    matchedPlayer2 = playerName;
}



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
                startGame();  // Transition to mode selection
            }
            else if (e.key.code == Keyboard::Down) {
                selectLevel();  // Show level selection
            }
            else if (e.key.code == Keyboard::Up) {
                showLeaderboard();  // Show leaderboard
            }
            else if (e.key.code == Keyboard::Right) {
                showProfile();  // Show player profile
            }

        }

        // Add mouse click handling for menu items
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);

            // Check if Start Game button is clicked
            if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 100 && mousePosition.y <= 130) {
                // Show matchmaking screen and switch to multiplayer mode
                std::string player1 = currentProfile->getUsername();  // Current logged-in player
                std::string player2 = leaderboard.getTopScorer();  // Get top scorer

                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  // Set Player 1 (logged-in user)
                    matchedPlayer2 = player2;  // Set Player 2 (top scorer)

                    // Switch to multiplayer mode
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;

                    cout << "Matchmaking: Player 1 (" << matchedPlayer1 << ") vs Player 2 (" << matchedPlayer2 << ")" << endl;
                }
                else {
                    cout << "No players in queue or unable to match. Try again later!" << endl;
                }

                startGame();  // Transition to mode selection
            }
            // Check if Select Level button is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 150 && mousePosition.y <= 180) {
                selectLevel();  // Show level selection
            }
            // Check if Leaderboard button is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 200 && mousePosition.y <= 230) {
                showLeaderboard();  // Show leaderboard
            }
            // Check if Profile button is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 250 && mousePosition.y <= 280) {
                showProfile();  // Show player profile
            }
            // Check if Friends List button is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 300 && mousePosition.y <= 330) {
                showingMainMenu = false;
                showFriendList();
            }
			else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
				mousePosition.y >= 350 && mousePosition.y <= 380) 
            {
                int themeId = -1;
				themeId = themeSelection();
				cout << "Theme ID selected: " << themeId << endl;

                if (themeId > 0 && themeId < 3)
                {

                    showingMainMenu = true;

                    AVLTree themeSearching;
                    AVLNode* themeNode = themeSearching.search(themeId);

                    if(themeNode)
                    {
                        MySprite tempSprite(themeNode->themepaths[0], 0, 0);
						cout << endl << themeNode->themepaths[0] << endl;
						this->bgSprite.setTexture(tempSprite.getPath());
                    }
                }
			}

        }
    }
}
// Mode Selection Screen
void Menu::selectMode()
{
    window.clear();

    Text modeSelectionText;
    modeSelectionText.setFont(font);
    modeSelectionText.setString("Select Mode:");
    modeSelectionText.setCharacterSize(30);
    modeSelectionText.setFillColor(Color::White);
    modeSelectionText.setPosition(200, 100);

    Text singlePlayerText;
    singlePlayerText.setFont(font);
    singlePlayerText.setString("1. Single Player");
    singlePlayerText.setCharacterSize(30);
    singlePlayerText.setFillColor(Color::White);
    singlePlayerText.setPosition(200, 150);

    Text multiplayerText;
    multiplayerText.setFont(font);
    multiplayerText.setString("2. Multiplayer");
    multiplayerText.setCharacterSize(30);
    multiplayerText.setFillColor(Color::White);
    multiplayerText.setPosition(200, 200);

    Text matchmakingText;
    matchmakingText.setFont(font);
    matchmakingText.setString("3. Matchmaking");
    matchmakingText.setCharacterSize(30);
    matchmakingText.setFillColor(Color::White);
    matchmakingText.setPosition(200, 250);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(200, 300);

	bgSprite.Draw(window);
    window.draw(modeSelectionText);
    window.draw(singlePlayerText);
    window.draw(multiplayerText);
    window.draw(matchmakingText);
    window.draw(backText);
    window.display();
}
// Handle Mode Selection Input
void Menu::handleModeSelection()
{
    Event e;
    while (window.pollEvent(e))
    {
        if (e.type == Event::Closed)
            window.close();

        if (e.type == Event::KeyPressed)
        {
            if (e.key.code == Keyboard::Num1 || e.key.code == Keyboard::Numpad1) {
                isSinglePlayerSelected = true;
                showingModeSelection = false;
                gameStarted = true;
                cout << "Single Player Mode Selected!" << endl;
            }
            else if (e.key.code == Keyboard::Num2 || e.key.code == Keyboard::Numpad2) {
                // Create a separate window for second player name input
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

                // After getting second player name
                // After getting second player name
                if (!matchedPlayer2.empty()) {
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;
                    // Sync the player names
                    syncPlayerNames();
                    cout << "Multiplayer Mode Selected with Player 2: " << matchedPlayer2 << endl;
                }
            }
            else if (e.key.code == Keyboard::Num3 || e.key.code == Keyboard::Numpad3) {
                // Existing matchmaking mode logic
                isMatchmakingEnabled = true;
                showingModeSelection = false;
                showingMatchmakingQueue = true;

                // Get Player 1 and Player 2 (top scorer)
                std::string player1 = currentProfile->getUsername();  // Current logged-in player
                std::string player2 = leaderboard.getTopScorer();  // Get top scorer

                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  // Set Player 1
                    matchedPlayer2 = player2;  // Set Player 2
                }
                else {
                    // Fallback if no top scorer found
                    matchedPlayer2 = "Player2";
                }

                cout << "Matchmaking Mode Selected!" << endl;
            }
            else if (e.key.code == Keyboard::Escape) {
                // Go back to main menu
                showingModeSelection = false;
                showingMainMenu = true;
            }
        }

        // Add mouse click handling with similar logic
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);

            // Check if Single Player option is clicked
            if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 150 && mousePosition.y <= 180) {
                isSinglePlayerSelected = true;
                showingModeSelection = false;
                gameStarted = true;
                cout << "Single Player Mode Selected!" << endl;
            }
            // Check if Multiplayer option is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 200 && mousePosition.y <= 230) {
                // Create a separate window for second player name input
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

                // After getting second player name
                if (!matchedPlayer2.empty()) {
                    isSinglePlayerSelected = false;
                    showingModeSelection = false;
                    gameStarted = true;
                    cout << "Multiplayer Mode Selected with Player 2: " << matchedPlayer2 << endl;
                }
            }
            // Check if Matchmaking option is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 250 && mousePosition.y <= 280) {
                // Existing matchmaking mode logic
                isMatchmakingEnabled = true;
                showingModeSelection = false;
                showingMatchmakingQueue = true;

                // Get Player 1 and Player 2 (top scorer)
                std::string player1 = currentProfile->getUsername();  // Current logged-in player
                std::string player2 = leaderboard.getTopScorer();  // Get top scorer

                if (!player2.empty() && player2 != player1) {
                    matchedPlayer1 = player1;  // Set Player 1
                    matchedPlayer2 = player2;  // Set Player 2
                }
                else {
                    // Fallback if no top scorer found
                    matchedPlayer2 = "Player2";
                }

                cout << "Matchmaking Mode Selected!" << endl;
            }
        }
    }
}




void Menu::showMatchmakingQueue() {
    window.clear();

    // Create the title text for matchmaking queue
    Text matchmakingTitleText;
    matchmakingTitleText.setFont(font);
    matchmakingTitleText.setString("Matchmaking Queue");
    matchmakingTitleText.setCharacterSize(30);
    matchmakingTitleText.setFillColor(Color::White);
    matchmakingTitleText.setPosition(200, 100);

    // Get the current player username
    std::string currentUsername = currentProfile->getUsername();

    // Get the top player from the leaderboard
    std::string topPlayer = matchmaker.getTopPlayerFromLeaderboard();

    // If there s only one player in the queue, we need to find the next available top player
    if (matchmaker.getQueueSize() < 2) {
        if (!topPlayer.empty() && topPlayer != currentUsername) {
            // Try to find the next player after the top player
            std::string nextTopPlayer = matchmaker.getNextTopPlayerFromLeaderboard(topPlayer);  // Get the next top player

            if (!nextTopPlayer.empty()) {
                // Show the next available player if found
                queueStatusText.setString("Ready to match with: " + nextTopPlayer);
            }
            else {
                // If no next player found, show that not enough players are in the queue
                queueStatusText.setString("Not enough players in queue.");
            }
        }
        else {
            // If there s no top player or top player is the same as the current player
            queueStatusText.setString("Waiting for other players...");
        }
    }
    else {
        // If there s more than one player in the queue, match with the top player
        if (!topPlayer.empty() && topPlayer != currentUsername) {
            queueStatusText.setString("Ready to match with: " + topPlayer);  // Show the top player to match with
        }
        else {
            // If top player is unavailable or already in the queue, show the queue size
            queueStatusText.setString("Players in Queue: " + std::to_string(matchmaker.getQueueSize()));
        }
    }

    // Style and position the queue status text
    queueStatusText.setCharacterSize(30);
    queueStatusText.setFillColor(Color::White);
    queueStatusText.setPosition(200, 150);

    // Instruction text for matchmaking controls
    Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("Press ENTER to start matchmaking\nPress ESC to cancel");
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(Color::White);
    instructionText.setPosition(200, 250);

    // Drawing all elements to the window
    window.draw(matchmakingTitleText);
    window.draw(queueStatusText);
    window.draw(instructionText);

    // Display the window content
    window.display();
}


std::string Menu::promptForSecondPlayerName() {
    // This could be a separate window or dialog
    // For simplicity, we'll use console input in this example
    std::string secondPlayerName;
    std::cout << "Enter the name of the second player: ";
    std::cin >> secondPlayerName;
    return secondPlayerName;
}


bool Menu::startMatchmaking() {
    // Get the current username
    std::string currentUsername = currentProfile->getUsername();

    // Populate queue with players from leaderboard, excluding the current user
    matchmaker.populateQueueFromLeaderboard(currentUsername);

    // Display the queue size and state for debugging
    std::cout << "Current Queue Size: " << matchmaker.getQueueSize() << std::endl;
    matchmaker.debugPrintQueue();  // Print the state of the queue for debugging

    // Get the top player from the leaderboard
    std::string topPlayerUsername = matchmaker.getTopPlayerFromLeaderboard();
    std::cout << "Top player from leaderboard: " << topPlayerUsername << std::endl;

    // Always ensure player1 is set to current user
    matchedPlayer1 = currentUsername;

    // If the current player is the top player, try to match with other players
    if (topPlayerUsername.empty() || topPlayerUsername == currentUsername) {
        std::cout << "Adding " << currentUsername << " to matchmaking queue..." << std::endl;
        int currentScore = matchmaker.readUserHighestScore(currentUsername);

        // Add the current player to the matchmaking queue with their score
        matchmaker.enterQueue(currentUsername, currentScore);

        // If the queue has only one player, try to match with the next top player from the leaderboard
        if (matchmaker.getQueueSize() < 2) {
            std::string nextTopPlayer = matchmaker.getNextTopPlayerFromLeaderboard(topPlayerUsername);

            if (!nextTopPlayer.empty() && nextTopPlayer != currentUsername) {
                matchedPlayer2 = nextTopPlayer;  // Match with next top player

                // Make sure secondPlayerName is also set
                secondPlayerName = matchedPlayer2;

                std::cout << "Only one player in the queue. Matching "
                    << matchedPlayer1 << " vs " << matchedPlayer2 << std::endl;

                // Log the match
                std::ofstream logFile("matchmaking_log.txt", std::ios_base::app);
                if (logFile.is_open()) {
                    logFile << "Matched players: " << matchedPlayer1
                        << " vs " << matchedPlayer2
                        << " (Next top leaderboard player)" << std::endl;
                    logFile.close();
                }

                // Proceed to start the game
                isSinglePlayerSelected = false;
                gameStarted = true;
                showingMatchmakingQueue = false;
                return true;
            }
            else {
                std::cout << "Not enough players to match. Current queue size: "
                    << matchmaker.getQueueSize() << std::endl;

                // Set default value for matchedPlayer2 even if matchmaking fails
                if (matchedPlayer2.empty()) {
                    matchedPlayer2 = "Player2";
                    secondPlayerName = matchedPlayer2;
                }

                return false;
            }
        }

        // If there are more than one player, continue the regular matchmaking
        if (matchmaker.matchPlayers(matchedPlayer1, matchedPlayer2)) {
            // Make sure secondPlayerName is also set
            secondPlayerName = matchedPlayer2;

            // Successful match
            isSinglePlayerSelected = false;
            gameStarted = true;
            showingMatchmakingQueue = false;
            std::cout << "Match found: " << matchedPlayer1 << " vs " << matchedPlayer2 << std::endl;
            return true;
        }
        else {
            std::cout << "Matchmaking failed. Try again later!" << std::endl;

            // Set default value for matchedPlayer2 even if matchmaking fails
            if (matchedPlayer2.empty()) {
                matchedPlayer2 = "Player2";
                secondPlayerName = matchedPlayer2;
            }

            return false;
        }
    }
    else {
        // If the top player isn't the same as the current player, match with them
        matchedPlayer2 = topPlayerUsername;  // Top player from leaderboard

        // Make sure secondPlayerName is also set
        secondPlayerName = matchedPlayer2;

        std::cout << "Match found: " << matchedPlayer1 << " vs " << matchedPlayer2 << std::endl;

        // Log the match
        std::string logMessage = "Matched players: " + matchedPlayer1 +
            " vs " + matchedPlayer2 +
            " (Top leaderboard player)";
        std::ofstream logFile("matchmaking_log.txt", std::ios_base::app);
        if (logFile.is_open()) {
            logFile << logMessage << std::endl;
            logFile.close();
        }

        // Proceed with matchmaking or start the game
        isSinglePlayerSelected = false;
        gameStarted = true;
        showingMatchmakingQueue = false;
        return true;
    }
}
// Handle matchmaking queue input
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
Level level;  // Define level object

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
    levelSelectionText.setPosition(200, 100);

    Text level1Text;
    level1Text.setFont(font);
    level1Text.setString("1. Level 1 (Easy)");
    level1Text.setCharacterSize(30);
    level1Text.setFillColor(Color::White);
    level1Text.setPosition(200, 150);

    Text level2Text;
    level2Text.setFont(font);
    level2Text.setString("2. Level 2 (Medium)");
    level2Text.setCharacterSize(30);
    level2Text.setFillColor(Color::White);
    level2Text.setPosition(200, 200);

    Text level3Text;
    level3Text.setFont(font);
    level3Text.setString("3. Level 3 (Hard)");
    level3Text.setCharacterSize(30);
    level3Text.setFillColor(Color::White);
    level3Text.setPosition(200, 250);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(200, 300);

	bgSprite.Draw(window);
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
    // Set menu state
    showingMainMenu = false;

    // Leaderboard mode tracking
    // 0 - Single Player
    // 1 - Multiplayer
    // 2 - Total Score
    int currentMode = 0;

    while (true) {
        // Clear the window for leaderboard display
        window.clear();
        bgSprite.Draw(window);

        // Create leaderboard title text
        Text leaderboardTitle;
        leaderboardTitle.setFont(font);
        leaderboardTitle.setString("LEADERBOARD");
        leaderboardTitle.setCharacterSize(40);
        leaderboardTitle.setFillColor(Color::White);
        leaderboardTitle.setPosition(200, 50);

        // Mode selection text
        Text modeText;
        modeText.setFont(font);
        string modeString;
        switch (currentMode) {
        case 0:
            leaderboard.setDisplayMode(0);  // Single Player
            modeString = "Mode: Single Player Scores";
            break;
        case 1:
            leaderboard.setDisplayMode(1);  // Multiplayer
            modeString = "Mode: Multiplayer Matches";
            break;
        case 2:
            leaderboard.setDisplayMode(0);  // Default to single player for combined scores
            modeString = "Mode: Combined Scores";
            break;
        }
        modeText.setString(modeString);
        modeText.setCharacterSize(20);
        modeText.setFillColor(Color::Yellow);
        modeText.setPosition(200, 100);

        // Draw title and mode
        window.draw(leaderboardTitle);
        window.draw(modeText);

        // Temporary array to store sorted leaderboard
        LeaderboardEntry sortedLeaderboard[HEAP_SIZE];
        leaderboard.sortForDisplay(sortedLeaderboard);

        // Display top 10 entries based on current mode
        for (int i = 0; i < 10; i++) {
            Text playerText;
            playerText.setFont(font);

            // Only display if there's an entry
            if (i < leaderboard.heapSize) {
                string displayText;

                switch (currentMode) {
                case 0: // Single Player
                    // Only show non-multiplayer entries
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

                case 1: // Multiplayer
                    // Only show multiplayer entries
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

                case 2: // Total Score
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
            playerText.setPosition(200, 150 + i * 40);

            window.draw(playerText);
        }

        // Instructions text
        Text instructionText;
        instructionText.setFont(font);
        instructionText.setString("Press SPACE to change view\nPress ESC to go back");
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::White);
        instructionText.setPosition(200, 550);

        window.draw(instructionText);

        // Display the window
        window.display();

        // Event handling
        Event e;
        bool exitLeaderboard = false;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    // Go back to main menu
                    showingMainMenu = true;
                    exitLeaderboard = true;
                    break;
                }

                if (e.key.code == Keyboard::Space) {
                    // Cycle through leaderboard modes
                    currentMode = (currentMode + 1) % 3;
                    break;  // Break inner event loop to redraw
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

    // Get proper player names for display, ensuring they're never empty
    string player1DisplayName = matchedPlayer1.empty() ? currentProfile->getUsername() : matchedPlayer1;
    string player2DisplayName = matchedPlayer2.empty() ? "Player2" : matchedPlayer2;

    // Determine winner text
    if (!isSinglePlayerSelected) {
        if (score1 > score2) {
            endText.setString(player1DisplayName + " Wins!");
            endText.setFillColor(Color::Green);
        }
        else if (score2 > score1) {
            endText.setString(player2DisplayName + " Wins!");
            endText.setFillColor(Color::Green);
        }
        else {
            endText.setString("It's a Tie!");
            endText.setFillColor(Color::Yellow);
        }
    }
    else {
        endText.setString("Game Over");
    }

    // Set end text properties
    endText.setCharacterSize(40);
    endText.setPosition(200, 100);

    // Set final score text with proper player names
    if (!isSinglePlayerSelected) {
        finalScoreText.setString(
            player1DisplayName + " Score: " + to_string(score1) + "\n" +
            player2DisplayName + " Score: " + to_string(score2)
        );
    }
    else {
        finalScoreText.setString("Your Score: " + to_string(score1));
    }
    finalScoreText.setCharacterSize(30);
    finalScoreText.setFillColor(Color::White);
    finalScoreText.setPosition(200, 150);

    // Record multiplayer result with validated player names
    if (!isSinglePlayerSelected) {
        leaderboard.recordGameResult(
            player1DisplayName,  // First player's name
            score1,              // First player's score
            player2DisplayName,  // Second player's name
            score2,              // Second player's score
            true,                // Is multiplayer
            currentLevel         // Game level
        );
    }
    else {
        // For single-player, use the current username
        string currentUsername = currentProfile->getUsername();
        leaderboard.recordGameResult(
            currentUsername,     // Player's name
            score1,              // Player's score
            "",                  // No second player
            0,                   // No second player score
            false,               // Not multiplayer
            currentLevel         // Game level
        );
    }

    // Restart and Exit texts
    Text restartText;
    restartText.setFont(font);
    restartText.setString("Press R to restart");
    restartText.setCharacterSize(30);
    restartText.setFillColor(Color::White);
    restartText.setPosition(200, 250);

    Text exitText;
    exitText.setFont(font);
    exitText.setString("Press ESC to exit");
    exitText.setCharacterSize(30);
    exitText.setFillColor(Color::White);
    exitText.setPosition(200, 300);

    // Draw all elements
    window.draw(endText);
    window.draw(finalScoreText);
    window.draw(restartText);
    window.draw(exitText);

    window.display();
}
void Menu::startGame()
{
    cout << "Starting game process!" << endl;
    showingMainMenu = false;
    showingModeSelection = true;
    // Note: gameStarted will be set to true after mode selection
}

// Restart the Game
void Menu::restartGame()
{
    cout << "Game Restarted" << endl;
    gameStarted = false;
    showingMainMenu = true;
    restartRequested = true;  // Set the flag to true

    // Reset scores when restarting
    player1Score = 0;
    player2Score = 0;
}

// Exit the Game
void Menu::exitGame()
{
    cout << "Exiting Game" << endl;
    window.close();
}
void Menu::displayLevelSelection()
{
    window.clear();

    Text levelSelectionText;
    levelSelectionText.setFont(font);
    levelSelectionText.setString("Select Level:");
    levelSelectionText.setCharacterSize(30);
    levelSelectionText.setFillColor(Color::White);
    levelSelectionText.setPosition(200, 100);

    Text level1Text;
    level1Text.setFont(font);
    level1Text.setString("1. Level 1 (Easy)");
    level1Text.setCharacterSize(30);
    level1Text.setFillColor(Color::White);
    level1Text.setPosition(200, 150);

    Text level2Text;
    level2Text.setFont(font);
    level2Text.setString("2. Level 2 (Medium)");
    level2Text.setCharacterSize(30);
    level2Text.setFillColor(Color::White);
    level2Text.setPosition(200, 200);

    Text level3Text;
    level3Text.setFont(font);
    level3Text.setString("3. Level 3 (Hard)");
    level3Text.setCharacterSize(30);
    level3Text.setFillColor(Color::White);
    level3Text.setPosition(200, 250);

    Text backText;
    backText.setFont(font);
    backText.setString("Press ESC to go back");
    backText.setCharacterSize(20);
    backText.setFillColor(Color::White);
    backText.setPosition(200, 300);

	bgSprite.Draw(window);  // Draw background sprite
    window.draw(levelSelectionText);
    window.draw(level1Text);
    window.draw(level2Text);
    window.draw(level3Text);
    window.draw(backText);
    window.display();
}

// Handles user input for level selection
void Menu::handleLevelSelection()
{
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

        // Add mouse click handling
        if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
        {
            Vector2i mousePosition = Mouse::getPosition(window);

            // Check if Level 1 option is clicked
            if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 150 && mousePosition.y <= 180) {
                currentLevel = 0;
                level.setLevel(currentLevel);
                cout << "Level 1 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            // Check if Level 2 option is clicked
            else if (mousePosition.x >= 200 && mousePosition.x <= 350 &&
                mousePosition.y >= 200 && mousePosition.y <= 230) {
                currentLevel = 1;
                level.setLevel(currentLevel);
                cout << "Level 2 selected!" << endl;
                showingLevelSelection = false;
                showingMainMenu = true;
            }
            // Check if Level 3 option is clicked
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
    // Set menu state
    showingMainMenu = false;

    // Clear the window
    window.clear();
    bgSprite.Draw(window);

    // Display the profile
    currentProfile->displayProfile(window,bgSprite);

    // Event loop for profile screen
    Event e;
    bool inProfileView = true;
    while (inProfileView) {
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            // Handle profile input
            if (!currentProfile->handleProfileInput(e)) {
                // Return to main menu
                inProfileView = false;
                showingMainMenu = true;
                break;
            }
        }

        // Optional: allow redrawing or other interactions
        if (!inProfileView) break;
    }

    // Redraw main menu when exiting profile
    if (showingMainMenu) {
        displayMainMenu();
    }
}


// Add destructor
Menu::~Menu()
{
    if (currentProfile) {
        delete currentProfile;
    }
}

void Menu::sendFriendRequestToUser(const string& sender, const string& receiver) {
    // Create receiver's friend request file
    std::string requestFileName = receiver + "_friend_requests.txt";

    // Open file in append mode to add new friend request
    std::ofstream requestFile(requestFileName, std::ios_base::app);

    if (requestFile.is_open()) {
        // Write the sender's username to the receiver's friend requests file
        requestFile << sender << std::endl;
        requestFile.close();

        cout << sender << " sent a friend request to " << receiver << endl;
    }
    else {
        cout << "Error: Could not save friend request" << endl;
    }
}

void Menu::showFriendList() {
    // Set screen state
    showingMainMenu = false;
    showingFriendList = true;

    // Get current logged-in username
    string currentUsername = currentProfile->getUsername();

    // Use PlayerHashTable to get or create friend list
    FriendList* friendList = playerHashTable.getFriendList(currentUsername);
    if (!friendList) {
        // If no friend list exists, add player to hash table
        playerHashTable.addPlayer(currentUsername);
        friendList = playerHashTable.getFriendList(currentUsername);
    }

    // Attempt to load existing friends and friend requests
    std::string friendsFilename = currentUsername + "_friends.txt";
    std::string requestsFilename = currentUsername + "_friend_requests.txt";

    // Load friends and requests from files
    friendList->loadFriendsFromFile(currentUsername);
    friendList->loadFriendRequestsFromFile(currentUsername);

    while (showingFriendList) {
        window.clear(Color(50, 50, 50));  // Dark background
        bgSprite.Draw(window);

        // Title
        Text titleText;
        titleText.setFont(font);
        titleText.setString("Friends List");
        titleText.setCharacterSize(40);
        titleText.setFillColor(Color::White);
        titleText.setPosition(100, 50);

        // Friends Section
        Text friendsHeaderText;
        friendsHeaderText.setFont(font);
        friendsHeaderText.setString("Your Friends:");
        friendsHeaderText.setCharacterSize(30);
        friendsHeaderText.setFillColor(Color::Green);
        friendsHeaderText.setPosition(100, 120);

        // Friends List Text
        Text friendsListText;
        friendsListText.setFont(font);
        friendsListText.setCharacterSize(20);
        friendsListText.setFillColor(Color::White);
        friendsListText.setPosition(100, 160);

        // Read and display friends
        std::string friends[100];
        int friendCount = 0;
        friendList->getFriendsList(friends, friendCount);

        std::string friendsDisplay;
        for (int i = 0; i < friendCount; ++i) {
            friendsDisplay += friends[i] + "\n";
        }

        if (friendsDisplay.empty()) {
            friendsDisplay = "No friends yet.\n";
        }
        friendsListText.setString(friendsDisplay);

        // Friend Requests Section
        Text requestHeaderText;
        requestHeaderText.setFont(font);
        requestHeaderText.setString("Friend Requests:");
        requestHeaderText.setCharacterSize(30);
        requestHeaderText.setFillColor(Color::Yellow);
        requestHeaderText.setPosition(100, 300);

        // Friend Requests List Text
        Text requestsListText;
        requestsListText.setFont(font);
        requestsListText.setCharacterSize(20);
        requestsListText.setFillColor(Color::White);
        requestsListText.setPosition(100, 340);

        // Read and display friend requests
        std::string requests[100];
        int requestCount = 0;
        friendList->getFriendRequestsList(requests, requestCount);

        std::string requestsDisplay;
        for (int i = 0; i < requestCount; ++i) {
            requestsDisplay += requests[i] + "\n";
        }

        if (requestsDisplay.empty()) {
            requestsDisplay = "No friend requests.\n";
        }
        requestsListText.setString(requestsDisplay);

        // Action Instructions
        Text instructionText;
        instructionText.setFont(font);
        instructionText.setString(
            "S: Send Friend Request\n"
            "A: Accept Request\n"
            "R: Reject Request\n"
            "ESC: Back to Menu"
        );
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(Color::White);
        instructionText.setPosition(100, 500);

        // Draw all elements
        window.draw(titleText);
        window.draw(friendsHeaderText);
        window.draw(friendsListText);
        window.draw(requestHeaderText);
        window.draw(requestsListText);
        window.draw(instructionText);

        window.display();

        // Event Handling
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                switch (e.key.code) {
                case Keyboard::Escape:
                    // Return to main menu
                    showingFriendList = false;
                    showingMainMenu = true;
                    return;

                case Keyboard::S:
                    // Send Friend Request
                    sendFriendRequest();
                    // Reload friends and requests after sending
                    friendList->loadFriendsFromFile(currentUsername);
                    friendList->loadFriendRequestsFromFile(currentUsername);
                    break;

                case Keyboard::A:
                    // Accept Friend Request
                {
                    // Read first friend request
                    std::ifstream requestFile(requestsFilename);
                    std::string firstRequest;
                    if (std::getline(requestFile, firstRequest)) {
                        requestFile.close();

                        // Temporary file for rewriting requests
                        std::string tempFilename = currentUsername + "_temp_requests.txt";
                        std::ofstream tempFile(tempFilename);

                        // Reopen original requests file to copy remaining requests
                        std::ifstream oldRequestFile(requestsFilename);
                        std::string tempRequest;
                        bool firstSkipped = false;

                        // Copy all requests except the first one
                        while (std::getline(oldRequestFile, tempRequest)) {
                            if (!firstSkipped) {
                                firstSkipped = true;
                                continue;
                            }
                            tempFile << tempRequest << std::endl;
                        }

                        oldRequestFile.close();
                        tempFile.close();

                        // Replace original requests file
                        std::remove(requestsFilename.c_str());
                        std::rename(tempFilename.c_str(), requestsFilename.c_str());

                        // Use FriendList method to accept friend request
                        bool accepted = friendList->acceptFriendRequestWithFile(currentUsername, firstRequest);

                        if (accepted) {
                            // Update PlayerHashTable
                            FriendList* requesterFriendList = playerHashTable.getFriendList(firstRequest);
                            if (requesterFriendList) {
                                requesterFriendList->addFriend(currentUsername);
                                requesterFriendList->saveFriendsToFile(firstRequest);
                            }

                            // Reload friends and requests
                            friendList->loadFriendsFromFile(currentUsername);
                            friendList->loadFriendRequestsFromFile(currentUsername);

                            cout << "Accepted friend request from: " << firstRequest << endl;
                        }
                    }
                }
                break;

                case Keyboard::R:
                    // Reject Friend Request
                {
                    // Read first friend request
                    std::ifstream requestFile(requestsFilename);
                    std::string firstRequest;
                    if (std::getline(requestFile, firstRequest)) {
                        requestFile.close();

                        // Temporary file for rewriting requests
                        std::string tempFilename = currentUsername + "_temp_requests.txt";
                        std::ofstream tempFile(tempFilename);

                        // Reopen original requests file to copy remaining requests
                        std::ifstream oldRequestFile(requestsFilename);
                        std::string tempRequest;
                        bool firstSkipped = false;

                        // Copy all requests except the first one
                        while (std::getline(oldRequestFile, tempRequest)) {
                            if (!firstSkipped) {
                                firstSkipped = true;
                                continue;
                            }
                            tempFile << tempRequest << std::endl;
                        }

                        oldRequestFile.close();
                        tempFile.close();

                        // Replace original requests file
                        std::remove(requestsFilename.c_str());
                        std::rename(tempFilename.c_str(), requestsFilename.c_str());

                        // Reload friend requests
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

    // Font setup
    Font windowFont;
    if (!windowFont.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return;
    }

    Text titleText;
    titleText.setFont(windowFont);
    titleText.setString("Select a User to Send Friend Request");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50, 50);

    // Get current logged-in username
    string currentUsername = currentProfile->getUsername();

    // Prepare arrays to store valid users
    string validUsernames[100];  // Assuming max 100 users
    int userCount = 0;

    // Go through all users
    UserNode* current = auth.getUserListHead();
    while (current != nullptr) {
        // Skip current user
        if (current->username != currentUsername) {
            // Use PlayerHashTable to get friend list
            FriendList* currentUserFriendList = playerHashTable.getFriendList(currentUsername);

            if (currentUserFriendList) {
                // Check if already friends
                bool isFriend = false;
                std::string friendsFilename = currentUsername + "_friends.txt";
                std::ifstream friendsFile(friendsFilename);
                std::string friendLine;

                while (std::getline(friendsFile, friendLine)) {
                    if (friendLine == current->username) {
                        isFriend = true;
                        break;
                    }
                }
                friendsFile.close();

                // If not a friend, check for existing requests
                if (!isFriend) {
                    // Check outgoing requests
                    bool outgoingRequestExists = false;
                    std::string outgoingRequestsFilename = currentUsername + "_friend_requests.txt";
                    std::ifstream outgoingRequestsFile(outgoingRequestsFilename);
                    std::string outgoingRequestLine;

                    while (std::getline(outgoingRequestsFile, outgoingRequestLine)) {
                        if (outgoingRequestLine == current->username) {
                            outgoingRequestExists = true;
                            break;
                        }
                    }
                    outgoingRequestsFile.close();

                    // Check incoming requests
                    bool incomingRequestExists = false;
                    std::string incomingRequestsFilename = current->username + "_friend_requests.txt";
                    std::ifstream incomingRequestsFile(incomingRequestsFilename);
                    std::string incomingRequestLine;

                    while (std::getline(incomingRequestsFile, incomingRequestLine)) {
                        if (incomingRequestLine == currentUsername) {
                            incomingRequestExists = true;
                            break;
                        }
                    }
                    incomingRequestsFile.close();

                    // Add to valid users if no requests exist
                    if (!outgoingRequestExists && !incomingRequestExists) {
                        validUsernames[userCount++] = current->username;
                    }
                }
            }
        }
        current = current->next;
    }

    // Validate user count
    if (userCount == 0) {
        // Show message that no other users exist
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

        // Wait for a moment
        sf::sleep(sf::milliseconds(2000));
        return;
    }

    // Dynamically allocate text objects
    Text* userTexts = new Text[userCount];

    // Populate text objects
    for (int index = 0; index < userCount; ++index) {
        // Configure text object
        userTexts[index].setFont(windowFont);
        userTexts[index].setString("\t" + validUsernames[index] + " (SEND)");
        userTexts[index].setCharacterSize(25);
        userTexts[index].setFillColor(Color::White);
        userTexts[index].setPosition(50, 150 + index * 40);
    }

    // Selected user index
    int selectedIndex = 0;

    while (usersWindow.isOpen()) {
        Event e;
        while (usersWindow.pollEvent(e)) {
            if (e.type == Event::Closed)
                usersWindow.close();

            // Keyboard navigation
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Down) {
                    // Move selection down
                    selectedIndex = (selectedIndex + 1) % userCount;
                }
                else if (e.key.code == Keyboard::Up) {
                    // Move selection up
                    selectedIndex = (selectedIndex - 1 + userCount) % userCount;
                }
                else if (e.key.code == Keyboard::Return) {
                    // Get selected user
                    string selectedUser = validUsernames[selectedIndex];

                    // Send friend request
                    sendFriendRequestToUser(currentUsername, selectedUser);

                    // Add to hash table's friend list
                    FriendList* currentUserFriendList = playerHashTable.getFriendList(currentUsername);
                    if (currentUserFriendList) {
                        currentUserFriendList->sendFriendRequest(selectedUser);
                    }

                    // Confirmation message
                    Text confirmText;
                    confirmText.setFont(windowFont);
                    confirmText.setString("Friend request sent to " + selectedUser);
                    confirmText.setCharacterSize(20);
                    confirmText.setFillColor(Color::Green);
                    confirmText.setPosition(50, 450);

                    usersWindow.clear(Color(50, 50, 50));
                    usersWindow.draw(titleText);
                    for (int i = 0; i < userCount; ++i) {
                        usersWindow.draw(userTexts[i]);
                    }
                    usersWindow.draw(confirmText);
                    usersWindow.display();

                    // Wait for a moment
                    sf::sleep(sf::milliseconds(2000));

                    // Close the window after sending request
                    usersWindow.close();
                }
                else if (e.key.code == Keyboard::Escape) {
                    // Cancel and close window
                    usersWindow.close();
                }
            }
        }

        // Clear the window
        usersWindow.clear(Color(50, 50, 50));

        // Draw title
        usersWindow.draw(titleText);

        // Draw usernames with highlighting
        for (int i = 0; i < userCount; ++i) {
            // Highlight selected user
            if (i == selectedIndex) {
                userTexts[i].setFillColor(Color::Yellow);
            }
            else {
                userTexts[i].setFillColor(Color::White);
            }
            usersWindow.draw(userTexts[i]);
        }

        // Display the window
        usersWindow.display();
    }

    // Clean up dynamically allocated memory
    delete[] userTexts;
}

void Menu::displayFriendRequests() {
    // Get current logged-in username
    string currentUsername = currentProfile->getUsername();

    // Create friend list for current user
    FriendList friendList;

    // Load friend requests from file
    std::string requestsFilename = currentUsername + "_friend_requests.txt";

    // Read friend requests
    std::ifstream requestsFile(requestsFilename);

    // Dynamically allocate array for requests
    string* requestsList = new string[100];  // Assuming max 100 requests
    int requestCount = 0;

    // Read requests from file
    std::string request;
    while (std::getline(requestsFile, request) && requestCount < 100) {
        requestsList[requestCount++] = request;
    }
    requestsFile.close();

    RenderWindow requestsWindow(VideoMode(400, 300), "Friend Requests");

    // Font setup
    Font windowFont;
    if (!windowFont.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        delete[] requestsList;
        return;
    }

    Text titleText, instructionText;
    titleText.setFont(windowFont);
    instructionText.setFont(windowFont);

    titleText.setString("Incoming Friend Requests");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50, 50);

    instructionText.setString(
        "Use Arrow Keys to Navigate\n"
        "Enter to Accept\n"
        "Backspace to Reject"
    );
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(Color::White);
    instructionText.setPosition(50, 250);

    // Selected request index
    int selectedIndex = 0;

    while (requestsWindow.isOpen()) {
        Event e;
        while (requestsWindow.pollEvent(e)) {
            if (e.type == Event::Closed)
                requestsWindow.close();

            if (e.type == Event::KeyPressed) {
                if (requestCount > 0) {
                    if (e.key.code == Keyboard::Down) {
                        // Move selection down
                        selectedIndex = (selectedIndex + 1) % requestCount;
                    }
                    else if (e.key.code == Keyboard::Up) {
                        // Move selection up
                        selectedIndex = (selectedIndex - 1 + requestCount) % requestCount;
                    }
                    else if (e.key.code == Keyboard::Return) {
                        // Accept friend request
                        string selectedUser = requestsList[selectedIndex];

                        // Accept request and update files
                        bool accepted = friendList.acceptFriendRequestWithFile(currentUsername, selectedUser);

                        if (accepted) {
                            // Remove the accepted request from the list
                            for (int i = selectedIndex; i < requestCount - 1; i++) {
                                requestsList[i] = requestsList[i + 1];
                            }
                            requestCount--;

                            // Adjust selected index if needed
                            if (selectedIndex >= requestCount) {
                                selectedIndex = requestCount > 0 ? requestCount - 1 : 0;
                            }

                            // Optional: Show confirmation
                            cout << "Accepted friend request from: " << selectedUser << endl;
                        }
                    }
                    else if (e.key.code == Keyboard::Backspace) {
                        // Reject friend request
                        string selectedUser = requestsList[selectedIndex];

                        // Remove the request from file
                        std::string requestsFilename = currentUsername + "_friend_requests.txt";

                        // Read all requests except the rejected one
                        std::ifstream inputFile(requestsFilename);
                        std::ofstream tempFile("temp_requests.txt");

                        std::string line;
                        while (std::getline(inputFile, line)) {
                            if (line != selectedUser) {
                                tempFile << line << std::endl;
                            }
                        }

                        inputFile.close();
                        tempFile.close();

                        // Replace the original file
                        std::remove(requestsFilename.c_str());
                        std::rename("temp_requests.txt", requestsFilename.c_str());

                        // Remove the rejected request from the list
                        for (int i = selectedIndex; i < requestCount - 1; i++) {
                            requestsList[i] = requestsList[i + 1];
                        }
                        requestCount--;

                        // Adjust selected index if needed
                        if (selectedIndex >= requestCount) {
                            selectedIndex = requestCount > 0 ? requestCount - 1 : 0;
                        }

                        // Optional: Show confirmation
                        cout << "Rejected friend request from: " << selectedUser << endl;
                    }
                }

                // Exit on Escape
                if (e.key.code == Keyboard::Escape) {
                    requestsWindow.close();
                }
            }
        }

        // Clear the window
        requestsWindow.clear(Color(50, 50, 50));

        // Draw title and instructions
        requestsWindow.draw(titleText);
        requestsWindow.draw(instructionText);

        // Draw friend requests
        for (int i = 0; i < requestCount; i++) {
            Text requestText;
            requestText.setFont(windowFont);
            requestText.setString(requestsList[i]);
            requestText.setCharacterSize(25);

            // Highlight selected request
            if (i == selectedIndex) {
                requestText.setFillColor(Color::Yellow);
            }
            else {
                requestText.setFillColor(Color::White);
            }

            requestText.setPosition(50, 100 + i * 40);
            requestsWindow.draw(requestText);
        }

        // Display the window
        requestsWindow.display();
    }

    // Clean up dynamically allocated memory
    delete[] requestsList;
}



void Menu::acceptFriendRequest() {
    string currentUsername = currentProfile->getUsername();
    std::string requestsFileName = currentUsername + "_friend_requests.txt";
    std::string friendsFileName = currentUsername + "_friends.txt";

    // Read requests
    std::ifstream requestsFile(requestsFileName);
    std::string requester;

    // Read first requester
    if (std::getline(requestsFile, requester)) {
        requestsFile.close();

        // Add to friends file
        std::ofstream friendsFile(friendsFileName, std::ios_base::app);
        friendsFile << requester << std::endl;
        friendsFile.close();

        // Remove the request from requests file
        std::ifstream inputFile(requestsFileName);
        std::ofstream tempFile("temp_requests.txt");
        std::string line;
        bool firstLineSkipped = false;

        while (std::getline(inputFile, line)) {
            if (!firstLineSkipped) {
                firstLineSkipped = true;
                continue;
            }
            tempFile << line << std::endl;
        }

        inputFile.close();
        tempFile.close();

        // Replace original file
        std::remove(requestsFileName.c_str());
        std::rename("temp_requests.txt", requestsFileName.c_str());

        cout << "Accepted friend request from: " << requester << endl;
    }
}

void Menu::rejectFriendRequest() {
    string currentUsername = currentProfile->getUsername();
    std::string requestsFileName = currentUsername + "_friend_requests.txt";

    // Read requests
    std::ifstream requestsFile(requestsFileName);
    std::string requester;

    // Read first requester
    if (std::getline(requestsFile, requester)) {
        requestsFile.close();

        // Remove the request from requests file
        std::ifstream inputFile(requestsFileName);
        std::ofstream tempFile("temp_requests.txt");
        std::string line;
        bool firstLineSkipped = false;

        while (std::getline(inputFile, line)) {
            if (!firstLineSkipped) {
                firstLineSkipped = true;
                continue;
            }
            tempFile << line << std::endl;
        }

        inputFile.close();
        tempFile.close();

        // Replace original file
        std::remove(requestsFileName.c_str());
        std::rename("temp_requests.txt", requestsFileName.c_str());

        cout << "Rejected friend request from: " << requester << endl;
    }
}



void Menu::syncPlayerNames() {
    // If matchedPlayer1 is empty, use current user
    if (matchedPlayer1.empty()) {
        matchedPlayer1 = currentProfile->getUsername();
    }

    // If matchedPlayer2 is empty in multiplayer, use default
    if (!isSinglePlayerSelected && matchedPlayer2.empty()) {
        matchedPlayer2 = "Player2";
    }

    // Set secondPlayerName to match matchedPlayer2
    secondPlayerName = matchedPlayer2;
}

int Menu::themeSelection()
{
    showingMainMenu = false;
    Text titleText, theme1Text, theme2Text, theme3Text;

    titleText.setFont(font);
    titleText.setString("Select Your Desired Theme");
    titleText.setCharacterSize(40);
    titleText.setFillColor(Color::White);
    titleText.setPosition(265, 50);

    theme1Text.setFont(font);
    theme1Text.setString("1. Ocean Theme");
    theme1Text.setCharacterSize(30);
    theme1Text.setFillColor(Color::White);
    theme1Text.setPosition(200, 150);

    theme2Text.setFont(font);
    theme2Text.setString("2. Forest Theme");
    theme2Text.setCharacterSize(30);
    theme2Text.setFillColor(Color::White);
    theme2Text.setPosition(200, 200);

    theme3Text.setFont(font);
    theme3Text.setString("3. Space Theme");
    theme3Text.setCharacterSize(30);
    theme3Text.setFillColor(Color::White);
    theme3Text.setPosition(200, 250);

    while (window.isOpen()) {
        window.clear(Color(50, 50, 50));
        window.draw(titleText);
        window.draw(theme1Text);
        window.draw(theme2Text);
        window.draw(theme3Text);
        window.display();

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (theme1Text.getGlobalBounds().contains(mousePos)) return 1;
                if (theme2Text.getGlobalBounds().contains(mousePos)) return 2;
                if (theme3Text.getGlobalBounds().contains(mousePos)) return 3;
            }
        }
    }

    return -1; 
}
