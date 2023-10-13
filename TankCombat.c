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

//  <---------- GLOBAL VARIABLES ----------> 

int i;                      //for loop counter (doesnt matter as i will always be initialized to zero)
int I;

int playerMissileAddress;
int bitMapAddress;

int *PMA_P0 = (int *)0xD000;
int *PMA_P1 = (int *)0xD001;

int *colLumPM0 = (int *)0x2C0;
int *colLumPM1 = (int *)0x2C1;

int horizontalStartP0 = 57;     //Starting horizontal location for player 0
int horizontalStartP1 = 190;    //Starting horizontal location for player 1

int verticalStartP0 = 137;
int verticalStartP1 = 393;

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
    Customizing display list
*/
void rearranging_display_list();
void enablePMG();

// <---------- MAIN DRIVER ---------->

int main() {
    //Set default display to graphics 3
    _graphics(3);
    rearranging_display_list();
    enablePMG();

    while (true) {

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
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

void tank_north_east_display(int location) {
    unsigned int data[] = {
        25,58,124,255,223,14,28,24
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

void tank_east_display(int location) {
    unsigned int data[] = {
        0,252,252,56,63,56,252,252
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

void tank_south_east_display(int location) {
    unsigned int data[] = {
        25,58,124,255,223,14,28,24
    }; 

    int counter = 7;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter--;
    }
}

void tank_south_display(int location) {
    unsigned int data[] = {
        99,99,127,127,127,107,8,8
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

void tank_south_west_display(int location) {
    unsigned int data[] = {
        152,92,62,255,251,112,56,24
    }; 

    int counter = 7;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter--;
    }
}

void tank_west_display(int location) {
    unsigned int data[] = {
        0,63,63,28,252,28,63,63
    };

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

void tank_north_west_display(int location) {
    unsigned int data[] = {
        152,92,62,255,251,112,56,24
    }; 

    int counter = 0;

    for (i = location; i < (location+8); i++) {
        POKE(playerMissileAddress+i,data[counter]);
        counter++;
    }
}

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

    bitMapAddress = PEEK(DLIST+9) + PEEK(DLIST+10)*256;

    // <--- Making the playfield ---> SHOULD IMPEMENT THIS IN A DIFFERENT FUNCTION
    //Making the top and bottom border
    for (i = 0; i < 10; i++) {
        POKE(bitMapAddress+i, 170);
        POKE(bitMapAddress+190+i, 170);
    }

    //Making the left border
    for (i = 10; i <= 180; i += 10) {
        POKE(bitMapAddress+i, 128);
    }

    //Making the right border
    for (i = 19; i <= 189; i += 10) {
        POKE(bitMapAddress+i, 2);
    }

    // <--- Making the sprites --->
    POKE(bitMapAddress+71, 40);
    POKE(bitMapAddress+121, 40);
    for (i = 81; i <= 111; i += 10) {
        POKE(bitMapAddress+i, 8);
    }

    POKE(bitMapAddress+78, 40);
    POKE(bitMapAddress+128, 40);
    for (i = 88; i <= 118; i += 10) {
        POKE(bitMapAddress+i, 32);
    }

    for (i = 44; i <= 64; i += 10) {
        POKE(bitMapAddress+i, 2);
    }

    for (i = 45; i <= 65; i += 10) {
        POKE(bitMapAddress+i, 128);
    }

    for (i = 134; i <= 154; i += 10) {
        POKE(bitMapAddress+i, 2);
    }

    for (i = 135; i <= 155; i += 10) {
        POKE(bitMapAddress+i, 128);
    }

    POKE(bitMapAddress+93, 168);
    POKE(bitMapAddress+103, 168);
    POKE(bitMapAddress+96, 42);
    POKE(bitMapAddress+106, 42);
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
