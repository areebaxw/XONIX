#pragma once
#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <iostream>
const int HEAP_SIZE = 10;

using namespace std;

struct LeaderboardEntry {
    std::string username;
    int score = 0;
    std::string player1Name;
    std::string player2Name;
    int player1Score = 0;
    int player2Score = 0;
    bool isMultiplayer = false;
    // Remove gameTimestamp if it was causing issues
    int gameLevel = 0;

    // Default constructor to ensure all fields are initialized
    LeaderboardEntry() :
        score(0),
        player1Score(0),
        player2Score(0),
        isMultiplayer(false),
        gameLevel(0) {}
};

class Leaderboard {
public:
    // Constructor
    Leaderboard();

    // Current heap size (number of entries)
    int heapSize;

    // Set and get display mode
    void setDisplayMode(int mode);
    int getDisplayMode() const;

    void saveLeaderboardFromScoreFile(const std::string& scoreFilePath, const std::string& leaderboardFilePath);



    // Insert a new player or update existing player's score
    void insertPlayer(const std::string& username, int score);

    // Update player score with full entry (handles all fields)
    void updatePlayerScore(const LeaderboardEntry& entry);

    // Display leaderboard (sorted from highest to lowest)
    void displayLeaderboard();

    // Sort leaderboard for display
    void sortForDisplay(LeaderboardEntry* sortedArray);

    // Save and load leaderboard from file
    void saveLeaderboard();
    void loadLeaderboard();

    // Record game result (multiplayer or single player)
    void recordGameResult(
        const std::string& player1,
        int score1,
        const std::string& player2 = "",
        int score2 = 0,
        bool isMultiplayer = false,
        int level = 0
    );

    // New method to get the top scorer (highest score)
    std::string getTopScorer();

    // Save detailed leaderboard entry
    void saveDetailedLeaderboard(const LeaderboardEntry& entry);

private:
    // Current display mode
    int currentDisplayMode = 0; // 0 for Single Player, 1 for Multiplayer

    // Min-heap array to store top players
    LeaderboardEntry heap[HEAP_SIZE];

    // Helper functions for heap operations
    void swapPlayers(LeaderboardEntry& a, LeaderboardEntry& b);
    void minHeapify(int index);
};

#endif // LEADERBOARD_H