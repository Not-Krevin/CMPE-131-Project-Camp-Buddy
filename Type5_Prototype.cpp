#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

//Note this is step 0 of this feature space most of this will need to be changed.

#include "Type1_Prototype.cpp"
#include "Type2_Prototype.cpp"
#include "Type3_Prototype.cpp"
#include "Type4_Prototype.cpp"

#include "Type6_Prototype.cpp"
#include "Type7_Prototype.cpp"

// ------------------------------------------
// USERS, TRIPS, AND GEAR ITEMS
// These can be combined with your earlier code.
// ------------------------------------------

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
    int assignedProfileId;  // -1 means nobody has claimed it.
};

// A simple notification stored in memory.
// Later, this could become a push notification, email, or database record.
struct Notification {
    int notificationId;
    int tripId;
    int recipientProfileId;
    string message;
    bool read;
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;
    vector<GearItem> gearItems;
    vector<Notification> notifications;

    int nextNotificationId = 1;

public:
    // ------------------------------------------
    // TEST DATA HELPERS
    // Replace these with account/trip code later.
    // ------------------------------------------

    void addTestUser(int profileId,
                     const string& name,
                     const string& email) {
        users.push_back({profileId, name, email});
    }

    void addTestTrip(int tripId,
                     const string& tripName,
                     const string& location,
                     const string& startDate,
                     const string& endDate,
                     int leaderProfileId,
                     const vector<int>& members) {
        Trip newTrip;

        newTrip.tripId = tripId;
        newTrip.tripName = tripName;
        newTrip.location = location;
        newTrip.startDate = startDate;
        newTrip.endDate = endDate;
        newTrip.leaderProfileId = leaderProfileId;
        newTrip.memberProfileIds = members;

        trips.push_back(newTrip);
    }

    void addTestGearItem(int gearItemId,
                         int tripId,
                         const string& itemName,
                         int quantity,
                         const string& category,
                         const string& notes) {
        GearItem newItem;

        newItem.gearItemId = gearItemId;
        newItem.tripId = tripId;
        newItem.itemName = itemName;
        newItem.quantity = quantity;
        newItem.category = category;
        newItem.notes = notes;
        newItem.status = GearStatus::UNASSIGNED;
        newItem.assignedProfileId = -1;

        gearItems.push_back(newItem);
    }

    // ------------------------------------------
    // REQUIREMENT:
    // Group members claim responsibility for gear.
    // A notification is created after assignment.
    // ------------------------------------------

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
            cout << "Only group members can claim an item.\n";
            return;
        }

        if (item->status == GearStatus::COMPLETED) {
            cout << "This item is already marked packed.\n";
            return;
        }

        if (item->assignedProfileId != -1 &&
            item->assignedProfileId != memberProfileId) {
            cout << item->itemName << " is already assigned to "
                 << getUserName(item->assignedProfileId) << ".\n";
            return;
        }

        item->assignedProfileId = memberProfileId;
        item->status = GearStatus::ASSIGNED;

        string message = getUserName(memberProfileId) +
                         " is bringing " +
                         item->itemName + ".";

        createNotificationForTripMembers(
            tripId,
            message,
            memberProfileId
        );

        cout << message << "\n";
    }

    // ------------------------------------------
    // REQUIREMENT:
    // An assigned member marks the item packed.
    // A notification is created after completion.
    // ------------------------------------------

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
            cout << "The item must be claimed before it is marked packed.\n";
            return;
        }

        if (item->assignedProfileId != memberProfileId) {
            cout << "Only "
                 << getUserName(item->assignedProfileId)
                 << " can mark this item packed.\n";
            return;
        }

        item->status = GearStatus::COMPLETED;

        string message = getUserName(memberProfileId) +
                         " marked " +
                         item->itemName +
                         " as packed and ready.";

        createNotificationForTripMembers(
            tripId,
            message,
            memberProfileId
        );

        cout << message << "\n";
    }

    // ------------------------------------------
    // REQUIREMENT:
    // Provide a trip dashboard with a summary of:
    // - Trip information
    // - Participants
    // - Gear list
    // - Packing progress
    // - Missing/unassigned equipment
    // ------------------------------------------

    void showTripDashboard(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can view this dashboard.\n";
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

        double packingPercent = 0.0;

        if (totalItems > 0) {
            packingPercent =
                (static_cast<double>(completedItems) / totalItems) * 100.0;
        }

        cout << "\n====================================\n";
        cout << "         CAMP BUDDIES DASHBOARD\n";
        cout << "====================================\n";

        cout << "\nTRIP INFORMATION\n";
        cout << "Trip: " << trip->tripName << "\n";
        cout << "Location: " << trip->location << "\n";
        cout << "Dates: " << trip->startDate
             << " to " << trip->endDate << "\n";
        cout << "Trip Leader: "
             << getUserName(trip->leaderProfileId) << "\n";

        cout << "\nGROUP PARTICIPANTS (" 
             << trip->memberProfileIds.size() << ")\n";

        for (int memberId : trip->memberProfileIds) {
            cout << "- " << getUserName(memberId);

            if (memberId == trip->leaderProfileId) {
                cout << " (Trip Leader)";
            }

            cout << "\n";
        }

        cout << "\nPACKING PROGRESS\n";
        cout << "Total Gear Items: " << totalItems << "\n";
        cout << "Completed/Packed: " << completedItems << "\n";
        cout << "Assigned, Not Packed: " << assignedItems << "\n";
        cout << "Unassigned: " << unassignedItems << "\n";
        cout << "Packing Progress: " << packingPercent << "%\n";

        cout << "\nGEAR RESPONSIBILITIES\n";

        if (totalItems == 0) {
            cout << "No gear items have been added yet.\n";
        }

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId) {
                cout << "- " << item.itemName
                     << " (Quantity: " << item.quantity << ")\n";

                cout << "  Status: "
                     << getGearStatusText(item.status) << "\n";

                if (item.assignedProfileId == -1) {
                    cout << "  Bringing it: Nobody assigned\n";
                } else {
                    cout << "  Bringing it: "
                         << getUserName(item.assignedProfileId) << "\n";
                }

                if (!item.notes.empty()) {
                    cout << "  Notes: " << item.notes << "\n";
                }
            }
        }

        cout << "\nMISSING OR INCOMPLETE EQUIPMENT\n";

        bool hasMissingEquipment = false;

        for (const GearItem& item : gearItems) {
            if (item.tripId == tripId &&
                item.status == GearStatus::UNASSIGNED) {
                cout << "- MISSING ASSIGNMENT: "
                     << item.itemName
                     << " still needs someone to bring it.\n";

                hasMissingEquipment = true;
            }

            if (item.tripId == tripId &&
                item.status == GearStatus::ASSIGNED) {
                cout << "- NOT PACKED YET: "
                     << item.itemName
                     << " is assigned to "
                     << getUserName(item.assignedProfileId)
                     << " but has not been marked ready.\n";

                hasMissingEquipment = true;
            }
        }

        if (!hasMissingEquipment) {
            cout << "All listed gear is assigned and packed.\n";
        }

        cout << "====================================\n";
    }

    // ------------------------------------------
    // REQUIREMENT:
    // View all notifications for one user.
    // ------------------------------------------

    void viewNotifications(int userProfileId) {
        if (!userExists(userProfileId)) {
            cout << "User not found.\n";
            return;
        }

        cout << "\n===== NOTIFICATIONS FOR "
             << getUserName(userProfileId) << " =====\n";

        bool hasNotifications = false;

        for (Notification& notification : notifications) {
            if (notification.recipientProfileId == userProfileId) {
                cout << "- " << notification.message;

                if (!notification.read) {
                    cout << " [NEW]";
                }

                cout << "\n";

                // Mark as read after displaying it.
                notification.read = true;
                hasNotifications = true;
            }
        }

        if (!hasNotifications) {
            cout << "No notifications.\n";
        }
    }

private:
    // Sends an in-app notification to every trip member except
    // the person who made the change.
    void createNotificationForTripMembers(
        int tripId,
        const string& message,
        int personWhoMadeChange
    ) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            return;
        }

        for (int memberId : trip->memberProfileIds) {
            if (memberId != personWhoMadeChange) {
                Notification newNotification;

                newNotification.notificationId = nextNotificationId++;
                newNotification.tripId = tripId;
                newNotification.recipientProfileId = memberId;
                newNotification.message = message;
                newNotification.read = false;

                notifications.push_back(newNotification);
            }
        }
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

    bool userExists(int profileId) const {
        for (const User& user : users) {
            if (user.profileId == profileId) {
                return true;
            }
        }

        return false;
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

int main() {
    CampBuddiesApp app;

    // Test users.
    app.addTestUser(1, "Alex", "alex@email.com");
    app.addTestUser(2, "Jordan", "jordan@email.com");
    app.addTestUser(3, "Taylor", "taylor@email.com");

    // Alex leads this group trip.
    app.addTestTrip(
        1,
        "Yosemite Weekend",
        "Upper Pines Campground, Yosemite National Park",
        "2026-10-17",
        "2026-10-19",
        1,
        {1, 2, 3}
    );

    // Gear list begins with unassigned gear.
    app.addTestGearItem(
        1,
        1,
        "Four-Person Tent",
        1,
        "Shelter",
        "Bring tent stakes and rain fly."
    );

    app.addTestGearItem(
        2,
        1,
        "Water Bottles",
        6,
        "Water",
        "At least two liters per person."
    );

    app.addTestGearItem(
        3,
        1,
        "First Aid Kit",
        1,
        "Safety",
        "Check expiration dates."
    );

    // Jordan claims and packs water bottles.
    app.claimGearItem(1, 2, 2);
    app.markGearItemPacked(1, 2, 2);

    // Taylor claims the first-aid kit but has not packed it yet.
    app.claimGearItem(1, 3, 3);

    // Alex views the summary dashboard.
    app.showTripDashboard(1, 1);

    // Alex sees notifications created by Jordan and Taylor.
    app.viewNotifications(1);

    return 0;
}
