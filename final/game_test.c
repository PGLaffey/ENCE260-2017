#include <stdio.h>

#define MAX_SHOTS 5

void add_shot (int start, int* shots, size_t* numShots, int dirrect)
{
    *(shots + (*numShots * 3)) = start;
    *(shots + (*numShots * 3) + 1) = 1; 
    *(shots + (*numShots * 3) + 2) = dirrect;
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
    printf("Tick\n\n");
}

void update_matrix (int* playerLoc, int* shots, size_t numShots)
{
    int count = 0;
    printf("Player Loc:\n%d\n\n", *playerLoc);
    printf("Shots:\n");
    while (count < numShots) {
        printf("%d", *(shots + (count * 3)));
        printf(",%d", *(shots + (count * 3) + 1));
        printf(" - %d\n", *(shots + (count * 3) + 2));
        count += 1;
    }
    printf("\n\n\n");
}



int main (void)
{
    
    
    int tick = 0;
    int playerLoc = 2;
    int shots[36] = {0}; //Column,Row,Dirrection
    size_t numShots = 0;
    tick += 1;
//    while (1)
//    {
//        
//
//        tick += 1;
//    }

    tick_shots(shots, numShots);
    update_matrix(&playerLoc, shots, numShots);
    playerLoc = 1;
    update_matrix(&playerLoc, shots, numShots);
    add_shot(playerLoc, shots, &numShots, 0);
    update_matrix(&playerLoc, shots, numShots);
    tick_shots(shots, numShots);
    update_matrix(&playerLoc, shots, numShots);
}
