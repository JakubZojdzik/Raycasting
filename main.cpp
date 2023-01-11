#include <bits/stdc++.h>
#include <cmath>
using namespace std;

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;
int map[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
double posX = 3, posY = 3;
double direction = 90;
const double FOV = 60;

/*
{'@', '%', '#', '*', '+', '=', '-', ':', '.'} // 9 chars
$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.
*/
char charset[] = {'@', '%', '#', '*', '+', '=', '-', ':', '.'};
const int COLORS = 9;

void setup()
{
    cout << "\x1B[2J\x1B[H";
    cout << "\033[0;0H";
}

void game_loop()
{
    while(1)
    {
        cout << "\033[0;0H";

        for(double deg = direction - FOV/2; deg <= direction + FOV/2; deg++)
        {
            double begX, begY, fullX, fullY;
            if(deg == 0)
            {

            }
            else if(deg == 90)
            {

            }
            else if(deg == 180)
            {

            }
            else if(deg == 270)
            {

            }
            else
            {
                begX = (1 - modf(posY, nullptr)) / tan(deg);
                begY = (1 - modf(posX, nullptr)) * tan(deg);
                fullX = 1 / tan(deg);
                fullY = tan(deg);
            }
        }

        // for(int i = 0; i < SCREEN_HEIGHT; i++)
        // {
        //     for(int j = 0; j < SCREEN_WIDTH; j++)
        //     {
        //         cout << charset[0];
        //     }
        //     cout << '\n';
        // }
    }
}

int main()
{
    cin.tie(0); cout.tie(0);
    ios_base::sync_with_stdio(0);


    setup();
    game_loop();
}

