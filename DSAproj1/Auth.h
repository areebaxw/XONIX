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
        : username(user), password(pass), next(NULL) {}
};

class Authentication {
private:
    //---------------------UI Elements---------------------
    Font font;
    Text titleText, usernameText, passwordText, modeText, messageText;
    RectangleShape usernameBox, passwordBox;


    //---------------------Authentication Data---------------------
    string currentUser;
    bool isLoginMode;



    //---------------------User List Management---------------------
    UserNode* userListHead;


    //---------------------Private Methods---------------------
    void loadUsers();
    void saveUsers();
    void addUser(const string& username, const string& password);
    bool findUser(const string& username);
    bool checkPassword(const string& username, const string& password);
    void clearUserList();

public:
   
    Text ourtext;

    //---------------------Friend List Management---------------------
    void getValidUsers(string* usernames, int& count, const string& currentUsername);

    //---------------------Getter---------------------
    UserNode* getUserListHead() 
    { 
        return userListHead; 
    }

    //---------------------Constructor and Destructor---------------------
    Authentication();
    ~Authentication();

    //---------------------Authentication Screen---------------------
    bool showAuthScreen(RenderWindow& window);

    //---------------------Current Username---------------------
    string getCurrentUsername() const;
};
