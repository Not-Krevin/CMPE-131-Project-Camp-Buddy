#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

//Note this is step 0 of this feature space most of this will need to be changed.

// --------------------------------------------
// EXISTING USER AND TRIP STRUCTURES
// --------------------------------------------

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

// --------------------------------------------
// MAP AND OFFLINE-ACCESS STRUCTURES
// --------------------------------------------

// Represents a saved/shared point on a map.
// Examples: campsite, trailhead, water source,
// parking area, emergency meeting point.
struct MapPoint {
    int mapPointId;
    int tripId;

    string pointName;
    string pointType;
    string notes;

    double latitude;
    double longitude;

    int sharedByProfileId;
};

// Represents the selected terrain/map area
// for a specific camping trip.
struct TripMap {
    int tripId;

    // The map's default center point.
    double centerLatitude;
    double centerLongitude;

    // A simple text description of terrain features
    // for this class-project prototype.
    string terrainFeatures;

    // Link to an online USGS map service.
    string mapServiceUrl;

    // Stores whether the selected map was downloaded.
    bool isDownloadedOffline;

    // A human-readable description of the downloaded area.
    string offlineAreaDescription;

    // Simulation of the saved local map-file name.
    string offlineFileName;
};

class CampBuddiesApp {
private:
    vector<User> users;
    vector<Trip> trips;
    vector<TripMap> tripMaps;
    vector<MapPoint> mapPoints;

    int nextMapPointId = 1;

    // USGS Topo base-map service.
    // A future web or mobile frontend can request/display
    // map imagery through this service.
    const string USGS_TOPO_SERVICE =
        "https://basemap.nationalmap.gov/arcgis/rest/services/"
        "USGSTopo/MapServer";

public:
    // --------------------------------------------
    // TEST DATA HELPERS
    // Replace these with your account/trip code.
    // --------------------------------------------

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
                     const vector<int>& memberIds) {
        Trip newTrip;

        newTrip.tripId = tripId;
        newTrip.tripName = tripName;
        newTrip.location = location;
        newTrip.startDate = startDate;
        newTrip.endDate = endDate;
        newTrip.leaderProfileId = leaderProfileId;
        newTrip.memberProfileIds = memberIds;

        trips.push_back(newTrip);
    }

    // --------------------------------------------
    // REQUIREMENT 1:
    // Provide a map view for each camping trip.
    // --------------------------------------------

    void createTripMap(int tripId,
                       int leaderProfileId,
                       double latitude,
                       double longitude,
                       const string& terrainFeatures) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (trip->leaderProfileId != leaderProfileId) {
            cout << "Only the trip leader can create a trip map.\n";
            return;
        }

        if (!isValidLatitude(latitude) ||
            !isValidLongitude(longitude)) {
            cout << "Invalid coordinates.\n";
            return;
        }

        if (findTripMap(tripId) != nullptr) {
            cout << "This trip already has a map. Update it instead.\n";
            return;
        }

        TripMap newMap;

        newMap.tripId = tripId;
        newMap.centerLatitude = latitude;
        newMap.centerLongitude = longitude;
        newMap.terrainFeatures = terrainFeatures;
        newMap.mapServiceUrl = USGS_TOPO_SERVICE;
        newMap.isDownloadedOffline = false;
        newMap.offlineAreaDescription = "";
        newMap.offlineFileName = "";

        tripMaps.push_back(newMap);

        cout << "Map view created for "
             << trip->tripName << ".\n";
    }

    // Shows the map information that a UI/frontend would render.
    void viewTripMap(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);
        TripMap* tripMap = findTripMap(tripId);

        if (trip == nullptr || tripMap == nullptr) {
            cout << "Trip or map not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can view this map.\n";
            return;
        }

        cout << fixed << setprecision(6);

        cout << "\n========== TRIP MAP ==========\n";
        cout << "Trip: " << trip->tripName << "\n";
        cout << "Location: " << trip->location << "\n";
        cout << "Map Center: "
             << tripMap->centerLatitude << ", "
             << tripMap->centerLongitude << "\n";

        cout << "Terrain Features: "
             << tripMap->terrainFeatures << "\n";

        cout << "USGS Map Service: "
             << tripMap->mapServiceUrl << "\n";

        if (tripMap->isDownloadedOffline) {
            cout << "Offline Status: AVAILABLE OFFLINE\n";
            cout << "Downloaded Area: "
                 << tripMap->offlineAreaDescription << "\n";
            cout << "Offline File: "
                 << tripMap->offlineFileName << "\n";
        } else {
            cout << "Offline Status: NOT DOWNLOADED\n";
        }

        cout << "==============================\n";
    }

    // --------------------------------------------
    // REQUIREMENT 2:
    // View terrain features relevant to the trip.
    // --------------------------------------------

    void viewTerrainFeatures(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);
        TripMap* tripMap = findTripMap(tripId);

        if (trip == nullptr || tripMap == nullptr) {
            cout << "Trip or map not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can view terrain features.\n";
            return;
        }

        cout << "\n===== TERRAIN FEATURES =====\n";
        cout << "Trip: " << trip->tripName << "\n";
        cout << tripMap->terrainFeatures << "\n";
    }

    // --------------------------------------------
    // REQUIREMENT 3:
    // Group members can share a location or point
    // with the trip group.
    // --------------------------------------------

    void shareMapPoint(int tripId,
                       int memberProfileId,
                       const string& pointName,
                       const string& pointType,
                       double latitude,
                       double longitude,
                       const string& notes) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, memberProfileId)) {
            cout << "Only group members can share map points.\n";
            return;
        }

        if (pointName.empty() || pointType.empty()) {
            cout << "Point name and point type are required.\n";
            return;
        }

        if (!isValidLatitude(latitude) ||
            !isValidLongitude(longitude)) {
            cout << "Invalid latitude or longitude.\n";
            return;
        }

        MapPoint newPoint;

        newPoint.mapPointId = nextMapPointId++;
        newPoint.tripId = tripId;
        newPoint.pointName = pointName;
        newPoint.pointType = pointType;
        newPoint.latitude = latitude;
        newPoint.longitude = longitude;
        newPoint.notes = notes;
        newPoint.sharedByProfileId = memberProfileId;

        mapPoints.push_back(newPoint);

        cout << getUserName(memberProfileId)
             << " shared the map point: "
             << pointName << ".\n";
    }

    // Displays every map point shared with the group.
    void viewSharedMapPoints(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);

        if (trip == nullptr) {
            cout << "Trip not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can view shared map points.\n";
            return;
        }

        cout << fixed << setprecision(6);

        cout << "\n===== SHARED MAP POINTS =====\n";
        cout << "Trip: " << trip->tripName << "\n";

        bool hasPoints = false;

        for (const MapPoint& point : mapPoints) {
            if (point.tripId == tripId) {
                cout << "\nPoint ID: " << point.mapPointId << "\n";
                cout << "Name: " << point.pointName << "\n";
                cout << "Type: " << point.pointType << "\n";
                cout << "Coordinates: "
                     << point.latitude << ", "
                     << point.longitude << "\n";

                cout << "Shared By: "
                     << getUserName(point.sharedByProfileId)
                     << "\n";

                if (point.notes.empty()) {
                    cout << "Notes: None\n";
                } else {
                    cout << "Notes: " << point.notes << "\n";
                }

                hasPoints = true;
            }
        }

        if (!hasPoints) {
            cout << "No map points have been shared yet.\n";
        }
    }

    // --------------------------------------------
    // REQUIREMENT 4:
    // Support access to USGS map data.
    //
    // This function displays the external service
    // a real frontend would call to display map data.
    // --------------------------------------------

    void showMapDataSource(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);
        TripMap* tripMap = findTripMap(tripId);

        if (trip == nullptr || tripMap == nullptr) {
            cout << "Trip or map not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can access this map source.\n";
            return;
        }

        cout << "\n===== MAP DATA SOURCE =====\n";
        cout << "Provider: USGS The National Map\n";
        cout << "Topo Service: "
             << tripMap->mapServiceUrl << "\n";

        cout << "Usage Note: The app should verify current "
             << "service availability and applicable terms "
             << "before requesting or storing map data.\n";
    }

    // --------------------------------------------
    // REQUIREMENT 5:
    // Allow users to download a selected map area
    // for offline viewing.
    //
    // This is a prototype simulation. It records
    // that a map area was saved locally.
    // --------------------------------------------

    void downloadMapForOfflineUse(
        int tripId,
        int memberProfileId,
        const string& selectedAreaDescription
    ) {
        Trip* trip = findTrip(tripId);
        TripMap* tripMap = findTripMap(tripId);

        if (trip == nullptr || tripMap == nullptr) {
            cout << "Trip or map not found.\n";
            return;
        }

        if (!isTripMember(*trip, memberProfileId)) {
            cout << "Only group members can download a trip map.\n";
            return;
        }

        if (selectedAreaDescription.empty()) {
            cout << "Please describe the selected map area.\n";
            return;
        }

        // In a real mobile or web application, this is where
        // map tiles, a PDF, or GIS data would be downloaded
        // and stored on the user's device.
        tripMap->isDownloadedOffline = true;
        tripMap->offlineAreaDescription = selectedAreaDescription;
        tripMap->offlineFileName =
            "trip_" + to_string(tripId) + "_offline_map.dat";

        cout << "Offline map download completed for: "
             << selectedAreaDescription << "\n";
    }

    // --------------------------------------------
    // REQUIREMENT 6:
    // Clearly indicate whether the map area is
    // downloaded and available offline.
    // --------------------------------------------

    void checkOfflineMapStatus(int tripId, int viewerProfileId) {
        Trip* trip = findTrip(tripId);
        TripMap* tripMap = findTripMap(tripId);

        if (trip == nullptr || tripMap == nullptr) {
            cout << "Trip or map not found.\n";
            return;
        }

        if (!isTripMember(*trip, viewerProfileId)) {
            cout << "Only group members can check offline map status.\n";
            return;
        }

        cout << "\n===== OFFLINE MAP STATUS =====\n";
        cout << "Trip: " << trip->tripName << "\n";

        if (tripMap->isDownloadedOffline) {
            cout << "Status: AVAILABLE OFFLINE\n";
            cout << "Area: "
                 << tripMap->offlineAreaDescription << "\n";
            cout << "Saved File: "
                 << tripMap->offlineFileName << "\n";
        } else {
            cout << "Status: NOT DOWNLOADED\n";
            cout << "Connect to the internet to view map data.\n";
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

    TripMap* findTripMap(int tripId) {
        for (TripMap& tripMap : tripMaps) {
            if (tripMap.tripId == tripId) {
                return &tripMap;
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

    bool isValidLatitude(double latitude) const {
        return latitude >= -90.0 && latitude <= 90.0;
    }

    bool isValidLongitude(double longitude) const {
        return longitude >= -180.0 && longitude <= 180.0;
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

    // Test users.
    app.addTestUser(1, "Alex", "alex@email.com");
    app.addTestUser(2, "Jordan", "jordan@email.com");
    app.addTestUser(3, "Taylor", "taylor@email.com");

    // Test trip. Alex is the group leader.
    app.addTestTrip(
        1,
        "Yosemite Weekend",
        "Upper Pines Campground, Yosemite National Park",
        "2026-10-17",
        "2026-10-19",
        1,
        {1, 2, 3}
    );

    // Alex creates the map view for the trip.
    app.createTripMap(
        1,
        1,
        37.745570,
        -119.593604,
        "Granite terrain, forested trails, elevation changes, "
        "water features, roads, and campground areas."
    );

    // Jordan shares a campsite location.
    app.shareMapPoint(
        1,
        2,
        "Upper Pines Campground",
        "Campsite",
        37.742900,
        -119.572300,
        "Primary campsite. Meet here before hiking."
    );

    // Taylor shares a trailhead.
    app.shareMapPoint(
        1,
        3,
        "Mist Trail Trailhead",
        "Trailhead",
        37.732500,
        -119.558000,
        "Start here for the hike toward Vernal Fall."
    );

    // Any trip member can view map and terrain data.
    app.viewTripMap(1, 2);
    app.viewTerrainFeatures(1, 3);
    app.viewSharedMapPoints(1, 1);
    app.showMapDataSource(1, 2);

    // Jordan downloads a selected map area.
    app.downloadMapForOfflineUse(
        1,
        2,
        "Upper Pines Campground, Mist Trail, and Vernal Fall area"
    );

    // Taylor checks whether the trip map is ready offline.
    app.checkOfflineMapStatus(1, 3);

    return 0;
}
