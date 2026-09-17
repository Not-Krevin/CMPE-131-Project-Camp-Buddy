#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Type1_Prototype.cpp"

#include "Type3_Prototype.cpp"
#include "Type4_Prototype.cpp"
#include "Type5_Prototype.cpp"
#include "Type6_Prototype.cpp"
#include "Type7_Prototype.cpp"

using namespace std;

//Note this is step 0 of this feature space most of this will need to be changed.

// Basic user information.
// This would normally be created by your User Accounts code.
struct User {
    int profileId;
    string name;
    string email;
};

// Stores all information for one camping trip.
struct Trip {
    int tripId;
    string tripName;
    string location;
    string startDate;
    string endDate;

    // The creator is automatically stored as the leader.
    int leaderProfileId;

    // Stores the profile IDs of all trip participants.
    vector<int> memberProfileIds;
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;

    int nextTripId {1};

public:
    // Temporary helper function for testing.
    // In your complete program, users would be added through createAccount().
    void addTestUser(int profileId, const string& name, const string& email) {
        User newUser;
        newUser.profileId {profileId};
        newUser.name {name};
        newUser.email {email};

        users.push_back(newUser);
    }

    // Requirement:
    // The system shall allow a user to create a camping trip.
    // The trip creator shall automatically become the trip leader.
    // The leader can enter name, location, start date, and end date.
    void createTrip(int creatorProfileId,
                    const string& tripName,
                    const string& location,
                    const string& startDate,
                    const string& endDate) {
        if (!userExists(creatorProfileId)) {
            cout << "Cannot create trip: creator profile was not found.\n";
            return;
        }

        if (tripName.empty() || location.empty() ||
            startDate.empty() || endDate.empty()) {
            cout << "All trip fields are required.\n";
            return;
        }

        Trip newTrip;
        newTrip.tripId {++nextTripId};
        newTrip.tripName {tripName};
        newTrip.location {location};
        newTrip.startDate {startDate};
        newTrip.endDate {endDate};

        // Creator automatically becomes the trip leader.
        newTrip.leaderProfileId {creatorProfileId};

        // The leader is also a member of the trip group.
        newTrip.memberProfileIds.push_back(creatorProfileId);

        trips.push_back(newTrip);

        cout << "Trip created successfully.\n";
        cout << tripName << " has Trip ID: " << newTrip.tripId << "\n";
        cout << "Profile ID " << creatorProfileId
             << " is the trip leader.\n";
    }

    // Requirement:
    // The trip leader shall be able to invite or add members.
    void addMemberToTrip(int tripId,
                         int leaderProfileId,
                         int newMemberProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can add members.\n";
            return;
        }

        if (!userExists(newMemberProfileId)) {
            cout << "The user you are trying to add does not exist.\n";
            return;
        }

        if (isMember(*trip, newMemberProfileId)) {
            cout << "This user is already a member of the trip.\n";
            return;
        }

        trip->memberProfileIds.push_back(newMemberProfileId);

        cout << getUserName(newMemberProfileId)
             << " was added to " << trip->tripName << ".\n";
    }

    // Requirement:
    // Group members shall be able to view trip details and participants.
    void viewTripDetails(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        // Only a participant can view the group's trip information.
        if (!isMember(*trip, viewerProfileId)) {
            cout << "You are not a member of this trip.\n";
            return;
        }

        cout << "\n===== TRIP DETAILS =====\n";
        cout << "Trip ID: " << trip->tripId << "\n";
        cout << "Trip Name: " << trip->tripName << "\n";
        cout << "Location: " << trip->location << "\n";
        cout << "Start Date: " << trip->startDate << "\n";
        cout << "End Date: " << trip->endDate << "\n";
        cout << "Trip Leader: "
             << getUserName(trip->leaderProfileId) << "\n";

        cout << "\nParticipants:\n";

        for (int memberId : trip->memberProfileIds) {
            cout << "- " << getUserName(memberId)
                 << " (Profile ID: " << memberId << ")";

            if (memberId == trip->leaderProfileId) {
                cout << " - Trip Leader";
            }

            cout << "\n";
        }
    }

    // Requirement:
    // The trip leader shall be able to remove members.
    void removeMemberFromTrip(int tripId,
                              int leaderProfileId,
                              int memberProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can remove members.\n";
            return;
        }

        // Do not allow the leader to remove themselves.
        // A future version could implement "transfer leadership."
        if (memberProfileId == trip->leaderProfileId) {
            cout << "The trip leader cannot remove themselves.\n";
            return;
        }

        auto memberPosition = find(
            trip->memberProfileIds.begin(),
            trip->memberProfileIds.end(),
            memberProfileId
        );

        if (memberPosition == trip->memberProfileIds.end()) {
            cout << "This user is not a member of the trip.\n";
            return;
        }

        string memberName = getUserName(memberProfileId);

        // erase() removes the item at the iterator position.
        trip->memberProfileIds.erase(memberPosition);

        cout << memberName << " was removed from "
             << trip->tripName << ".\n";
    }

    // Requirement:
    // The trip leader shall be able to update trip details.
    void updateTripDetails(int tripId,
                           int leaderProfileId,
                           const string& newTripName,
                           const string& newLocation,
                           const string& newStartDate,
                           const string& newEndDate) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can update trip details.\n";
            return;
        }

        if (newTripName.empty() || newLocation.empty() ||
            newStartDate.empty() || newEndDate.empty()) {
            cout << "All updated trip fields are required.\n";
            return;
        }

        trip->tripName {newTripName};
        trip->location {newLocation};
        trip->startDate {newStartDate};
        trip->endDate {newEndDate};

        cout << "Trip details updated successfully.\n";
    }

    // Shows the trips for which a user is a participant.
    void viewMyTrips(int profileId) {
        if (!userExists(profileId)) {
            cout << "User not found.\n";
            return;
        }

        cout << "\n===== MY CAMPING TRIPS =====\n";

        bool hasTrips = false;

        for (const Trip& trip : trips) {
            if (isMember(trip, profileId)) {
                cout << "Trip ID: " << trip.tripId
                     << " | " << trip.tripName
                     << " | " << trip.location << "\n";

                hasTrips = true;
            }
        }

        if (!hasTrips) {
            cout << "You are not currently part of any trips.\n";
        }
    }

private:
    // Finds a trip by ID and returns a pointer to it.
    // Returning a pointer allows functions to edit the original trip.
    Trip* findTrip(int tripId) {
        for (Trip& trip : trips) {
            if (trip.tripId == tripId) {
                return &trip;
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

    bool isMember(const Trip& trip, int profileId) const {
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
};

int main() {
    CampBuddiesApp app;

    // Sample accounts for testing.
    app.addTestUser(1, "Alex", "alex@email.com");
    app.addTestUser(2, "Jordan", "jordan@email.com");
    app.addTestUser(3, "Taylor", "taylor@email.com");

    // Alex creates a trip and automatically becomes leader.
    app.createTrip(
        1,
        "Yosemite Weekend",
        "Yosemite National Park, California",
        "2026-10-16",
        "2026-10-18"
    );

    // Alex, the leader, adds Jordan and Taylor.
    app.addMemberToTrip(1, 1, 2);
    app.addMemberToTrip(1, 1, 3);

    // Jordan can view shared trip information and other participants.
    app.viewTripDetails(1, 2);

    // Alex updates the location and dates.
    app.updateTripDetails(
        1,
        1,
        "Yosemite Fall Weekend",
        "Upper Pines Campground, Yosemite National Park",
        "2026-10-17",
        "2026-10-19"
    );

    // Alex removes Taylor.
    app.removeMemberFromTrip(1, 1, 3);

    // Jordan views the updated trip group.
    app.viewTripDetails(1, 2);

    return 0;
}
