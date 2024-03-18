# Portal
#### Video Demo:
https://www.youtube.com/watch?v=Cc2VF1QHnZo
#### Description:
Portal is a C++ terminal game for Windows, developed using linked lists as a data structure. The game supports both single-player and multiplayer features.

### Gameplay
#### Screenshots
![App Screenshot](/screenshots/screenshot1.png)
![App Screenshot](/screenshots/screenshot2.png)
![App Screenshot](/screenshots/screenshot3.png)

#### Objective
The goal for each player is reach to 200 of Stardust without going down to less than 5 of Stardust, being this the Minimum Amount of Stardust to invest.
#### Portals
Each level has 10 portals, including 3 Negative Portals. If you enter a Negative Portal, you’ll lose all the Stardust invested in that turn.

A Normal Portal, on the other hand, multiplies the invested amount by 1.2x. You can choose to withdraw your Stardust at the current level, or try your luck and aim for the highest level.

#### Game over
The minimum amount of Stardust to invest is 5. Once a player’s Stardust decreases to less than 5, the game ends for that player.

#### Climate
Every time a player wins or loses, the number of Bad Portals per level increases by 1, leading to a change in the climate.

Once the number of Bad Portals reaches 5, it will no longer increase.

#### Black Matter Detector
Each player has 3 Black Matter Detectors. This resource allows them to display a random portal at their current level.

### Installation
To play the game, you must be using the Windows OS and have g++ installed. I recommend using Windows PowerShell in fullscreen mode and setting the background color to black.


- Run the following command to compile the c++ file and generate a .exe

```PowerShell
g++ main.cpp -o portal.exe
```

### Implementation Details
In the development of this game, I used the following structs to represent Portals, Levels, Players, and BadPortals.

```c++
struct Portal {
    bool bad;
    int x_position;
    Portal *next;
};

struct Level {
    Portal *portals;
    int y_position;
    Level *next;
};

struct Player {
    string name;
    int portal;
    int level;
    float star_dust;
    float invested_star_dust;
    Player *next;
    bool removed;
    int dark_matter_detectors;
};

struct BadPortal {
    int x_position;
    int y_position;
    BadPortal *next;
};
```

The board is represented by a linked list of 8 Levels, each containing their Y-Coordinate and a linked list of Portals for that specific Level. On the other hand, each Portal contains a boolean indicating whether it’s a Bad or Good Portal.

For each turn, the game randomly generates a new Board for each level with 3 Bad Portals. Every time the user exits the game, the current Board gets saved in a .csv file. To save the Board, instead of saving every Level with every Portal, we use another linked list representing Bad Portals. Each Bad Portal has its X and Y-Coordinates. Using this information, we can load the file full of Bad Portals to continue the game.

Finally, there’s a linked list for Players. Each Player contains a string for their name, their current portal (X-Coordinate), their level (Y-Coordinate), the amount of dark matter detectors, stardust and invested stardust, and a boolean to indicate if the player was removed (It’s true if the player either reaches 200 or has less than 5).
