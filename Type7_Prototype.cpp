#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

// ----------------------------------------------------
// DATA STRUCTURES
// ----------------------------------------------------

struct User {
    int profileId;
    string name;
    string email;
};

struct Trip {
    int tripId;
    string tripName;
    string location;
    string startDate;
    string endDate;

    int leaderProfileId;
    vector<int> memberProfileIds;
};

enum class GearStatus {
    UNASSIGNED,
    ASSIGNED,
    COMPLETED
};

struct GearItem {
    int gearItemId;
    int tripId;

    string itemName;
    int quantity;
    string category;
    string notes;

    GearStatus status;
    int assignedProfileId;  // -1 = no assigned member.
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;
    vector<GearItem> gearItems;

    int nextProfileId = 1;
    int nextTripId = 1;
    int nextGearItemId = 1;

    // -1 means no user is logged in.
    int loggedInProfileId = -1;

public:
    // ----------------------------------------------------
    // USER ACCOUNTS / SIMPLE LOGIN
    // ----------------------------------------------------

    void createAccount(const string& name, const string& email) {
        if (name.empty()) {
            cout << "Error: Name is required to create an account.\n";
            return;
        }

        if (email.empty()) {
            cout << "Error: Email is required to create an account.\n";
            return;
        }

        if (emailAlreadyExists(email)) {
            cout << "Error: An account already exists with that email.\n";
            return;
        }

        User newUser;
        newUser.profileId = nextProfileId++;
        newUser.name = name;
        newUser.email = email;

        users.push_back(newUser);

        cout << "Account created successfully.\n";
        cout << "Your Profile ID is: " << newUser.profileId << "\n";
    }

    void login(int profileId) {
        if (!userExists(profileId)) {
            cout << "Error: No user exists with that Profile ID.\n";
            return;
        }

        loggedInProfileId = profileId;

        cout << "Logged in as "
             << getUserName(loggedInProfileId)
             << ".\n";
    }

    void logout() {
        if (!isLoggedIn()) {
            cout << "Error: No user is currently logged in.\n";
            return;
        }

        cout << getUserName(loggedInProfileId)
             << " has logged out.\n";

        loggedInProfileId = -1;
    }

    // ----------------------------------------------------
    // TRIP MANAGEMENT
    // ----------------------------------------------------

    void createTrip(const string& tripName,
                    const string& location,
                    const string& startDate,
                    const string& endDate) {
        if (!isLoggedIn()) {
            cout << "Error: You must log in before creating a trip.\n";
            return;
        }

        // Required-field validation.
        if (tripName.empty()) {
            cout << "Error: Trip name is required.\n";
            return;
        }

        if (location.empty()) {
            cout << "Error: Trip location is required.\n";
            return;
        }

        if (startDate.empty() || endDate.empty()) {
            cout << "Error: Start date and end date are required.\n";
            return;
        }

        Trip newTrip;
        newTrip.tripId = nextTripId++;
        newTrip.tripName = tripName;
        newTrip.location = location;
        newTrip.startDate = startDate;
        newTrip.endDate = endDate;

        // The logged-in creator becomes the trip leader.
        newTrip.leaderProfileId = loggedInProfileId;

        // The leader is automatically a group member.
        newTrip.memberProfileIds.push_back(loggedInProfileId);

        trips.push_back(newTrip);

        cout << "Trip created successfully.\n";
        cout << "Trip ID: " << newTrip.tripId << "\n";
        cout << "You are the trip leader.\n";
    }

    void addMemberToTrip(int tripId, int newMemberProfileId) {
        Trip* trip = findTrip(tripId);

        if (!isLoggedIn()) {
            cout << "Error: You must log in before adding members.\n";
            return;
        }

        if (trip == nullptr) {
            cout << "Error: Trip ID " << tripId << " was not found.\n";
            return;
        }

        // Access control: only the leader can add members.
        if (trip->leaderProfileId != loggedInProfileId) {
            cout << "Error: Only the trip leader can add group members.\n";
            return;
        }

        if (!userExists(newMemberProfileId)) {
            cout << "Error: That Profile ID does not belong to a user.\n";
            return;
        }

        if (isTripMember(*trip, newMemberProfileId)) {
            cout << "Error: That user is already in this trip group.\n";
            return;
        }

        trip->memberProfileIds.push_back(newMemberProfileId);

        cout << getUserName(newMemberProfileId)
             << " was added to "
             << trip->tripName << ".\n";
    }

    // ----------------------------------------------------
    // SHARED GEAR LIST MANAGEMENT
    // ----------------------------------------------------

    void addGearItem(int tripId,
                     const string& itemName,
                     int quantity,
                     const string& category,
                     const string& notes) {
        Trip* trip = findTrip(tripId);

        if (!isLoggedIn()) {
            cout << "Error: You must log in before adding gear.\n";
            return;
        }

        if (trip == nullptr) {
            cout << "Error: Trip ID " << tripId << " was not found.\n";
            return;
        }

        // A user must belong to the trip to add gear.
        if (!isTripMember(*trip, loggedInProfileId)) {
            cout << "Error: You cannot add gear to a trip you have not joined.\n";
            return;
        }

        // Required-field validation.
        if (itemName.empty()) {
            cout << "Error: Gear item name is required.\n";
            return;
        }

        if (category.empty()) {
            cout << "Error: Gear category is required.\n";
            return;
        }

        if (quantity <= 0) {
            cout << "Error: Gear quantity must be at least 1.\n";
            return;
        }

        GearItem newItem;
        newItem.gearItemId = nextGearItemId++;
        newItem.tripId = tripId;
        newItem.itemName = itemName;
        newItem.quantity = quantity;
        newItem.category = category;
        newItem.notes = notes;
        newItem.status = GearStatus::UNASSIGNED;
        newItem.assignedProfileId = -1;

        gearItems.push_back(newItem);

        cout << itemName << " was added to the shared gear list.\n";
    }

    // ----------------------------------------------------
    // GEAR ASSIGNMENT AND PACKING CHECK-OFF
    // ----------------------------------------------------

    void claimGearItem(int tripId, int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (!isLoggedIn()) {
            cout << "Error: You must log in before claiming gear.\n";
            return;
        }

        if (trip == nullptr || item == nullptr) {
            cout << "Error: Trip or gear item was not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "Error: This gear item does not belong to this trip.\n";
            return;
        }

        // Prevent users outside the group from making changes.
        if (!isTripMember(*trip, loggedInProfileId)) {
            cout << "Error: You cannot claim gear for a trip you have not joined.\n";
            return;
        }

        if (item->status == GearStatus::COMPLETED) {
            cout << "Error: This item is already packed and completed.\n";
            return;
        }

        if (item->assignedProfileId != -1 &&
            item->assignedProfileId != loggedInProfileId) {
            cout << "Error: "
                 << item->itemName
                 << " is already assigned to "
                 << getUserName(item->assignedProfileId)
                 << ".\n";
            return;
        }

        item->assignedProfileId = loggedInProfileId;
        item->status = GearStatus::ASSIGNED;

        cout << "You are now responsible for bringing "
             << item->itemName << ".\n";
    }

    void markGearItemPacked(int tripId, int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (!isLoggedIn()) {
            cout << "Error: You must log in before marking gear packed.\n";
            return;
        }

        if (trip == nullptr || item == nullptr) {
            cout << "Error: Trip or gear item was not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "Error: This gear item does not belong to this trip.\n";
            return;
        }

        if (!isTripMember(*trip, loggedInProfileId)) {
            cout << "Error: You cannot update a trip you have not joined.\n";
            return;
        }

        if (item->assignedProfileId == -1) {
            cout << "Error: This item must be claimed before it is packed.\n";
            return;
        }

        // Only the person responsible for the item can check it off.
        if (item->assignedProfileId != loggedInProfileId) {
            cout << "Error: Only "
                 << getUserName(item->assignedProfileId)
                 << " can mark this item as packed.\n";
            return;
        }

        item->status = GearStatus::COMPLETED;

        cout << item->itemName
             << " is marked packed and ready.\n";
    }

    // ----------------------------------------------------
    // DASHBOARD / VIEWING
    // ----------------------------------------------------

    void showTripDashboard(int tripId) {
        Trip* trip = findTrip(tripId);

        if (!isLoggedIn()) {
            cout << "Error: You must log in before viewing the dashboard.\n";
            return;
        }

        if (trip == nullptr) {
            cout << "Error: Trip ID " << tripId << " was not found.\n";
            return;
        }

        // Users cannot view trips they did not join.
        if (!isTripMember(*trip, loggedInProfileId)) {
            cout << "Error: You cannot view a trip you have not joined.\n";
            return;
        }

        int totalItems = 0;
        int unassignedItems = 0;
        int assignedItems = 0;
        int completedItems = 0;

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId) {
                totalItems++;

                if (item.status == GearStatus::UNASSIGNED) {
                    unassignedItems++;
                } else if (item.status == GearStatus::ASSIGNED) {
                    assignedItems++;
                } else if (item.status == GearStatus::COMPLETED) {
                    completedItems++;
                }
            }
        }

        double packingProgress = 0.0;

        if (totalItems > 0) {
            packingProgress =
                (static_cast<double>(completedItems) / totalItems) * 100.0;
        }

        cout << "\n========================================\n";
        cout << "          CAMP BUDDIES DASHBOARD\n";
        cout << "========================================\n";

        cout << "\nTRIP INFORMATION\n";
        cout << "Trip ID: " << trip->tripId << "\n";
        cout << "Trip: " << trip->tripName << "\n";
        cout << "Location: " << trip->location << "\n";
        cout << "Dates: " << trip->startDate
             << " to " << trip->endDate << "\n";
        cout << "Leader: "
             << getUserName(trip->leaderProfileId) << "\n";

        cout << "\nPARTICIPANTS\n";

        for (int memberId : trip->memberProfileIds) {
            cout << "- " << getUserName(memberId);

            if (memberId == trip->leaderProfileId) {
                cout << " (Trip Leader)";
            }

            cout << "\n";
        }

        cout << "\nPACKING PROGRESS\n";
        cout << "Total Items: " << totalItems << "\n";
        cout << "Unassigned: " << unassignedItems << "\n";
        cout << "Assigned, Not Packed: " << assignedItems << "\n";
        cout << "Completed/Packed: " << completedItems << "\n";
        cout << fixed << setprecision(1);
        cout << "Progress: " << packingProgress << "%\n";

        cout << "\nSHARED GEAR LIST\n";

        if (totalItems == 0) {
            cout << "No gear items have been added yet.\n";
        }

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId) {
                cout << "\n- Item ID: " << item.gearItemId << "\n";
                cout << "  Name: " << item.itemName << "\n";
                cout << "  Quantity: " << item.quantity << "\n";
                cout << "  Category: " << item.category << "\n";
                cout << "  Status: "
                     << getGearStatusText(item.status) << "\n";

                if (item.assignedProfileId == -1) {
                    cout << "  Bringing It: Nobody assigned\n";
                } else {
                    cout << "  Bringing It: "
                         << getUserName(item.assignedProfileId)
                         << "\n";
                }

                if (!item.notes.empty()) {
                    cout << "  Notes: " << item.notes << "\n";
                }
            }
        }

        cout << "\nMISSING OR INCOMPLETE EQUIPMENT\n";

        bool hasIssues = false;

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId &&
                item.status == GearStatus::UNASSIGNED) {
                cout << "- NEEDS ASSIGNMENT: "
                     << item.itemName << "\n";

                hasIssues = true;
            }

            if (item.tripId == tripId &&
                item.status == GearStatus::ASSIGNED) {
                cout << "- NOT PACKED YET: "
                     << item.itemName
                     << " is assigned to "
                     << getUserName(item.assignedProfileId)
                     << "\n";

                hasIssues = true;
            }
        }

        if (!hasIssues && totalItems > 0) {
            cout << "All gear is assigned and packed.\n";
        }

        cout << "========================================\n";
    }

    // ----------------------------------------------------
    // SAMPLE MVP TEST
    // This satisfies the requirement to test a trip
    // with multiple users and multiple gear items.
    // ----------------------------------------------------

    void runSampleMVPTest() {
        cout << "\n========== RUNNING SAMPLE MVP TEST ==========\n";

        // Create multiple users.
        createAccount("Alex", "alex@email.com");
        createAccount("Jordan", "jordan@email.com");
        createAccount("Taylor", "taylor@email.com");

        // Alex logs in and creates the trip.
        login(1);

        createTrip(
            "Yosemite Weekend",
            "Upper Pines Campground, Yosemite National Park",
            "2026-10-17",
            "2026-10-19"
        );

        // Alex adds Jordan and Taylor.
        addMemberToTrip(1, 2);
        addMemberToTrip(1, 3);

        // Alex creates initial gear items.
        addGearItem(
            1,
            "Four-Person Tent",
            1,
            "Shelter",
            "Bring tent stakes and rain fly."
        );

        addGearItem(
            1,
            "First Aid Kit",
            1,
            "Safety",
            "Check expiration dates."
        );

        logout();

        // Jordan claims and packs water.
        login(2);

        addGearItem(
            1,
            "Water Bottles",
            6,
            "Water",
            "At least two liters per person."
        );

        claimGearItem(1, 3);
        markGearItemPacked(1, 3);

        logout();

        // Taylor claims the first-aid kit,
        // but has not marked it packed yet.
        login(3);
        claimGearItem(1, 2);
        logout();

        // Alex views the completed test dashboard.
        login(1);
        showTripDashboard(1);
        logout();

        cout << "========== SAMPLE MVP TEST COMPLETE ==========\n";
    }

private:
    bool isLoggedIn() const {
        return loggedInProfileId != -1;
    }

    bool userExists(int profileId) const {
        for (const User& user : users) {
            if (user.profileId == profileId) {
                return true;
            }
        }

        return false;
    }

    bool emailAlreadyExists(const string& email) const {
        for (const User& user : users) {
            if (user.email == email) {
                return true;
            }
        }

        return false;
    }

    Trip* findTrip(int tripId) {
        for (Trip& trip : trips) {
            if (trip.tripId == tripId) {
                return &trip;
            }
        }

        return nullptr;
    }

    GearItem* findGearItem(int gearItemId) {
        for (GearItem& item : gearItems) {
            if (item.gearItemId == gearItemId) {
                return &item;
            }
        }

        return nullptr;
    }

    bool isTripMember(const Trip& trip, int profileId) const {
        return find(
            trip.memberProfileIds.begin(),
            trip.memberProfileIds.end(),
            profileId
        ) != trip.memberProfileIds.end();
    }

    string getUserName(int profileId) const {
        for (const User& user : users) {
            if (user.profileId == profileId) {
                return user.name;
            }
        }

        return "Unknown User";
    }

    string getGearStatusText(GearStatus status) const {
        switch (status) {
            case GearStatus::UNASSIGNED:
                return "Unassigned";

            case GearStatus::ASSIGNED:
                return "Assigned, Not Packed";

            case GearStatus::COMPLETED:
                return "Completed/Packed";
        }

        return "Unknown";
    }
};

// ----------------------------------------------------
// SIMPLE CONSOLE INTERFACE
// ----------------------------------------------------

void showMenu() {
    cout << "\n========== CAMP BUDDIES ==========\n";
    cout << "1. Create account\n";
    cout << "2. Log in\n";
    cout << "3. Log out\n";
    cout << "4. Create camping trip\n";
    cout << "5. Add group member\n";
    cout << "6. Add gear item\n";
    cout << "7. Claim gear item\n";
    cout << "8. Mark gear item packed\n";
    cout << "9. View trip dashboard\n";
    cout << "10. Run sample MVP test\n";
    cout << "0. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    CampBuddiesApp app;
    int choice;

    do {
        showMenu();

        // Validate that the menu selection is a number.
        while (!(cin >> choice)) {
            cout << "Error: Enter a valid menu number: ";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string name;
        string email;
        string tripName;
        string location;
        string startDate;
        string endDate;
        string itemName;
        string category;
        string notes;

        int profileId;
        int tripId;
        int gearItemId;
        int quantity;

        switch (choice) {
            case 1:
                cout << "Enter your name: ";
                getline(cin, name);

                cout << "Enter your email: ";
                getline(cin, email);

                app.createAccount(name, email);
                break;

            case 2:
                cout << "Enter your Profile ID: ";
                cin >> profileId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.login(profileId);
                break;

            case 3:
                app.logout();
                break;

            case 4:
                cout << "Trip name: ";
                getline(cin, tripName);

                cout << "Location: ";
                getline(cin, location);

                cout << "Start date (YYYY-MM-DD): ";
                getline(cin, startDate);

                cout << "End date (YYYY-MM-DD): ";
                getline(cin, endDate);

                app.createTrip(tripName, location, startDate, endDate);
                break;

            case 5:
                cout << "Trip ID: ";
                cin >> tripId;

                cout << "Profile ID of member to add: ";
                cin >> profileId;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.addMemberToTrip(tripId, profileId);
                break;

            case 6:
                cout << "Trip ID: ";
                cin >> tripId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Gear item name: ";
                getline(cin, itemName);

                cout << "Quantity: ";
                cin >> quantity;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Category: ";
                getline(cin, category);

                cout << "Notes (optional): ";
                getline(cin, notes);

                app.addGearItem(
                    tripId,
                    itemName,
                    quantity,
                    category,
                    notes
                );
                break;

            case 7:
                cout << "Trip ID: ";
                cin >> tripId;

                cout << "Gear Item ID to claim: ";
                cin >> gearItemId;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.claimGearItem(tripId, gearItemId);
                break;

            case 8:
                cout << "Trip ID: ";
                cin >> tripId;

                cout << "Gear Item ID to mark packed: ";
                cin >> gearItemId;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.markGearItemPacked(tripId, gearItemId);
                break;

            case 9:
                cout << "Trip ID: ";
                cin >> tripId;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                app.showTripDashboard(tripId);
                break;

            case 10:
                app.runSampleMVPTest();
                break;

            case 0:
                cout << "Thank you for using Camp Buddies.\n";
                break;

            default:
                cout << "Error: Please select a number from 0 to 10.\n";
        }

    } while (choice != 0);

    return 0;
}
