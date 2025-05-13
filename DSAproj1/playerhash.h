#pragma once

#include <string>
#include "friendlist.h"

class PlayerHashTable {
private:
    //---------------------Constants---------------------
    static const int tablesize = 100;

    //---------------------Hash Node Structure---------------------
    struct HashNode {
        string username;
        FriendList friendList;  
        HashNode* next;

        HashNode(const string& name) : username(name), next(NULL) {}
    };

    //---------------------Hash Table---------------------
    HashNode* table[tablesize];

    //---------------------Hash Function---------------------
    int hashUsername(const string& username)
    {
        unsigned long hash = 5381;
        for (char c : username) {
            hash = ((hash << 5) + hash) + c; 
        }
        return hash % tablesize;
    }

public:
    //---------------------Constructor and Destructor---------------------
    PlayerHashTable();
    ~PlayerHashTable();

    //---------------------Player Management Methods---------------------
    void addPlayer(const string& username);  
    void removePlayer(const string& username); 

    //---------------------Friend List Management---------------------
    void findMutualFriends(const string& user1, const string& user2, string mutualFriends[], int& count);

    FriendList* getFriendList(const string& username);  

    //---------------------User Existence Check---------------------
    bool userExists(const string& username);  
};

