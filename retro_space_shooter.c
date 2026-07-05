#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Linux special
#include <string.h>
#include <termios.h>  //Linux special
#include <sys/select.h> //Linux special
#include <time.h>

//constant values for display size and bullet + enemy max rendering
#define W 60
#define H 30
#define MB 20
#define ME 15

//data sets for player bullets and enemy
typedef struct { int x, y, a; } Bullet;
typedef struct { int x, y, t, a; } Enemy;

//global variables
int px, py, hp, score, lvl, st, step;
Bullet b[MB];
Enemy e[ME];
struct termios ot;

//Linux terminal stuff... (clearing the terminal, setting it up for the game)
void restoreTerm() { tcsetattr(0, TCSANOW, &ot); printf("\033[?25h"); } //resets the terminal
void setupTerm() {
    struct termios r;
    tcgetattr(0, &ot); r = ot;
    r.c_lflag &= ~(ICANON | ECHO); 
    r.c_cc[VMIN] = 0; r.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &r);
    atexit(restoreTerm);
    printf("\033[?25l");
}

//user's input detection and action as the input suggests function
int keyHit() {
    struct timeval tv = {0, 0};
    fd_set f; FD_ZERO(&f); FD_SET(0, &f);
    select(1, &f, 0, 0, &tv);
    return FD_ISSET(0, &f);
}
char getKey() { char c = 0; read(0, &c, 1); return c; }

//enemy displaying and enemy score determing functions
char eChar(int t) { return t == 0 ? 'O' : t == 1 ? 'T' : t == 2 ? 'V' : t == 3 ? 'X' : 'Y'; }
int eScore(int t) { return t + 1; } //enemy's score is it's order number...

//display reseting function... important for restaring the game...
void reset() {
    int i;
    px = W / 2; py = H - 2; hp = 100; score = 0; lvl = 1; st = 0; step = 0;
    for (i = 0; i < MB; i++) b[i].a = 0;
    for (i = 0; i < ME; i++) e[i].a = 0;
}

//player's shooting function
void shoot() {
    int i;
    for (i = 0; i < MB; i++) if (!b[i].a) { b[i].x = px; b[i].y = py - 1; b[i].a = 1; break; }
}

//enemy spawing function
void spawn() {
    int i;
    for (i = 0; i < ME; i++) if (!e[i].a) {
        e[i].x = rand() % (W - 2) + 1; e[i].y = 0;
        e[i].t = rand() % 5; e[i].a = 1;
        break;
    }
}

//game displaying function
void draw() {
    static char buf[1 << 16];
    char s[H][W];
    int p = 0;
    int x, y, i;

    //drawing game border
    for (y = 0; y < H; y++) 
        for (x = 0; x < W; x++)
            s[y][x] = (x == 0 || x == W - 1 || y == 0 || y == H - 1) ? '#' : ' ';
    
    //drawing bullets
    for (i = 0; i < MB; i++)
        if (b[i].a && b[i].y >= 0 && b[i].y < H && b[i].x >= 0 && b[i].x < W)
            s[b[i].y][b[i].x] = '|';

    //drawing enemies
    for (i = 0; i < ME; i++)
        if (e[i].a && e[i].y >= 0 && e[i].y < H && e[i].x >= 0 && e[i].x < W)
            s[e[i].y][e[i].x] = eChar(e[i].t);

    //drawing the player
    if (px >= 0 && px < W && py >= 0 && py < H)
        s[py][px] = 'A';

    for (y = 0; y < H; y++) {
        p += sprintf(buf + p, "\033[%d;1H", y + 1);
        for (x = 0; x < W; x++)
            buf[p++] = s[y][x];
    }
    //drawing the HUD
    p += sprintf(buf + p, "\033[%d;1HHP:%3d  Score:%3d  Level:%2d   ",
                 H + 1, hp, score, lvl > 10 ? 10 : lvl); //capping the level at 10 because the game will end if and only if the player dies!!!

    fwrite(buf, 1, p, stdout);
    fflush(stdout);
}

//main game engine
void update() {
    int i, j, sl, sr;
    step++;

    //bullet moving upwards
    for (i = 0; i < MB; i++) if (b[i].a) { b[i].y--; if (b[i].y <= 0) b[i].a = 0; }

    //capping enemy difficulty level at level 10 because the game doesn't end at level 10... game difficulty increases each level using sr...
    sl = lvl > 10 ? 10 : lvl;

    //sr = spawn rate... lower sr means more enemy spawing harder game... to not to make the game difficult the the sr is capped at 3...
    st++; sr = 12 - sl; if (sr < 3) sr = 3;

    //st = is a counter when (st >= st) new enemy spawns and counter restarts 
    if (st >= sr) { st = 0; spawn(); }

    //moving enemy downwards
    for (i = 0; i < ME; i++) {
        if (!e[i].a) continue;
        if (step % 2 == 0) e[i].y++;
        if (e[i].y >= H - 1) { e[i].a = 0; } //preventing getting out of screen
        else if (abs(e[i].x - px) <= 1 && abs(e[i].y - py) <= 1) {
            e[i].a = 0; hp -= 10;
        } //checking if enemy hit player... if so removing enemy and deducting 10 hp from player... (enemy collision hp drop control)
    }

    //bullet control and enemy elemination score determining and adding to player score... 
    for (i = 0; i < MB; i++) {
        if (!b[i].a) continue;
        for (j = 0; j < ME; j++)
            if (e[j].a && b[i].y == e[j].y && b[i].x == e[j].x) {
                b[i].a = 0; e[j].a = 0; score += eScore(e[j].t); //enemy elemination score is equivalant to enemy serial number
                break;
            }
    }
    lvl = score / 10 + 1; //10 score required to progress to the next level
    if (hp < 0) hp = 0;
}

//game over screen display fuction
int gameOver() {
    static char buf[256];
    int p = 0;
    p += sprintf(buf + p, "\033[2J\033[%d;%dHGAME OVER", H / 2, W / 2 - 4);
    p += sprintf(buf + p, "\033[%d;%dHFinal Score: %d", H / 2 + 1, W / 2 - 7, score);
    p += sprintf(buf + p, "\033[%d;%dHPress R to Retry or Q to Quit", H / 2 + 2, W / 2 - 14);
    fwrite(buf, 1, p, stdout);
    fflush(stdout);
    while (1) {
        if (keyHit()) {
            char c = getKey();
            if (c == 'r' || c == 'R') return 1;
            if (c == 'q' || c == 'Q') return 0;
        }
        usleep(20000);
    }
}

//assembling and running the entire game
int main() {
    srand(time(0));
    setupTerm();
    reset();
    while (1) {
        printf("\033[2J");
        while (hp > 0) {

            //main game control 
            if (keyHit()) {
                char k = getKey();
                if (k == 'a' || k == 'A') { if (px > 1) px -= 2; }
                else if (k == 'd' || k == 'D') { if (px < W - 2) px += 2; }
                else if (k == 'w' || k == 'W') { if (py > 2) py -= 2; }
                else if (k == 's' || k == 'S') { if (py < H - 3) py += 2; }
                else if (k == ' ') shoot();
                else if (k == 'q' || k == 'Q') return 0;
            }
            update(); draw(); usleep(50000);
        }
        if (gameOver()) reset(); else break;
    }
    return 0;
}
