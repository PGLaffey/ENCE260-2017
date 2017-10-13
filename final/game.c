#include <stdio.h>
#include "system.h"
#include "display.h"
#include "pacer.h"
#include "tinygl.h"
#include "navswitch.h"
#include "ir_uart.h"

#define MAX_SHOTS 5

void add_shot (int start, int* shots, size_t* numShots, int direct)
{
    *(shots + (*numShots * 3)) = start;
    *(shots + (*numShots * 3) + 1) = 1; 
    *(shots + (*numShots * 3) + 2) = direct;
    *numShots += 1;
}

void remove_shot (int* shots, size_t* numShots)
{
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
    numShots -= 1;
}

void check_shot (int* shots, size_t numShots) 
{
    if (*(shots + 1) == MAX_SHOTS) {
        //Transmit shot
        remove_shot(shots, &numShots);
    }
}

void tick_shots (int* shots, size_t numShots)
{
    check_shot(shots, numShots);
    int count = 0;
    while (count < numShots) {
        *(shots + (count * 2) + 1) += 1; 
        count += 1;
    }

}

void update_matrix (int* shots, size_t numShots)
{
    int count = 0;

    while (count < numShots) {
        display_pixel_set(*(shots + (count * 3)), *(shots + (count * 3) + 1), 1);
        display_pixel_set(*(shots + (count * 3)), *(shots + (count * 3) + 1) - 1, 0);
        count += 1;
    }
    display_update();

}

void check_stick (int* playerLoc, int* addShot)
{
    if (navswitch_push_event_p(NAVSWITCH_EAST) && *playerLoc < 4) {
        display_pixel_set(*playerLoc, 0, 0);
        *playerLoc += 1;
        display_pixel_set(*playerLoc, 0, 1);
    }
    else if (navswitch_push_event_p(NAVSWITCH_WEST) && *playerLoc > 0) {
        display_pixel_set(*playerLoc, 0, 0);
        *playerLoc -= 1;
        display_pixel_set(*playerLoc, 0, 1);
    }
    else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        *addShot = 1;
    }
}

int main (void)
{
    system_init();
    display_init();
    pacer_init(250);
    display_update();
    
    int tick = 0;
    int addShot = 0;
    int playerLoc = 2;
    int shots[36] = {0}; //Column,Row,Direction
    size_t numShots = 0;
    tick += 1;
    while (1)
    {
        pacer_wait();
        check_stick(&playerLoc, &addShot);
        if (tick % 30 == 0) {
            if (addShot) {
                add_shot(playerLoc, shots, &numShots, 0);
                addShot = 0;
            }
            update_matrix(shots, numShots);
        }
        
        tick += 1;
    }

    tick_shots(shots, numShots);
    update_matrix( shots, numShots);
    playerLoc = 1;
    update_matrix(shots, numShots);
    add_shot(playerLoc, shots, &numShots, 0);
    update_matrix( shots, numShots);
    tick_shots(shots, numShots);
    update_matrix( shots, numShots);
    return 0;
}

