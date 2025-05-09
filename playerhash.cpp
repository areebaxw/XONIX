#include "playerhash.h"
#include <cmath>
#include <iostream>

PlayerHashTable::PlayerHashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        table[i] = nullptr;
    }
}

PlayerHashTable::~PlayerHashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

void PlayerHashTable::addPlayer(const std::string& username) {
    // Prevent duplicate players
    if (userExists(username)) return;

    int index = hashUsername(username);
    HashNode* newNode = new HashNode(username);

    if (table[index] == nullptr) {
        table[index] = newNode;
    }
    else {
        // Handle collision by chaining
        HashNode* current = table[index];
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void PlayerHashTable::findMutualFriends(const std::string& user1,
    const std::string& user2,
    std::string mutualFriends[],
    int& count) {
    count = 0;

    // Get friend lists for both users
    FriendList* friendList1 = getFriendList(user1);
    FriendList* friendList2 = getFriendList(user2);

    if (!friendList1 || !friendList2) return;

    // Get friends for both users
    std::string friends1[100];
    std::string friends2[100];
    int count1 = 0, count2 = 0;

    friendList1->getFriendsList(friends1, count1);
    friendList2->getFriendsList(friends2, count2);

    // Find mutual friends
    for (int i = 0; i < count1; ++i) {
        for (int j = 0; j < count2; ++j) {
            if (friends1[i] == friends2[j]) {
                mutualFriends[count++] = friends1[i];
                break;
            }
        }
    }
}

FriendList* PlayerHashTable::getFriendList(const std::string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];

    while (current != nullptr) {
        if (current->username == username) {
            return &(current->friendList);
        }
        current = current->next;
    }

    return nullptr;
}

bool PlayerHashTable::userExists(const std::string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];

    while (current != nullptr) {
        if (current->username == username) {
            return true;
        }
        current = current->next;
    }

    return false;
}

void PlayerHashTable::removePlayer(const std::string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];
    HashNode* prev = nullptr;

    while (current != nullptr) {
        if (current->username == username) {
            if (prev == nullptr) {
                table[index] = current->next;
            }
            else {
                prev->next = current->next;
            }

            delete current;
            return;
        }

        prev = current;
        current = current->next;
    }
}

// Global hash table definition
PlayerHashTable playerHashTable;