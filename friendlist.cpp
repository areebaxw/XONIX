#include "friendlist.h"

// Friend Node Constructor
FriendList::FriendNode::FriendNode(const std::string& username) :
    friendUsername(username), next(nullptr) {}

// Friend Request Node Constructor
FriendList::FriendRequestNode::FriendRequestNode(const std::string& username) :
    requestUsername(username), next(nullptr) {}

// Constructor
FriendList::FriendList() : friendsHead(nullptr), friendRequestsHead(nullptr) {}

// Destructor
FriendList::~FriendList() {
    // Clear friends list
    while (friendsHead != nullptr) {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
    }

    // Clear friend requests list
    while (friendRequestsHead != nullptr) {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
    }
}

// Add a friend
void FriendList::addFriend(const std::string& username) {
    // Prevent duplicate friends
    if (hasFriend(username)) return;

    FriendNode* newNode = new FriendNode(username);

    if (friendsHead == nullptr) {
        friendsHead = newNode;
        return;
    }

    FriendNode* current = friendsHead;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = newNode;
}

// Remove a friend
void FriendList::removeFriend(const std::string& username) {
    // If list is empty
    if (friendsHead == nullptr) return;

    // If first node is the one to remove
    if (friendsHead->friendUsername == username) {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
        return;
    }

    // Traverse and remove
    FriendNode* current = friendsHead;
    while (current->next != nullptr) {
        if (current->next->friendUsername == username) {
            FriendNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return;
        }
        current = current->next;
    }
}

// Check if a user is a friend
bool FriendList::hasFriend(const std::string& username) const {
    FriendNode* current = friendsHead;
    while (current != nullptr) {
        if (current->friendUsername == username) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Send a friend request
void FriendList::sendFriendRequest(const std::string& username) {
    // Prevent duplicate requests
    if (friendRequestsHead == nullptr) {
        friendRequestsHead = new FriendRequestNode(username);
        return;
    }

    // Check if request already exists
    FriendRequestNode* current = friendRequestsHead;
    while (current != nullptr) {
        if (current->requestUsername == username) {
            return; // Request already exists
        }
        current = current->next;
    }

    // Add new request
    current = friendRequestsHead;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = new FriendRequestNode(username);
}

bool FriendList::acceptFriendRequest(const std::string& username) {
    // Find and remove the request
    if (friendRequestsHead == nullptr) return false;

    // If first node is the request to accept
    if (friendRequestsHead->requestUsername == username) {
        // Add to friends
        addFriend(username);

        // Remove from friend requests
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
        return true;
    }

    // Traverse to find the request
    FriendRequestNode* current = friendRequestsHead;
    while (current->next != nullptr) {
        if (current->next->requestUsername == username) {
            // Add to friends
            addFriend(username);

            // Remove from friend requests
            FriendRequestNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return true;
        }
        current = current->next;
    }
    return false;
}


bool FriendList::acceptFriendRequestWithFile(const std::string& currentUsername, const std::string& username) {
    // Attempt to accept the friend request
    bool accepted = acceptFriendRequest(username);

    if (accepted) {
        // Save updated friends list to file
        std::string friendsFilename = currentUsername + "_friends.txt";
        std::ofstream friendsFile(friendsFilename, std::ios_base::app);
        if (friendsFile.is_open()) {
            friendsFile << username << std::endl;
            friendsFile.close();
        }

        // Remove the request from file
        std::string requestsFilename = currentUsername + "_friend_requests.txt";

        // Read all requests except the accepted one
        std::ifstream inputFile(requestsFilename);
        std::ofstream tempFile("temp_requests.txt");

        std::string line;
        while (std::getline(inputFile, line)) {
            if (line != username) {
                tempFile << line << std::endl;
            }
        }

        inputFile.close();
        tempFile.close();

        // Replace the original file
        std::remove(requestsFilename.c_str());
        std::rename("temp_requests.txt", requestsFilename.c_str());
    }

    return accepted;
}

// Reject a friend request
void FriendList::rejectFriendRequest(const std::string& username) {
    // If list is empty
    if (friendRequestsHead == nullptr) return;

    // If first node is the request to reject
    if (friendRequestsHead->requestUsername == username) {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
        return;
    }

    // Traverse and remove
    FriendRequestNode* current = friendRequestsHead;
    while (current->next != nullptr) {
        if (current->next->requestUsername == username) {
            FriendRequestNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return;
        }
        current = current->next;
    }
}

// Get total number of friends
int FriendList::getFriendCount() const {
    int count = 0;
    FriendNode* current = friendsHead;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

// Get total number of friend requests
int FriendList::getFriendRequestCount() const {
    int count = 0;
    FriendRequestNode* current = friendRequestsHead;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

// Get list of friends
void FriendList::getFriendsList(std::string friendsList[], int& count) const {
    count = 0;
    FriendNode* current = friendsHead;
    while (current != nullptr && count < 100) {  // Assuming max 100 friends
        friendsList[count] = current->friendUsername;
        count++;
        current = current->next;
    }
}

// Get list of friend requests
void FriendList::getFriendRequestsList(std::string requestsList[], int& count) const {
    count = 0;
    FriendRequestNode* current = friendRequestsHead;
    while (current != nullptr && count < 100) {  // Assuming max 100 requests
        requestsList[count] = current->requestUsername;
        count++;
        current = current->next;
    }
}


void FriendList::saveFriendsToFile(const std::string& username) {
    std::string filename = username + "_friends.txt";
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open friends file for " << username << std::endl;
        return;
    }

    FriendNode* current = friendsHead;
    while (current != nullptr) {
        file << current->friendUsername << std::endl;
        current = current->next;
    }

    file.close();
}

void FriendList::saveFriendRequestsToFile(const std::string& username) {
    std::string filename = username + "_friend_requests.txt";
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open friend requests file for " << username << std::endl;
        return;
    }

    FriendRequestNode* current = friendRequestsHead;
    while (current != nullptr) {
        file << current->requestUsername << std::endl;
        current = current->next;
    }

    file.close();
}

void FriendList::loadFriendsFromFile(const std::string& username) {
    std::string filename = username + "_friends.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "No friends file found for " << username << std::endl;
        return;
    }

    // Clear existing friends list
    while (friendsHead != nullptr) {
        FriendNode* temp = friendsHead;
        friendsHead = friendsHead->next;
        delete temp;
    }

    std::string friendUsername;
    while (std::getline(file, friendUsername)) {
        addFriend(friendUsername);
    }

    file.close();
}

void FriendList::loadFriendRequestsFromFile(const std::string& username) {
    std::string filename = username + "_friend_requests.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "No friend requests file found for " << username << std::endl;
        return;
    }

    // Clear existing friend requests list
    while (friendRequestsHead != nullptr) {
        FriendRequestNode* temp = friendRequestsHead;
        friendRequestsHead = friendRequestsHead->next;
        delete temp;
    }

    std::string requestUsername;
    while (std::getline(file, requestUsername)) {
        sendFriendRequest(requestUsername);
    }

    file.close();
}