#ifndef PLAYER_HASH_H
#define PLAYER_HASH_H

#include <string>
#include "friendlist.h"

class PlayerHashTable {
private:
    static const int TABLE_SIZE = 100;

    struct HashNode {
        std::string username;
        FriendList friendList;  // Each node now has a FriendList
        HashNode* next;

        HashNode(const std::string& name) :
            username(name), next(nullptr) {}
    };

    HashNode* table[TABLE_SIZE];

    // Improved hash function
    int hashUsername(const std::string& username) {
        unsigned long hash = 5381;
        for (char c : username) {
            hash = ((hash << 5) + hash) + c; // hash * 33 + c
        }
        return hash % TABLE_SIZE;
    }

public:
    PlayerHashTable();
    ~PlayerHashTable();

    // Add player if not exists
    void addPlayer(const std::string& username);

    // Find mutual friends between two users
    void findMutualFriends(const std::string& user1,
        const std::string& user2,
        std::string mutualFriends[],
        int& count);

    // Get friend list for a username
    FriendList* getFriendList(const std::string& username);

    // Check if a user exists
    bool userExists(const std::string& username);

    // Remove a player
    void removePlayer(const std::string& username);
};

// Global hash table instance
extern PlayerHashTable playerHashTable;

#endif // PLAYER_HASH_H