/**
 * fifteen.c
 *
 * Refer to Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

#ifndef INT_MAX
    #define INT_MAX 12345678
#endif // INT_MAX

// board removed for extra credit
//int board[DIM_MAX][DIM_MAX];

// dimensions removed for extra credit
//int d;

// prototypes
void greet(void);
void init(int** board, int d);
void draw(int** board, int d);
short move(int** board, int d, int tile);
short won(int** board, int d);
int get_int();

int main(int argc, char* argv[])
{


    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    int d;

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    int** board = malloc(d * sizeof(int *));

    if (board == NULL) {
        printf("Memory allocation failed for board!\n");
        return 1;
    }
    
    for (int i = 0; i < d; i++) {
        board[i] = malloc(d * sizeof(int));
        if (board[i] == NULL) {
            printf("Memory allocation for row failed for board at: %d\n", i);
            return 1;
        }
        
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init(board, d);

    // accept moves until game is won
    while (1)
    {
        // draw the current state of the board
        draw(board, d);

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won(board, d))
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();
        printf("\n");

        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(board, d, tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }

    // close log
    fclose(file);

    for (int i = 0; i < d; i++) {
        free(board[i]);
    }

    free(board);

    // success
    return 0;
}

/**
 * Get an non-negative integer from user input
 * If the input is not non-negative integer, return INT_MAX
 */
 int get_int()
 {
    int input = 0;
    short invalid = 0;

    char c = getchar();
    if (c == '\n')
        return INT_MAX;

    while (c != '\n') 
    {
        if ((c >= '0') && ( c <= '9'))
        {
            input = input * 10 + (c - '0');
        } else {
            invalid = 1;
        }

        c = getchar();
    }

    if (invalid)
        return INT_MAX;
    else 
        return input;
 }

/**
 * Greets player.
 */
void greet(void)
{    
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(int** board, int d)
{
    // TODO
    int counter = (d*d) - 1;
    for(int i = 0; i < d; i ++){
        for(int j = 0; j < d; j ++){
            if (d%2 ==0){
                                if(counter == 2){
                    board[i][j] = 1;
                    counter--;
                }
                
                else if(counter == 1){
                    board[i][j] = 2;
                    counter--;
                }else{

            board[i][j] = counter;
            counter--;
                }
            }
            else{
                board[i][j] = counter;
                counter--;
            }
        }
    }

}

/**
 * Prints the board in its current state.
 */
void draw(int **board, int d)
{
    // TODO
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            if (board[i][j] == 0)  {
                printf("   "); 
            }
            else {
                printf("%2d ", board[i][j]); 
            }
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * If tile borders empty space, moves tile and returns 1, else
 * returns 0.
 */
short move(int** board, int d, int tile)
{

    int tileX, tileY;
    int spaceX, spaceY;
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            if (board[i][j] == tile) {
                tileX = i;
                tileY = j;
            }
            else if (board[i][j] == 0) {
                spaceX = i;
                spaceY = j;
            }
        }
    }

    if (tileX == spaceX) {
        if (tileY - spaceY == 1 || tileY - spaceY == -1) {
            board[tileX][tileY] = 0;
            board[spaceX][spaceY] = tile;

            return 1;
        }
    }

    if (tileY == spaceY) {
        if (tileX - spaceX == 1 || tileX - spaceX== -1) {
            board[tileX][tileY] = 0;
            board[spaceX][spaceY] = tile;

            return 1;
        }
    }

    // TODO
    return 0;
}

/**
 * Returns 1 if game is won (i.e., board is in winning configuration),
 * else 0.
 */
short won(int** board, int d)
{

    int counter = 1;

    if (board[d-1][d-1] != 0) {
        return 0;
    }
    // TODO
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            if (board[i][j] != counter) {
                if (i != d - 1 && j != d - 1) {
                    return 0;
                }
            }
            counter++;
        }
    }

    return 1;
}
