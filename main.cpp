#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
using namespace std;


// ----------------------  CONTROLS  ----------------------
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
double posX = 1, posY = 3;
double direction = 90;
const double MOVEMENT_SPEED = .3;
const double ROTATION_SPEED = .3;
const double ANGLE_INCREMENT = 1;
// --------------------------------------------------------

const double FOV = SCREEN_WIDTH / 2;
int screen[SCREEN_WIDTH][SCREEN_HEIGHT];
struct termios oldSettings, newSettings;


/*
{'@', '%', '#', '*', '+', '=', '-', ':', '.'} // 9 chars
{'$', '@', 'B', '%', '8', '&', 'W', 'M', '#', '*', 'o', 'a', 'h', 'k', 'b', 'd', 'p', 'q', 'w', 'm', 'Z', 'O', '0', 'Q', 'L', 'C', 'J', 'U', 'Y', 'X', 'z', 'c', 'v', 'u', 'n', 'x', 'r', 'j', 'f', 't', '/', '|', '(', ')', '1', '{', '}', '[', ']', '?', '-', '_', '+', '~', '<', '>', 'i', '!', 'l', 'I', ';', ':', ',', '"', '^', '`', '\'', '.'} // 68 chars
{'@', '%', '=', ':', '.'} // 5 chars
{'█', '▓', '▒', '░'} // 4 chars


    {1, 1, 1, 1, X, X, X, 1},
    {1, 0, 0, G, 0, 0, 0, X},
    {1, 0, 0, 0, 0, 0, 0, X},
    {1, 0, 0, 0, 0, 0, 0, X},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}

*/
char charset[] = {'@', '%', '=', ':', '.'};
const int COLORS = 5;

double to_rad(double degree)
{
    double pi = 3.14159265359;
    return (degree * (pi / 180));
}

void listen()
{
    while ( 1 )
    {
        fd_set set;
        struct timeval tv;

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO( &set );
        FD_SET( fileno( stdin ), &set );

        int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

        if( res > 0 )
        {
            char c;
            read( fileno( stdin ), &c, 1 );
            switch (c)
            {
            case 'w':
                posY += MOVEMENT_SPEED;
                break;

            case 's':
                posY -= MOVEMENT_SPEED;
                break;

            case 'd':
                posX += MOVEMENT_SPEED;
                break;

            case 'a':
                posX += MOVEMENT_SPEED;
                break;

            case 'l':
                direction += ROTATION_SPEED;
                if(direction < 0) direction += 360;
                if(direction >= 360) direction -= 360;
                break;

            case 'k':
                direction -= ROTATION_SPEED;
                if(direction < 0) direction += 360;
                if(direction >= 360) direction -= 360;
                break;
            default:
                break;
            }
        }
    }

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
}

void setup()
{
    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr( fileno( stdin ), TCSANOW, &newSettings );

    cout << "\x1B[2J\x1B[H";
    cout << "\033[0;0H";
}

void game_loop()
{
    while(1)
    {
        int itr = 0;
        for(double deg = direction - FOV/2; deg <= direction + FOV/2; deg += ANGLE_INCREMENT)
        {
            if(deg < 0) deg += 360;
            if(deg >= 360) deg -= 360;
            cerr << "deg = " << deg << '\n';
            double currX, currY, fullX, fullY, syf;
            currX = abs((1 - modf(posX, &syf)) / cos(to_rad(deg)));
            currY = abs((1 - modf(posY, &syf)) / sin(to_rad(deg)));
            fullX = abs(1 / cos(to_rad(deg)));
            fullY = abs(1 / sin(to_rad(deg)));
            if(direction == 0 || direction == 180)
            {
                currX = abs(1 - modf(posX, &syf));
                fullX = 1;
                currY = 1e17;
                fullY = 1e17;
            }
            if(direction == 90 || direction == 270)
            {
                currX = 1e17;
                fullX = 1e17;
                currY = abs(1 - modf(posY, &syf));
                fullY = 1;
            }

            int stepX, stepY;
            if(deg >= 90 && deg < 270) stepX = -1;
            else stepX = 1;
            if(deg >= 0 && deg < 180) stepY = 1;
            else stepY = -1;
            // cerr << "currX: " << currX << ", fullX: " << fullX << '\n';
            // cerr << "currY: " << currY << ", fullY: " << fullY << '\n';
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
            // cerr << "zobaczylem (" << mapX << ", " << mapY << ")\n";
            double dist;
            if(side == 0) dist = currX - fullX;
            else dist = currY - fullY;
            cerr << "D: " << dist << '\n';
            int h = SCREEN_HEIGHT / round(dist);
            // int color = COLORS - ((dist-1) * COLORS) / (SCREEN_WIDTH + SCREEN_HEIGHT);
            int color = COLORS - ((dist - 1) * COLORS) / (MAP_WIDTH + MAP_HEIGHT);
            color = min(color, COLORS-1);
            color = max(color, 0);
            int line_beg = max(0, SCREEN_HEIGHT / 2 - h / 2);
            int line_end = min(SCREEN_HEIGHT / 2 + h / 2, SCREEN_HEIGHT-1);
            // cerr << "zapisuje od " << line_beg << " do " << line_end << " kolor " << color << '\n';
            for(int i = line_beg; i <= line_end; i++)
            {
                screen[itr][i] = color;
                screen[itr+1][i] = color;
            }
            itr += 2;
        }
        cerr << "itr = " << itr << '\n';
        // break;

        cout << "\033[0;0H";
        // sleep(.6);
        for(int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                cout << charset[screen[x][y]];
            }
            cout << "\n";
        }

        for(int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                screen[x][y] = 0;
            }
        }

        direction += .1;
        if(direction >= 360) direction -= 360;
    }
}

int main()
{
    cin.tie(0); cout.tie(0);
    ios_base::sync_with_stdio(0);


    setup();
    game_loop();
}

