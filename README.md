# CMPE-131-Project-Camp-Buddy

App Description
Camp Buddies is a free mobile app designed for avid campers who want to organize group trips more effectively and access useful camping information in one place.

The app allows trip leaders to create and share group gear lists with all team members. Each member can view assigned items, indicate what they are bringing, and check off completed gear tasks. This helps groups avoid duplicate supplies, identify missing essentials, and coordinate responsibilities before the trip.

Camp Buddies will also include USGS map support, allowing teams to view and share terrain features, plan routes, and download maps for use when internet access is limited. By showing what each group member plans to bring, the app helps campers strategize, pack efficiently, and prepare more safely for their trip.

* HIGH-LEVEL REQUIREMENTS

1. User Accounts and Authentication //File name: Type1_Prototype.cpp
The system shall allow a new user to create an account.
The system shall allow an existing user to log in and log out securely.
The system shall store basic user information, such as name, email, and profile identifier.
The system shall associate trips, gear lists, and gear assignments with user accounts.

2. Trip and Group Management //File name: Type2_Prototype.cpp
The system shall allow a user to create a camping trip.
The trip creator shall automatically become the trip leader.
The trip leader shall be able to enter basic trip information, including trip name, location, start date, and end date.
The trip leader shall be able to invite or add members to the camping group.
Group members shall be able to view the trip details and the list of other participants.
The trip leader shall be able to remove members or update trip details.

3. Shared Gear Lists //File name: Type3_Prototype.cpp
The system shall allow a trip leader to create a shared gear list for a specific trip.
The system shall allow users to add gear items to the list, including an item name, quantity, category, and optional notes.
The system shall allow users to view all gear items needed for the trip.
The system shall display whether each gear item is unassigned, assigned, or completed.
The system shall allow the trip leader to edit or remove gear items.

4. Gear Assignments and Check-Offs //File name: Type4_Prototype.cpp
The system shall allow a group member to claim responsibility for bringing a gear item.
The system shall show all group members which participant is bringing each item.
The system shall allow a member to mark an assigned item as packed or ready.
The system shall update the shared gear list so all members can see current assignment and completion status.
The system shall prevent confusion by clearly identifying duplicate or unassigned gear items.

5. Team Coordination //File name: Type5_Prototype.cpp
The system shall provide a trip dashboard that summarizes the trip, participants, gear list, and packing progress.
The dashboard shall show which gear items are still unassigned or not yet packed.
The system shall allow group members to view what other members are bringing.
The system shall allow the group to identify missing equipment before the trip begins.
The system should provide basic notifications or reminders when gear assignments or packing statuses change.

6. USGS Maps and Offline Access //File name: Type6_Prototype.cpp
The system shall provide a map view for each camping trip.
The system shall allow users to view terrain features relevant to the trip location.
The system shall allow group members to share a selected camping location or map point with the trip group.
The system shall support access to USGS map data or terrain-map imagery, subject to the availability and licensing requirements of the selected map service.
The system should allow users to download selected map areas for offline viewing.
The system shall clearly indicate whether a map area has been downloaded and is available offline.

7. Prototype and Quality Requirements //File name: Type7_Prototype.cpp
The application shall provide a clear, simple interface suitable for users organizing a camping trip.
The MVP shall support the main workflow: create a trip, add members, create a gear list, assign gear, and mark items as packed.
The application shall validate required fields, such as trip name and gear-item name.
The application shall display clear error messages when an action cannot be completed.
The application shall protect users from viewing or modifying trips that they have not joined.
The team shall test the core MVP workflow with at least one sample trip containing multiple users and gear items.
