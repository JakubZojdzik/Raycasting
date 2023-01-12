#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

// ----------------------  CONTROLS  ----------------------
const int MAP_WIDTH = 12;
const int MAP_HEIGHT = 12;
int board[MAP_WIDTH][MAP_HEIGHT] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 50;
double posX = 2.5, posY = 2.5;
double direction = 90;
const double MOVEMENT_SPEED = .4;
const double ROTATION_SPEED = 4.0;
// --------------------------------------------------------

const double FOV = SCREEN_WIDTH / 2;
int screen[SCREEN_WIDTH][SCREEN_HEIGHT];
struct termios oldSettings, newSettings;

/*
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

{'@', '%', '#', '*', '+', '=', '-', ':', '.'} // 9 chars
{'$', '@', 'B', '%', '8', '&', 'W', 'M', '#', '*', 'o', 'a', 'h', 'k', 'b', 'd', 'p', 'q', 'w', 'm', 'Z', 'O', '0', 'Q', 'L', 'C', 'J', 'U', 'Y', 'X', 'z', 'c', 'v', 'u', 'n', 'x', 'r', 'j', 'f', 't', '/', '|', '(', ')', '1', '{', '}', '[', ']', '?', '-', '_', '+', '~', '<', '>', 'i', '!', 'l', 'I', ';', ':', ',', '"', '^', '`', '\'', '.'} // 68 chars
{'@', '%', '=', ':', '.'} // 5 chars
{'█', '▓', '▒', '░'} // 4 chars
*/
char charset[] = {'@', '%', '#', '*', ':', '=', '+', '-', '.'};
const int COLORS = 9;
char ceilC = '`', floorC = '`';

double to_rad(double degree)
{
    double pi = 3.14159265359;
    return (degree * (pi / 180));
}

void normalize_angle(double &angle)
{
    while(angle < 0) angle += 360.0;
    while(angle >= 360) angle -= 360.0;
}

void *listen(void *vargp)
{
    while(1)
    {
        fd_set set;
        struct timeval tv;

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO(&set);
        FD_SET(fileno(stdin), &set);

        int res = select(fileno(stdin) + 1, &set, NULL, NULL, &tv);

        if(res > 0)
        {
            char c;
            read(fileno( stdin ), &c, 1);
            double stepDir = direction;
            double incX, incY;
            switch (c)
            {
            case 'w':
                normalize_angle(stepDir);
                incX = cos(to_rad(stepDir));
                incY = sin(to_rad(stepDir));
                if(!board[long(posX+incX)][long(posY+incY)])
                {
                    posX += incX * MOVEMENT_SPEED;
                    posY += incY * MOVEMENT_SPEED;
                    break;
                }
                break;

            case 's':
                stepDir += 180;
                normalize_angle(stepDir);
                incX = cos(to_rad(stepDir));
                incY = sin(to_rad(stepDir));
                if(!board[long(posX+incX)][long(posY+incY)])
                {
                    posX += incX * MOVEMENT_SPEED;
                    posY += incY * MOVEMENT_SPEED;
                }
                break;

            case 'd':
                stepDir -= 90;
                normalize_angle(stepDir);
                incX = cos(to_rad(stepDir));
                incY = sin(to_rad(stepDir));
                if(!board[long(posX+incX)][long(posY+incY)])
                {
                    posX += incX * MOVEMENT_SPEED;
                    posY += incY * MOVEMENT_SPEED;
                }
                break;

            case 'a':
                stepDir += 90;
                normalize_angle(stepDir);
                incX = cos(to_rad(stepDir));
                incY = sin(to_rad(stepDir));
                if(!board[long(posX+incX)][long(posY+incY)])
                {
                    posX += incX * MOVEMENT_SPEED;
                    posY += incY * MOVEMENT_SPEED;
                }
                break;

            case 'l':
                direction -= ROTATION_SPEED;
                normalize_angle(direction);
                break;

            case 'k':
                direction += ROTATION_SPEED;
                normalize_angle(direction);
                break;
            }
        }
    }
    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    pthread_exit((void*) vargp);
}

void draw()
{
    std::cout << "\033[0;0H";
    for(int y = SCREEN_HEIGHT-1; y >= 0; y--)
    {
        for(int x = 0; x < SCREEN_WIDTH; x++)
        {
            if(screen[x][y] == -1)
            {
                std::cout << ceilC;
            }
            else if(screen[x][y] == -2)
            {
                std::cout << floorC;
            }
            else
            {
                std::cout << charset[screen[x][y]];
            }
        }
        std::cout << "\n";
    }
    std::cout << "(" << posX << ", " << posY << "), " << direction << "                                \n";
    for(int y = MAP_HEIGHT-1; y >= 0; y--)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            if(y == long(posY) && x == long(posX))
                std::cout << '^';
            else if(board[x][y])
                std::cout << '#';
            else
                std::cout << '.';
        }
        std::cout << '\n';
    }
}

void setup()
{
    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr( fileno( stdin ), TCSANOW, &newSettings );

    std::cout << "\x1B[2J\x1B[H";
    std::cout << "\033[0;0H";
}

void *game_loop(void *vargp)
{
    while(1)
    {
        int line = 0;
        double curr_dir = direction;
        double curr_posX = posX, curr_posY = posY;
        for(double degitr = curr_dir + FOV/2; degitr >= curr_dir - FOV/2; degitr--)
        {
            double deg = degitr;
            normalize_angle(deg);
            double currX, currY, fullX, fullY, syf;
            fullX = abs(1 / cos(to_rad(deg)));
            fullY = abs(1 / sin(to_rad(deg)));

            int stepX, stepY;
            if(deg >= 90 && deg < 270)
            {
                currX = abs(modf(curr_posX, &syf) / cos(to_rad(deg)));
                stepX = -1;
            }
            else
            {
                currX = abs((1 - modf(curr_posX, &syf)) / cos(to_rad(deg)));
                stepX = 1;
            }
            if(deg >= 0 && deg < 180)
            {
                currY = abs((1 - modf(curr_posY, &syf)) / sin(to_rad(deg)));
                stepY = 1;
            }
            else
            {
                currY = abs(modf(curr_posY, &syf) / sin(to_rad(deg)));
                stepY = -1;
            }
            int mapX = long(curr_posX), mapY = long(curr_posY);
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
            // dist *= cos(to_rad(abs(deg-curr_dir)));

            int h = SCREEN_HEIGHT / dist;
            int color = COLORS - h * COLORS / SCREEN_HEIGHT;
            color = std::min(color, COLORS-1);
            color = std::max(color, 0);
            int line_beg = std::max(0, SCREEN_HEIGHT / 2 - h / 2);
            int line_end = std::min(SCREEN_HEIGHT / 2 + h / 2, SCREEN_HEIGHT-1);
            if(line+1 >= SCREEN_WIDTH) break;
            for(int i = line_beg; i <= line_end; i++)
            {
                screen[line][i] = color;
                screen[line+1][i] = color;
            }
            line += 2;
        }
        draw();

        for(int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for(int x = 0; x < SCREEN_WIDTH; x++)
            {
                screen[x][y] = -1;
                if(y >= SCREEN_HEIGHT/2) screen[x][y] = -2;
            }
        }
    }

    pthread_exit((void*) vargp);
}

int main()
{
    setup();
    pthread_t display, controls;
    pthread_create(&display, NULL, game_loop, NULL);
    pthread_create(&controls, NULL, listen, NULL);
    pthread_join(display, NULL);
}

