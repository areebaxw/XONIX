#include "player.h"
#include <iostream>
#include <sstream>
#include <ctime>

PlayerProfile::PlayerProfile(const std::string& playerUsername) {
    // Initialize username
    username = playerUsername;

    // Initialize other members
    friendCount = 0;
    matchHistoryCount = 0;
    highestScore = 0;

    // Try to load existing profile data
    loadProfileData();
}

void PlayerProfile::saveProfileData() {
    // Create filename based on username
    std::string filename = username + "_profile.txt";

    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Could not open file to save profile: " << filename << std::endl;
        return;
    }

    // Write profile data in a human-readable format
    outFile << "Username: " << username << std::endl;
    outFile << "Highest Score: " << highestScore << std::endl;

    // Write friends
    outFile << "Friends Count: " << friendCount << std::endl;
    for (int i = 0; i < friendCount; i++) {
        outFile << "Friend " << (i + 1) << ": " << friends[i] << std::endl;
    }

    // Write match history
    outFile << "Match History Count: " << matchHistoryCount << std::endl;
    for (int i = 0; i < matchHistoryCount; i++) {
        outFile << "Match " << (i + 1) << ":" << std::endl;
        outFile << "  Date: " << matchHistory[i].date << std::endl;
        outFile << "  Game Mode: " << matchHistory[i].gameMode << std::endl;
        outFile << "  Score: " << matchHistory[i].score << std::endl;
        outFile << "  Result: " << (matchHistory[i].isWin ? "Win" : "Loss") << std::endl;
    }

    // Open and read score.txt to get additional recent match details
    std::ifstream inFile("score.txt");
    if (!inFile) {
        std::cerr << "Could not open score.txt file" << std::endl;
        outFile.close();
        return;
    }

    // Variables to track the most recent match details
    int recentPowerUps = 0;
    std::string recentPowerUpStatus = "None";
    std::string mostRecentGameMode = "Unknown";
    int mostRecentBonusLevel = 0;

    std::string line;
    std::string userScorePrefix = "Game Score for " + username;

    // Temporary in-memory storage of file contents
    std::vector<std::string> fileContents;
    while (std::getline(inFile, line)) {
        fileContents.push_back(line);
    }
    inFile.close();

    // Search from the end of the file
    for (int i = fileContents.size() - 1; i >= 0; --i) {
        line = fileContents[i];

        // Check if this is a game score entry for the current username
        if (line.find(userScorePrefix) != std::string::npos) {
            // Search subsequent lines for details
            for (int j = i + 1; j < fileContents.size(); ++j) {
                std::string currentLine = fileContents[j];

                // Break if we've reached another game score entry
                if (currentLine.find("Game Score for ") != std::string::npos) {
                    break;
                }

                // Extract Game Mode
                if (currentLine.find("Game Mode:") != std::string::npos) {
                    int modePos = currentLine.find("Game Mode:");
                    mostRecentGameMode = currentLine.substr(modePos + 11);
                }

                // Extract Bonus Level
                if (currentLine.find("Bonus Level:") != std::string::npos) {
                    int bonusPos = currentLine.find("Bonus Level:");
                    mostRecentBonusLevel = std::stoi(currentLine.substr(bonusPos + 12));
                }

                // Extract Power-Ups Collected
                if (currentLine.find("Power-Ups Collected:") != std::string::npos) {
                    int powerUpPos = currentLine.find("Power-Ups Collected:");
                    recentPowerUps = std::stoi(currentLine.substr(powerUpPos + 20));
                }

                // Extract Power-Ups Used Status
                if (currentLine.find("Power-Ups Used:") != std::string::npos) {
                    if (currentLine.find("Power-Ups Used: Yes") != std::string::npos) {
                        recentPowerUpStatus = "Used";
                    }
                    else if (currentLine.find("Power-Ups Used: No") != std::string::npos) {
                        recentPowerUpStatus = "Not Used";
                    }
                    break;
                }
            }
            break;  // Stop after processing the most recent match
        }
    }

    // Append most recent match details to the file
    outFile << "Most Recent Game Mode: " << mostRecentGameMode << std::endl;
    outFile << "Most Recent Bonus Level: " << mostRecentBonusLevel << std::endl;
    outFile << "Most Recent Power-Ups Collected: " << recentPowerUps << std::endl;
    outFile << "Most Recent Power-Up Status: " << recentPowerUpStatus << std::endl;

    outFile.close();
}
void PlayerProfile::loadProfileData() {
    // Create filename based on username and open the score.txt file
    std::string filename = "score.txt";  // Use just the score.txt file
    std::ifstream inFile(filename);

    if (!inFile) {
        std::cerr << "No existing profile found for " << username << std::endl;
        return;
    }

    std::string line;
    int extractedHighestScore = 0;   // To track the highest score from score.txt
    int recentPowerUps = 0;           // To track the power-ups collected in the most recent match
    std::string recentPowerUpStatus = "None";  // To track power-up usage status from the most recent match

    bool foundMostRecentMatch = false;  // Flag to track if we've processed the most recent match

    // Read through the file to find relevant information for the given username
    // Process the file in reverse order
    std::vector<std::string> lines;
    while (std::getline(inFile, line)) {
        lines.push_back(line);  // Store each line in a vector
    }

    // Start from the last line and go upwards to find the most recent match
    for (int i = lines.size() - 1; i >= 0; --i) {
        line = lines[i];
        if (line.find("Game Score for " + username) != std::string::npos) {  // Check if this is the user's score entry
            int currentScore = 0;

            // Extract Final Score
            for (int j = i + 1; j < lines.size(); ++j) {
                int scorePos = lines[j].find("Final Score:");
                if (scorePos != std::string::npos) {
                    std::string scoreStr = lines[j].substr(scorePos + 12);
                    try {
                        currentScore = std::stoi(scoreStr);
                        extractedHighestScore = std::max(extractedHighestScore, currentScore);  // Update highest score if current one is higher
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Error parsing score: " << e.what() << std::endl;
                    }
                    break;
                }
            }

            // Extract most recent match details (ONLY from the most recent match)
            if (!foundMostRecentMatch) {
                for (int j = i + 1; j < lines.size(); ++j) {
                    // Break if we've reached another game score entry
                    if (lines[j].find("Game Score for ") != std::string::npos) {
                        break;
                    }

                    // Extract Power-Ups Collected
                    int powerUpPos = lines[j].find("Power-Ups Collected:");
                    if (powerUpPos != std::string::npos) {
                        try {
                            recentPowerUps = std::stoi(lines[j].substr(powerUpPos + 20));
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error parsing power-ups: " << e.what() << std::endl;
                        }
                    }

                    // Extract Power-Ups Used
                    int powerUpUsedPos = lines[j].find("Power-Ups Used:");
                    if (powerUpUsedPos != std::string::npos) {
                        if (lines[j].find("Power-Ups Used: Yes") != std::string::npos) {
                            recentPowerUpStatus = "Used";
                        }
                        else if (lines[j].find("Power-Ups Used: No") != std::string::npos) {
                            recentPowerUpStatus = "Not Used";
                        }
                        break;
                    }
                }

                foundMostRecentMatch = true;
            }
        }
    }

    // Update the profile with the data extracted from the file
    if (extractedHighestScore > highestScore) {
        highestScore = extractedHighestScore;
        saveProfileData();
    }

    // Print out some debug information (optional)
    std::cout << "Loaded Profile Data for " << username << std::endl;
    std::cout << "Highest Score: " << highestScore << std::endl;
    std::cout << "Most Recent Power-Ups: " << recentPowerUps << std::endl;
    std::cout << "Recent Power-Up Status: " << recentPowerUpStatus << std::endl;

    inFile.close();
}


std::string PlayerProfile::getUsername() const {
    return username;
}

int PlayerProfile::getHighestScore() const {
    return highestScore;
}

int PlayerProfile::getFriendCount() const {
    return friendCount;
}

int PlayerProfile::getMatchHistoryCount() const {
    return matchHistoryCount;
}

void PlayerProfile::getFriendsList(std::string friendsList[]) const {
    // Copy friends list to the provided array
    for (int i = 0; i < friendCount; i++) {
        friendsList[i] = friends[i];
    }
}

void PlayerProfile::getMatchHistory(MatchRecord matchList[]) const {
    // Copy match history to the provided array
    for (int i = 0; i < matchHistoryCount; i++) {
        matchList[i] = matchHistory[i];
    }
}

bool PlayerProfile::addFriend(const std::string& friendUsername) {
    // Check if friends list is full
    if (friendCount >= MAX_FRIENDS) {
        return false;
    }

    // Check for duplicate
    for (int i = 0; i < friendCount; i++) {
        if (friends[i] == friendUsername) {
            return false;  // Friend already exists
        }
    }

    // Add new friend
    friends[friendCount] = friendUsername;
    friendCount++;

    // Save updated profile
    saveProfileData();
    return true;
}

bool PlayerProfile::removeFriend(const std::string& friendUsername) {
    for (int i = 0; i < friendCount; i++) {
        if (friends[i] == friendUsername) {
            // Shift remaining friends
            for (int j = i; j < friendCount - 1; j++) {
                friends[j] = friends[j + 1];
            }
            friendCount--;

            // Save updated profile
            saveProfileData();
            return true;
        }
    }
    return false;
}

void PlayerProfile::recordMatch(const MatchRecord& match) {
    // Shift match history if full
    if (matchHistoryCount >= MAX_MATCH_HISTORY) {
        // Remove oldest match
        for (int i = 0; i < MAX_MATCH_HISTORY - 1; i++) {
            matchHistory[i] = matchHistory[i + 1];
        }
        matchHistoryCount--;
    }

    // Add new match
    matchHistory[matchHistoryCount] = match;
    matchHistoryCount++;

    // Save updated profile
    saveProfileData();
}

void PlayerProfile::updateHighestScore(int newScore) {
    if (newScore > highestScore) {
        highestScore = newScore;

        // Save updated profile
        saveProfileData();
    }
}

void PlayerProfile::displayProfile(sf::RenderWindow& window, MySprite& bgSprite) {
    // Load font
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return;
    }

    // Variables to store extracted data
    int extractedHighestScore = 0;
    int mostRecentPowerUps = 0;
    std::string mostRecentPowerUpStatus = "None";
    std::string mostRecentGameMode = "Unknown";
    int mostRecentBonusLevel = 0;

    // Extract friends from file
    std::string extractedFriends[MAX_FRIENDS];
    int extractedFriendCount = 0;

    // Open friends file
    std::string friendsFilename = username + "_friends.txt";
    std::ifstream friendsFile(friendsFilename);
    if (friendsFile.is_open()) {
        std::string friendName;
        while (std::getline(friendsFile, friendName) && extractedFriendCount < MAX_FRIENDS) {
            extractedFriends[extractedFriendCount] = friendName;
            extractedFriendCount++;
        }
        friendsFile.close();

        // Update friendCount in the class
        friendCount = extractedFriendCount;
    }

    // Open and read score.txt
    std::ifstream scoreFile("score.txt");
    if (scoreFile.is_open()) {
        std::string line;
        std::string gameScoreLinePrefix = "Game Score for " + username;

        // Reset file pointer to beginning
        scoreFile.seekg(0, std::ios::beg);

        // Read entire file line by line
        while (std::getline(scoreFile, line)) {
            // Check if this is a game score entry for the current username
            if (line.find(gameScoreLinePrefix) != std::string::npos) {
                std::string currentLine;
                int currentMatchScore = 0;

                // Read next few lines to extract score and most recent match details
                while (std::getline(scoreFile, currentLine) &&
                    currentLine.find("--- Game Score for ") == std::string::npos) {
                    // Extract Final Score for ALL matches of this user
                    if (currentLine.find("Final Score:") != std::string::npos) {
                        size_t scorePos = currentLine.find("Final Score:");
                        std::string scoreStr = currentLine.substr(scorePos + 12);
                        try { ///what is this
                            currentMatchScore = std::stoi(scoreStr);
                            // Update HIGHEST score for this user
                            extractedHighestScore = std::max(extractedHighestScore, currentMatchScore);
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error parsing score: " << e.what() << std::endl;
                        }
                    }

                    // Most recent match details (only for the last match)
                    // Extract Game Mode
                    if (currentLine.find("Game Mode:") != std::string::npos) {
                        size_t modePos = currentLine.find("Game Mode:");
                        mostRecentGameMode = currentLine.substr(modePos + 11);
                    }

                    // Extract Bonus Level
                    if (currentLine.find("Bonus Level:") != std::string::npos) {
                        size_t bonusPos = currentLine.find("Bonus Level:");
                        try {
                            mostRecentBonusLevel = std::stoi(currentLine.substr(bonusPos + 12));
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error parsing bonus level: " << e.what() << std::endl;
                        }
                    }

                    // Extract Power-Ups Collected
                    if (currentLine.find("Power-Ups Collected:") != std::string::npos) {
                        size_t powerUpPos = currentLine.find("Power-Ups Collected:");
                        try {
                            mostRecentPowerUps = std::stoi(currentLine.substr(powerUpPos + 20));
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error parsing power-ups: " << e.what() << std::endl;
                        }
                    }

                    // Extract Power-Ups Used Status
                    if (currentLine.find("Power-Ups Used:") != std::string::npos) {
                        if (currentLine.find("Power-Ups Used: Yes") != std::string::npos) {
                            mostRecentPowerUpStatus = "Used";
                        }
                        else if (currentLine.find("Power-Ups Used: No") != std::string::npos) {
                            mostRecentPowerUpStatus = "Not Used";
                        }
                        break;
                    }
                }
            }
        }
        scoreFile.close();
    }
    else {
        std::cerr << "Unable to open score.txt" << std::endl;
    }

    // Update highest score if the extracted score is higher
    if (extractedHighestScore > highestScore) {
        highestScore = extractedHighestScore;
        saveProfileData();
    }

    // Clear the window
    window.clear(sf::Color(50, 50, 50));

    // Create text for profile information
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString("Player Profile");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(50, 50);

    // Username
    sf::Text usernameText;
    usernameText.setFont(font);
    usernameText.setString("Username: " + username);
    usernameText.setCharacterSize(30);
    usernameText.setFillColor(sf::Color::White);
    usernameText.setPosition(50, 150);

    // Highest Score
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Highest Score: " + std::to_string(highestScore));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(50, 200);

    // Most Recent Match Details
    sf::Text matchDetailsText;
    matchDetailsText.setFont(font);
    matchDetailsText.setString("Most Recent Match Details:\n" +
        std::string("Game Mode: ") + mostRecentGameMode + "\n" +
        "Bonus Level: " + std::to_string(mostRecentBonusLevel)
    );
    matchDetailsText.setCharacterSize(30);
    matchDetailsText.setFillColor(sf::Color::White);
    matchDetailsText.setPosition(50, 250);

    // Power-Up Information for the most recent match
    sf::Text powerUpText;
    powerUpText.setFont(font);
    powerUpText.setString("Most Recent Power-Ups: " + std::to_string(mostRecentPowerUps) +
        "\nRecent Power-Up Status: " + mostRecentPowerUpStatus);
    powerUpText.setCharacterSize(30);
    powerUpText.setFillColor(sf::Color::White);
    powerUpText.setPosition(50, 350);

    // Friends List
    sf::Text friendsText;
    std::string friendsStr = "Friends (" + std::to_string(extractedFriendCount) + "): ";
    for (int i = 0; i < extractedFriendCount; i++) {
        friendsStr += extractedFriends[i];
        if (i < extractedFriendCount - 1) friendsStr += ", ";
    }

    // Handle case when no friends exist
    if (extractedFriendCount == 0) {
        friendsStr += "No friends yet";
    }

    friendsText.setFont(font);
    friendsText.setString(friendsStr);
    friendsText.setCharacterSize(30);
    friendsText.setFillColor(sf::Color::White);
    friendsText.setPosition(50, 450);

    // Match History
    sf::Text matchHistoryText;
    std::string matchHistoryStr = "Recent Matches (" + std::to_string(matchHistoryCount) + "):\n";
    for (int i = 0; i < matchHistoryCount; i++) {
        matchHistoryStr += matchHistory[i].date +
            " - Score: " + (matchHistory[i].score == 0 ? "0" : std::to_string(matchHistory[i].score)) +
            " - Mode: " + matchHistory[i].gameMode +
            " - " + (matchHistory[i].isWin ? "Win" : "Loss") + "\n";
    }
    matchHistoryText.setFont(font);
    matchHistoryText.setString(matchHistoryStr);
    matchHistoryText.setCharacterSize(25);
    matchHistoryText.setFillColor(sf::Color::White);
    matchHistoryText.setPosition(50, 550);

    // Draw all texts
	bgSprite.Draw(window);  // Draw background sprite
    window.draw(titleText);
    window.draw(usernameText);
    window.draw(scoreText);
    window.draw(matchDetailsText);
    window.draw(powerUpText);
    window.draw(friendsText);
    window.draw(matchHistoryText);

    // Display the window
    window.display();
}

bool PlayerProfile::handleProfileInput(sf::Event& event) {
    // Example of basic profile interaction
    // You can expand this later with more complex interactions
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Exit profile view
            return false;
        }
    }
    return true;
}