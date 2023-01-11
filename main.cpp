#include <bits/stdc++.h>
#include <cmath>
using namespace std;

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;
int board[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
int screen[SCREEN_WIDTH][SCREEN_HEIGHT];
double posX = 3, posY = 3;
double direction = 90;
const double FOV = SCREEN_WIDTH / 2;

/*
{'@', '%', '#', '*', '+', '=', '-', ':', '.'} // 9 chars
$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.
*/
char charset[] = {'@', '%', '#', '*', '+', '=', '-', ':', '.'};
const int COLORS = 9;

double to_rad(double degree)
{
    double pi = 3.14159265359;
    return (degree * (pi / 180));
}

void setup()
{
    cout << "\x1B[2J\x1B[H";
    cout << "\033[0;0H";
}

void game_loop()
{
    while(1)
    {
        int itr = 0;
        for(double deg = direction - FOV/2; deg <= direction + FOV/2; deg++)
        {
            if(deg < 0) deg += 360;
            cerr << "deg = " << deg << '\n';
            double currX, currY, fullX, fullY, syf;
            currX = (1 - modf(posX, &syf)) / cos(to_rad(deg));
            currY = (1 - modf(posY, &syf)) / sin(to_rad(deg));
            fullX = 1 / cos(to_rad(deg));
            fullY = 1 / sin(to_rad(deg));

            int stepX, stepY;
            if(deg >= 90 && deg < 270) stepX = -1;
            else stepX = 1;
            if(deg >= 0 && deg < 180) stepY = 1;
            else stepY = -1;
            cerr << "currX: " << currX << ", fullX: " << fullX << '\n';
            cerr << "currY: " << currY << ", fullY: " << fullY << '\n';
            int mapX = long(posX), mapY = long(posY);
            int side;
            bool wall = 0;
            while(!wall)
            {
                if(currX < currY)
                {
                    currX += fullX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    currY += fullY;
                    mapY += stepY;
                    side = 1;
                }
                if(board[mapX][mapY]) wall = 1;
            }
            double dist;
            if(side == 0) dist = currX - fullX;
            else dist = currY - fullY;
            cerr << "D: " << dist << '\n';

            int h = SCREEN_HEIGHT / round(dist);
            // int color = ((dist-1) * COLORS) / (SCREEN_WIDTH + SCREEN_HEIGHT);
            int color = COLORS - ((dist-1) * COLORS) / (SCREEN_WIDTH + SCREEN_HEIGHT);
            color = min(color, COLORS-1);
            color = max(color, 0);
            int line_beg = SCREEN_HEIGHT / 2 - h / 2;
            int line_end = SCREEN_HEIGHT / 2 + h / 2;
            // cerr << "zapisuje od " << line_beg << " do " << line_end << " kolor " << color << '\n';
            for(int i = line_beg; i <= line_end; i++)
            {
                screen[itr][i] = color;
                screen[itr+1][i] = color;
            }
            itr += 2;
        }
        cerr << "itr = " << itr << '\n';

        cout << "\033[0;0H";
        sleep(0.1);
        for(int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                cout << charset[screen[x][y]];
            }
            cout << "\n";
        }
    }
}

int main()
{
    cin.tie(0); cout.tie(0);
    ios_base::sync_with_stdio(0);


    setup();
    game_loop();
}

