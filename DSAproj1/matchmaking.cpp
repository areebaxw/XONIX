#include "matchmaking.h"
#include <fstream>
#include <iostream>
#include <string>

// Constructor
Matchmaking::Matchmaking() :
    frontIndex(0),
    rearIndex(0),
    currentSize(0),
    nextEntryOrder(0) {}

// Safely copy string to character array
void Matchmaking::copyString(char* destination, const std::string& source) {
    int maxLength = MAX_USERNAME_LENGTH - 1;
    int i = 0;
    while (i < source.length() && i < maxLength) {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

// Manual robust string to integer conversion
int Matchmaking::stringToInt(const std::string& str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (!str.empty() && str[0] == '-') {
        sign = -1;
        i++;
    }

    // Process digits
    while (i < str.length() && str[i] >= '0' && str[i] <= '9') {
        // Basic overflow prevention
        if (result > (2147483647 - (str[i] - '0')) / 10) {
            // Handle overflow by returning max or min int
            return sign > 0 ? 2147483647 : -2147483648;
        }
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

// Read user's highest score from profile
int Matchmaking::readUserHighestScore(const std::string& username) {
    std::string filename = username + "_profile.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open profile file for " << username << std::endl;
        return 0;
    }

    std::string line;
    int highestScore = 0;

    while (std::getline(file, line)) {
        int pos = line.find("Highest Score:");
        if (pos != std::string::npos) {
            std::string scoreStr = line.substr(pos + 14);

            // Manually trim leading whitespace
            int firstNonSpace = -1;
            for (int i = 0; i < scoreStr.length(); ++i) {
                if (!std::isspace(scoreStr[i])) {
                    firstNonSpace = i;
                    break;
                }
            }

            // Manually trim trailing whitespace
            int lastNonSpace = -1;
            for (int i = scoreStr.length() - 1; i >= 0; --i) {
                if (!std::isspace(scoreStr[i])) {
                    lastNonSpace = i;
                    break;
                }
            }

            // Extract trimmed score string
            if (firstNonSpace != -1 && lastNonSpace != -1 && firstNonSpace <= lastNonSpace) {
                scoreStr = scoreStr.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
            }
            else {
                scoreStr = ""; // Empty string if no valid score
            }

            // Convert to integer
            highestScore = stringToInt(scoreStr);
            break;
        }
    }

    file.close();

    // Detailed debug logging
    std::cout << "Loaded highest score for " << username
        << ": " << highestScore << std::endl;

    return highestScore;
}

void Matchmaking::sortQueueByPriority() {
    // Bubble sort with improved priority logic
    for (int i = 0; i < currentSize - 1; i++) {
        for (int j = 0; j < currentSize - i - 1; j++) {
            bool shouldSwap = false;

            // Primary sort: Highest score (descending)
            if (players[j].highestScore < players[j + 1].highestScore) {
                shouldSwap = true;
            }
            // Secondary sort: Earlier entry if scores are equal
            else if (players[j].highestScore == players[j + 1].highestScore &&
                players[j].queueEntryOrder > players[j + 1].queueEntryOrder) {
                shouldSwap = true;
            }

            // Swap players if needed
            if (shouldSwap) {
                MatchPlayer temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}


// Check if two players are unique
bool Matchmaking::arePlayersUnique(const MatchPlayer& p1, const MatchPlayer& p2) {
    // Manual string comparison
    for (int i = 0; i < MAX_USERNAME_LENGTH; ++i) {
        if (p1.username[i] != p2.username[i]) {
            return true;
        }
        if (p1.username[i] == '\0') {
            break;
        }
    }
    return false;
}

// Log matchmaking events to file
void Matchmaking::logMatchmakingEvent(const std::string& eventDescription) {
    std::ofstream logFile("matchmaking_log.txt", std::ios_base::app);
    if (logFile.is_open()) {
        logFile << eventDescription << std::endl;
        logFile.close();
    }
}

// Comprehensive queue debugging method
void Matchmaking::debugPrintQueue() {
    std::cout << "\n--- Detailed Queue Debug Information ---" << std::endl;
    std::cout << "Current Queue Size: " << currentSize << std::endl;

    if (currentSize == 0) {
        std::cout << "Queue is empty!" << std::endl;
        return;
    }

    std::cout << "Queue Contents:" << std::endl;
    for (int i = 0; i < currentSize; i++) {
        std::cout << "Player " << (i + 1)
            << ": Username = " << players[i].username
            << ", Score = " << players[i].highestScore
            << ", Entry Order = " << players[i].queueEntryOrder
            << std::endl;
    }
    std::cout << "--------------------------------------\n" << std::endl;
}

// Modify enterQueue to optionally accept a score
bool Matchmaking::enterQueue(const std::string& username, int highestScore) {
    // Validate input
    if (username.empty() || username.length() >= MAX_USERNAME_LENGTH) {
        std::cerr << "Invalid username: " << username << std::endl;
        return false;
    }

    // Check if queue is full
    if (currentSize >= MAX_QUEUE_SIZE) {
        std::cerr << "Matchmaking queue is full" << std::endl;
        return false;
    }

    // Create new match player
    MatchPlayer newPlayer;
    copyString(newPlayer.username, username);
    newPlayer.highestScore = highestScore;
    newPlayer.queueEntryOrder = nextEntryOrder++;

    // Add player to queue
    players[rearIndex] = newPlayer;
    rearIndex = (rearIndex + 1) % MAX_QUEUE_SIZE;
    currentSize++;

    // Sort queue by priority
    sortQueueByPriority();

    // Log queue entry
    std::string logMessage = "Player " + username +
        " entered queue with score " + std::to_string(highestScore);
    logMatchmakingEvent(logMessage);

    // Debug: Print queue state after entry
    std::cout << "\nPlayer " << username << " entered the queue:" << std::endl;
    debugPrintQueue();

    return true;
}


bool Matchmaking::matchPlayers(std::string& player1Username, std::string& player2Username) {
    // Need at least 2 players to match
    if (currentSize < 2) {
        std::cout << "Not enough players to match. Current queue size: "
            << currentSize << std::endl;
        debugPrintQueue();
        return false;
    }

    // Debug: Print initial queue state
    std::cout << "Pre-Match Queue State:" << std::endl;
    debugPrintQueue();

    // Select first player (highest score)
    MatchPlayer player1 = players[0];

    // Try to find a suitable match
    int secondPlayerIndex = -1;

    // Strategy 1: Find a player with a higher or equal score
    for (int i = 1; i < currentSize; i++) {
        if (arePlayersUnique(player1, players[i]) &&
            players[i].highestScore >= player1.highestScore) {
            secondPlayerIndex = i;
            break;
        }
    }

    // Strategy 2: If no player with equal or higher score, find the closest lower score
    if (secondPlayerIndex == -1) {
        int closestLowerScoreIndex = -1;
        int closestLowerScoreDiff = 1000000;  // Large constant

        for (int i = 1; i < currentSize; i++) {
            if (arePlayersUnique(player1, players[i])) {
                int scoreDiff = player1.highestScore - players[i].highestScore;

                // Find the closest lower score
                if (scoreDiff > 0 && scoreDiff < closestLowerScoreDiff) {
                    closestLowerScoreIndex = i;
                    closestLowerScoreDiff = scoreDiff;
                }
            }
        }

        secondPlayerIndex = closestLowerScoreIndex;
    }

    // If no player is found, return false
    if (secondPlayerIndex == -1) {
        std::cout << "No suitable match found for player: "
            << player1.username << std::endl;
        debugPrintQueue();
        return false;
    }

    // Copy usernames before potentially removing from queue
    player1Username = std::string(player1.username);
    player2Username = std::string(players[secondPlayerIndex].username);

    // Log match details
    std::string logMessage = "Matched players: " + player1Username +
        " (Score: " + std::to_string(player1.highestScore) + ") vs " +
        player2Username + " (Score: " +
        std::to_string(players[secondPlayerIndex].highestScore) + ")";
    logMatchmakingEvent(logMessage);

    //// Display match details
    //std::cout << "Matchmaking: Player 1 (" << player1Username
    //    << ", Score: " << player1.highestScore
    //    << ") vs Player 2 (" << player2Username
    //    << ", Score: " << players[secondPlayerIndex].highestScore << ")" << std::endl;

    // Dequeue the matched players
    // Remove first player
    for (int i = 0; i < currentSize - 1; i++) {
        players[i] = players[i + 1];
    }
    currentSize--;

    // Remove second player
    for (int i = 0; i < currentSize; i++) {
        if (std::string(players[i].username) == player2Username) {
            for (int j = i; j < currentSize - 1; j++) {
                players[j] = players[j + 1];
            }
            currentSize--;
            break;
        }
    }

    // Re-sort queue after match
    sortQueueByPriority();

    // Debug: Print post-match queue state
    std::cout << "Post-Match Queue State:" << std::endl;
    debugPrintQueue();

    return true;
}


// Get current queue size
int Matchmaking::getQueueSize() const {
    return currentSize;
}

// Get top player from leaderboard
std::string Matchmaking::getTopPlayerFromLeaderboard() {
    std::string topPlayer = "";
    int highestScore = -1;

    // Open the leaderboard file
    std::ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        return ""; // Return empty string if file fails to open
    }

    std::string line;
    while (std::getline(file, line)) {
        int commaPos = line.find(',');  // Find the position of the comma
        if (commaPos != std::string::npos) {
            std::string player = line.substr(0, commaPos);  // Extract player name
            std::string scoreStr = line.substr(commaPos + 1);  // Extract score
            int score = stringToInt(scoreStr);  // Convert score to integer

            // Ensure the score is higher than the current highest score
            if (score > highestScore) {
                highestScore = score;  // Update highest score
                topPlayer = player;    // Update the top player
            }
        }
    }

    file.close();  // Close the file after reading
    return topPlayer;  // Return the player with the highest score
}

void Matchmaking::populateQueueFromLeaderboard(const std::string& loggedInUser) {
    std::ifstream file("leaderboard.txt");
    std::string line;

    while (std::getline(file, line)) {
        int commaPos = line.find(',');  // Find the position of the comma separating name and score
        if (commaPos != std::string::npos) {
            std::string playerName = line.substr(0, commaPos);
            int playerScore = stringToInt(line.substr(commaPos + 1));  // Convert score to integer

            // Skip the logged-in user
            if (playerName != loggedInUser) {
                // Add player to matchmaking queue
                enterQueue(playerName, playerScore);
                std::cout << "Added " << playerName << " with score " << playerScore << " to the queue." << std::endl;
            }
            else {
                std::cout << "Skipped adding logged-in user " << loggedInUser << " to the queue." << std::endl;
            }
        }
    }

    file.close();
}

std::string Matchmaking::getNextTopPlayerFromLeaderboard(const std::string& topPlayerUsername) {
    std::ifstream file("leaderboard.txt");
    std::string line;
    bool foundTopPlayer = false;

    while (std::getline(file, line)) {
        int commaPos = line.find(',');  // Find the position of the comma
        if (commaPos != std::string::npos) {
            std::string player = line.substr(0, commaPos);  // Extract player name
            if (foundTopPlayer) {
                return player;  // Return the next player after the top player
            }
            if (player == topPlayerUsername) {
                foundTopPlayer = true;  // Skip the current top player
            }
        }
    }

    file.close();  // Close the file after reading
    return ""; // Return empty string if no next player is found
}