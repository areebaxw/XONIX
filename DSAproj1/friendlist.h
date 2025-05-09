#ifndef FRIEND_LIST_H
#define FRIEND_LIST_H
#include <string>
#include <iostream>
#include <fstream>


// Friend Node Structure (for storing friends)
struct FriendNode {
    std::string friendUsername;  // Friend's username
    FriendNode* next;            // Pointer to the next friend node

    // Constructor to initialize a FriendNode with a username
    FriendNode(const std::string& username);
};

// Friend Request Node Structure (for storing pending requests)
struct FriendRequestNode {
    std::string requestUsername;  // Username of the user who sent the request
    FriendRequestNode* next;     // Pointer to the next request node

    // Constructor to initialize a FriendRequestNode with a username
    FriendRequestNode(const std::string& username);
};



class FriendList {
private:
    // Constants
    static const int MAX_FRIENDS = 100;
    static const int MAX_REQUESTS = 100;

    // Friend Node Structure
    struct FriendNode {
        std::string friendUsername;
        FriendNode* next;

        FriendNode(const std::string& username);
    };

    // Friend Request Node Structure
    struct FriendRequestNode {
        std::string requestUsername;
        FriendRequestNode* next;

        FriendRequestNode(const std::string& username);
    };

    std::string friendsList[MAX_FRIENDS];
    std::string friendRequestsList[MAX_REQUESTS];
    int friendCount = 0;
    int requestCount = 0;


public:
    // Constructor and Destructor
    FriendList();
    ~FriendList();

    // Friend Management Methods
    void addFriend(const std::string& username);
    void removeFriend(const std::string& username);
    bool hasFriend(const std::string& username) const;

    // Friend Request Methods
    void sendFriendRequest(const std::string& username);
 
    // Checking Methods
    bool friendRequestExists(const std::string& username) const;

    // Counting Methods
    int getFriendCount() const;
    int getFriendRequestCount() const;

    // List Retrieval Methods
    void getFriendsList(std::string friendsList[], int& count) const;
    void getFriendRequestsList(std::string requestsList[], int& count) const;


    // Utility Methods
    void clearFriendRequests();
    void clearFriendsList();

    // Centralized File Management Method
    static bool manageFriendRequest(const std::string& sender, const std::string& receiver, int action);


    // Head pointers for the linked lists of friends and friend requests
    FriendNode* friendsHead;
    FriendRequestNode* friendRequestsHead;

    bool acceptFriendRequest(const std::string& username);
    void rejectFriendRequest(const std::string& requestUsername);
    
    void addFriendRequest(const std::string& requestUsername);
  

    bool acceptFriendRequestWithFile(const std::string& currentUsername, const std::string& username);

    // Add these to the public section of FriendList class
    void loadFriendRequestsFromFile(const std::string& username);
    void loadFriendsFromFile(const std::string& username);
    void saveFriendsToFile(const std::string& username);
    void saveFriendRequestsToFile(const std::string& username);

};

#endif // FRIEND_LIST_H