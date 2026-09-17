#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>


#include "Type2_Prototype.cpp"
#include "Type3_Prototype.cpp"
#include "Type4_Prototype.cpp"
#include "Type5_Prototype.cpp"
#include "Type6_Prototype.cpp"
#include "Type7_Prototype.cpp"


//Note this is step 0 of this feature space most of this will need to be changed.

using namespace std;

// Represents one registered Camp Buddies user.
struct User {
    int profileId;
    string name;
    string email;

    // Prototype only:
    // In a real app, this must be a password hash, not plaintext.
    string password;
};

// Represents a camping trip owned by one user.
struct Trip {
    int tripId;
    string tripName;
    int ownerProfileId;
};

// Represents a shared gear list that belongs to a trip.
struct GearList {
    int gearListId;
    int tripId;
    int createdByProfileId;
};

// Represents a gear item assigned to a user.
struct GearAssignment {
    int assignmentId;
    string itemName;
    int tripId;
    int assignedToProfileId;
};

class CampBuddiesApp {
private:
    unordered_map<string, User> usersByEmail;

    vector<Trip> trips;
    vector<GearList> gearLists;
    vector<GearAssignment> gearAssignments;

    int nextProfileId {1};
    int nextTripId {1};
    int nextGearListId {1};
    int nextAssignmentId {1};

    string loggedInEmail = "";

public:
    // Requirement: Allow a new user to create an account.
    bool createAccount(const string& name,
                       const string& email,
                       const string& password) {
        if (usersByEmail.find(email) != usersByEmail.end()) {
            cout << "An account with that email already exists.\n";
            return false;
        }

        if (name.empty() || email.empty() || password.empty()) {
            cout << "Name, email, and password are required.\n";
            return false;
        }

        User newUser;
        newUser.profileId {++nextProfileId};
        newUser.name {name};
        newUser.email {email};
        newUser.password {password}; // Replace with password hash in a real app.

        usersByEmail[email] {newUser};

        cout << "Account created successfully.\n";
        cout << "Your profile ID is: " << newUser.profileId << "\n";
        return true;
    }

    // Requirement: Allow an existing user to log in.
    bool login(const string& email, const string& password) {
        if (isLoggedIn()) {
            cout << "Please log out before logging in to another account.\n";
            return false;
        }

        auto user {usersByEmail.find(email)};

        if (user == usersByEmail.end()) {
            cout << "No account was found with that email.\n";
            return false;
        }

        if (user->second.password != password) {
            cout << "Incorrect password.\n";
            return false;
        }

        loggedInEmail {email};
        cout << "Welcome, " << user->second.name << "!\n";
        return true;
    }

    // Requirement: Allow a logged-in user to log out.
    void logout() {
        if (!isLoggedIn()) {
            cout << "No user is currently logged in.\n";
            return;
        }

        cout << usersByEmail[loggedInEmail].name
             << " has been logged out.\n";

        loggedInEmail = "";
    }

    bool isLoggedIn() const {
        return !loggedInEmail.empty();
    }

    // Gets the currently logged-in user.
    User* getCurrentUser() {
        if (!isLoggedIn()) {
            return nullptr;
        }

        return &usersByEmail[loggedInEmail];
    }

    // Requirement: Associate a trip with the logged-in user's account.
    void createTrip(const string& tripName) {
        User* currentUser = getCurrentUser();

        if (currentUser == nullptr) {
            cout << "You must log in before creating a trip.\n";
            return;
        }

        if (tripName.empty()) {
            cout << "Trip name cannot be empty.\n";
            return;
        }

        Trip newTrip;
        newTrip.tripId = nextTripId++;
        newTrip.tripName = tripName;
        newTrip.ownerProfileId = currentUser->profileId;

        trips.push_back(newTrip);

        cout << "Trip created: " << newTrip.tripName
             << " (Trip ID: " << newTrip.tripId << ")\n";
    }

    // Requirement: Associate a gear list with a trip and user account.
    void createGearList(int tripId) {
        User* currentUser = getCurrentUser();

        if (currentUser == nullptr) {
            cout << "You must log in before creating a gear list.\n";
            return;
        }

        if (!userOwnsTrip(tripId, currentUser->profileId)) {
            cout << "You can only create a gear list for one of your trips.\n";
            return;
        }

        GearList newGearList;
        newGearList.gearListId = nextGearListId++;
        newGearList.tripId = tripId;
        newGearList.createdByProfileId = currentUser->profileId;

        gearLists.push_back(newGearList);

        cout << "Gear list created for Trip ID " << tripId << ".\n";
    }

    // Requirement: Associate gear assignments with user accounts.
    void assignGear(int tripId,
                    const string& itemName,
                    int assignedToProfileId) {
        User* currentUser = getCurrentUser();

        if (currentUser == nullptr) {
            cout << "You must log in before assigning gear.\n";
            return;
        }

        if (!userOwnsTrip(tripId, currentUser->profileId)) {
            cout << "You can only manage gear for one of your trips.\n";
            return;
        }

        if (!profileExists(assignedToProfileId)) {
            cout << "That profile ID does not exist.\n";
            return;
        }

        GearAssignment newAssignment;
        newAssignment.assignmentId {++nextAssignmentId};
        newAssignment.itemName {itemName};
        newAssignment.tripId {tripId};
        newAssignment.assignedToProfileId {assignedToProfileId};

        gearAssignments.push_back(newAssignment);

        cout << itemName << " has been assigned to Profile ID "
             << assignedToProfileId << ".\n";
    }

    // Displays all trips belonging to the logged-in user.
    void viewMyTrips() {
        User* currentUser = getCurrentUser();

        if (currentUser == nullptr) {
            cout << "You must log in to view your trips.\n";
            return;
        }

        cout << "\n--- My Trips ---\n";

        bool foundTrip {false};

        for (const Trip& trip : trips) {
            if (trip.ownerProfileId == currentUser->profileId) {
                cout << "Trip ID: " << trip.tripId
                     << " | Name: " << trip.tripName << "\n";
                foundTrip = true;
            }
        }

        if (!foundTrip) {
            cout << "You have not created any trips yet.\n";
        }
    }

    // Displays gear assignments for a particular trip.
    void viewGearAssignments(int tripId) {
        User* currentUser = getCurrentUser();

        if (currentUser == nullptr) {
            cout << "You must log in to view gear assignments.\n";
            return;
        }

        if (!userOwnsTrip(tripId, currentUser->profileId)) {
            cout << "You can only view gear for one of your trips.\n";
            return;
        }

        cout << "\n--- Gear Assignments for Trip ID "
             << tripId << " ---\n";

        bool foundAssignment {false};

        for (const GearAssignment& assignment : gearAssignments) {
            if (assignment.tripId == tripId) {
                cout << "Item: " << assignment.itemName
                     << " | Assigned to Profile ID: "
                     << assignment.assignedToProfileId << "\n";

                foundAssignment {true};
            }
        }

        if (!foundAssignment) {
            cout << "No gear has been assigned yet.\n";
        }
    }

private:
    bool profileExists(int profileId) const {
        for (const auto& pair : usersByEmail) {
            if (pair.second.profileId == profileId) {
                return true;
            }
        }

        return false;
    }

    bool userOwnsTrip(int tripId, int profileId) const {
        for (const Trip& trip : trips) {
            if (trip.tripId == tripId &&
                trip.ownerProfileId == profileId) {
                return true;
            }
        }

        return false;
    }
};

int main() {
    CampBuddiesApp app;

    int choice;

    do {
        cout << "\n===== CAMP BUDDIES =====\n";
        cout << "1. Create account\n";
        cout << "2. Log in\n";
        cout << "3. Log out\n";
        cout << "4. Create trip\n";
        cout << "5. View my trips\n";
        cout << "6. Create gear list\n";
        cout << "7. Assign gear\n";
        cout << "8. View gear assignments\n";
        cout << "9. Exit\n";
        cout << "Choose an option: ";

        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string name;
        string email;
        string password;
        string tripName;
        string itemName;
        int tripId;
        int profileId;

        switch (choice) {
            case 1:
                cout << "Enter your name: ";
                getline(cin, name);

                cout << "Enter your email: ";
                getline(cin, email);

                cout << "Create a password: ";
                getline(cin, password);

                app.createAccount(name, email, password);
                break;

            case 2:
                cout << "Email: ";
                getline(cin, email);

                cout << "Password: ";
                getline(cin, password);

                app.login(email, password);
                break;

            case 3:
                app.logout();
                break;

            case 4:
                cout << "Enter trip name: ";
                getline(cin, tripName);

                app.createTrip(tripName);
                break;

            case 5:
                app.viewMyTrips();
                break;

            case 6:
                cout << "Enter Trip ID: ";
                cin >> tripId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.createGearList(tripId);
                break;

            case 7:
                cout << "Enter Trip ID: ";
                cin >> tripId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter gear item: ";
                getline(cin, itemName);

                cout << "Enter the member's Profile ID: ";
                cin >> profileId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.assignGear(tripId, itemName, profileId);
                break;

            case 8:
                cout << "Enter Trip ID: ";
                cin >> tripId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.viewGearAssignments(tripId);
                break;

            case 9:
                cout << "Closing Camp Buddies.\n";
                break;

            default:
                cout << "Invalid option. Please choose 1 through 9.\n";
        }

    } while (choice != 9);

    return 0;
}
