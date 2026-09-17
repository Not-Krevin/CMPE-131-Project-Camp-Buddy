#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

//Note this is step 0 of this feature space most of this will need to be changed.

#include "Type1_Prototype.cpp"
#include "Type2_Prototype.cpp"
#include "Type3_Prototype.cpp"

#include "Type5_Prototype.cpp"
#include "Type6_Prototype.cpp"
#include "Type7_Prototype.cpp"


// ----------------------------------------
// EXISTING USER AND TRIP STRUCTURES
// ----------------------------------------

struct User {
    int profileId;
    string name;
    string email;
};

struct Trip {
    int tripId;
    string tripName;
    int leaderProfileId;

    // All users allowed to participate in this trip.
    vector<int> memberProfileIds;
};

// ----------------------------------------
// GEAR LIST STRUCTURES
// ----------------------------------------

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

    // -1 means no person has claimed responsibility.
    int assignedProfileId;
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;
    vector<GearItem> gearItems;

    int nextGearItemId = 1;

public:
    // ----------------------------------------
    // TEST DATA FUNCTIONS
    // ----------------------------------------

    void addTestUser(int profileId,
                     const string& name,
                     const string& email) {
        users.push_back({profileId, name, email});
    }

    void addTestTrip(int tripId,
                     const string& tripName,
                     int leaderProfileId,
                     const vector<int>& memberIds) {
        trips.push_back({
            tripId,
            tripName,
            leaderProfileId,
            memberIds
        });
    }

    // Adds a gear item to the selected trip.
    // Your Shared Gear Lists code can replace this function.
    void addGearItem(int tripId,
                     const string& itemName,
                     int quantity,
                     const string& category,
                     const string& notes) {
        GearItem newItem;

        newItem.gearItemId {++nextGearItemId};
        newItem.tripId {tripId};
        newItem.itemName {itemName};
        newItem.quantity {quantity};
        newItem.category {category};
        newItem.notes {notes};
        newItem.status {GearStatus::UNASSIGNED};
        newItem.assignedProfileId = -1;

        gearItems.push_back(newItem);
    }

    // ----------------------------------------
    // REQUIREMENT 1:
    // A group member claims responsibility
    // for bringing a gear item.
    // ----------------------------------------
    void claimGearItem(int tripId,
                       int memberProfileId,
                       int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (trip == nullptr || item == nullptr) {
            cout << "Trip or gear item was not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "This gear item does not belong to this trip.\n";
            return;
        }

        if (!isTripMember(*trip, memberProfileId)) {
            cout << "Only a member of this camping group can claim gear.\n";
            return;
        }

        if (item->status == GearStatus::COMPLETED) {
            cout << "This item is already packed and completed.\n";
            return;
        }

        // Prevent two people from being responsible for the same item.
        if (item->assignedProfileId != -1 &&
            item->assignedProfileId != memberProfileId) {
            cout << item->itemName << " is already assigned to "
                 << getUserName(item->assignedProfileId) << ".\n";
            return;
        }

        item->assignedProfileId = memberProfileId;
        item->status = GearStatus::ASSIGNED;

        cout << getUserName(memberProfileId)
             << " is now responsible for bringing "
             << item->itemName << ".\n";
    }

    // ----------------------------------------
    // REQUIREMENT 2:
    // Show all group members who is bringing
    // each item and the item's current status.
    // ----------------------------------------
    void viewGearAssignments(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only trip members may view the gear list.\n";
            return;
        }

        cout << "\n===== GEAR ASSIGNMENTS =====\n";
        cout << "Trip: " << trip->tripName << "\n\n";

        bool gearExists = false;

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId) {
                cout << "Gear Item ID: " << item.gearItemId << "\n";
                cout << "Item: " << item.itemName << "\n";
                cout << "Quantity Needed: " << item.quantity << "\n";
                cout << "Category: " << item.category << "\n";
                cout << "Status: "
                     << getGearStatusText(item.status) << "\n";

                if (item.assignedProfileId == -1) {
                    cout << "Bringing It: Nobody assigned yet\n";
                } else {
                    cout << "Bringing It: "
                         << getUserName(item.assignedProfileId)
                         << " (Profile ID: "
                         << item.assignedProfileId << ")\n";
                }

                cout << "------------------------------\n";
                gearExists = true;
            }
        }

        if (!gearExists) {
            cout << "There are no gear items for this trip.\n";
        }
    }

    // ----------------------------------------
    // REQUIREMENT 3:
    // The member assigned to an item can mark
    // it packed or ready.
    // ----------------------------------------
    void markGearItemPacked(int tripId,
                            int memberProfileId,
                            int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (trip == nullptr || item == nullptr) {
            cout << "Trip or gear item was not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "This gear item does not belong to this trip.\n";
            return;
        }

        if (!isTripMember(*trip, memberProfileId)) {
            cout << "You are not a member of this trip.\n";
            return;
        }

        if (item->assignedProfileId == -1) {
            cout << "This item must be claimed before it can be marked packed.\n";
            return;
        }

        if (item->assignedProfileId != memberProfileId) {
            cout << "Only "
                 << getUserName(item->assignedProfileId)
                 << " can mark this item as packed.\n";
            return;
        }

        item->status = GearStatus::COMPLETED;

        cout << item->itemName
             << " has been marked packed and ready.\n";
    }

    // ----------------------------------------
    // REQUIREMENT 4:
    // Shared list automatically reflects the
    // current status because every function
    // changes the same GearItem object.
    // ----------------------------------------

    // ----------------------------------------
    // REQUIREMENT 5:
    // Detect and clearly identify unassigned
    // and duplicate gear items.
    // ----------------------------------------
    void showGearWarnings(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can view gear warnings.\n";
            return;
        }

        cout << "\n===== GEAR WARNINGS =====\n";

        bool hasWarnings {false};

        // First, identify unassigned items.
        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId &&
                item.status == GearStatus::UNASSIGNED) {
                cout << "UNASSIGNED: "
                     << item.itemName
                     << " has not been claimed by a group member.\n";

                hasWarnings {true};
            }
        }

        // Next, identify duplicate item names.
        // The outer loop checks each item.
        for (size_t i = 0; i < gearItems.size(); i++) {
            if (gearItems[i].tripId != tripId) {
                continue;
            }

            int duplicateCount {0};

            for (size_t j{0}; j < gearItems.size(); ++j) {
                if (gearItems[j].tripId == tripId &&
                    namesMatch(gearItems[i].itemName,
                               gearItems[j].itemName)) {
                    duplicateCount++;
                }
            }

            // Print each duplicate warning only once.
            bool alreadyReported {false};

            for (size_t k{0}; k < i; ++k) {
                if (gearItems[k].tripId == tripId &&
                    namesMatch(gearItems[i].itemName,
                               gearItems[k].itemName)) {
                    alreadyReported = true;
                    break;
                }
            }

            if (duplicateCount > 1 && !alreadyReported) {
                cout << "DUPLICATE: \""
                     << gearItems[i].itemName
                     << "\" appears "
                     << duplicateCount
                     << " times in this gear list.\n";

                hasWarnings = true;
            }
        }

        if (!hasWarnings) {
            cout << "No duplicate or unassigned gear items found.\n";
        }
    }

private:
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
                return "Assigned";

            case GearStatus::COMPLETED:
                return "Completed";
        }

        return "Unknown";
    }

    // Converts a string to lowercase.
    // This allows "Tent", "tent", and "TENT"
    // to be treated as duplicate names.
    string toLowerCase(string text) const {
        for (char& character : text) {
            character = tolower(
                static_cast<unsigned char>(character)
            );
        }

        return text;
    }

    bool namesMatch(const string& firstName,
                    const string& secondName) const {
        return toLowerCase(firstName) == toLowerCase(secondName);
    }
};

int main() {
    CampBuddiesApp app;

    // Create test users.
    app.addTestUser(1, "Alex", "alex@email.com");
    app.addTestUser(2, "Jordan", "jordan@email.com");
    app.addTestUser(3, "Taylor", "taylor@email.com");

    // Alex is the leader. All three users are group members.
    app.addTestTrip(
        1,
        "Yosemite Weekend",
        1,
        {1, 2, 3}
    );

    // Add test gear items.
    app.addGearItem(
        1,
        "Four-Person Tent",
        1,
        "Shelter",
        "Bring tent stakes and rain fly."
    );

    app.addGearItem(
        1,
        "Water Bottles",
        6,
        "Water",
        "Two liters per person."
    );

    // Intentional duplicate so you can test the warning system.
    app.addGearItem(
        1,
        "water bottles",
        4,
        "Water",
        "Check whether this is still needed."
    );

    app.addGearItem(
        1,
        "First Aid Kit",
        1,
        "Safety",
        ""
    );

    // Jordan claims and packs water bottles.
    app.claimGearItem(1, 2, 2);
    app.markGearItemPacked(1, 2, 2);

    // Taylor tries to claim Jordan's item.
    // The program prevents duplicate responsibility.
    app.claimGearItem(1, 3, 2);

    // Every trip member can see who is responsible for every item.
    app.viewGearAssignments(1, 3);

    // Show duplicate and unassigned-item warnings.
    app.showGearWarnings(1, 1);

    return 0;
}
