#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include <string>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;


class Matchmaking {
public:
    // Constants
    static const int MAX_USERNAME_LENGTH = 50;
    static const int MAX_QUEUE_SIZE = 100;

    // Player structure for matchmaking
    struct MatchPlayer {
        char username[MAX_USERNAME_LENGTH];
        int highestScore;
        int queueEntryOrder;
    };

    // Queue storage
    MatchPlayer players[MAX_QUEUE_SIZE];
    int frontIndex;
    int rearIndex;
    int currentSize;
    int nextEntryOrder;

    // Private utility methods
    void copyString(char* destination, const std::string& source);
    int stringToInt(const std::string& str);
    int readUserHighestScore(const std::string& username);
    void sortQueueByPriority();
    bool arePlayersUnique(const MatchPlayer& p1, const MatchPlayer& p2);

    void populateQueueFromLeaderboard(const std::string& loggedInUser);

    void logMatchmakingEvent(const std::string& eventDescription);




public:
    // Constructor
    Matchmaking();
    void debugPrintQueue();
    // Enter a player into the matchmaking queue
    bool enterQueue(const std::string& username, int hs);

    // void populateQueueFromLeaderboard();

     // Match players
    bool matchPlayers(std::string& player1Username, std::string& player2Username);

    // Get current queue size
    int getQueueSize() const;

    // Clear the queue
    void clearQueue();
    string getNextTopPlayerFromLeaderboard(const string& topPlayerUsername);
    std::string getTopPlayerFromLeaderboard();
};

#endif // MATCHMAKING_H