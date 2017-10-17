/**
 * Authors: Patrick Laffey, Alex Bull
 * Purpose: Football 1v1 game for two or more UC Fun Kits
 */

#include "system.h"
#include "display.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "../fonts/font3x5_1.h"


/**
 * Adds a shot to the queue.
 */ 
void add_shot (int col, int row, int* shots, int* numShots, int direct)
{
    *(shots + (*numShots * 3)) = col;
    *(shots + (*numShots * 3) + 1) = row; 
    *(shots + (*numShots * 3) + 2) = direct;
    *numShots += 1;
    update_shots(shots, numShots);
}


/**
 * Removes a shot from the queue and moves the rest up.
 */ 
void remove_shot (int* shots, int* numShots)
{
    display_pixel_set(*shots, *(shots + 1), 0);
    int count = 0;
    while (count < *numShots - 1) {
        *(shots + (count * 3)) = *(shots + (count * 3) + 3);
        *(shots + (count * 3) + 1) = *(shots + (count * 3) + 4); 
        *(shots + (count * 3) + 2) = *(shots + (count * 3) + 5);
        count += 1;
    }
    *(shots + (count * 3)) = 0;
    *(shots + (count * 3) + 1) = 0;
    *(shots + (count * 3) + 2) = 0;
    *numShots -= 1;
}


/**
 * Checks if a shot is ready to be transmitted or has scored.
 */ 
void check_shot (int* shots, int* numShots, int playerLoc) 
{
    if (*(shots + 1) >= 6 && *(shots + 2) == 0) {
        ir_uart_putc(*shots);
        remove_shot(shots, numShots);
    }
    else if (*(shots + 1) <= 1 && *(shots + 2) == 1) {
        if (*shots != playerLoc) {
            ir_uart_putc(5);
        }
        remove_shot(shots, numShots);
    }
}


/**
 * Performs a tick on the current shots
 */ 
void tick_shots (int* shots, int* numShots, int playerLoc)
{
    check_shot(shots, numShots, playerLoc);
    int count = 0;
    while (count < *numShots) {
        if (*(shots + (count * 3) + 2)) {
            *(shots + (count * 3) + 1) -= 1; 
        }
        else {
            *(shots + (count * 3) + 1) += 1; 
        }
        count += 1;
    }
    update_shots(shots, numShots);
}


/**
 * Updates the locations of the current shots on the LED matrix by one tick
 */ 
void update_shots (int* shots, int* numShots)
{
    int count = 0;

    while (count < *numShots) {
        display_pixel_set(*(shots + (count * 3)), *(shots + (count * 3) + 1), 1);
        if (*(shots + (count * 3) + 2)) {
            display_pixel_set(*(shots + (count * 3)), *(shots + (count * 3) + 1) + 1, 0);
        }
        else {
            display_pixel_set(*(shots + (count * 3)), *(shots + (count * 3) + 1) - 1, 0);
        }
        count += 1;
    }
}


/**
 * Checks if there is an input from the Nav-switch.
 */ 
void check_stick (int* playerLoc, int* addShot)
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_EAST) && *playerLoc < 4) {
        display_pixel_set(*playerLoc, 1, 0);
        *playerLoc += 1;
    
    }
    else if (navswitch_push_event_p(NAVSWITCH_WEST) && *playerLoc > 0) {
        display_pixel_set(*playerLoc, 1, 0);
        *playerLoc -= 1;
    }
    else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        *addShot = 1;
    }
    display_pixel_set(*playerLoc, 1, 1);
    display_update();
}


/**
 * Updates the score on the LED matrix.
 */ 
void update_score (int score)
{
    switch(score) {
        case 5:
            display_pixel_set(4, 0, 1);
        case 4:
            display_pixel_set(3, 0, 1);
        case 3:
            display_pixel_set(2, 0, 1);
        case 2:
            display_pixel_set(1, 0, 1);
        case 1:
            display_pixel_set(0, 0, 1);
    }
}


/**
 * Checks if there is an infra-red input.
 */ 
void check_infra (int* addShot, int* addLoc, int* score, int* gameover) 
{
    if (ir_uart_read_ready_p()) {
        int received = ir_uart_getc();
        if (received < 5 && received >= 0) {
            *addShot = 1;
            *addLoc = received;
        }
        else if (received == 5) {
            *score += 1;
        }
        else if (received == 6) {
            *gameover = 1;
        }
    }
}


/**
 * Prints the end message on the LED matrix.
 */ 
void end_message(int score) 
{
    if (score == 6) {
        tinygl_text("GAME OVER!  WINNER!:)");
    }
    else {
        tinygl_text("GAME OVER!  LOSER!:(");
    }
    
    while (1) {
        pacer_wait();
        tinygl_update();
    }
}


/**
 * Prints the welcome message on the LED matrix.
 */ 
void welcome_message(void) {
    tinygl_text("--WELCOME TO 1V1 FOOTBALL--");
    while (!navswitch_push_event_p(NAVSWITCH_PUSH)) {
        pacer_wait();
        tinygl_update();
        navswitch_update();
    }
}


/**
 * Initializes the system.
 */
void initialize (void) 
{
    system_init();
    display_init();
    navswitch_init();
    pacer_init(250);
    ir_uart_init();
    tinygl_init(1000);
    display_update();
    
    tinygl_font_set(&font3x5_1);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
    tinygl_text_speed_set(100);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
}


/**
 * Main running of the game.
 */ 
int main (void)
{
    initialize();
    int score = 0;
    int tick = 0;
    int addOutgoing = 0;
    int addIncoming = 0;
    int incomingLoc = 0;
    int playerLoc = 2;
    int gameover = 0;
    
    //Each shot requires 3 integers. Column, row, direction.
    int shots[36] = {0}; 
    int numShots = 0;
    
    welcome_message();
    display_clear();
    
    while (!gameover)
    {
        pacer_wait();
        if (tick == 70) {
            tick = 0;
            tick_shots(shots, &numShots, playerLoc);
            if (addOutgoing) {
                add_shot(playerLoc, 2, shots, &numShots, 0);
                addOutgoing = 0;
            }
            if (addIncoming) {
                add_shot(incomingLoc, 6, shots, &numShots, 1);
                addIncoming = 0;
            }
        }
        check_infra(&addIncoming, &incomingLoc, &score, &gameover);
        update_score(score);
        check_stick(&playerLoc, &addOutgoing);
        tick += 1;
        if (score > 5) {
            gameover = 1;
            ir_uart_putc(6);
        }
    }
    display_clear();
    end_message(score);
    return 0;
}

