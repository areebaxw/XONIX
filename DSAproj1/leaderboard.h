#pragma once
#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <iostream>
const int HEAP_SIZE = 10;

using namespace std;

struct LeaderboardEntry {
    //---------------------Player Information---------------------
    string username;
    int score = 0;
    string player1Name;
    string player2Name;
    int player1Score = 0;
    int player2Score = 0;
    bool isMultiplayer = false;
    int gameLevel = 0;

    //---------------------Constructor---------------------
    LeaderboardEntry() :
        score(0),
        player1Score(0),
        player2Score(0),
        isMultiplayer(false),
        gameLevel(0) {}
};

class Leaderboard {
public:
    //---------------------Constructor---------------------
    Leaderboard();

    //---------------------Current Heap Size---------------------
    int heapSize;

    //---------------------Display Mode Methods---------------------
    void setDisplayMode(int mode);
    int getDisplayMode() const;

    //---------------------Leaderboard Management---------------------
    void saveLeaderboardFromScoreFile(const string& scoreFilePath, const string& leaderboardFilePath);
    void insertPlayer(const string& username, int score);
    void updatePlayerScore(const LeaderboardEntry& entry);
    void displayLeaderboard();
    void sortForDisplay(LeaderboardEntry* sortedArray);
    void saveLeaderboard();
    void loadLeaderboard();

    //---------------------Game Result Recording---------------------
    void recordGameResult(
        const string& player1,
        int score1,
        const string& player2 = "",
        int score2 = 0,
        bool isMultiplayer = false,
        int level = 0
    );

    //---------------------Leaderboard Utilities---------------------
    string getTopScorer();
    void saveDetailedLeaderboard(const LeaderboardEntry& entry);

private:
    //---------------------Current Display Mode---------------------
    int currentDisplayMode = 0; // 0 for Single Player, 1 for Multiplayer

    //---------------------Min-Heap Data Structure---------------------
    LeaderboardEntry heap[HEAP_SIZE];

    //---------------------Helper Functions---------------------
    void swapPlayers(LeaderboardEntry& a, LeaderboardEntry& b);
    void minHeapify(int index);
};

#endif // LEADERBOARD_H
