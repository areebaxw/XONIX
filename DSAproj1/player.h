#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "friendlist.h"
#include "Sprite.h"

const int MAX_FRIENDS = 10;
const int MAX_MATCH_HISTORY = 20;

using namespace std;
using namespace sf;

struct MatchRecord {
    //---------------------Match Record Information---------------------
    string date;
    bool isWin;
    int score;
    string gameMode;
};

class PlayerProfile {
private:
    //---------------------Profile Data---------------------
    string username;
    string friends[MAX_FRIENDS];
    int friendCount;
    MatchRecord matchHistory[MAX_MATCH_HISTORY];
    int matchHistoryCount;
    int highestScore;
   Font font;
   Text profileText;

    //---------------------Private Methods---------------------
    void saveProfileData(); 
    void loadProfileData(); 

public:
    //---------------------Constructor---------------------
    PlayerProfile(const string& username);

    //---------------------Getters---------------------
    string getUsername() const;
    int getHighestScore() const;
    int getFriendCount() const;
    int getMatchHistoryCount() const;
    void getFriendsList(string friendsList[]) const;
    void getMatchHistory(MatchRecord matchList[]) const;

    //---------------------Profile Modification Methods---------------------
    bool addFriend(const string& friendUsername);
    bool removeFriend(const string& friendUsername);
    void recordMatch(const MatchRecord& match);
    void updateHighestScore(int newScore);

    //---------------------Profile Display and Interaction---------------------
    void displayProfile(RenderWindow& window, MySprite& bgSprite);
    bool handleProfileInput(Event& event);

    //---------------------Friend List Management---------------------
    FriendList* friendList;

    void loadFriendRequestsFromFile(const string& username) {
        friendList->loadFriendRequestsFromFile(username);  
    }


    void getFriendRequestsList(string requestsList[], int& count) const {
        friendList->getFriendRequestsList(requestsList, count);  
    }

    void acceptFriendRequest(const string& username) {
        friendList->acceptFriendRequest(username);  
    }

    void rejectFriendRequest(const string& username) {
        friendList->rejectFriendRequest(username);  
    }

    //---------------------File Management Methods---------------------
    void saveFriendsToFile(const string& username);
    void saveFriendRequestsToFile(const string& username);  


    //---------------------Helper Method---------------------
    int extractNumber(const string& str, int startPos);
};

#endif // PLAYERPROFILE_H
