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
#include <6502.h>
#include <stdio.h>
#include <stdlib.h>
#include <joystick.h>

//  <---------- GLOBAL VARIABLES ----------> 

#define P1PF 0xD005 // Player 1 to playfield collisions register
#define HITCLR 0xD01E // Colisiion Clear register

int i;                      //for loop counter (doesnt matter as i will always be initialized to zero)

//Variables to keep track of player 1 and player 2 score
char *player1Score = 0;
char player2Score = 0;

int PMBAddress;
int playerAddress;
int missileAddress;
int bitMapAddress;

int *PMA_P0 = (int *)0xD000;
int *PMA_P1 = (int *)0xD001;

int *colLumPM0 = (int *)0x2C0;
int *colLumPM1 = (int *)0x2C1;

int horizontalStartP0 = 57;     //Starting horizontal location for player 0
int horizontalStartP1 = 190;    //Starting horizontal location for player 1

int verticalStartP0 = 131;
int verticalStartP1 = 387;

char key;



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

void tank_north_display(int location);
void tank_north_east_display(int location);
void tank_east_display(int location);
void tank_south_east_display(int location);
void tank_south_display(int location);
void tank_south_west_display(int location);
void tank_west_display(int location);
void tank_north_west_display(int location);

/*
    Functions to build game
*/
void rearranging_display_list();
void createBitMap();
void enablePMG();
void movePlayers();

// <---------- MAIN DRIVER ---------->

int main() {
    _graphics(18);      //Set default display to graphics 3 + 16 (+16 displays mode with graphics, eliminating the text window)
    POKE(0x2C5, 26);    //Sets bitmap color to yellow
    rearranging_display_list();
    createBitMap();
    enablePMG();

    // Main loop
    while (true) {
        movePlayers();
        POKE(HITCLR, 1); // clear all of the collision registers 
        waitvsync(); // wait for the next frame
    }

    return 0;
}

// <---------- FUNCTION IMPLEMENTATIONS ---------->
void tank_north_display(int location){
    unsigned int data[] = {
        8,8,107,127,127,127,99,99
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
}

void tank_north_east_display(int location) {
    unsigned int data[] = {
        25,58,124,255,223,14,28,24
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
}

void tank_east_display(int location) {
    unsigned int data[] = {
        0,252,252,56,63,56,252,252
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
}

void tank_south_east_display(int location) {
    unsigned int data[] = {
        25,58,124,255,223,14,28,24
    }; 

    int counter = 7;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter--;
    }
}

void tank_south_display(int location) {
    unsigned int data[] = {
        99,99,127,127,127,107,8,8
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
}

void tank_south_west_display(int location) {
    unsigned int data[] = {
        152,92,62,255,251,112,56,24
    }; 

    int counter = 7;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter--;
    }
}

void tank_west_display(int location) {
    unsigned int data[] = {
        0,63,63,28,252,28,63,63
    };

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
}

void tank_north_west_display(int location) {
    unsigned int data[] = {
        152,92,62,255,251,112,56,24
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerAddress+i,data[counter]);
        counter++;
    }
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
    missileAddress = (PMBAddress * 256) + 768;
    for (i = 0; i < 512; i++) {
        POKE(playerAddress + i, 0);
    }

    //Testing Missile Location
    //Set missile location, to just the horizontal and vertical positions of the tank (middle barrel position to be exact)
    POKE(0xD004, 57);
    POKE(0xD005, 56);
    POKE(missileAddress+174, 6);

    //Player 1
    POKE(PMA_P0, 57);
    POKE(colLumPM0,196);
    POKE(playerAddress+131, 0);
    POKE(playerAddress+132, 252);
    POKE(playerAddress+133, 252);
    POKE(playerAddress+134, 56);
    POKE(playerAddress+135, 63);
    POKE(playerAddress+136, 56);
    POKE(playerAddress+137, 252);
    POKE(playerAddress+138, 252);

    //Player 2
    POKE(PMA_P1, 190);
    POKE(colLumPM1, 116);
    POKE(playerAddress+387, 0);
    POKE(playerAddress+388, 63);
    POKE(playerAddress+389, 63);
    POKE(playerAddress+390, 28);
    POKE(playerAddress+391, 252);
    POKE(playerAddress+392, 28);
    POKE(playerAddress+393, 63);
    POKE(playerAddress+394, 63);

    //Testing Missile Location
    //Set missile location, to just the horizontal and vertical positions of the tank (middle barrel position to be exact)
    POKE(0xD004, 60);
    POKE(0xD005, 55);
    POKE(missileAddress+200, 255);
    
}

//moving based off of joystick input, or firing the tank if the player chooses
/* void movePlayers() {
    //joystick code
    //CODE NEEDS TO HAVE UPDATED JOYSTICK METHODS ---------------//
    //	if(JOY_BTN_FIRE(joy)){      }
    //
    unsigned char player1move = joy_read(JOY_1);
    //unsigned char player2move = joy_read(JOY_2);
    //moving player 1
    //if(player1move == JOY_BTN_1(player1move)) fire(1);
    if (player1move == JOY_UP(player1move)){
        POKE(playerAddress+(verticalStartP1+7), 0);
        verticalStartP1--;
        tank_north_display(verticalStartP1);
        if (PEEK(P1PF) !=  0x00){
            verticalStartP1+=3; // go back 3 pixels if there was collision
        }
    } else if (player1move == JOY_DOWN(player1move)){
        POKE(playerAddress+verticalStartP1, 0);
        verticalStartP1++;
        tank_south_display(verticalStartP1);
        if (PEEK(P1PF) !=  0x00){
            verticalStartP1-=3; // go back 3 pixels if there was collision
        }
    } else if (player1move == JOY_LEFT(player1move)){
        tank_west_display(verticalStartP1);
        horizontalStartP1--;
        if (PEEK(P1PF) !=  0x00){
            horizontalStartP1+=3; // go back 3 pixels if there was collision
        }
        POKE(PMA_P1, horizontalStartP1);
    } else if (player1move == JOY_RIGHT(player1move)){
        tank_east_display(verticalStartP1);
        horizontalStartP1++;
        if (PEEK(P1PF) !=  0x00){
            horizontalStartP1-=3; // go back 3 pixels if there was collision
        }
        POKE(PMA_P1, horizontalStartP1);
    } */

    /* //moving player 2
    if(player2move == JOY_BTN_1) fire(2);
    else if(player2move == JOY_UP) moveForward(2);
    else if(player2move == JOY_DOWN) moveBackward(2);
    else if(player2move == JOY_LEFT || player2move == JOY_RIGHT) turnplayer(player2move, 2);
    
} */   

void movePlayers(){
    if (kbhit() != 0){
       key = cgetc(); 
    }else{
        return;
    }
     switch (key) 
        { 
            case 'w':
                POKE(playerAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    verticalStartP1+=3; // go back 2 pixels
                }
                break;
            case 'e':
                horizontalStartP1++;
                POKE(playerAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_east_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    verticalStartP1+=3; // go back 2 pixels
                    horizontalStartP1-=3; // go back 2 pixels
                }
                POKE(PMA_P1, horizontalStartP1);
                break;  
            case 'a':
                tank_west_display(verticalStartP1);
                horizontalStartP1--;
                if (PEEK(P1PF) !=  0x00){
                    horizontalStartP1+=3;
                }
                POKE(PMA_P1, horizontalStartP1); 
                break; 
            case 's':
                POKE(playerAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    verticalStartP1-=3;
                }
                break; 
            case 'd':
                tank_east_display(verticalStartP1);
                horizontalStartP1++;
                if (PEEK(P1PF) !=  0x00){
                    horizontalStartP1-=3;
                }
                POKE(PMA_P1, horizontalStartP1);
                break;
            case 'q':
                horizontalStartP1--;
                POKE(playerAddress+(verticalStartP1+7), 0);
                verticalStartP1--;
                tank_north_west_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    horizontalStartP1+=3;
                    verticalStartP1+=3;
                }
                POKE(PMA_P1, horizontalStartP1);
                break;
            case 'z':
                horizontalStartP1--;
                POKE(playerAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_west_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    horizontalStartP1+=3;
                    verticalStartP1-=3;
                }
                POKE(PMA_P1, horizontalStartP1);
                break;                
            case 'c':
                horizontalStartP1++;
                POKE(playerAddress+verticalStartP1, 0);
                verticalStartP1++;
                tank_south_east_display(verticalStartP1);
                if (PEEK(P1PF) !=  0x00){
                    horizontalStartP1-=3;
                    verticalStartP1-=3;
                }
                POKE(PMA_P1, horizontalStartP1);
                break;
    }
}    
}
