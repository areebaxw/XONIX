#include "playerhash.h"
#include <cmath>
#include <iostream>



PlayerHashTable playerHashTable;


PlayerHashTable::PlayerHashTable() 
{
    for (int i = 0; i < tablesize; ++i) {
        table[i] = NULL;
    }
}

PlayerHashTable::~PlayerHashTable() 
{
    for (int i = 0; i < tablesize; ++i) {
        HashNode* current = table[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
}


void PlayerHashTable::addPlayer(const string& username) {

    if (userExists(username)) 
        return;

    int index = hashUsername(username);
    HashNode* newNode = new HashNode(username);

    if (table[index] == NULL) {
        table[index] = newNode;
    }
    else {
        //--------------------- Handling collision by chaining--------------------------//
        HashNode* current = table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

//-------------HASH FUNCTION for username-----------------------------//
void PlayerHashTable::findMutualFriends(const string& user1,const string& user2,string mutualFriends[],int& count) {
    count = 0;

    
    FriendList* friendList1 = getFriendList(user1);
    FriendList* friendList2 = getFriendList(user2);

    if (!friendList1 || !friendList2) return;


    string friends1[100];
    string friends2[100];
    int count1 = 0, count2 = 0;

    friendList1->getFriendsList(friends1, count1);
    friendList2->getFriendsList(friends2, count2);



    //-------------MUTUAL FRIENDS-----------------------------//
    for (int i = 0; i < count1; ++i) {
        for (int j = 0; j < count2; ++j) {
            if (friends1[i] == friends2[j]) {
                mutualFriends[count++] = friends1[i];
                break;
            }
        }
    }
}

//-------------HASH FUNCTION for friends-----------------------------//
FriendList* PlayerHashTable::getFriendList(const string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];

    while (current != NULL) {
        if (current->username == username) {
            return &(current->friendList);
        }
        current = current->next;
    }

    return NULL;
}

//-------------HASH FUNCTION for user exits-----------------------------//
bool PlayerHashTable::userExists(const string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];

    while (current != NULL) {
        if (current->username == username) {
            return true;
        }
        current = current->next;
    }

    return false;
}
//-------------HASH FUNCTION for player-----------------------------//
void PlayerHashTable::removePlayer(const string& username) {
    int index = hashUsername(username);
    HashNode* current = table[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (current->username == username) {
            if (prev == NULL) {
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
