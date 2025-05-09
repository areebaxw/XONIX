#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include "friendlist.h"

const int MAX_FRIENDS = 10;
const int MAX_MATCH_HISTORY = 20;

struct MatchRecord {
    std::string date;
    bool isWin;
    int score;
    std::string gameMode;
};

class PlayerProfile {
private:
    std::string username;
    std::string friends[MAX_FRIENDS];
    int friendCount;
    MatchRecord matchHistory[MAX_MATCH_HISTORY];
    int matchHistoryCount;
    int highestScore;
    sf::Font font;
    sf::Text profileText;

    // Private method to save profile data
    void saveProfileData();

    // Private method to load profile data
    void loadProfileData();

public:
    PlayerProfile(const std::string& username);

    // Getters
    std::string getUsername() const;
    int getHighestScore() const;
    int getFriendCount() const;
    int getMatchHistoryCount() const;
    void getFriendsList(std::string friendsList[]) const;
    void getMatchHistory(MatchRecord matchList[]) const;

    // Methods to modify profile
    bool addFriend(const std::string& friendUsername);
    bool removeFriend(const std::string& friendUsername);
    void recordMatch(const MatchRecord& match);
    void updateHighestScore(int newScore);

    // Method to display profile
    void displayProfile(sf::RenderWindow& window);

    // Method to handle profile interactions
    bool handleProfileInput(sf::Event& event);



    FriendList* friendList;  // Instance of FriendList
    void loadFriendRequestsFromFile(const std::string& username) {
        friendList->loadFriendRequestsFromFile(username);  // Delegate to FriendList
    }

    void getFriendRequestsList(std::string requestsList[], int& count) const {
        friendList->getFriendRequestsList(requestsList, count);  // Delegate to FriendList
    }

    void acceptFriendRequest(const std::string& username) {
        friendList->acceptFriendRequest(username);  // Delegate the request to FriendList
    }

    void rejectFriendRequest(const std::string& username) {
        friendList->rejectFriendRequest(username);  // Delegate the request to FriendList
    }



    // Function to save friends list to file
    void saveFriendsToFile(const std::string& username);

    // Function to save friend requests list to file
    void saveFriendRequestsToFile(const std::string& username);  // Function declaration

};

#endif // PLAYERPROFILE_H