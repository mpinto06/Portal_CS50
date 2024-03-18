#include <iostream>
#include <conio.h>
#include <direct.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <limits>
#include "macros.cpp"

using namespace std;

//Declaration of structs
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

//Global Variables
Player *GLOBAL_PLAYERS = NULL;
Level *GLOBAL_LEVELS = NULL;
BadPortal *GLOBAL_BAD_PORTALS = NULL;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition; 
int BAD_PORTALS = MINIMUM_BAD_PORTALS;

//Utility Functions
int GenerateRandom(int a, int b) {
    int difference = b - a, random;
    random = rand() % (difference + 1);
    return (a + random);
}

bool ValidateInt(int *input) {
    cin >> *input; 
    if (cin.good()) {
        return *input > 0;
    }
    else {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        return false;
    }
}
void PressKey(char ch) {
    char input = '+';
    while (input != ch && input != toupper(ch)) {
        input = getch();
    }
}

void SetCursor(bool visible, DWORD size) {
    CONSOLE_CURSOR_INFO Cursor;	
	Cursor.bVisible = visible;
	Cursor.dwSize = size;
	SetConsoleCursorInfo(console, &Cursor);
}

void GoToXY(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

//Basic Linked Lists Functions
Portal *CreatePortal(bool bad) {
    Portal *portal= new Portal();
    portal -> bad = bad;
    portal -> x_position = 0;
    portal -> next = NULL;
    return portal;
}

Level *CreateLevel(int y_position) {
    Level *level = new Level();
    level -> portals = NULL;
    level -> next= NULL;
    level -> y_position = y_position;
}

Player *CreatePlayer(string name, int portal, int level, float star_dust, int dark_matter_detectors, float invested_star_dust) {
    Player *player = new Player();
    player -> name = name;
    player -> portal = portal;
    player -> level = level;
    player -> star_dust = star_dust;
    player -> invested_star_dust = invested_star_dust;
    player -> dark_matter_detectors = dark_matter_detectors;
    player -> removed = (star_dust > MAXIMUM_STAR_DUST || star_dust < MINIMUM_STAR_DUST );
    player -> next = NULL;
    return player;
}

BadPortal *CreateBadPortal(int x_position, int y_position) {
    BadPortal *bad_portal = new BadPortal();
    bad_portal-> x_position = x_position;
    bad_portal -> y_position = y_position;
    bad_portal -> next = NULL;
    return bad_portal;
}

void InsertPortal(Portal **portals, bool bad) {
    Portal *portal = CreatePortal(bad);
    portal -> next = *portals;
    *portals = portal;
}

void InsertLevel(Level **levels, int y_position) {
    Level *level = CreateLevel(y_position);
    level -> next = *levels;
    *levels = level;
}

void InsertPlayer(Player **players, string name, int portal, int level, float star_dust, int dark_matter_detectors, float invested_star_dust) {
    Player *player = CreatePlayer(name, portal, level, star_dust, dark_matter_detectors, invested_star_dust);
    Player *aux = *players;
    if (aux == NULL) {
        *players = player;
    }
    else {
        while( aux -> next != NULL) {
            aux = aux -> next;
        }
        aux -> next = player;
    }
}

void InsertBadPortal(BadPortal **bad_portals, int x_position, int y_position) {
    BadPortal *bad_portal = CreateBadPortal(x_position, y_position);
    bad_portal -> next = *bad_portals;
    *bad_portals = bad_portal;
}

void FreeBadPortals(BadPortal **bad_portals) {
    BadPortal *previous = NULL;
    while (*bad_portals != NULL) {
        previous = *bad_portals;
        *bad_portals = (*bad_portals) -> next;
        delete previous;
    }
}

void FreePlayers(Player **players) {
    Player *previous = NULL;
    while (*players != NULL) {
        previous = *players;
        *players = (*players) -> next;
        delete previous;
    }
}

void FreePortals(Portal **portals) {
    Portal *previous = NULL;
    while (*portals != NULL) {
        previous = *portals;
        *portals = (*portals) -> next;
        delete previous;
    }
}

void FreeLevels(Level **levels) {
    Level *previous = NULL;
    while (*levels != NULL) {
        previous = *levels;
        *levels = (*levels) -> next;
        FreePortals(&(previous -> portals));
        delete previous;
    }
}

void FreeGlobals() {
    FreeBadPortals(&GLOBAL_BAD_PORTALS);
    FreePlayers(&GLOBAL_PLAYERS);
    FreeLevels(&GLOBAL_LEVELS);
}
//File Functions
Player *LoadPlayers(string file_name) {
    ifstream input_file(file_name);
    Player *players = NULL;
    if (input_file.is_open()) {
        string line;
        while (getline(input_file, line)) {
            string name, level, star_dust, portal, detectors, invested_star_dust;
            istringstream ss(line);
            getline(ss, name, ',');
            getline(ss, portal, ',');
            getline(ss, level, ',');
            getline(ss, star_dust, ',');
            getline(ss, detectors, ',');
            getline(ss, invested_star_dust, ',');
            InsertPlayer(&players, name, stoi(portal), stoi(level), stof(star_dust), stoi(detectors), stoi(invested_star_dust));
        }
        input_file.close();
        return players;
    } 
    else {
        return NULL;
    }
}

void SavePlayers(Player *players, string file_name){
    ofstream output_file(file_name + ".csv");

    if (output_file.is_open()) {
        while (players != NULL) {
            output_file << players -> name << "," << players -> portal << ",";  
            output_file << players -> level << "," << players -> star_dust  << ",";
            output_file << players -> dark_matter_detectors << "," << players -> invested_star_dust << endl;
            players = players -> next;
        }
        output_file.close();
    } 
}

BadPortal *LoadBadPortals(string file_name) {

    ifstream input_file(file_name);
    BadPortal *bad_portals = NULL;

    if (input_file.is_open()) {  

        string line;
        while (getline(input_file, line)) {
            string x_position, y_position;
            istringstream ss(line);
            getline(ss, x_position, ',');
            getline(ss, y_position, ',');
            InsertBadPortal(&bad_portals, stoi(x_position), stoi(y_position));
        }
        input_file.close();
        return bad_portals;
    } 
    else {
        return NULL;
    }
}

void SaveBadPortals(BadPortal *bad_portals, string file_name){
    ofstream output_file(file_name + ".csv");
    
    if (output_file.is_open()) {
        while (bad_portals != NULL) {
            output_file << bad_portals -> x_position << ",";  
            output_file << bad_portals -> y_position << endl;
            bad_portals = bad_portals -> next;
        }
        output_file.close();
    } 
}

void SaveGame(Player *players, BadPortal *bad_portals){
    SavePlayers(players,"players_save");
    SaveBadPortals(bad_portals,"bad_portals_save");
}

//Visual/Input Functions
void ShowMessageGame(string line1, string line2, string line3, string line4) {
    GoToXY(4,42); cout<<"---------------------------------------------"; 
    GoToXY(4,43); cout<<"|                                           |";
    GoToXY(4,44); cout<<"|                                           |"; 
    GoToXY(4,45); cout<<"|                                           |";
    GoToXY(4,46); cout<<"|                                           |";
    GoToXY(4,47); cout<<"|                                           |";
    GoToXY(4,48); cout<<"|                                           |";   
    GoToXY(4,49); cout<<"---------------------------------------------";
    GoToXY(6,44); cout << line1;
    GoToXY(6,45); cout << line2;
    GoToXY(6,46); cout << line3;
    GoToXY(6,47); cout << line4;
}

void DeleteMessageGame() {
    GoToXY(4,44); cout<<"|                                           |"; 
    GoToXY(4,45); cout<<"|                   ^_^                     |";
    GoToXY(4,46); cout<<"|                                           |";
    GoToXY(4,47); cout<<"|                                           |";
}

void ShowMessageMenu(string line1, string line2, string line3) {
    GoToXY(20,12); cout<<"---------------------------------------------"; 
    GoToXY(20,13); cout<<"|                                           |";
    GoToXY(20,14); cout<<"|                                           |"; 
    GoToXY(20,15); cout<<"|                                           |";
    GoToXY(20,16); cout<<"|                                           |";
    GoToXY(20,17); cout<<"|                                           |";
    GoToXY(20,18); cout<<"---------------------------------------------";
    GoToXY(22,14); cout << line1;
    GoToXY(22,15); cout << line2;
    GoToXY(22,16); cout << line3;
}

void DeleteMessageMenu() {
    GoToXY(20,12); cout<<"---------------------------------------------"; 
    GoToXY(20,13); cout<<"|                                           |";
    GoToXY(20,14); cout<<"|                                           |"; 
    GoToXY(20,15); cout<<"|                                           |";
    GoToXY(20,16); cout<<"|                                           |";
    GoToXY(20,17); cout<<"---------------------------------------------";
}

void MainTitle(int x, int y) {
    GoToXY(x,y); cout << " _______                        __                __ ";
    GoToXY(x,y + 1 ); cout << R"(/       \                      /  |              /  |)";
    GoToXY(x,y + 2); cout << "$$$$$$$  | ______    ______   _$$ |_     ______  $$ |";
    GoToXY(x,y + 3); cout << R"($$ |__$$ |/      \  /      \ / $$   |   /      \ $$ |)";
    GoToXY(x,y + 4); cout << "$$    $$//$$$$$$  |/$$$$$$  |$$$$$$/    $$$$$$  |$$ |";
    GoToXY(x,y + 5); cout << "$$$$$$$/ $$ |  $$ |$$ |  $$/   $$ | __  /    $$ |$$ |";
    GoToXY(x,y + 6); cout << R"($$ |     $$ \__$$ |$$ |        $$ |/  |/$$$$$$$ |$$ |)";
    GoToXY(x,y + 7); cout << "$$ |     $$    $$/ $$ |        $$  $$/ $$    $$ |$$ |";
    GoToXY(x,y + 8); cout << "$$/       $$$$$$/  $$/          $$$$/   $$$$$$$/ $$/ ";
}

void Ranking(int x, int y, Player *sorted_players) {
    GoToXY(x,y); cout << " _____             _    _             ";
    GoToXY(x,y + 1); cout << R"(|  __ \           | |  (_))";
    GoToXY(x,y + 2); cout << R"(| |__) |__ _ _ __ | | ___ _ __   __ _)";
    GoToXY(x,y + 3); cout << R"(|  _  // _` | '_ \| |/ / | '_ \ / _` |)";
    GoToXY(x,y + 4); cout << R"(| | \ \ (_| | | | |   <| | | | | (_| |)" ;
    GoToXY(x,y + 5); cout << R"(|_|  \_\__,_|_| |_|_|\_\_|_| |_|\__, |)";
    GoToXY(x,y + 6); cout << R"(                                 __/ |)";
    GoToXY(x,y + 7); cout << R"(                                |___/)";
    GoToXY(x,y + 8); cout <<"----------------------------------------";    
    int count = 0;
    GoToXY(x, y + 10); cout << "NAMES";
    GoToXY(x + 20, y + 10); cout << "START_DUST";
    while (sorted_players != NULL && count < 5)
    {
        GoToXY(x, y + 12 + count); cout << count + 1 << ". " << sorted_players -> name;
        GoToXY(x + 20, y + 12 + count); cout << sorted_players -> star_dust;
        sorted_players = sorted_players -> next;
        count++;
    }
    if (sorted_players != NULL) {
        GoToXY(x, y + 12 + count); cout << "....";
    }    
    FreePlayers(&sorted_players);      
}

void Controls(int x, int y) {
    GoToXY(x,y); cout << "   _____            _             _";
    GoToXY(x,y + 1); cout << "  / ____|          | |           | |";
    GoToXY(x,y + 2); cout << " | |     ___  _ __ | |_ _ __ ___ | |___ ";
    GoToXY(x,y + 3); cout << R"( | |    / _ \| '_ \| __| '__/ _ \| / __|)";
    GoToXY(x,y + 4); cout << R"( | |___| (_) | | | | |_| | | (_) | \__ \)";
    GoToXY(x,y + 5); cout << R"(  \_____\___/|_| |_|\__|_|  \___/|_|___/)";
    GoToXY(x,y + 7); cout <<"----------------------------------------------";
	GoToXY(x,y + 9); cout <<"'A' -> LEFT" ;
	GoToXY(x + 25,y + 9); cout <<"'D' -> RIGHT";
    GoToXY(x,y + 11); cout <<"'W' -> UP";
    GoToXY(x + 25,y + 11); cout <<"'R' -> WITHDRAW";
    GoToXY(x,y + 13); cout <<"'F' -> DARK MATTER DETECTOR";
	GoToXY(x,y + 15); cout <<"'K' -> EXIT";
}

void Tutorial(int x, int y) {
    GoToXY(x,y); cout << " ________            __                          __            __ ";
    GoToXY(x,y + 1); cout << "/        |          /  |                        /  |          /  |";
    GoToXY(x,y + 2); cout << "$$$$$$$$/__    __  _$$ |_     ______    ______  $$/   ______  $$ |";
    GoToXY(x,y + 3); cout << R"(   $$ | /  |  /  |/ $$   |   /      \  /      \ /  | /      \ $$ |)";
    GoToXY(x,y + 4); cout << R"(   $$ | $$ |  $$ |$$$$$$/   /$$$$$$  |/$$$$$$  |$$ | $$$$$$  |$$ |)";
    GoToXY(x,y + 5); cout << "   $$ | $$ |  $$ |  $$ | __ $$ |  $$ |$$ |  $$/ $$ | /    $$ |$$ |";
    GoToXY(x,y + 6); cout << "   $$ | $$ |  $$ |  $$ | __ $$ |  $$ |$$ |  $$/ $$ | /    $$ |$$ |";
    GoToXY(x,y + 7); cout << R"(   $$ | $$ \__$$ |  $$ |/  |$$ \__$$ |$$ |      $$ |/$$$$$$$ |$$ |)";
    GoToXY(x,y + 8); cout << "   $$/   $$$$$$/     $$$$/   $$$$$$/  $$/       $$/  $$$$$$$/ $$/";
}
         
           
void PlayerInfo(int x, int y, Player *player) {
    GoToXY(x,y); cout << "  _____  _                       ";
    GoToXY(x,y + 1); cout << R"( |  __ \| |                      )";
    GoToXY(x,y + 2); cout << R"( | |__) | | __ _ _   _  ___ _ __ )";
    GoToXY(x,y + 3); cout << R"( |  ___/| |/ _` | | | |/ _ \ '__|)";
    GoToXY(x,y + 4); cout << R"( | |    | | (_| | |_| |  __/ |)";
    GoToXY(x,y + 5); cout << R"( |_|    |_|\__,_|\__, |\___|_|)";
    GoToXY(x,y + 6); cout << R"(                  __/ |       )";
    GoToXY(x,y + 7); cout << R"(                 |___/        )";
    GoToXY(x,y + 8); cout <<"----------------------------------------------";
    GoToXY(x,y + 10); cout << "                                              ";
    GoToXY(x,y + 12); cout << "                                              ";
    GoToXY(x,y + 14); cout << "                                              ";
    GoToXY(x,y + 16); cout << "                                              ";
    GoToXY(x,y + 18); cout << "                                              ";
    GoToXY(x,y + 20); cout << "                                              ";
    GoToXY(x,y + 10); cout << "NAME -> " << player -> name;
    GoToXY(x,y + 12); cout << "STAR DUST -> " << player -> star_dust;
    GoToXY(x,y + 14); cout << "INVESTED STAR DUST -> " << player -> invested_star_dust;
    GoToXY(x,y + 16); cout << "DARK MATTER DETECTORS -> " << player -> dark_matter_detectors;
    GoToXY(x,y + 18); cout << "PAYOUT RATIO -> " << (float) pow(PAYOUT_RATIO, player -> level);
    GoToXY(x,y + 20); cout << "PAYOUT -> " << ((player -> invested_star_dust) * (float) pow(PAYOUT_RATIO, player -> level - 1)) - (player -> invested_star_dust);
}

void DisplayHiddenPortals(int levels, int portals) {
    for (int i = 0; i < levels; i++) {
        for (int j = 0; j < portals; j++) {
            GoToXY(PORTALS_X + j*12 , PORTALS_Y + i*5); cout << "______      ";
        }
    }
}

void DisplaySavedPlanets(int x, int y, Player *players) {
    int planets = 0;
    while(players != NULL) {
        if (players -> star_dust > MAXIMUM_STAR_DUST) {
            planets++;
        }
        players = players -> next;
    }
    GoToXY(x + 8, y); cout << "SAVED PLANETS -> "; cout << planets;
    GoToXY(x,y + 2); cout << "             _____";
    GoToXY(x,y + 3); cout << "          .-'.  ':'-.";
    GoToXY(x,y + 4); cout << "        .''::: .:    '.";
    GoToXY(x,y + 5); cout << R"(       /   :::::'      \)";
    GoToXY(x,y + 6); cout << "      ;.    ':' `       ";
    GoToXY(x,y + 7); cout << "      |       '..       |";
    GoToXY(x,y + 8); cout << "      ; '      ::::.    ";
    GoToXY(x,y + 9); cout << R"(       \       '::::   /)";
    GoToXY(x,y + 10); cout << "        '.      :::  .'";
    GoToXY(x,y + 11); cout << "           '-.___'_.-'";
}

void DisplayClimate(int x, int y, int bad_portals) {
    if (bad_portals == 3) {
        GoToXY(x + 7, y); cout << "CLIMATE";
        GoToXY(x, y + 1); cout << "      ;   :   ;";
        GoToXY(x, y + 2); cout << R"(   .   \_,!,_/   ,)";
        GoToXY(x, y + 3); cout << "    `.,'     `.,'";
        GoToXY(x, y + 4); cout << R"(     /  o  o  \)";
        GoToXY(x, y + 5); cout << R"(     \    w    /)";
        GoToXY(x, y + 6); cout << R"(    ,'`._   _.'`.)";
        GoToXY(x, y + 7); cout << R"(   '   / `!` \   `)";
        GoToXY(x, y + 8);cout << R"(      ;   :   ;  )";
        GoToXY(x, y + 9); cout << "The Sun is shining!";
        GoToXY(x, y + 10); cout << "Normal Portals -> " << PORTALS - bad_portals;
    }
    else {
        GoToXY(x + 7, y); cout << "CLIMATE";
        GoToXY(x, y + 1); cout << "       _";
        GoToXY(x, y + 2); cout << "     _( )_          _      ";
        GoToXY(x, y + 3); cout << "   _(     )_      _( )_";
        GoToXY(x, y + 4); cout << "  (_________)   _(     )_   ";
        GoToXY(x, y + 5); cout << R"(    \  \  \    (_________))";
        GoToXY(x, y + 6); cout << R"(      \  \       \  \  \)";
        GoToXY(x, y + 7); cout << R"(                   \  \)";
        GoToXY(x - 4, y + 8); cout << "A storm is raging across the galaxy...";
        GoToXY(x, y + 10); cout << "Safe Portals -> " << PORTALS - bad_portals;
    }
}

void ASCII_1(int x, int y) {
    GoToXY(x, y); cout << "                        .   *        .       .";
    GoToXY(x, y + 1); cout << "         *      -0-";
    GoToXY(x, y + 2); cout << "            .                .  *       - )-";
    GoToXY(x, y + 3); cout << "         .      *       o       .       *";
    GoToXY(x, y + 4); cout << "   o                |";
    GoToXY(x, y + 5); cout << "             .     -O-";
    GoToXY(x, y + 6); cout << "  .                 |        *      .     -0-";
    GoToXY(x, y + 7); cout << "         *  o     .    '       *      .        o";
    GoToXY(x, y + 8); cout << "                .         .        |      *";
    GoToXY(x, y + 9); cout << "     *             *              -O-          .";
    GoToXY(x, y + 10); cout << "           .             *         |     ,";
    GoToXY(x, y + 11); cout << "                  .           o";
    GoToXY(x, y + 12); cout << "          .---.";
    GoToXY(x, y + 13); cout << R"(    =   _/__~0_\_     .  *            o       ')";
    GoToXY(x, y + 14); cout << "   = = (_________)             .";
    GoToXY(x, y + 15); cout << "                   .                        *";
    GoToXY(x, y + 16); cout << "         *               - ) -       *";
    GoToXY(x, y + 17); cout << "                .               .";
}

void ASCII_2(int x, int y) {
    GoToXY(x, y); cout << "        _..._";
    GoToXY(x, y + 1); cout << "      .'     '.      _";
    GoToXY(x, y + 2); cout << R"(     /    .-""-\   _/ \)";
    GoToXY(x, y + 3); cout << "   .-|   /:.   |  |   |";
    GoToXY(x, y + 4); cout << R"(   |  \  |:.   /.-'-./)";
    GoToXY(x, y + 5); cout << "   | .-'-;:__.'    =/";
    GoToXY(x, y + 6); cout << "   .'=  *=|     _.='";
    GoToXY(x, y + 7); cout << "  /   _.  |    ;";
    GoToXY(x, y + 8); cout << R"( ;-.-'|    \   |)";
    GoToXY(x, y + 9); cout << R"(/   | \    _\  _\)";
    GoToXY(x, y + 10); cout << R"(\__/'._;.  ==' ==\)";
    GoToXY(x, y + 11); cout << "         /    /   /";
    GoToXY(x, y + 12); cout << "          .---.";
    GoToXY(x, y + 13); cout << "         /-._/-._/";
    GoToXY(x, y + 14); cout << R"(         \   `\  \)";
    GoToXY(x, y + 15); cout << "          `-._/._/";
}
void DisplayInnerBorders() {
    for (int i = 0; i < SCREEN_HEIGHT - 1; i++) {
        for (int j = 0; j < BORDER_WIDTH; j++) {
            GoToXY(LEFT_BORDER_DISTANCE + j, i + 1); cout << "|";
            GoToXY(SCREEN_WIDTH - RIGTH_BORDER_DISTANCE - j, i + 1); cout << "|";
        }
    }
}

void DisplayExternalBorders() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        GoToXY(i + 1, 0); cout << "-";
        GoToXY(i + 1, SCREEN_HEIGHT); cout << "-";
    }

    for (int i = 0; i < SCREEN_HEIGHT - 1; i++) {
        GoToXY(SCREEN_WIDTH, i + 1); cout << "|";
        GoToXY(1, i + 1); cout << "|";
    }
}

void Instructions(int x, int y) {
    system("cls");
    DisplayExternalBorders();
    Tutorial(TITLE_X, TITLE_Y);
    GoToXY(x, y);
    GoToXY(x, y + 2); cout << "Welcome to Portal! The universe is in danger, and various natural disasters are responsible for the disappearance of worlds. The life of all living beings is in danger.";
    GoToXY(x, y + 4); cout << "In our vast universe, there are particles known as Stardust, which are responsible for the formation of planets in galaxies.";
    GoToXY(x, y + 6); cout << "Your goal as an interstellar explorer will be to collect the minimum amount of Stardust (200) to generate a planet and prevent universal extinction.";
    GoToXY(x, y + 8); cout << "Along your journey, you will encounter various intergalactic portals that connect to galaxies full of Stardust.";
    GoToXY(x, y + 10); cout << "Through cunning and a little luck, you will be able to collect the dust from these galaxies. However, there are other portals that connect to dangerous galaxies.";
    GoToXY(x, y + 12); cout << "These galaxies have negative effects on your search for Stardust, so try not to encounter them.";
    GoToXY(x, y + 14); cout << "Furthermore, the cosmos hums with a delicate balance. Every choice you make, whether it's saving a planet or succumbing to death, ";
    GoToXY(x, y + 16); cout << "changes the weather and, as a consequence, increases the number of dangerous portals. Use your Stardust wisely accordingly!";
    GoToXY(x, y + 18); cout << "You'll also have access to a resource that will assist you throughout your journey, called Dark Matter Detectors. Use one of them and you'll reveal a random Portal!";
    GoToXY(x, y + 20); cout << "Finally, each time you start a journey, you will need to invest at least 5 Stardust for the fuel of the ship.";
    GoToXY(x, y + 22); cout << "The more Stardust you invest, the longer you can stay in each galaxy and thus collect more Stardust.";
    GoToXY(x, y + 24); cout << "However, if you no longer have enough Stardust to travel, you will run out of fuel, leaving you exposed in the universe, a slow, cold, and lonely death.";
    GoToXY(x, y + 26); cout << "Good luck!";
    GoToXY(x, y + 28); cout << "Press any key to return to the menu.";
    getche();

}

void DisplayBoard(Player *player, Player *sorted_players) {
    DisplayInnerBorders();
    DisplayExternalBorders();
    MainTitle(TITLE_X, TITLE_Y);
    Ranking(RANKING_X, RANKING_Y, sorted_players);
    Controls(CONTROLS_X, CONTROLES_Y);
    DisplayHiddenPortals(LEVELS, PORTALS);
    PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
    DisplayClimate(CLIMATE_X, CLIMATE_Y, BAD_PORTALS);
    DisplaySavedPlanets(PLANETS_X, PLANETS_Y, GLOBAL_PLAYERS);
}


void DisplayPlayer(Player *player) {
    int x_position = player -> portal;
    int y_position = player -> level;
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position)- 3);
    cout << "  ()  ";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position)- 2);
    cout << R"( /||\ )";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position) - 1);
    cout << R"(  /\  )";
}

void DisplayPortal(bool bad, int x_position, int y_position) {
    if (bad) {
        SetConsoleTextAttribute(console, 12);
    }
    else {
        SetConsoleTextAttribute(console, 9);
    }
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position)- 3);
    cout << " **** ";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position) - 2);
    cout << "******";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position) - 1);
    cout << " **** ";
    SetConsoleTextAttribute(console, 7);
}

void DisplayRandomPortal(int level, Level *levels) {
    Portal *portals = NULL;
    int random, count = 1;
    while (levels != NULL)
    {
        if (levels -> y_position == level) {
            portals = levels -> portals;
            random = GenerateRandom(1, PORTALS);
            while (portals != NULL) {
                if (count == random) {
                    DisplayPortal(portals -> bad, portals -> x_position, level);
                }
                count++;
                portals = portals -> next; 
            }       
        }
        levels = levels -> next;  
    }
}

void DisplayPortals(int level, Level *levels) {
    Portal *portals = NULL;
    while (levels != NULL)
    {
        if (levels -> y_position == level) {
            portals = levels -> portals;
            while (portals != NULL) {
                DisplayPortal(portals -> bad, portals -> x_position, level);
                portals = portals -> next;  
            }         
        }
        levels = levels -> next; 
    }
}
void DeletePlayer(Player *player) {
    int x_position = player -> portal;
    int y_position = player -> level;
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position)- 3);
    cout << "      ";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position) - 2);
    cout << "      ";
    GoToXY(PORTALS_X + 12*(x_position - 1), PORTALS_Y + 5*(LEVELS - y_position) - 1);
    cout << "      ";
}

int StartDustInput(Player *player) {
    int star_dust = 0;
    bool valid = false;
    while (star_dust <  MINIMUM_STAR_DUST || star_dust > player -> star_dust || !valid) {
        ShowMessageGame("", "Enter Star Dust amount: ", "", "");
        valid = ValidateInt(&star_dust);
        if (valid) {
            if (star_dust < MINIMUM_STAR_DUST) {
                ShowMessageGame("", "Start Dust amount must be greater than 5", "Press 'c' to continue", "");
                PressKey('c');
            }
            else if (star_dust > player -> star_dust) {
                ShowMessageGame("", "Not enough Star Dust!", "Press 'c' to continue", "");
                PressKey('c');

            }
        }
        else {
            ShowMessageGame("", "Must type a number!", "Press 'c' to continue", "");
            PressKey('c');
        }
        DeleteMessageGame();
    }
    return star_dust;
}

int InputAmountPlayers() {
    int n;
    bool valid = false;
    while (!valid) {
        ShowMessageMenu("", "Enter amount of players: ", "");
        valid = ValidateInt(&n);
        if (!valid) {
            ShowMessageMenu("", "Invalid Number", "Press 'c' to continue");
            PressKey('c');
            DeleteMessageMenu();
        }
    }
    return n;
}


void CongratulatePlayer(Player *player) {
    ShowMessageGame("Congratulations " + player -> name + "!", "Your efforts created a new Planet ", "The Universe is in debt to you","Press 'c' to continue");
    PressKey('c');
    DeleteMessageGame();
}

void RemovePlayer(Player *player) {
    ShowMessageGame("Sorry " + player -> name + "...", "You have run out of Stardust", "Press 'c' to continue", "");
    PressKey('c');
    DeleteMessageGame();
}

void Thanks() {
    ShowMessageGame("Thanks for playing!", "Made by: ", "Miguel Pinto", "Press 'c' to continue");
    PressKey('c');
    DeleteMessageGame();
}
//Sort, generate, insert, etc.  Functions
Portal *GeneratePortals(int n) {
    Portal *portals = NULL;
    bool bad;
    for (int i = 0; i < n; i++) {
        if (i < BAD_PORTALS) {
            bad = true;
        }
        else {
            bad = false;
        }
        InsertPortal(&portals, bad);
    }
    return portals;
}

Portal *GenerateEmptyPortals(int n) {
    Portal *portals = NULL;
    for (int i = 0; i < n; i++) {
        InsertPortal(&portals, false);
    }
    return portals;
}

int SizePortals(Portal *portals) {
    int count = 0;
    while (portals != NULL)
    {
        count ++;
        portals = portals -> next;
    }
    return count;
}

void InsertPortalIndex(Portal **portals, int index, bool bad) {
    Portal *aux = *portals, *new_portal = CreatePortal(bad), *previous = NULL;
    int count = 0;
    while (aux != NULL && (count + 1 ) < index) {
        previous = aux;
        aux = aux -> next;
        count ++;
    }

    if (previous == NULL) {
        new_portal -> next = *portals;
        *portals = new_portal;
    }
    else {
        new_portal -> next = previous -> next;
        previous -> next = new_portal;
    }
}

void RandomizePortals(Portal **portals) {
    Portal *randomized_portals = NULL;
    bool bad;
    int random;
    Portal *previous = NULL;
    while ((*portals) != NULL) {
        bad = (*portals) -> bad;
        random = GenerateRandom(1, SizePortals(randomized_portals) + 1);
        InsertPortalIndex(&randomized_portals, random, bad);
        previous = *portals;
        *portals = (*portals) -> next;
        delete previous;
    }
    *portals = randomized_portals;
}

void SetPortalsPosition(Portal *portal) {
    int x_position = 1;
    Portal *aux = portal;
    while (aux != NULL)
    {
        aux -> x_position = x_position;
        aux = aux -> next;
        x_position++;
    }
}

void SetLevelsPortals(Level *levels) {
    Portal *portals = NULL;
    while(levels != NULL) {
        portals = GeneratePortals(PORTALS);
        RandomizePortals(&portals);
        SetPortalsPosition(portals);
        levels -> portals = portals;
        levels = levels -> next;
    }
}

void SetLevelsEmptyPortals(Level *levels) {
    Portal *portals = NULL;
    while(levels != NULL) {
        portals = GenerateEmptyPortals(PORTALS);
        SetPortalsPosition(portals);
        levels -> portals = portals;
        levels = levels -> next;
    }
}

Level *GenerateLevelsPositions() {
    int y_position;
    Level *levels = NULL;
    for (int i = 0; i < LEVELS; i++) {
        y_position = LEVELS - i;
        InsertLevel(&levels, y_position);
    }
    return levels;
}

Player *GeneratePlayers(int n) {
    Player *players = NULL;
    for (int i = 0; i < n; i++) {
        string name;
        ShowMessageMenu("", "Add Player " + to_string(i + 1) + ": ", "");
        cin >> name;
        DeleteMessageMenu();
        InsertPlayer(&players, name, 1, 1, INITIAL_STAR_DUST, DARK_MATTER_DETECTOR, 0);
    }
    return players;
}
bool IsBad(int x_position, int y_position, Level *levels) {
    Portal *portals = NULL;
    while(levels != NULL) {
        if (levels -> y_position == y_position) {
            portals = levels -> portals;
            while (portals != NULL)
            {
                if (portals -> x_position == x_position) {
                    return portals -> bad;
                }
                portals = portals -> next;
            }
            
        }
        levels = levels -> next;
    }
}

void SortPlayersTurn(Player *current_player) {
    if (GLOBAL_PLAYERS != current_player) {
        Player *aux_1 = GLOBAL_PLAYERS, *aux_2 = current_player;
        while(aux_1 -> next != current_player) {
            aux_1 = aux_1 -> next;
        }
        aux_1 -> next = NULL;
        while (aux_2 -> next != NULL) {
            aux_2 = aux_2 -> next;
        }
        aux_2 -> next = GLOBAL_PLAYERS;
        GLOBAL_PLAYERS = current_player;   
    }
}

void InsertPlayerDesc(Player **players, Player *player) {
    Player *new_player = CreatePlayer(player -> name, player -> portal, player -> level, player -> star_dust, player -> dark_matter_detectors, player -> invested_star_dust); 
    Player *aux = *players, *previous = NULL;
    if ((*players) == NULL || new_player -> star_dust >= (*players) -> star_dust ) {
        aux = *players,
        *players = new_player;
        new_player -> next = aux;   
    }
    else {
        while(aux != NULL && new_player -> star_dust < aux -> star_dust) {
            previous = aux;
            aux = aux -> next;
        }
        previous -> next = new_player;
        new_player -> next = aux;
    }
}

Player *SortPlayersRanking(Player *players) {
    Player *sorted_players = NULL;
    while (players != NULL) {
        InsertPlayerDesc(&sorted_players, players);
        players = players -> next;
    }
    return sorted_players;
}

BadPortal *GenerateBadPortals(Level *levels) {
    BadPortal *bad_portals = NULL; Portal *aux_portals = NULL; Level *aux_levels = levels; 
    while (aux_levels != NULL){
        aux_portals = aux_levels -> portals;
        while(aux_portals != NULL){
            if (aux_portals -> bad){
                InsertBadPortal(&bad_portals, aux_portals -> x_position, aux_levels -> y_position);
            }
            aux_portals = aux_portals -> next;
        }
        aux_levels = aux_levels -> next;
    }
    return bad_portals;
}

void FillEmptyPortals(BadPortal *bad_portals, Level *levels){
    Portal *aux_portals = NULL; Level *aux_levels = levels;
    int y_position, x_position;
    while (bad_portals != NULL){
        while (aux_levels != NULL)
        {
            aux_portals = aux_levels -> portals;
            y_position = aux_levels -> y_position;
            while (aux_portals != NULL) 
            {
                x_position = aux_portals -> x_position;
                if (x_position == bad_portals -> x_position && y_position == bad_portals -> y_position) {
                    aux_portals -> bad = true;
                }
                aux_portals = aux_portals -> next;
            }
            aux_levels = aux_levels -> next;
        }
        aux_levels = levels;
        bad_portals = bad_portals -> next;
    
    }
}

int RemainingPlayers(Player *players) {
    int count = 0;
    while(players!= NULL) {
        if (!players -> removed) {
            count++;
        }
        players = players -> next;
    }
    return count;
}

void GenerateBoard() {
    FreeLevels(&GLOBAL_LEVELS);
    FreeBadPortals(&GLOBAL_BAD_PORTALS);
    GLOBAL_LEVELS = GenerateLevelsPositions();
    SetLevelsPortals(GLOBAL_LEVELS);
    GLOBAL_BAD_PORTALS = GenerateBadPortals(GLOBAL_LEVELS);
}

void UpdateBadPortals(Player *players) {
    BAD_PORTALS = MINIMUM_BAD_PORTALS;
    while (players != NULL) {
        if (players -> removed) {
            BAD_PORTALS++;
        }
        players = players -> next;
    }
    if (BAD_PORTALS > MAXIMUM_BAD_PORTALS) {
        BAD_PORTALS = MAXIMUM_BAD_PORTALS;
    }
}

//Debugging
void DisplayLevels() {
    Level *levels = GLOBAL_LEVELS;
    Portal *portals = NULL;
    while (levels != NULL)
    {
        portals = levels -> portals;
        cout << levels -> y_position << ":  ";
        while (portals != NULL)
        {
            cout << portals -> bad << "/" << portals -> x_position <<  " -> ";
            portals = portals -> next;

        }
        cout << endl;
        levels = levels -> next;
        
    }
}

//Game Logic Functions
void PlayTurn(Player *player, bool *exit) {
    system("cls");
    UpdateBadPortals(GLOBAL_PLAYERS);
    //GoToXY(1, SCREEN_HEIGHT + 2);
    //DisplayLevels();
    DisplayBoard(player, SortPlayersRanking(GLOBAL_PLAYERS));
    DisplayPlayer(player);
    if (player -> invested_star_dust == 0) {
        player -> invested_star_dust = StartDustInput(player);
    }
    if (player -> level != 1) {
        for (int i = 1; i < player -> level; i++) {
            DisplayPortals(i, GLOBAL_LEVELS);
        }
    }
    while (1) {
        PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
        char ch = getch();
        int x_displacement = 0;
        int level = player -> level;
        if (ch == 'a' || ch == 'A') {
            if (player -> portal > 1) {
                x_displacement = -1;
            }
        }
        if ( ch == 'd' || ch == 'D') {
            if (player -> portal <  PORTALS) {
                x_displacement = 1;
            }
        }
        if (ch == 'w' || ch == 'W') {
            bool bad = IsBad(player -> portal, player -> level, GLOBAL_LEVELS);
            if (bad) {
                DisplayPortal(bad, player -> portal, player -> level);
                player -> star_dust -= player -> invested_star_dust;
                player -> invested_star_dust = 0;
                PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
                ShowMessageGame(player -> name + " encountered a bad portal :(", "Turn is over", "Press 'c' to continue", "");
                PressKey('c');
                DeleteMessageGame();
                player -> level = 1;
                break;
            }
            else if (player -> level < LEVELS) {
                DisplayPortals(player -> level, GLOBAL_LEVELS);
                player -> level++;
            }
            else {
                DisplayPortals(player -> level, GLOBAL_LEVELS);
                player -> star_dust += ((player -> invested_star_dust) * (float) pow(PAYOUT_RATIO, player -> level - 1)) - (player -> invested_star_dust);
                player -> invested_star_dust = 0;
                PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
                ShowMessageGame(player -> name + " reached the highest level! ", "Turn is over", "Press 'c' to continue", "");
                PressKey('c');
                DeleteMessageGame();
                player -> level = 1;
                break;
            }
        }
        if (ch == 'r' || ch == 'R') {
            player -> star_dust += ((player -> invested_star_dust) * (float) pow( PAYOUT_RATIO, player -> level - 1)) - (player -> invested_star_dust);
            player -> invested_star_dust = 0;
            PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
            ShowMessageGame(player -> name + " made a withdrawal" , "Turn is over", "Press 'c' to continue", "");
            PressKey('c');
            DeleteMessageGame();
            player -> level = 1;
            break; 
        }
        if (ch == 'f' || ch == 'F') {
            if (player -> dark_matter_detectors > 0) {
                DisplayRandomPortal(player -> level, GLOBAL_LEVELS);
                player -> dark_matter_detectors--;
                PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
                ShowMessageGame("", "You have used a Dark Matter Detector", to_string(player -> dark_matter_detectors) + " Remaining...", "Press 'c' to continue");
                PressKey('c');
                DeleteMessageGame();
            }
            else {
                ShowMessageGame("", "You have run out of Dark Matter Detectors", "Press 'c' to continue", "");
                PressKey('c');
                DeleteMessageGame();
            }
        }
        if (ch == 'k' || ch == 'K') {
            *exit = true;
            SortPlayersTurn(player);
            SaveGame(GLOBAL_PLAYERS, GLOBAL_BAD_PORTALS);
            break;
        } 
        DeletePlayer(player);
        player -> portal += x_displacement;
        DisplayPlayer(player);
    }
    DeletePlayer(player);
    PlayerInfo(PLAYER_INFO_X, PLAYER_INFO_Y, player);
    if (player -> star_dust < MINIMUM_STAR_DUST || player -> star_dust >= MAXIMUM_STAR_DUST) {
        player -> removed = true;
        if (player -> star_dust < MINIMUM_STAR_DUST) {
            RemovePlayer(player);
        }
        else {
            CongratulatePlayer(player);
        }
    }
    GenerateBoard();  
}

void Play() {
    bool exit = false;
    int remaining = RemainingPlayers(GLOBAL_PLAYERS);
    while (remaining > 0 && !exit) {
        Player *aux_players = GLOBAL_PLAYERS;
        while (aux_players != NULL && !exit) {
            if (!aux_players -> removed) {
                PlayTurn(aux_players, &exit);
            }
            aux_players = aux_players -> next;
        }
        remaining = RemainingPlayers(GLOBAL_PLAYERS);
    }
    SaveGame(GLOBAL_PLAYERS, GLOBAL_BAD_PORTALS);
    Thanks();
}

//Menu Functions
void CreateNewGame() {
    system("cls");
    DisplayExternalBorders();
    MainTitle(TITLE_X, TITLE_Y);
    int n;
    n = InputAmountPlayers();
    GLOBAL_PLAYERS = GeneratePlayers(n);
    GenerateBoard();  
    Play();  
}


void ContinueGame() {
    GLOBAL_PLAYERS = LoadPlayers("players_save.csv");
    GLOBAL_BAD_PORTALS = LoadBadPortals("bad_portals_save.csv");
    int remaining = RemainingPlayers(GLOBAL_PLAYERS);
    if (GLOBAL_PLAYERS == NULL || GLOBAL_BAD_PORTALS == NULL) {
        ShowMessageMenu("There's no saved game", "Go back to the menu and create a new game", "Press 'c' to continue");
        PressKey('c');
        DeleteMessageMenu();
    }
    else if (remaining == 0) {
        ShowMessageMenu("The current game has no players remaining", "Go back to the menu and create a new game", "Press 'c' to continue");
        Ranking(TITLE_X - 6, TITLE_Y + 20, GLOBAL_PLAYERS);
        DisplaySavedPlanets(TITLE_X + 40 , TITLE_Y + 20, GLOBAL_PLAYERS);
        PressKey('c');
        DeleteMessageMenu();
    }
    else {
        GLOBAL_LEVELS = GenerateLevelsPositions();
        SetLevelsEmptyPortals(GLOBAL_LEVELS);
        FillEmptyPortals(GLOBAL_BAD_PORTALS, GLOBAL_LEVELS);
        Play();
    }
}

void SelectGame() {
    char option = ' ';
    while (option != '3') {
        system("cls");
        DisplayExternalBorders();
        MainTitle(TITLE_X, TITLE_Y);
        int x = MENU_X, y = MENU_Y;
        GoToXY(x, y); cout << "1. New Game";
        GoToXY(x, y + 2); cout << "2. Continue Game";
        GoToXY(x, y + 4); cout << "3. Back to Menu";
        GoToXY(x, y + 7); cout << "Select Option: ";
        option = getche();

        if (option == '1') CreateNewGame();
        else if (option == '2') ContinueGame();
    }
}
