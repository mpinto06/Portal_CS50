#include "game_utils.cpp"
using namespace std;


//Main 
int main() {
    SetCursor(false, 1);
    srand( (unsigned) time(NULL));
    char opcion = ' ';
    while (opcion != '3') {
        system("cls");
        DisplayExternalBorders();
        ASCII_1(ASCII_1_X, ASCII_1_Y);
        ASCII_2(ASCII_2_X, ASCII_2_Y);
        MainTitle(TITLE_X, TITLE_Y);
		int x = MENU_X, y = MENU_Y;
        GoToXY(100, 45); cout<<"Made by Miguel Pinto";
        GoToXY(x, y); cout << "1. Play";
        GoToXY(x, y + 2); cout << "2. Instructions";
        GoToXY(x, y + 4); cout << "3. Exit";
        GoToXY(x, y + 7); cout << "Select Option: ";
        opcion = getche();

        if (opcion == '1') SelectGame();
        else if (opcion == '2') Instructions(MENU_X, MENU_Y);
    }
    FreeGlobals();
}

