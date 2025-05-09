#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;
using namespace sf;



struct UserNode {
    string username;
    string password;
    UserNode* next;

    UserNode(const string& user, const string& pass)
        : username(user), password(pass), next(nullptr) {}
};

class Authentication {
private:
    Font font;
    Text titleText, usernameText, passwordText, modeText, messageText;
    RectangleShape usernameBox, passwordBox;
    string currentUser;
    bool isLoginMode;

    UserNode* userListHead;

    void loadUsers();
    void saveUsers();
    void addUser(const string& username, const string& password);
    bool findUser(const string& username);
    bool checkPassword(const string& username, const string& password);
    void clearUserList();

public:



    // friendlist 
 // New method to get valid users
    void getValidUsers(std::string* usernames, int& count, const std::string& currentUsername);

    // Optional: Getter for userListHead if needed
    UserNode* getUserListHead() { return userListHead; }
    Authentication();
    ~Authentication();
    bool showAuthScreen(RenderWindow& window);
    string getCurrentUsername() const;
};