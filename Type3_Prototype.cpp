#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

//Note this is step 0 of this feature space most of this will need to be changed.

// Represents a Camp Buddies user.
struct User {
    int profileId;
    string name;
    string email;
};

// Represents a camping trip.
struct Trip {
    int tripId;
    string tripName;
    string location;
    string startDate;
    string endDate;

    int leaderProfileId;
    vector<int> memberProfileIds;
};

// Represents the current state of a gear item.
enum class GearStatus {
    UNASSIGNED,
    ASSIGNED,
    COMPLETED
};

// Represents one item in a shared trip gear list.
struct GearItem {
    int gearItemId;
    int tripId;

    string itemName;
    int quantity;
    string category;
    string notes;

    GearStatus status;

    // -1 means no group member has claimed the item yet.
    int assignedProfileId;
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;
    vector<GearItem> gearItems;

    int nextGearItemId = 1;

public:
    // --------------------------------------------------
    // TEST DATA HELPERS
    // Replace these with your account and trip code later.
    // --------------------------------------------------

    void addTestUser(int profileId,
                     const string& name,
                     const string& email) {
        users.push_back({profileId, name, email});
    }

    void addTestTrip(int tripId,
                     const string& tripName,
                     int leaderProfileId,
                     const vector<int>& members) {
        Trip newTrip;

        newTrip.tripId = tripId;
        newTrip.tripName = tripName;
        newTrip.location = "Not entered yet";
        newTrip.startDate = "Not entered yet";
        newTrip.endDate = "Not entered yet";
        newTrip.leaderProfileId = leaderProfileId;
        newTrip.memberProfileIds = members;

        trips.push_back(newTrip);
    }

    // --------------------------------------------------
    // SHARED GEAR LIST REQUIREMENTS
    // --------------------------------------------------

    // Requirement:
    // The trip leader can create a shared gear list for a specific trip.
    //
    // In this version, the "gear list" is all GearItem objects
    // that have the same tripId.
    void createSharedGearList(int tripId, int leaderProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can create a shared gear list.\n";
            return;
        }

        cout << "Shared gear list is ready for: "
             << trip->tripName << "\n";
    }

    // Requirement:
    // Users can add gear items with a name, quantity, category,
    // and optional notes.
    //
    // This example allows any trip member to add an item.
    // If your team wants leader-only item creation, replace
    // isTripMember() with isTripLeader().
    void addGearItem(int tripId,
                     int userProfileId,
                     const string& itemName,
                     int quantity,
                     const string& category,
                     const string& notes) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, userProfileId)) {
            cout << "Only group members can add gear items.\n";
            return;
        }

        if (itemName.empty() || category.empty()) {
            cout << "Item name and category are required.\n";
            return;
        }

        if (quantity <= 0) {
            cout << "Quantity must be at least 1.\n";
            return;
        }

        GearItem newItem;

        newItem.gearItemId = nextGearItemId++;
        newItem.tripId = tripId;
        newItem.itemName = itemName;
        newItem.quantity = quantity;
        newItem.category = category;
        newItem.notes = notes;

        // A newly added item has not been claimed yet.
        newItem.status = GearStatus::UNASSIGNED;
        newItem.assignedProfileId = -1;

        gearItems.push_back(newItem);

        cout << "\"" << itemName
             << "\" was added to the shared gear list.\n";
    }

    // Optional but useful feature:
    // A group member claims responsibility for bringing an item.
    //
    // This changes its status from Unassigned to Assigned.
    void assignGearItem(int tripId,
                        int userProfileId,
                        int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (trip == nullptr || item == nullptr) {
            cout << "Trip or gear item not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "This gear item does not belong to this trip.\n";
            return;
        }

        if (!isTripMember(*trip, userProfileId)) {
            cout << "Only trip members can claim gear items.\n";
            return;
        }

        if (item->status == GearStatus::COMPLETED) {
            cout << "This item is already marked as completed.\n";
            return;
        }

        item->assignedProfileId = userProfileId;
        item->status = GearStatus::ASSIGNED;

        cout << getUserName(userProfileId)
             << " is bringing: " << item->itemName << "\n";
    }

    // Optional but useful feature:
    // The user assigned to the item can mark it as packed/completed.
    void markGearItemCompleted(int tripId,
                               int userProfileId,
                               int gearItemId) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (trip == nullptr || item == nullptr) {
            cout << "Trip or gear item not found.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "This gear item does not belong to this trip.\n";
            return;
        }

        if (!isTripMember(*trip, userProfileId)) {
            cout << "You are not a member of this trip.\n";
            return;
        }

        if (item->assignedProfileId != userProfileId) {
            cout << "Only the assigned group member can complete this item.\n";
            return;
        }

        item->status = GearStatus::COMPLETED;

        cout << item->itemName << " is now marked as completed.\n";
    }

    // Requirement:
    // Users can view all gear items needed for a trip.
    // The system displays Unassigned, Assigned, or Completed.
    void viewSharedGearList(int tripId, int userProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, userProfileId)) {
            cout << "Only trip members can view this gear list.\n";
            return;
        }

        cout << "\n===== SHARED GEAR LIST =====\n";
        cout << "Trip: " << trip->tripName << "\n\n";

        bool foundItems = false;

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId) {
                cout << "Gear Item ID: " << item.gearItemId << "\n";
                cout << "Item: " << item.itemName << "\n";
                cout << "Quantity: " << item.quantity << "\n";
                cout << "Category: " << item.category << "\n";

                if (item.notes.empty()) {
                    cout << "Notes: None\n";
                } else {
                    cout << "Notes: " << item.notes << "\n";
                }

                cout << "Status: " << getGearStatusText(item.status) << "\n";

                if (item.assignedProfileId == -1) {
                    cout << "Assigned To: Nobody yet\n";
                } else {
                    cout << "Assigned To: "
                         << getUserName(item.assignedProfileId)
                         << " (Profile ID: "
                         << item.assignedProfileId << ")\n";
                }

                cout << "-----------------------------\n";
                foundItems = true;
            }
        }

        if (!foundItems) {
            cout << "No gear items have been added yet.\n";
        }
    }

    // Requirement:
    // The trip leader can edit a gear item.
    void editGearItem(int tripId,
                      int leaderProfileId,
                      int gearItemId,
                      const string& newItemName,
                      int newQuantity,
                      const string& newCategory,
                      const string& newNotes) {
        Trip* trip = findTrip(tripId);
        GearItem* item = findGearItem(gearItemId);

        if (trip == nullptr || item == nullptr) {
            cout << "Trip or gear item not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can edit gear items.\n";
            return;
        }

        if (item->tripId != tripId) {
            cout << "This item does not belong to the selected trip.\n";
            return;
        }

        if (newItemName.empty() || newCategory.empty()) {
            cout << "Item name and category cannot be empty.\n";
            return;
        }

        if (newQuantity <= 0) {
            cout << "Quantity must be at least 1.\n";
            return;
        }

        item->itemName = newItemName;
        item->quantity = newQuantity;
        item->category = newCategory;
        item->notes = newNotes;

        cout << "Gear item updated successfully.\n";
    }

    // Requirement:
    // The trip leader can remove a gear item.
    void removeGearItem(int tripId,
                        int leaderProfileId,
                        int gearItemId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can remove gear items.\n";
            return;
        }

        for (auto itemPosition = gearItems.begin();
             itemPosition != gearItems.end();
             ++itemPosition) {

            if (itemPosition->gearItemId == gearItemId &&
                itemPosition->tripId == tripId) {

                string deletedItemName = itemPosition->itemName;

                gearItems.erase(itemPosition);

                cout << deletedItemName
                     << " was removed from the gear list.\n";

                return;
            }
        }

        cout << "Gear item not found for this trip.\n";
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

            default:
                return "Unknown";
        }
    }
};

int main() {
    CampBuddiesApp app;

    // Test users.
    app.addTestUser(1, "Alex", "alex@email.com");
    app.addTestUser(2, "Jordan", "jordan@email.com");
    app.addTestUser(3, "Taylor", "taylor@email.com");

    // Alex is the leader of Trip ID 1.
    app.addTestTrip(
        1,
        "Yosemite Weekend",
        1,
        {1, 2, 3}
    );

    // Leader creates the shared gear list.
    app.createSharedGearList(1, 1);

    // Group members can add needed gear.
    app.addGearItem(
        1,
        1,
        "Four-Person Tent",
        1,
        "Shelter",
        "Bring tent stakes and rain fly."
    );

    app.addGearItem(
        1,
        2,
        "Water Bottles",
        6,
        "Water",
        "At least two liters per person."
    );

    app.addGearItem(
        1,
        3,
        "First Aid Kit",
        1,
        "Safety",
        ""
    );

    // Jordan claims the water bottles.
    app.assignGearItem(1, 2, 2);

    // Jordan marks the water bottles as packed/completed.
    app.markGearItemCompleted(1, 2, 2);

    // Any trip member can view the full list.
    app.viewSharedGearList(1, 3);

    // Alex, the leader, edits an item.
    app.editGearItem(
        1,
        1,
        1,
        "Four-Person Tent",
        1,
        "Shelter",
        "Bring tent stakes, rain fly, and ground tarp."
    );

    // Alex, the leader, removes the first-aid-kit item.
    app.removeGearItem(1, 1, 3);

    return 0;
}
