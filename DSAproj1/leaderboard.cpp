

#include "leaderboard.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std;



using namespace std;

const int MAX_PLAYERS = 10;  // Maximum number of players we can store

// Define arrays to store player names and their highest scores
string playerNames[MAX_PLAYERS];
int playerScores[MAX_PLAYERS];
int playerCount = 0;  // Current number of players in the arrays

void Leaderboard::saveLeaderboardFromScoreFile(const string& scoreFilePath, const string& leaderboardFilePath) {
    ifstream scoreFile(scoreFilePath);
    ofstream leaderboardFile(leaderboardFilePath);

    if (!scoreFile.is_open()) {
        cout << "Error: Could not open score file for reading." << endl;
        return;
    }
    if (!leaderboardFile.is_open()) {
        cout << "Error: Could not open leaderboard file for writing." << endl;
        return;
    }

    string line;
    string playerName;
    int playerScore = 0;
    bool isSinglePlayer = false;

    // Read through the score file line by line
    while (getline(scoreFile, line)) {
        // Check for "Game Mode" to ensure we are processing a single-player game
        if (line.find("Game Mode: Single Player") != string::npos) {
            isSinglePlayer = true;  // This is a single-player game
        }

        // Extract player name from "Game Score for <playerName>"
        if (line.find("Game Score for ") != string::npos) {
            playerName = line.substr(15);  // Skip "Game Score for "
        }

        // Extract player score from "Final Score: <score>"
        if (line.find("Final Score:") != string::npos && isSinglePlayer) {
            playerScore = stoi(line.substr(13));  // Get score after "Final Score: "

            // Check if player is already in the leaderboard (based on name)
            bool playerFound = false;
            for (int i = 0; i < playerCount; i++) {
                if (playerNames[i] == playerName) {
                    // If the player already exists, update their score if the new score is higher
                    if (playerScore > playerScores[i]) {
                        playerScores[i] = playerScore;  // Update score
                    }
                    playerFound = true;
                    break;
                }
            }

            // If player is not found, add them to the leaderboard
            if (!playerFound && playerCount < MAX_PLAYERS) {
                playerNames[playerCount] = playerName;
                playerScores[playerCount] = playerScore;
                playerCount++;
            }

            // Reset for the next score entry
            isSinglePlayer = false;
        }
    }

    // Write the unique player names and their highest scores to the leaderboard file
    for (int i = 0; i < playerCount; i++) {
        leaderboardFile << playerNames[i] << "," << playerScores[i] << endl;
    }

    scoreFile.close();
    leaderboardFile.close();

    cout << "Leaderboard saved to " << leaderboardFilePath << " successfully." << endl;
}

// Constructor to initialize heap with empty players
Leaderboard::Leaderboard() : heapSize(0) {
    for (int i = 0; i < HEAP_SIZE; i++) {
        heap[i].username = "";
        heap[i].score = 0;
    }
}

void Leaderboard::setDisplayMode(int mode) {
    currentDisplayMode = mode;
}

int Leaderboard::getDisplayMode() const {
    return currentDisplayMode;
}

// Insert a player or update score if player already exists
void Leaderboard::insertPlayer(const string& username, int score) {
    // Check if the player already exists in the heap
    for (int i = 0; i < heapSize; i++) {
        if (heap[i].username == username) {
            // If score is higher, update it
            if (score > heap[i].score) {
                heap[i].score = score;

                // Bubble up if needed
                int current = i;
                while (current > 0 && heap[(current - 1) / 2].score > heap[current].score) {
                    swapPlayers(heap[current], heap[(current - 1) / 2]);
                    current = (current - 1) / 2;
                }
                minHeapify(i);
            }
            return;
        }
    }

    // If player not in heap, insert new player
    if (heapSize < HEAP_SIZE) {
        heap[heapSize].username = username;
        heap[heapSize].score = score;

        // Bubble up the new entry
        int current = heapSize;
        while (current > 0 && heap[(current - 1) / 2].score > heap[current].score) {
            swapPlayers(heap[current], heap[(current - 1) / 2]);
            current = (current - 1) / 2;
        }

        heapSize++;
    }
    // If heap is full and new score is higher than root
    else if (score > heap[0].score) {
        // Replace root with new player
        heap[0].username = username;
        heap[0].score = score;

        // Restore min-heap property
        minHeapify(0);
    }
}

void Leaderboard::updatePlayerScore(const LeaderboardEntry& entry) {
    // Check if the entry already exists
    for (int i = 0; i < heapSize; i++) {
        // For multiplayer, check individual players
        if (entry.isMultiplayer) {
            // Check for Player 1
            if (heap[i].player1Name == entry.player1Name) {
                // Update if new score is higher
                if (entry.player1Score > heap[i].player1Score) {
                    heap[i].player1Score = entry.player1Score;
                    // Update total score if needed
                    heap[i].score = heap[i].player1Score + heap[i].player2Score;
                }
            }

            // Check for Player 2
            if (heap[i].player2Name == entry.player2Name) {
                // Update if new score is higher
                if (entry.player2Score > heap[i].player2Score) {
                    heap[i].player2Score = entry.player2Score;
                    // Update total score if needed
                    heap[i].score = heap[i].player1Score + heap[i].player2Score;
                }
            }
        }
        // For single-player
        else {
            if (heap[i].username == entry.username) {
                // Update if new score is higher
                if (entry.score > heap[i].score) {
                    heap[i] = entry;
                    minHeapify(i);
                }
                return;
            }
        }
    }

    // If not found, insert new entry
    if (heapSize < HEAP_SIZE) {
        heap[heapSize] = entry;

        // Bubble up the new entry
        int current = heapSize;
        while (current > 0 && heap[(current - 1) / 2].score > heap[current].score) {
            swapPlayers(heap[current], heap[(current - 1) / 2]);
            current = (current - 1) / 2;
        }

        heapSize++;
    }
    // If heap is full and new score is higher than root
    else if (entry.score > heap[0].score) {
        // Replace root with new entry
        heap[0] = entry;
        minHeapify(0);
    }
}

void Leaderboard::recordGameResult(const string& player1, int score1,
    const string& player2, int score2,
    bool isMultiplayer, int level) {

    // Ensure usernames are not empty
    string safePlayer1 = player1.empty() ? "Player1" : player1;
    string safePlayer2 = player2.empty() ? "Player2" : player2;

    // Update leaderboard entries
    LeaderboardEntry entry;
    entry.player1Name = safePlayer1;
    entry.player1Score = score1;
    entry.player2Name = safePlayer2;  // This ensures the second player's name is saved
    entry.player2Score = score2;
    entry.isMultiplayer = isMultiplayer;
    entry.gameLevel = level;

    // For multiplayer, calculate the total score
    if (isMultiplayer) {
        entry.username = safePlayer1 + " vs " + safePlayer2;
        entry.score = score1 + score2;  // Use the combined score of both players
    }
    else {
        entry.username = safePlayer1;
        entry.score = score1;
    }

    // Insert or update entry in the leaderboard
    updatePlayerScore(entry);

    // Save detailed entry to file
    saveDetailedLeaderboard(entry);
}


// Modify saveDetailedLeaderboard to remove timestamp
void Leaderboard::saveDetailedLeaderboard(const LeaderboardEntry& entry) {
    ofstream detailedFile("detailed_leaderboard.txt", ios::app);
    if (detailedFile.is_open()) {
        detailedFile << entry.username << ","
            << entry.score << ","
            << entry.player1Name << ","
            << entry.player1Score << ","
            << entry.player2Name << ","
            << entry.player2Score << ","
            << (entry.isMultiplayer ? "Multiplayer" : "Single Player") << ","
            // Remove timestamp
            << entry.gameLevel
            << endl;
        detailedFile.close();
    }
}
void Leaderboard::loadLeaderboard() {
    // Clear existing heap
    heapSize = 0;

    // Open score file
    std::ifstream scoreFile("score.txt");
    if (!scoreFile.is_open()) {
        std::cout << "No score file found." << std::endl;
        insertPlayer("Player", 100);  // Default player if no file exists
        return;
    }

    std::string line;
    std::string currentGameMode = "";
    std::string playerName = "";
    int playerScore = 0;

    while (std::getline(scoreFile, line)) {
        // Skip empty lines or separator lines
        if (line.empty() || line.find("---") != std::string::npos) continue;

        // Check for game mode
        if (line.find("Game Mode:") != std::string::npos) {
            currentGameMode = line.substr(line.find(":") + 2);
            std::cout << "Current Game Mode: " << currentGameMode << std::endl;
            playerName = "";
            playerScore = 0;
            continue;
        }

        // Extract player name
        size_t namePos = line.find("Game Score for ");
        if (namePos != std::string::npos) {
            playerName = line.substr(namePos + 15);
            int spacePos = playerName.find(" ");
            if (spacePos != std::string::npos) {
                playerName = playerName.substr(0, spacePos);  // Extract the name before any space
            }
            std::cout << "Player Name: " << playerName << std::endl;
        }

        // Handle Single Player Mode
        if (currentGameMode == "Single Player") {
            size_t scorePos = line.find("Final Score:");
            if (scorePos != std::string::npos) {
                playerScore = std::stoi(line.substr(scorePos + 12));

                LeaderboardEntry entry;
                entry.username = playerName;
                entry.score = playerScore;
                entry.player1Name = playerName;
                entry.player1Score = playerScore;
                entry.isMultiplayer = false;

                updatePlayerScore(entry);  // Update leaderboard with single-player entry

                std::cout << "Single Player Entry - Name: " << playerName
                    << ", Score: " << playerScore << std::endl;
            }
        }
        // Handle Multiplayer Mode
        else if (currentGameMode == "Multiplayer") {
            // Check for Player 1 Score
            size_t player1ScorePos = line.find("Player 1 Score:");
            if (player1ScorePos != std::string::npos) {
                size_t openParen = line.find("(");
                int player1Score = std::stoi(line.substr(player1ScorePos + 15,
                    (openParen != std::string::npos ? openParen : line.length()) - (player1ScorePos + 15)));

                // Read the next line for Player 2 details
                std::string player2Line;
                std::getline(scoreFile, player2Line);

                // Extract Player 2 name and score
                std::string player2Name = "";
                int player2Score = 0;

                size_t player2NamePos = player2Line.find("Game Score for ");
                if (player2NamePos != std::string::npos) {
                    player2Name = player2Line.substr(player2NamePos + 15);
                    int spacePos = player2Name.find(" ");
                    if (spacePos != std::string::npos) {
                        player2Name = player2Name.substr(0, spacePos);  // Extract name before space
                    }
                }

                size_t player2ScorePos = player2Line.find("Player 2 Score:");
                if (player2ScorePos != std::string::npos) {
                    size_t openParen = player2Line.find("(");
                    player2Score = std::stoi(player2Line.substr(player2ScorePos + 15,
                        (openParen != std::string::npos ? openParen : player2Line.length()) - (player2ScorePos + 15)));
                }

                // Create multiplayer entry
                LeaderboardEntry entry;
                entry.player1Name = playerName;
                entry.player1Score = player1Score;
                entry.player2Name = player2Name;
                entry.player2Score = player2Score;
                entry.isMultiplayer = true;
                entry.username = playerName + " vs " + player2Name;
                entry.score = player1Score + player2Score;

                updatePlayerScore(entry);  // Update leaderboard with multiplayer entry

                std::cout << "Multiplayer Entry - Player 1: " << playerName
                    << ", Score: " << player1Score
                    << " vs Player 2: " << player2Name
                    << ", Score: " << player2Score << std::endl;
            }
        }
    }

    scoreFile.close();

    // Add default entry if no scores found
    if (heapSize == 0) {
        insertPlayer("Player", 100);  // Insert a default player if no entries exist
    }

    std::cout << "Total Leaderboard Entries: " << heapSize << std::endl;
}


void Leaderboard::sortForDisplay(LeaderboardEntry* sortedArray) {
    // Create separate arrays for single-player and multiplayer entries
    LeaderboardEntry singlePlayerEntries[HEAP_SIZE];
    LeaderboardEntry multiplayerEntries[HEAP_SIZE];
    int singlePlayerCount = 0;
    int multiplayerCount = 0;

    // Collect entries based on type
    for (int i = 0; i < heapSize; i++) {
        if (heap[i].isMultiplayer) {
            multiplayerEntries[multiplayerCount++] = heap[i];
        }
        else {
            singlePlayerEntries[singlePlayerCount++] = heap[i];
        }
    }

    // Bubble sort for single-player entries (descending order by player1Score)
    for (int i = 0; i < singlePlayerCount - 1; i++) {
        for (int j = 0; j < singlePlayerCount - i - 1; j++) {
            if (singlePlayerEntries[j].player1Score < singlePlayerEntries[j + 1].player1Score) {
                LeaderboardEntry temp = singlePlayerEntries[j];
                singlePlayerEntries[j] = singlePlayerEntries[j + 1];
                singlePlayerEntries[j + 1] = temp;
            }
        }
    }

    // Bubble sort for multiplayer entries (descending order by total score of both players)
    for (int i = 0; i < multiplayerCount - 1; i++) {
        for (int j = 0; j < multiplayerCount - i - 1; j++) {
            int currentMatchScore = multiplayerEntries[j].player1Score + multiplayerEntries[j].player2Score;
            int nextMatchScore = multiplayerEntries[j + 1].player1Score + multiplayerEntries[j + 1].player2Score;

            if (currentMatchScore < nextMatchScore) {
                LeaderboardEntry temp = multiplayerEntries[j];
                multiplayerEntries[j] = multiplayerEntries[j + 1];
                multiplayerEntries[j + 1] = temp;
            }
        }
    }

    // Select entries based on the current mode
    LeaderboardEntry* selectedEntries;
    int selectedCount;

    if (currentDisplayMode == 0) { // Single Player Mode
        selectedEntries = singlePlayerEntries;
        selectedCount = singlePlayerCount;
    }
    else { // Multiplayer Mode
        selectedEntries = multiplayerEntries;
        selectedCount = multiplayerCount;
    }

    // Copy selected entries to sortedArray
    for (int i = 0; i < selectedCount; i++) {
        sortedArray[i] = selectedEntries[i];
    }

    // Fill remaining slots with empty entries if there are not enough players
    for (int i = selectedCount; i < HEAP_SIZE; i++) {
        sortedArray[i] = LeaderboardEntry();  // Empty entry
    }
}

// Helper function to swap two players
void Leaderboard::swapPlayers(LeaderboardEntry& a, LeaderboardEntry& b) {
    LeaderboardEntry temp = a;
    a = b;
    b = temp;
}

// Helper function for min-heap property maintenance
void Leaderboard::minHeapify(int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heapSize && heap[left].score < heap[smallest].score) {
        smallest = left;
    }

    if (right < heapSize && heap[right].score < heap[smallest].score) {
        smallest = right;
    }

    if (smallest != index) {
        swapPlayers(heap[index], heap[smallest]);
        minHeapify(smallest);
    }
}
// leaderboard.cpp

// Implementation of the getTopScorer function
std::string Leaderboard::getTopScorer() {
    if (heapSize > 0) {
        // Top scorer is at the root of the heap (highest score
        return heap[0].username;  
    }
    return "";  
}

