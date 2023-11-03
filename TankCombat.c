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
int I;
int playerMissileAddress;

int *PMA_P1 = (int *)0xD000;
int *PMA_P2 = (int *)0xD001;

int *colLumPM1 = (int *)0x2C0;
int *colLumPM2 = (int *)0x2C1;

unsigned int p1Direction = EAST;
unsigned int p2Direction = WEST;
unsigned char p1LastMove;
unsigned char p2LastMove;

int verticalStartP1= 137;
int verticalStartP2 = 393;
int horizontalStartP1 = 57;
int horizontalStartP2 = 190;

int p1Location = 137;
int p2Location = 393;
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
    unsigned char player1move = joy_read(JOY_1);
    //unsigned char player2move = joy_read(JOY_2);

    if(JOY_BTN_1(player1move)) fire(1);
    else if(JOY_UP(player1move)) moveForward(1);
    else if(JOY_DOWN(player1move)) moveBackward(1);
    else if(JOY_LEFT(player1move) || JOY_RIGHT(player1move)) turnplayer(player1move, 1);

    //moving player 2
    //if(JOY_BTN_1(player2move)) fire(2);
    //else if(JOY_UP(player2move)) moveForward(2);
    //else if(JOY_DOWN(player2move)) moveBackward(2);
    //else if(JOY_LEFT(player2move) || JOY_RIGHT(player2move)) turnplayer(player2move, 2);

    //for keyboard testing
//    key = cgetc();
//    if(key == 'w') moveForward(1);
//    if(key == 's') moveBackward(1);
//    if(key == 'a') turnplayer('L', 1);
//    if(key == 'd') turnplayer('R', 1);
}

//rotating the player
void turnplayer(unsigned char turn, int player){
    //for player 1
    if(player == 1){
        //handling edge cases
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
                POKE(playerMissileAddress + i, tankPics[p1Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p1Location; i < p1Location + 8; i++){
                POKE(playerMissileAddress + i, tankPics[p1Direction][counter]);
                counter++;
            }
        }
    }

    //updating player 2
    else if(player == 2){
        if(p2Direction == SOUTH_15 || p2Direction == SOUTH_60 || p2Direction == SOUTH_WEST || p2Direction == EAST_15 || p2Direction == EAST_SOUTH || p2Direction == EAST_60){
            int counter = 7;
            for(i = p2Location; i < p2Location + 8; i++){
                POKE(playerMissileAddress + i, tankPics[p2Direction][counter]);
                counter--;
            }
        }
        else{
            int counter = 0;
            for(i = p2Location; i < p2Location + 8; i++){
                POKE(playerMissileAddress + i, tankPics[p2Direction][counter]);
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
            POKE(playerMissileAddress+(p1Location+7), 0);
            p1Location--;

        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerMissileAddress+p1Location, 0);
            p1Location++;
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
            if(p1Direction < 4) p1Horizontal = p1Horizontal + x;
            else p1Horizontal = p1Horizontal - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerMissileAddress+(p1Location +7), 0);
            POKE(playerMissileAddress+(p1Location +6), 0);
            p1Location = p1Location - y;
            updateplayerDir(1);
            POKE(PMA_P1, p1Horizontal);
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
            if(p1Direction < 8) p1Horizontal = p1Horizontal + x;
            else p1Horizontal = p1Horizontal - x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerMissileAddress+(p1Location), 0);
            POKE(playerMissileAddress+(p1Location +1), 0);
            p1Location = p1Location + y;
            updateplayerDir(1);
            POKE(PMA_P1, p1Horizontal);
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
            POKE(playerMissileAddress+p1Location, 0);
            p1Location++;
        }
        //movement for south
        if(p1Direction == SOUTH){
            POKE(playerMissileAddress+(p1Location+7), 0);
            p1Location--;
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
            if(p1Direction > 4) p1Horizontal = p1Horizontal + x;
            else p1Horizontal = p1Horizontal - x;
            //x = -2, -1, 1, 2
            //y = 2, 1

            POKE(playerMissileAddress+(p1Location), 0);
            POKE(playerMissileAddress+(p1Location + 1), 0);
            p1Location = p1Location + y;
            updateplayerDir(1);
            POKE(PMA_P1, p1Horizontal);

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
            if(p1Direction < 8) p1Horizontal = p1Horizontal - x;
            else p1Horizontal = p1Horizontal + x;
            //x possible outcomes = -2, -1, 1, 2
            //y possible outcomes = 2, 1

            POKE(playerMissileAddress+(p1Location + 7), 0);
            POKE(playerMissileAddress+(p1Location +6), 0);
            p1Location = p1Location - y;
            updateplayerDir(1);
            POKE(PMA_P1, p1Horizontal);
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

// <- MAIN DRIVER CALLS ->

int main() {
    //Set default display to graphics 3
    joy_load_driver(joy_stddrv);
    joy_install(joy_static_stddrv); //install the joystick driver
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
    POKE(PMA_P1, 57);
    POKE(colLumPM1,88);
    POKE(playerMissileAddress+137, 0);
    POKE(playerMissileAddress+138, 252);
    POKE(playerMissileAddress+139, 252);
    POKE(playerMissileAddress+140, 56);
    POKE(playerMissileAddress+141, 63);
    POKE(playerMissileAddress+142, 56);
    POKE(playerMissileAddress+143, 252);
    POKE(playerMissileAddress+144, 252);

    //Player 2
    POKE(PMA_P2, 190);
    POKE(colLumPM2, 88);
    POKE(playerMissileAddress+393, 0);
    POKE(playerMissileAddress+394, 63);
    POKE(playerMissileAddress+395, 63);
    POKE(playerMissileAddress+396, 28);
    POKE(playerMissileAddress+397, 252);
    POKE(playerMissileAddress+398, 28);
    POKE(playerMissileAddress+399, 63);
    POKE(playerMissileAddress+400, 63);


}
