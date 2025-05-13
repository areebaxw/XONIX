#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include <string>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class Matchmaking {
public:
    //---------------------Constants---------------------
    static const int MAX_USERNAME_LENGTH = 50;
    static const int MAX_QUEUE_SIZE = 100;

    //---------------------Match Player Structure---------------------
    struct MatchPlayer
    {
        char username[MAX_USERNAME_LENGTH];
        int highestScore;
        int queueEntryOrder;
    };

    //---------------------Queue Management---------------------
    MatchPlayer players[MAX_QUEUE_SIZE];
    int frontIndex;
    int rearIndex;
    int currentSize;
    int nextEntryOrder;

    //---------------------Helper Methods---------------------
    void copyString(char* destination, const string& source);
    int stringToInt(const string& str);
    int readUserHighestScore(const string& username);
    void sortQueueByPriority();
    bool arePlayersUnique(const MatchPlayer& p1, const MatchPlayer& p2);

    //---------------------Queue Population---------------------
    void populateQueueFromLeaderboard(const string& loggedInUser);

public:
    //---------------------Constructor---------------------
    Matchmaking();

    //---------------------Queue Management Methods---------------------
    bool enterQueue(const string& username, int hs);
    bool matchPlayers(string& player1Username, string& player2Username);
    int getQueueSize() const;
    void clearQueue();

    //---------------------Leaderboard Interaction---------------------
    string getNextTopPlayerFromLeaderboard(const string& topPlayerUsername);
    string getTopPlayerFromLeaderboard();
};

#endif
