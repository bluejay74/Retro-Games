/*
    Programmer: Andrew Lim, Casey Duncan, Jayden
    Project: Tank Combat Source Code
    Goal: Porting from Atari 2600 to Atari 800 using C
    Compiler: CC65 Cross Compiler
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

//defining the 16 movement positions
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

//different tank pictures to be printed
unsigned int tankPics[16][8] = {
        {8,8,107,127,127,127,99,99}, //NORTH
        {36,100,121,255,255,78,14,4}, //NORTH_15
        {25,58,124,255,223,14,28,24}, //NORTH_EAST
        {28,120,251,124,28,31,62,24}, //NORTH_60
        {0,252,252,56,63,56,252,252}, //EAST
        {24,62,31,28,124,251,120,28}, //EAST_15
        {25,58,124,255,223,14,28,24}, //SOUTH_EAST
        {4,14,78,255,255,121,36}, //EAST_60
        {99,99,127,127,127,107,8,8}, //SOUTH
        {32,112,114,255,255,158,38,36}, //SOUTH_15
        {152,92,62,255,251,112,56,24}, //SOUTH_WEST
        {24,124,248,56,62,223,30,56}, //SOUTH_60
        {0,63,63,28,252,28,63,63}, //WEST
        {56,30,223,62,56,248,124,24}, //WEST_15
        {152,92,62,255,251,112,56,24}, //NORTH_WEST
        {36,38,158,255,255,114,112,32} //WEST_60
};

//  <---------- GLOBAL VARIABLES ----------> 

int i;                      //for loop counter (doesnt matter as i will always be initialized to zero)
int PMBAddress;
int PMBAddress;
int missileAddress;
int playerAddress;
int bitMapAddress;

int *PMA_P1 = (int *)0xD000;
int *PMA_P2 = (int *)0xD001;

int *colLumPM1 = (int *)0x2C0;
int *colLumPM2 = (int *)0x2C1;

unsigned int p1Direction = EAST;
unsigned int p2Direction = WEST;
unsigned char p1LastMove;
unsigned char p2LastMove;

int verticalStartP1= 131;
int verticalStartP2 = 387;
int horizontalStartP1 = 57;
int horizontalStartP2 = 190;

int p1Location = 131;
int p2Location = 387;
int p1Horizontal = 57;
int p2Horizontal = 190;


char key; //char for keyboard input

//  <---------- FUNCTION DECLARATIONS ---------->
//functions to be implemented
void fire( int tank);
void moveForward(int tank);
void moveBackward(int tank);
void checkCollision();
//functions to turn and update tank positions
void turnplayer(unsigned char turn, int player);
void updateplayerDir(int player);



//moving based off of joystick input, or firing the tank if the player chooses
void movePlayers(){
    //joystick code
    //CODE NEEDS TO HAVE UPDATED JOYSTICK METHODS ---------------//
    // joy = joy_read(JOY_1);
    //	if(JOY_BTN_FIRE(joy)){      }
    //
    //unsigned char player1move = joy_read(JOY_1);
    //unsigned char player2move = joy_read(JOY_2);
    //moving player 1
    //if(player1move == JOY_BTN_1) fire(1);
    //else if(player1move == JOY_UP) moveForward(1);
    //else if(player1move == JOY_DOWN) moveBackward(1);
    //else if(player1move == JOY_LEFT || player1move == JOY_RIGHT) turnplayer(player1move, 1);

    //moving player 2
    //if(player2move == JOY_BTN_1) fire(2);
    //else if(player2move == JOY_UP) moveForward(2);
    //else if(player2move == JOY_DOWN) moveBackward(2);
    //else if(player2move == JOY_LEFT || player2move == JOY_RIGHT) turnplayer(player2move, 2);

    //for keyboard testing
    key = cgetc();
    if(key == 'w') moveForward(1);
    if(key == 's') moveBackward(1);
    if(key == 'a') turnplayer('L', 1);
    if(key == 'd') turnplayer('R', 1);
}

//rotating the player
void turnplayer(unsigned char turn, int player){
    //for player 1
    if(player == 1){
        //handling edge cases
        if(p1Direction == WEST_60 && turn == 'R'){
            p1Direction = NORTH;
        }
        else if(p1Direction == NORTH && turn == 'L'){
            p1Direction = WEST_60;
        }
        //if the joystick is left,
        else if(turn == 'L'){
            p1Direction = p1Direction - 1;
        }
        //if the joystick is right
        else if(turn == 'R'){
            p1Direction = p1Direction + 1;
        }
        updateplayerDir(1);
    }
//    //for player 2
//    else if(player == 2){
//        if(p2Direction == WEST_60 && turn == 'R'){
//            p2Direction = NORTH;
//        }
//        else if(p2Direction == NORTH && turn == 'L'){
//            p2Direction = WEST_60;
//        }
//            //if the joystick is left,
//        else if(turn == 'L'){
//            p2Direction = p2Direction - 1;
//        }
//            //if the joystick is right
//        else if(turn == 'R'){
//            p2Direction = p2Direction + 1;
//        }
//        updateplayerDir(2);
//    }
}

//updating the player's orientation, or position
void updateplayerDir(int player){
    //updating player 1
    if(player == 1){
        if(p1Direction == SOUTH_WEST || p1Direction == EAST_SOUTH){
            int counter = 7;
            for(i = p1Location; i < p1Location + 8; i++){
                POKE(playerAddress + i, tankPics[p1Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p1Location; i < p1Location + 8; i++){
                POKE(playerAddress + i, tankPics[p1Direction][counter]);
                counter++;
            }
        }
    }

    //updating player 2
    else if(player == 2){
        if(p2Direction == SOUTH_15 || p2Direction == SOUTH_60 || p2Direction == SOUTH_WEST || p2Direction == EAST_15 || p2Direction == EAST_SOUTH || p2Direction == EAST_60){
            int counter = 7;
            for(i = p2Location; i < p2Location + 8; i++){
                POKE(playerAddress + i, tankPics[p2Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p2Location; i < p2Location + 8; i++){
                POKE(playerAddress + i, tankPics[p2Direction][counter]);
                counter++;
            }
        }
    }

    //checking to see if the movement caused a collision, is it needed here?
    checkCollision();
}

//add a check to the collision registers
void checkCollision(){

}

//move the tank forward
void moveForward(int tank){
    //moving forward tank 1
    if(tank == 1){
        //movement for north
        if(p1Direction == NORTH){
            POKE(playerAddress+(p1Location+7), 0);
            p1Location--;

        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerAddress+p1Location, 0);
            p1Location++;
        }
        //movement north-ish cases
        if(p1Direction == NORTH_15 || p1Direction == NORTH_60 || p1Direction == NORTH_EAST || p1Direction == WEST_15 || p1Direction == WEST_NORTH || p1Direction == WEST_60){
            unsigned int x;
            unsigned int y;

        }
        //movement south-ish cases
        if(p1Direction == SOUTH_15 || p1Direction == SOUTH_60 || p1Direction == SOUTH_WEST || p1Direction == EAST_15 || p1Direction == EAST_SOUTH || p1Direction == EAST_60){
            unsigned int x;
            unsigned int y;

        }
        //movement west
        if(p1Direction == WEST){
            p1Horizontal--;
            POKE(PMA_P1, p1Horizontal);
        }
        //movement east
        if(p1Direction == EAST){
            p1Horizontal++;
            POKE(PMA_P1, p1Horizontal);
        }
        updateplayerDir(1);
    }

    //moving forward tank 2
    else if(tank == 2){

    }
}

//move the tank backward
void moveBackward(int tank){
    //movement for tank 1
    if(tank == 1){
        //movement for north
        if(p1Direction == NORTH){
            POKE(playerAddress+p1Location, 0);
            p1Location++;
        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerAddress+(p1Location+7), 0);
            p1Location--;
        }
        //movement north-ish cases
        if(p1Direction == NORTH_15 || p1Direction == NORTH_60 || p1Direction == NORTH_EAST || p1Direction == WEST_15 || p1Direction == WEST_NORTH || p1Direction == WEST_60){
            unsigned int x;
            unsigned int y;

        }
        //movement south-ish cases
        if(p1Direction == SOUTH_15 || p1Direction == SOUTH_60 || p1Direction == SOUTH_WEST || p1Direction == EAST_15 || p1Direction == EAST_SOUTH || p1Direction == EAST_60){
            unsigned int x;
            unsigned int y;

        }
        //movement west
        if(p1Direction == WEST){
            p1Horizontal++;
            POKE(PMA_P1, p1Horizontal);
        }
        //movement east
        if(p1Direction == EAST){
            p1Horizontal--;
            POKE(PMA_P1, p1Horizontal);
        }
        updateplayerDir(1);
    }

    if(tank == 2){

    }
}

//fire a projectile from the tank
void fire(int tank){

}

/*
    Customizing display list
*/
void rearranging_display_list();
void enablePMG();
void createBitMap();

// <- MAIN DRIVER CALLS ->

int main() {
    //Set default display to graphics 3
    joy_install(joy_stddrv); //install the joystick driver
    _graphics(3);
    rearranging_display_list();
    enablePMG();

    while (true) {
        movePlayers();

    }
    return 0;
}



void rearranging_display_list() {
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

    for (i = 0; i < sizeof(displayList); i++) {
        POKE(DLIST + i, displayList[i]);
    }


    POKE(0x58,224);
    POKE(0X59,156);
    POKE(0X57,2);
    cputsxy(5, 0, "0");
    cputsxy(14, 0, "0");

    bitMapAddress = PEEK(DLIST+7) + PEEK(DLIST+8)*256;

    createBitMap();
}

void createBitMap()
{
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
}

void enablePMG() {
    //Enable DMA
    POKE(0x22F, 62);
    PMBAddress = PEEK(0x6A)-8;
    POKE(0xD407, PMBAddress);
    POKE(0xD01D, 3);

    //Clear up Player Missile Character
    playerAddress = (PMBAddress * 256) + 1024;
    for (i = 0; i < 512; i++) {
        POKE(playerAddress + i, 0);
    }

    //Player 1
    POKE(PMA_P1, 57);
    POKE(colLumPM1,88);
    POKE(playerAddress+131, 0);
    POKE(playerAddress+132, 252);
    POKE(playerAddress+133, 252);
    POKE(playerAddress+134, 56);
    POKE(playerAddress+135, 63);
    POKE(playerAddress+136, 56);
    POKE(playerAddress+137, 252);
    POKE(playerAddress+138, 252);

    //Player 2
    POKE(PMA_P2, 190);
    POKE(colLumPM1, 116);
    POKE(playerAddress+387, 0);
    POKE(playerAddress+388, 63);
    POKE(playerAddress+389, 63);
    POKE(playerAddress+390, 28);
    POKE(playerAddress+391, 252);
    POKE(playerAddress+392, 28);
    POKE(playerAddress+393, 63);
    POKE(playerAddress+394, 63);


    /* Loop until Q is pressed */
//    while ((key = cgetc()) != 't')
//    {
//        switch (key)
//        {
//            //Player 1 Controls
//            case 'w':
//                POKE(playerAddress+(verticalStartP1+7), 0);
//                verticalStartP1--;
//                tank_north_display(verticalStartP1);
//                break;
//            case 'e':
//                horizontalStartP1++;
//                POKE(playerAddress+(verticalStartP1+7), 0);
//                verticalStartP1--;
//                tank_north_east_display(verticalStartP1);
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//            case 'a':
//                tank_west_display(verticalStartP1);
//                horizontalStartP1--;
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//            case 's':
//                POKE(playerAddress+verticalStartP1, 0);
//                verticalStartP1++;
//                tank_south_display(verticalStartP1);
//                break;
//            case 'd':
//                tank_east_display(verticalStartP1);
//                horizontalStartP1++;
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//            case 'q':
//                horizontalStartP1--;
//                POKE(playerAddress+(verticalStartP1+7), 0);
//                verticalStartP1--;
//                tank_north_west_display(verticalStartP1);
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//            case 'z':
//                horizontalStartP1--;
//                POKE(playerAddress+verticalStartP1, 0);
//                verticalStartP1++;
//                tank_south_west_display(verticalStartP1);
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//            case 'c':
//                horizontalStartP1++;
//                POKE(playerAddress+verticalStartP1, 0);
//                verticalStartP1++;
//                tank_south_east_display(verticalStartP1);
//                POKE(PMA_P1, horizontalStartP1);
//                break;
//        }
//    }
}
