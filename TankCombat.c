/* -------------------------------------------------------------------
    Programmer: Andrew Lim, Casey Duncan, Jayden Anderson
    Project: Tank Combat Source Code
    Goal: Porting from Atari 2600 to Atari 800 using C
    Compiler: CC65 Cross Compiler
   -------------------------------------------------------------------
*/

#include <atari.h>
#include <_antic.h>
#include <_atarios.h>
#include <peekpoke.h>
#include <conio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <joystick.h>

//Defining the 16 tank rotations 
#define NORTH               0
#define NORTH_15            1
#define NORTH_EAST          2
#define NORTH_60            3
#define EAST                4
#define EAST_15             5
#define EAST_SOUTH          6
#define EAST_60             7
#define SOUTH               8
#define SOUTH_15            9
#define SOUTH_WEST          10
#define SOUTH_60            11
#define WEST                12
#define WEST_15             13
#define WEST_NORTH          14
#define WEST_60             15


//collision detection definitions, add all registers
#define P1PF                0xD005






#define HITCLR              0xD01E

/*
 * <-------------------- GLOBAL VARIABLES --------------------> 
 */

//Different tank pictures to be printed
unsigned int tankPics[16][8] = {
        {8,8,107,127,127,127,99,99},        //NORTH
        {36,100,121,255,255,78,14,4},       //NORTH_15
        {25,58,124,255,223,14,28,24},       //NORTH_EAST
        {28,120,251,124,28,31,62,24},       //NORTH_60
        {0,252,252,56,63,56,252,252},       //EAST
        {24,62,31,28,124,251,120,28},       //EAST_15
        {25,58,124,255,223,14,28,24},       //SOUTH_EAST
        {4,14,78,255,255,121,36},               //EAST_60
        {99,99,127,127,127,107,8,8},        //SOUTH
        {32,112,114,255,255,158,38,36},     //SOUTH_15
        {152,92,62,255,251,112,56,24},      //SOUTH_WEST
        {24,124,248,56,62,223,30,56},       //SOUTH_60
        {0,63,63,28,252,28,63,63},          //WEST
        {56,30,223,62,56,248,124,24},       //WEST_15
        {152,92,62,255,251,112,56,24},      //NORTH_WEST
        {36,38,158,255,255,114,112,32}      //WEST_60
};

int i;
int frameDelayCounter = 0;
//Adresses
int bitMapAddress;
int PMBaseAddress;
int playerAddress;
int missileAddress;

//Horizontal Positions Registers
int *horizontalRegister_P0 = (int *)0xD000;
int *horizontalRegister_M0 = (int *)0xD004;
int *horizontalRegister_P1 = (int *)0xD001;
int *horizontalRegister_M1 = (int *)0xD005;

//Color-Luminance Registers
int *colLumPM0 = (int *)0x2C0;
int *colLumPM1 = (int *)0x2C1;

//Starting direction of each Players
unsigned int p0Direction = EAST;
unsigned int p1Direction = WEST;
unsigned char p0LastMove;
unsigned char p1LastMove;
unsigned char p0history;
unsigned char p1history;

//Starting vertical and horizontal position of players 
const int verticalStartP0 = 131;
const int horizontalStartP0 = 57;
const int verticalStartP1 = 387;
const int horizontalStartP1 = 190;

//Variables to track vertical and horizontal locations of players
int p0VerticalLocation = 131;
int p0HorizontalLocation = 57;
int p1VerticalLocation = 387;
int p1HorizontalLocation = 190;

//variables for missile tracking
int m0LastHorizontalLocation;
int m0LastVerticalLocation;
int m1LastHorizontalLocation;
int m1LastVerticalLocation;


/*
 * <-------------------- FUNCTION DECLARATIONS --------------------> 
 */
//functions to be implemented
void rearrangingDisplayList();
void initializeScore();
void createBitMap();
void enablePMGraphics();
void setUpTankDisplay();

void movePlayers();
void fire(int tank);
void missileLocationHelper(unsigned int tankDirection, int pLastHorizontalLocation, int pLastVerticalLocation, int tank);
void traverseMissile(unsigned int tankDirection, int mHorizontalLocation, int mVerticalLocation, int tank);
void moveForward(int tank);
void moveBackward(int tank);
void checkCollision();
//functions to turn and update tank positions
void turnplayer(unsigned char turn, int player);
void updateplayerDir(int player);


/*
 * <-------------------- DRIVER MAIN --------------------> 
 */
int main() {
    joy_load_driver(joy_stddrv);        //Load joystick driver
    joy_install(joy_static_stddrv);     //Install joystick driver

    _graphics(18);                      //Set default display to graphics 3 + 16 (+16 displays mode with graphics, eliminating the text window)
    rearrangingDisplayList();           //rearranging graphics 3 display list
    initializeScore();
    createBitMap();
    enablePMGraphics();
    setUpTankDisplay();

    while (true) {
        //Slows down character movement e.g. (60fps/5) = 12moves/second (it is actually slower than this for some reason)
        if (frameDelayCounter == 5){
            movePlayers();
            frameDelayCounter = 0;
        }else{
            frameDelayCounter++;
        }
        checkCollision();
        p1history = p1LastMove; //helps to fix collision bug
        p0history = p0LastMove; //helps to fix collision bug
        waitvsync();
    }
    return 0;
}


/*
 * <-------------------- FUNCTION IMPLEMENTATIONS --------------------> 
 */
void rearrangingDisplayList() {
    unsigned int *DLIST_ADDRESS  = OS.sdlstl + OS.sdlsth*256;
    unsigned char *DLIST = (unsigned char *)DLIST_ADDRESS;

    // Write the Display List for Graphics Mode
    // Must contain a total of 192 Scan Lines for display list to function properly 
    unsigned char displayList[] = {
        DL_BLK8,  // 8 blank lines
        DL_BLK8,
        DL_BLK8,
        DL_LMS(DL_CHR20x16x2), 
        0xE0, 0x9C,  // Text Memory 
        DL_LMS(DL_MAP40x8x4),
        0x70,0x9E,  //Screen memory
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4, 
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4, 
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4, 
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_MAP40x8x4,
        DL_JVB,  // Jump and vertical blank
        0x4E, 0x9E  // Jump address (in this case, loop back to the start)
    };

    int i;

    for (i = 0; i < 30; i++) {
        POKE(DLIST + i, displayList[i]);
    }

    bitMapAddress = PEEK(DLIST+7) + PEEK(DLIST+8)*256;
}

void initializeScore() {
    //Temp code
    POKE(0x58,224);
    POKE(0X59,156);
    POKE(0X57,2);
    cputsxy(5, 0, "0");
    cputsxy(14, 0, "0");
}

void createBitMap() {
    //Making the top and bottom border
    for (i = 0; i < 10; i++)
    {
        POKE(bitMapAddress+i, 170);
        POKE(bitMapAddress+210+i, 170);
    }

    //Making the left border
    for (i = 10; i <= 200; i += 10)
    {
        POKE(bitMapAddress+i, 128);
    }

    //Making the right border
    for (i = 19; i <= 209; i += 10)
    {
        POKE(bitMapAddress+i, 2);
    }

    // <--- Making the sprites --->
    POKE(bitMapAddress+81, 40);
    POKE(bitMapAddress+131, 40);
    for (i = 91; i <= 121; i += 10)
    {
        POKE(bitMapAddress+i, 8);
    }

    POKE(bitMapAddress+88, 40);
    POKE(bitMapAddress+138, 40);
    for (i = 98; i <= 128; i += 10) 
    {
        POKE(bitMapAddress+i, 32);
    }

    for (i = 54; i <= 74; i += 10)
    {
        POKE(bitMapAddress+i, 2);
    }

    for (i = 55; i <= 75; i += 10) 
    {
        POKE(bitMapAddress+i, 128);
    }

    for (i = 144; i <= 164; i += 10) 
    {
        POKE(bitMapAddress+i, 2);
    }

    for (i = 145; i <= 165; i += 10) 
    {
        POKE(bitMapAddress+i, 128);
    }

    POKE(bitMapAddress+103, 168);
    POKE(bitMapAddress+113, 168);
    POKE(bitMapAddress+106, 42);
    POKE(bitMapAddress+116, 42);

    POKE(0x2C5, 26);    //Sets bitmap color to yellow
}

void enablePMGraphics() {
    POKE(0x22F, 62);                    //Enable Player-Missile DMA single line
    //PMBaseAddress = PEEK(0x6A)-8;       //Get Player-Missile base address
    PMBaseAddress = 0x2800;
    POKE(0xD407, PMBaseAddress);        //Store Player-Missile base address in base register
    POKE(0xD01D, 3);                    //Enable Player-Missile DMA

    //Clear up Player Missile Character
    playerAddress = (PMBaseAddress * 256) + 1024;
    missileAddress = (PMBaseAddress * 256) + 768;
    
    //Clear up default built-in characters in Player's address
    for (i = 0; i < 512; i++) {
        POKE(playerAddress + i, 0);

        if (i <= 256)
        {
            POKE(missileAddress + i, 0);
        }
    }
}

void setUpTankDisplay() {
    int counter = 0;

    //Set up player 0 tank 
    POKE(horizontalRegister_P0, 57);
    POKE(colLumPM0, 196);

    for (i = 131; i < 139; i++) {
        POKE(playerAddress+i, tankPics[EAST][counter]);
        counter++;
    }
    counter = 0;

    //Set up player 1 tank
    POKE(horizontalRegister_P1, 190);
    POKE(colLumPM1, 116);

    for (i = 387; i < 395; i++) {
        POKE(playerAddress+i, tankPics[WEST][counter]);
        counter++;
    }
    counter = 0;
}

//moving based off of joystick input, or firing the tank if the player chooses
void movePlayers(){
    //joystick code
    unsigned char player1move = joy_read(JOY_1);
    unsigned char player2move = joy_read(JOY_2);
    p0LastMove = player1move;
    p1LastMove = player2move;

    if(JOY_BTN_1(player1move)) fire(1);
    else if(JOY_UP(player1move)) moveForward(1);
    else if(JOY_DOWN(player1move)) moveBackward(1);
    else if(JOY_LEFT(player1move) || JOY_RIGHT(player1move)) turnplayer(player1move, 1);

    //moving player 2
    if(JOY_BTN_1(player2move)) fire(2);
    else if(JOY_UP(player2move)) moveForward(2);
    else if(JOY_DOWN(player2move)) moveBackward(2);
    else if(JOY_LEFT(player2move) || JOY_RIGHT(player2move)) turnplayer(player2move, 2);
}

//rotating the player
void turnplayer(unsigned char turn, int player){
    //for player 1
    if(player == 1){
        //handling edge cases
        if(p0Direction == WEST_60 && JOY_RIGHT(turn)){
            p0Direction = NORTH;
        }
        else if(p0Direction == NORTH && JOY_LEFT(turn)){
            p0Direction = WEST_60;
        }
        //if the joystick is left,
        else if(JOY_LEFT(turn)){
            p0Direction = p0Direction - 1;
        }
        //if the joystick is right
        else if(JOY_RIGHT(turn)){
            p0Direction = p0Direction + 1;
        }
        updateplayerDir(1);
    }
    //for player 2
    else if(player == 2){
        if(p1Direction == WEST_60 && JOY_RIGHT(turn)){
            p1Direction = NORTH;
        }
        else if(p1Direction == NORTH && JOY_LEFT(turn)){
            p1Direction = WEST_60;
        }
            //if the joystick is left,
        else if(JOY_LEFT(turn)){
            p1Direction = p1Direction - 1;
        }
            //if the joystick is right
        else if(JOY_RIGHT(turn)){
            p1Direction = p1Direction + 1;
        }
        updateplayerDir(2);
    }
}

//updating the player's orientation, or position
void updateplayerDir(int player){
    //updating player 1
    if(player == 1){
        if(p0Direction == SOUTH_WEST || p0Direction == EAST_SOUTH){
            int counter = 7;
            for(i = p0VerticalLocation; i < p0VerticalLocation + 8; i++){
                POKE(playerAddress + i, tankPics[p0Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p0VerticalLocation; i < p0VerticalLocation + 8; i++){
                POKE(playerAddress + i, tankPics[p0Direction][counter]);
                counter++;
            }
        }
    }

    //updating player 2
    else if(player == 2){
        if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH){
            int counter = 7;
            for(i = p1VerticalLocation; i < p1VerticalLocation + 8; i++){
                POKE(playerAddress + i, tankPics[p1Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p1VerticalLocation; i < p1VerticalLocation + 8; i++){
                POKE(playerAddress + i, tankPics[p1Direction][counter]);
                counter++;
            }
        }
    }

    //checking to see if the movement caused a collision, is it needed here?
    //checkCollision();
}

//move the tank forward
void moveForward(int tank){
    //moving forward tank 1--------------------------------
    if(tank == 1){
        //movement for north
        if(p0Direction == NORTH){
            POKE(playerAddress+(p0VerticalLocation+7), 0);
            p0VerticalLocation--;

        }
        //movement for south
        if(p0Direction == SOUTH){
            POKE(playerAddress+p0VerticalLocation, 0);
            p0VerticalLocation++;
        }
        //movement north-ish cases
        if(p0Direction == NORTH_15 || p0Direction == NORTH_60 || p0Direction == NORTH_EAST || p0Direction == WEST_15 || p0Direction == WEST_NORTH || p0Direction == WEST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p0Direction == NORTH_EAST || p0Direction == WEST_NORTH || p0Direction == NORTH_15 || p0Direction == WEST_60) x = 1;
            if(p0Direction == NORTH_60 || p0Direction == WEST_15) x = 2;
            //Y ifs
            if(p0Direction == NORTH_EAST || p0Direction == WEST_NORTH || p0Direction == NORTH_60 || p0Direction == WEST_15) y = 1;
            if(p0Direction == NORTH_15 || p0Direction == WEST_60) y = 2;
            if(p0Direction < 4) p0HorizontalLocation = p0HorizontalLocation + x;
            else p0HorizontalLocation = p0HorizontalLocation - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p0VerticalLocation +7), 0);
            POKE(playerAddress+(p0VerticalLocation +6), 0);
            p0VerticalLocation = p0VerticalLocation - y;
            //updateplayerDir(1);
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        //movement south-ish cases
        if(p0Direction == SOUTH_15 || p0Direction == SOUTH_60 || p0Direction == SOUTH_WEST || p0Direction == EAST_15 || p0Direction == EAST_SOUTH || p0Direction == EAST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p0Direction == SOUTH_WEST || p0Direction == EAST_SOUTH || p0Direction == SOUTH_15 || p0Direction == EAST_60) x = 1;
            if(p0Direction == SOUTH_60 || p0Direction == EAST_15) x = 2;
            //Y ifs
            if(p0Direction == SOUTH_WEST || p0Direction == EAST_SOUTH || p0Direction == SOUTH_60 || p0Direction == EAST_15) y = 1;
            if(p0Direction == SOUTH_15 || p0Direction == EAST_60) y = 2;
            if(p0Direction < 8) p0HorizontalLocation = p0HorizontalLocation + x;
            else p0HorizontalLocation = p0HorizontalLocation - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p0VerticalLocation), 0);
            POKE(playerAddress+(p0VerticalLocation +1), 0);
            p0VerticalLocation = p0VerticalLocation + y;
            //updateplayerDir(1);
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        //movement west
        if(p0Direction == WEST){
            p0HorizontalLocation--;
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        //movement east
        if(p0Direction == EAST){
            p0HorizontalLocation++;
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        updateplayerDir(1);
    }



    //moving forward tank 2------------------------------
    else if(tank == 2){
        //movement for north
        if(p1Direction == NORTH){
            POKE(playerAddress+(p1VerticalLocation+7), 0);
            p1VerticalLocation--;

        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerAddress+p1VerticalLocation, 0);
            p1VerticalLocation++;
        }
        //movement north-ish cases
        if(p1Direction == NORTH_15 || p1Direction == NORTH_60 || p1Direction == NORTH_EAST || p1Direction == WEST_15 || p1Direction == WEST_NORTH || p1Direction == WEST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p1Direction == NORTH_EAST || p1Direction == WEST_NORTH || p1Direction == NORTH_15 || p1Direction == WEST_60) x = 1;
            if(p1Direction == NORTH_60 || p1Direction == WEST_15) x = 2;
            //Y ifs
            if(p1Direction == NORTH_EAST || p1Direction == WEST_NORTH || p1Direction == NORTH_60 || p1Direction == WEST_15) y = 1;
            if(p1Direction == NORTH_15 || p1Direction == WEST_60) y = 2;
            if(p1Direction < 4) p1HorizontalLocation = p1HorizontalLocation + x;
            else p1HorizontalLocation = p1HorizontalLocation - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p1VerticalLocation +7), 0);
            POKE(playerAddress+(p1VerticalLocation +6), 0);
            p1VerticalLocation = p1VerticalLocation - y;
            //updateplayerDir(2);
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        //movement south-ish cases
        if(p1Direction == SOUTH_15 || p1Direction == SOUTH_60 || p1Direction == SOUTH_WEST || p1Direction == EAST_15 || p1Direction == EAST_SOUTH || p1Direction == EAST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH || p1Direction == SOUTH_15 || p1Direction == EAST_60) x = 1;
            if(p1Direction == SOUTH_60 || p1Direction == EAST_15) x = 2;
            //Y ifs
            if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH || p1Direction == SOUTH_60 || p1Direction == EAST_15) y = 1;
            if(p1Direction == SOUTH_15 || p1Direction == EAST_60) y = 2;
            if(p1Direction < 8) p1HorizontalLocation = p1HorizontalLocation + x;
            else p1HorizontalLocation = p1HorizontalLocation - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p1VerticalLocation), 0);
            POKE(playerAddress+(p1VerticalLocation +1), 0);
            p1VerticalLocation = p1VerticalLocation + y;
            //updateplayerDir(1);
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        //movement west
        if(p1Direction == WEST){
            p1HorizontalLocation--;
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        //movement east
        if(p1Direction == EAST){
            p1HorizontalLocation++;
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        updateplayerDir(2);
    }
}



//move the tank backward
void moveBackward(int tank){
    //moving backward tank 1-------------------------------
    if(tank == 1){
        //movement for north
        if(p0Direction == NORTH){
            POKE(playerAddress+p0VerticalLocation, 0);
            p0VerticalLocation++;
        }
        //movement for south
        if(p0Direction == SOUTH){
            POKE(playerAddress+(p0VerticalLocation+7), 0);
            p0VerticalLocation--;
        }
        //movement north-ish cases
        if(p0Direction == NORTH_15 || p0Direction == NORTH_60 || p0Direction == NORTH_EAST || p0Direction == WEST_15 || p0Direction == WEST_NORTH || p0Direction == WEST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p0Direction == NORTH_EAST || p0Direction == WEST_NORTH || p0Direction == NORTH_15 || p0Direction == WEST_60) x = 1;
            if(p0Direction == NORTH_60 || p0Direction == WEST_15) x = 2;
            //Y ifs
            if(p0Direction == NORTH_EAST || p0Direction == WEST_NORTH || p0Direction == NORTH_60 || p0Direction == WEST_15) y = 1;
            if(p0Direction == NORTH_15 || p0Direction == WEST_60) y = 2;
            if(p0Direction > 4) p0HorizontalLocation = p0HorizontalLocation + x;
            else p0HorizontalLocation = p0HorizontalLocation - x;
            //x = -2, -1, 1, 2
            //y = 2, 1

            POKE(playerAddress+(p0VerticalLocation), 0);
            POKE(playerAddress+(p0VerticalLocation + 1), 0);
            p0VerticalLocation = p0VerticalLocation + y;
            //updateplayerDir(1);
            POKE(horizontalRegister_P0, p0HorizontalLocation);

        }
        //movement south-ish cases
        if(p0Direction == SOUTH_15 || p0Direction == SOUTH_60 || p0Direction == SOUTH_WEST || p0Direction == EAST_15 || p0Direction == EAST_SOUTH || p0Direction == EAST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p0Direction == SOUTH_WEST || p0Direction == EAST_SOUTH || p0Direction == SOUTH_15 || p0Direction == EAST_60) x = 1;
            if(p0Direction == SOUTH_60 || p0Direction == EAST_15) x = 2;
            //Y ifs
            if(p0Direction == SOUTH_WEST || p0Direction == EAST_SOUTH || p0Direction == SOUTH_60 || p0Direction == EAST_15) y = 1;
            if(p0Direction == SOUTH_15 || p0Direction == EAST_60) y = 2;
            if(p0Direction < 8) p0HorizontalLocation = p0HorizontalLocation - x;
            else p0HorizontalLocation = p0HorizontalLocation + x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p0VerticalLocation + 7), 0);
            POKE(playerAddress+(p0VerticalLocation +6), 0);
            p0VerticalLocation = p0VerticalLocation - y;
            //updateplayerDir(1);
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        //movement west
        if(p0Direction == WEST){
            p0HorizontalLocation++;
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        //movement east
        if(p0Direction == EAST){
            p0HorizontalLocation--;
            POKE(horizontalRegister_P0, p0HorizontalLocation);
        }
        updateplayerDir(1);
    }


    //moving backward tank 2-------------------------------
    if(tank == 2){
        //movement for north
        if(p1Direction == NORTH){
            POKE(playerAddress+p1VerticalLocation, 0);
            p1VerticalLocation++;
        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerAddress+(p1VerticalLocation+7), 0);
            p1VerticalLocation--;
        }
        //movement north-ish cases
        if(p1Direction == NORTH_15 || p1Direction == NORTH_60 || p1Direction == NORTH_EAST || p1Direction == WEST_15 || p1Direction == WEST_NORTH || p1Direction == WEST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p1Direction == NORTH_EAST || p1Direction == WEST_NORTH || p1Direction == NORTH_15 || p1Direction == WEST_60) x = 1;
            if(p1Direction == NORTH_60 || p1Direction == WEST_15) x = 2;
            //Y ifs
            if(p1Direction == NORTH_EAST || p1Direction == WEST_NORTH || p1Direction == NORTH_60 || p1Direction == WEST_15) y = 1;
            if(p1Direction == NORTH_15 || p1Direction == WEST_60) y = 2;
            if(p1Direction > 4) p1HorizontalLocation = p1HorizontalLocation + x;
            else p1HorizontalLocation = p1HorizontalLocation - x;
            //x = -2, -1, 1, 2
            //y = 2, 1

            POKE(playerAddress+(p1VerticalLocation), 0);
            POKE(playerAddress+(p1VerticalLocation + 1), 0);
            p1VerticalLocation = p1VerticalLocation + y;
            //updateplayerDir(2);
            POKE(horizontalRegister_P1, p1HorizontalLocation);

        }
        //movement south-ish cases
        if(p1Direction == SOUTH_15 || p1Direction == SOUTH_60 || p1Direction == SOUTH_WEST || p1Direction == EAST_15 || p1Direction == EAST_SOUTH || p1Direction == EAST_60){
            int x = 0;
            int y = 0;
            //X ifs
            if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH || p1Direction == SOUTH_15 || p1Direction == EAST_60) x = 1;
            if(p1Direction == SOUTH_60 || p1Direction == EAST_15) x = 2;
            //Y ifs
            if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH || p1Direction == SOUTH_60 || p1Direction == EAST_15) y = 1;
            if(p1Direction == SOUTH_15 || p1Direction == EAST_60) y = 2;
            if(p1Direction < 8) p1HorizontalLocation = p1HorizontalLocation - x;
            else p1HorizontalLocation = p1HorizontalLocation + x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerAddress+(p1VerticalLocation + 7), 0);
            POKE(playerAddress+(p1VerticalLocation +6), 0);
            p1VerticalLocation = p1VerticalLocation - y;
            //updateplayerDir(2);
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        //movement west
        if(p1Direction == WEST){
            p1HorizontalLocation++;
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        //movement east
        if(p1Direction == EAST){
            p1HorizontalLocation--;
            POKE(horizontalRegister_P1, p1HorizontalLocation);
        }
        updateplayerDir(2);
    }
}

//add a check to the collision registers, and act if they're triggered (not finished)
void checkCollision(){
    if(PEEK(P1PF) != 0x0000){
        if(JOY_UP(p1history)){
            moveBackward(2);
            moveBackward(2);
            moveBackward(2);
            moveBackward(2);
        }
        else if(JOY_DOWN(p1history)){
            moveForward(2);
            moveForward(2);
            moveForward(2);
            moveForward(2);
        }
    }
    POKE(HITCLR, 1); // Clear ALL of the Collision Registers
}

//fire a projectile from the tank
void fire(int tank){
    if (tank == 0)
    {
        POKE(missileAddress+m0LastVerticalLocation, 0);
        missileLocationHelper(p0Direction, p0HorizontalLocation, p0VerticalLocation, tank);
        POKE(horizontalRegister_M0, m0LastHorizontalLocation);
        POKE(missileAddress+m0LastVerticalLocation, 2);
        //traverseMissile(p0Direction, m0LastHorizontalLocation, m0LastVerticalLocation, tank);
    }
    else if (tank == 1)
    {
        POKE(missileAddress+m1LastVerticalLocation, 0);
        missileLocationHelper(p1Direction, p1HorizontalLocation, p1VerticalLocation, tank);
        POKE(horizontalRegister_M1, m1LastHorizontalLocation);
        POKE(missileAddress+m1LastVerticalLocation, 8);
        //traverseMissile(p1Direction, m1LastHorizontalLocation, m1LastVerticalLocation, 1);
    }
}

//This fucntion is just to determine where the missile should be poked at. It tracks the locations of the tank and
//sets up the missile to be located at the tank's barrel
void missileLocationHelper(unsigned int tankDirection, int pHorizontalLocation, int pVerticalLocation, int tank)
{ 
    int mLastHorizontalLocation = 0;
    int mLastVerticalLocation = 0;

    if (tankDirection == NORTH)
    {
        mLastHorizontalLocation = pHorizontalLocation+4;
        mLastVerticalLocation = pVerticalLocation;
    }
    else if (tankDirection == NORTH_15)
    {
        mLastHorizontalLocation = pHorizontalLocation+5;
        mLastVerticalLocation = pVerticalLocation;
    }
    else if (tankDirection == NORTH_EAST)
    {
        mLastHorizontalLocation = pHorizontalLocation+7;
        mLastVerticalLocation = pVerticalLocation;
    }
    else if (tankDirection == NORTH_60)
    {
        mLastHorizontalLocation = pHorizontalLocation+7;
        mLastVerticalLocation = pVerticalLocation+2;
    }
    else if (tankDirection == EAST)
    {
        mLastHorizontalLocation = pHorizontalLocation+7;
        mLastVerticalLocation = pVerticalLocation+4;
    }
    else if (tankDirection == EAST_15)
    {
        mLastHorizontalLocation = pHorizontalLocation+7;
        mLastVerticalLocation = pVerticalLocation+5;
    }
    else if (tankDirection == EAST_SOUTH)
    {
        mLastHorizontalLocation = pHorizontalLocation+7;
        mLastVerticalLocation = pVerticalLocation+7;
    }
    else if (tankDirection == EAST_60)
    {
        mLastHorizontalLocation = pHorizontalLocation+5;
        mLastVerticalLocation = pVerticalLocation+7;
    }
    else if (tankDirection == SOUTH)
    {
        mLastHorizontalLocation = pHorizontalLocation+4;
        mLastVerticalLocation = pVerticalLocation+7;
    }
    else if (tankDirection == SOUTH_15)
    {
        mLastHorizontalLocation = pHorizontalLocation+2;
        mLastVerticalLocation = pVerticalLocation+7;
    }
    else if (tankDirection == SOUTH_WEST)
    {
        mLastHorizontalLocation = pHorizontalLocation;
        mLastVerticalLocation = pVerticalLocation+7;
    }
    else if (tankDirection == SOUTH_60)
    {
        mLastHorizontalLocation = pHorizontalLocation;
        mLastVerticalLocation = pVerticalLocation+5;
    }
    else if (tankDirection == WEST)
    {
        mLastHorizontalLocation = pHorizontalLocation;
        mLastVerticalLocation = pVerticalLocation+4;
    }
    else if (tankDirection == WEST_15)
    {
        mLastHorizontalLocation = pHorizontalLocation;
        mLastVerticalLocation = pVerticalLocation+2;
    }
    else if (tankDirection == WEST_NORTH)
    {
        mLastHorizontalLocation = pHorizontalLocation;
        mLastVerticalLocation = pVerticalLocation;
    }
    else if (tankDirection == WEST_60)
    {
        mLastHorizontalLocation = pHorizontalLocation+2;
        mLastVerticalLocation = pVerticalLocation+2;
    }

    //Update missile location based on which player is being passed in
    if (tank == 0)
    {
        m0LastHorizontalLocation = mLastHorizontalLocation;
        m0LastVerticalLocation = mLastVerticalLocation;
    }
    else if (tank == 1)
    {
        m1LastHorizontalLocation = mLastHorizontalLocation;
        m1LastVerticalLocation = mLastVerticalLocation - 256;
    }      
}

//This function is to poke and start the animation, moving the missile until collision is true
void traverseMissile(unsigned int tankDirection, int mHorizontalLocation, int mVerticalLocation, int tank)
{
    int counter = 0;

    while (counter <= 75) // Change While condition so that it breaks when collision is true
    {
        counter++;
        
        POKE(missileAddress+mVerticalLocation, 0);

        if (tankDirection == NORTH)
        {
            mVerticalLocation--;
        }
        else if (tankDirection == NORTH_15)
        {
            mVerticalLocation -= 2;
            mHorizontalLocation++;
        } 
        else if (tankDirection == NORTH_EAST)
        {
            mVerticalLocation--;
            mHorizontalLocation++;              
        }
        else if (tankDirection == NORTH_60)
        {
            mVerticalLocation--;
            mHorizontalLocation += 2;
        } 
        else if (tankDirection == EAST)
        {
            mHorizontalLocation++;
        } 
        else if (tankDirection == EAST_15)
        {
            mVerticalLocation++;
            mHorizontalLocation += 2;
        }
        else if (tankDirection == EAST_SOUTH)
        {
            mVerticalLocation++;
            mHorizontalLocation++;
        }
        else if (tankDirection == EAST_60)
        {
            mVerticalLocation += 2;
            mHorizontalLocation++;
        }
        else if (tankDirection == SOUTH)
        {
            mVerticalLocation++;
        }
        else if (tankDirection == SOUTH_15)
        {
            mVerticalLocation += 2;
            mHorizontalLocation--;
        }
        else if (tankDirection == SOUTH_WEST)
        {
            mVerticalLocation++;
            mHorizontalLocation--;
        }
        else if (tankDirection == SOUTH_60)
        {
            mVerticalLocation++;
            mHorizontalLocation -= 2;
        }
        else if (tankDirection == WEST)
        {
            mHorizontalLocation--;
        }
        else if (tankDirection == WEST_15)
        {
            mVerticalLocation--;
            mHorizontalLocation -= 2;
        }
        else if (tankDirection == WEST_NORTH)
        {
            mVerticalLocation--;
            mHorizontalLocation--;
        }
        else if (tankDirection == WEST_60)
        {
            mVerticalLocation -= 2;
            mHorizontalLocation--;
        }


        if (tank == 0)
        {
            POKE(horizontalRegister_M0, mHorizontalLocation);
            POKE(missileAddress+mVerticalLocation, 2);
        }
        else if (tank == 1)
        {
            POKE(horizontalRegister_M1, mHorizontalLocation);
            POKE(missileAddress+mVerticalLocation, 8);
        }

        for (i = 0; i < 1000; i++) {
            // This loop does nothing and consumes time slows down the missile
        }
    }

    //While loop breaks when collision is true. If there is collision you would want to make sure that missile would not appear anymore
    POKE(missileAddress+mVerticalLocation, 0);
}
