#ifndef FRIEND_LIST_H
#define FRIEND_LIST_H
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class FriendList
{
private:
    //---------------------Constants---------------------
    static const int MAX_FRIENDS = 100;
    static const int MAX_REQUESTS = 100;

    //---------------------Friend List Structures---------------------
    struct FriendNode
    {
        string friendUsername;
        FriendNode* next;
        FriendNode(const string& username);
    };

    struct FriendRequestNode
    {
        string requestUsername;
        FriendRequestNode* next;

        FriendRequestNode(const string& username);
    };

    //---------------------Friend Data---------------------
    string friendsList[MAX_FRIENDS];
    string friendRequestsList[MAX_REQUESTS];
    int friendCount = 0;
    int requestCount = 0;

public:
    //---------------------Constructor and Destructor---------------------
    FriendList();
    ~FriendList();

    //---------------------Friend Management Methods---------------------
    void addFriend(const string& username);
    void removeFriend(const string& username);
    bool hasFriend(const string& username) const;

    //---------------------Friend Request Methods---------------------
    void sendFriendRequest(const string& username);
    bool friendRequestExists(const string& username) const;
    int getFriendCount() const;
    int getFriendRequestCount() const;
    void getFriendsList(string friendsList[], int& count) const;
    void getFriendRequestsList(string requestsList[], int& count) const;
    void clearFriendRequests();
    void clearFriendsList();
    static bool manageFriendRequest(const string& sender, const string& receiver, int action);

    //---------------------Friend Request Handling---------------------
    bool acceptFriendRequest(const string& username);
    void rejectFriendRequest(const string& requestUsername);
    void addFriendRequest(const string& requestUsername);
    bool acceptFriendRequestWithFile(const string& currentUsername, const string& username);

    //---------------------File I/O Methods---------------------
    void loadFriendRequestsFromFile(const string& username);
    void loadFriendsFromFile(const string& username);
    void saveFriendsToFile(const string& username);
    void saveFriendRequestsToFile(const string& username);

    //---------------------Friend List Pointers---------------------
    FriendNode* friendsHead;
    FriendRequestNode* friendRequestsHead;
};

#endif
