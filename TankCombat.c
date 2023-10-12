/*
    Programmer: Andrew Lim
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

//defining the 16 movement positions
#define NORTH               0x0000
#define NORTH_15            0x0001
#define NORTH_EAST          0x0010
#define NORTH_60            0x0011
#define EAST                0x0100
#define EAST_15             0x0101
#define EAST_SOUTH          0x0110
#define EAST_60             0x0111
#define SOUTH               0x1000
#define SOUTH_15            0x1001
#define SOUTH_WEST          0x1010
#define SOUTH_60            0x1011
#define WEST                0x1100
#define WEST_15             0x1101
#define WEST_NORTH          0x1110
#define WEST_60             0x1111

//different tank pictures to be printed
unsigned int tankPics[16][8] = {
        {8,8,107,127,127,127,99,99}, //NORTH
        {},
        {25,58,124,255,223,14,28,24}, //NORTH_EAST
        {},
        {0,252,252,56,63,56,252,252}, //EAST
        {},
        {25,58,124,255,223,14,28,24}, //SOUTH_EAST
        {},
        {99,99,127,127,127,107,8,8}, //SOUTH
        {},
        {152,92,62,255,251,112,56,24}, //SOUTH_WEST
        {},
        {0,63,63,28,252,28,63,63}, //WEST
        {},
        {152,92,62,255,251,112,56,24}, //NORTH_WEST
        {}
};

//  <---------- GLOBAL VARIABLES ----------> 

int i;                      //for loop counter (doesnt matter as i will always be initialized to zero)
int I;
int playerMissileAddress;

int *PMA_P0 = (int *)0xD000;
int *PMA_P1 = (int *)0xD001;

int *colLumPM0 = (int *)0x2C0;
int *colLumPM1 = (int *)0x2C1;

int horizontalStartP0 = 57;
int horizontalStartP1 = 190;

unsigned char p1Direction = EAST;
unsigned char p2Direction = WEST;
unsigned char p1LastMove;

int verticalStartP0 = 137;
int verticalStartP1 = 393;

extern char joy_driver; //an external char used to grab the joystick drivers

//char key;

//  <---------- FUNCTION DECLARATIONS ---------->  
/*
    For display direction of tanks
    1. North
    2. North East
    3. East
    4. South East
    5. South
    6. South West
    7. West
    8. North West
*/

//void tank_north_display(int location);
//void tank_north_east_display(int location);
//void tank_east_display(int location);
//void tank_south_east_display(int location);
//void tank_south_display(int location);
//void tank_south_west_display(int location);
//void tank_west_display(int location);
//void tank_north_west_display(int location);


//functions to be implemented
void fire( int tank);
void moveForward(int tank);
void moveBackward(int tank);
void turnplayer(unsigned char turn, int player);

//moving based off of joystick input, or firing the tank if the player chooses
void movePlayers(){
    unsigned char player1move = joy_read(JOY_1);
    unsigned char player2move = joy_read(JOY_2);
    //moving player 1
    if(player1move == JOY_BTN_1) fire(1);
    else if(player1move == JOY_UP) moveForward(1);
    else if(player1move == JOY_DOWN) moveBackward(1);
    else if(player1move == JOY_LEFT || player1move == JOY_RIGHT) turnplayer(player1move, 1);
    //moving player 2
    if(player2move == JOY_BTN_1) fire(2);
    else if(player2move == JOY_UP) moveForward(2);
    else if(player2move == JOY_DOWN) moveBackward(2);
    else if(player2move == JOY_LEFT || player2move == JOY_RIGHT) turnplayer(player2move, 2);
}

void turnplayer(unsigned char turn, int player){
    //for player 1
    if(player == 1){
        //handling edge cases
        if(p1Direction == WEST_60 && turn == JOY_RIGHT){
            p1Direction = NORTH;
        }
        else if(p1Direction == NORTH && turn == JOY_LEFT){
            p1Direction = WEST_60;
        }
        //if the joystick is left,
        else if(turn == JOY_LEFT){
            p1Direction = p1Direction - 0x0001;
        }






    }


    //for player 2
    if(player == 2){

    }
}


/*
    Customizing display list
*/
void rearranging_display_list();
void enablePMG();

// <- MAIN DRIVER CALLS ->

int main() {
    //Set default display to graphics 3
    joy_install(&joy_driver); //install the joystick driver
    _graphics(3);
    rearranging_display_list();
    enablePMG();

    while (true) {

    }
    return 0;
}

// FUNCTION IMPLEMENTATIONS
//void tank_north_display(int location){
//    //tank picture
//    unsigned int data[] = {
//        8,8,107,127,127,127,99,99
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

//void tank_north_east_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        25,58,124,255,223,14,28,24
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

//void tank_east_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        0,252,252,56,63,56,252,252
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

//void tank_south_east_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        25,58,124,255,223,14,28,24
//    };
//
//    int counter = 7;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter--;
//    }
//}

//void tank_south_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        99,99,127,127,127,107,8,8
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

//void tank_south_west_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        152,92,62,255,251,112,56,24
//    };
//
//    int counter = 7;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter--;
//    }
//}

//void tank_west_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        0,63,63,28,252,28,63,63
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

//void tank_north_west_display(int location) {
//    //tank picture
//    unsigned int data[] = {
//        152,92,62,255,251,112,56,24
//    };
//
//    int counter = 0;
//
//    for (i = location; i < (location+8); i++) {
//        POKE(playerMissileAddress+i,data[counter]);
//        counter++;
//    }
//}

void rearranging_display_list() {
    unsigned int *DLIST_ADDRESS  = OS.sdlstl + OS.sdlsth*256;
    unsigned char *DLIST = (unsigned char *)DLIST_ADDRESS;

    // Write the Display List for Graphics Mode 3
    unsigned char displayList[] = {
        DL_BLK8,  // 8 blank lines
        DL_BLK8,
        DL_BLK8,
        DL_LMS(DL_CHR40x8x1), // Load memory scan, 40x8 characters, 1 color
        0x60, 0x9F,  // Text Memory 
        DL_CHR40x8x1,
        DL_CHR40x8x1,
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
        DL_JVB,  // Jump and vertical blank
        0x4E, 0x9E  // Jump address (in this case, loop back to the start)
    };

    int i;
    // Copy the display list into memory, for example at 0x4000
    for (i = 0; i < sizeof(displayList); i++) {
        POKE(DLIST + i, displayList[i]);
    }
}

void enablePMG() {
    //Enable DMA
    POKE(0x22F, 62);
    I = PEEK(0x6A)-8;
    POKE(0xD407, I);
    POKE(0xD01D, 3);

    //Clear up Player Missile Character
    playerMissileAddress = (I * 256) + 1024;
    for (i = 0; i < 512; i++) {
        POKE(playerMissileAddress + i, 0);
    }

    //Player 1
    POKE(PMA_P0, 57);
    POKE(colLumPM0,88);
    POKE(playerMissileAddress+137, 0);
    POKE(playerMissileAddress+138, 252);
    POKE(playerMissileAddress+139, 252);
    POKE(playerMissileAddress+140, 56);
    POKE(playerMissileAddress+141, 63);
    POKE(playerMissileAddress+142, 56);
    POKE(playerMissileAddress+143, 252);
    POKE(playerMissileAddress+144, 252);

    //Player 2
    POKE(PMA_P1, 190);
    POKE(colLumPM1, 88);
    POKE(playerMissileAddress+393, 0);
    POKE(playerMissileAddress+394, 63);
    POKE(playerMissileAddress+395, 63);
    POKE(playerMissileAddress+396, 28);
    POKE(playerMissileAddress+397, 252);
    POKE(playerMissileAddress+398, 28);
    POKE(playerMissileAddress+399, 63);
    POKE(playerMissileAddress+400, 63);


    /* Loop until Q is pressed */
    while ((key = cgetc()) != 't')
    {
        switch (key) 
        { 
            //Player 1 Controls 
            case 'w':
                POKE(playerMissileAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_display(verticalStartP1);
                break;
            case 'e':
                horizontalStartP1++;
                POKE(playerMissileAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_east_display(verticalStartP1);
                POKE(PMA_P1, horizontalStartP1);
                break;  
            case 'a':
                tank_west_display(verticalStartP1);
                horizontalStartP1--;
                POKE(PMA_P1, horizontalStartP1); 
                break; 
            case 's':
                POKE(playerMissileAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_display(verticalStartP1);
                break; 
            case 'd':
                tank_east_display(verticalStartP1);
                horizontalStartP1++;
                POKE(PMA_P1, horizontalStartP1);
                break;
            case 'q':
                horizontalStartP1--;
                POKE(playerMissileAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_west_display(verticalStartP1);
                POKE(PMA_P1, horizontalStartP1);
                break;
            case 'z':
                horizontalStartP1--;
                POKE(playerMissileAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_west_display(verticalStartP1);
                POKE(PMA_P1, horizontalStartP1);
                break;                
            case 'c':
                horizontalStartP1++;
                POKE(playerMissileAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_east_display(verticalStartP1);
                POKE(PMA_P1, horizontalStartP1);
                break;
        }
    }
}
