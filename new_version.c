#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOUND 6

int MY_COLOR;
int BOARD[BOUND][BOUND];
int PRIORITY[BOUND][BOUND];
int CAT, KITTEN;
int FULL;

typedef struct piece PIECE;

struct piece
{
    /* data */
    char color;
    int catOrKITTEN;
    int row, col;
    int location;
    int pick;
};

void init(char *color, char *board_file);
void output_step(int CatOrKITTEN, int PutLocation, char *step_file, int *arr);
void add_priority(int row, int col, int points);
void compute_priority(int catOrKitten);
int pick_position();
int *pick_or_not();
void swap_position(int board[BOUND][BOUND], int piece, int r1, int c1, int r2, int c2);
void put_cat(int board[BOUND][BOUND], int row, int col, int catOrKitten);
int if_straight(int board_copy[BOUND][BOUND], int row, int col, int catOrKitten);
void print();

int main(int argc, char *argv[])
{
    char *color = argv[1];
    char *board_file = argv[2];
    char *step_file = argv[3];

    init(color, board_file);

    int catOrKitten = KITTEN > 0 ? 2 : 1;

    compute_priority(catOrKitten);

    int putLocation = pick_position();

    if (KITTEN > 0)
    {
        put_cat(BOARD, (putLocation - 1) / 6, (putLocation - 1) % 6, catOrKitten);
    }
    else
    {
        put_cat(BOARD, (putLocation - 1) / 6, (putLocation - 1) % 6, catOrKitten);
    }

    int *p = pick_or_not();

    output_step(catOrKitten, putLocation, step_file, p);

    print();

    return 0;
}

void init(char *color, char *board_file)
{
    // color
    if (!strcmp(color, "gray"))
        MY_COLOR = 1;
    else
        MY_COLOR = 2;

    // BOARD & priority
    FILE *myBoard = fopen(board_file, "r");
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            fscanf(myBoard, "%d,", &BOARD[i][j]);
            if (i == 0 || i == 5 || j == 0 || j == 5)
            {
                PRIORITY[i][j] = 0;
            }
            else if (i == 1 || i == 4 || j == 1 || j == 4)
            {
                PRIORITY[i][j] = 3;
            }
            else
            {
                PRIORITY[i][j] = 6;
            }
        }
    }

    // CAT number//
    int o_cat, o_kitten;
    int g_cat, g_kitten;
    fscanf(myBoard, "%d,", &g_kitten);
    fscanf(myBoard, "%d,", &g_cat);
    fscanf(myBoard, "%d,", &o_kitten);
    fscanf(myBoard, "%d,", &o_cat);
    fclose(myBoard);

    if (MY_COLOR == 1)
    {
        CAT = g_cat;
        KITTEN = g_kitten;
    }
    else
    {
        CAT = o_cat;
        KITTEN = o_kitten;
    }

    FULL = 0;
}

void output_step(int CatOrKITTEN, int PutLocation, char *step_file, int *arr)
{
    FILE *myStep = fopen(step_file, "w");

    fprintf(myStep, "%d,%d,\n", CatOrKITTEN, PutLocation);

    if (arr == NULL)
    {
        fprintf(myStep, "%d,\n", 0);
    }
    else if (FULL)
    {
        fprintf(myStep, "%d,\n", *arr);
        free(arr);
    }
    else
    {
        fprintf(myStep, "%d,%d,%d,\n", *arr, *(arr + 1), *(arr + 2));
        free(arr);
    }
}

void add_priority(int row, int col, int points)
{
    for (int i = row - 1; i <= row + 1; ++i)
    {
        for (int j = col - 1; j <= col + 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;
            if (i >= 0 && i < BOUND && j >= 0 && j < BOUND)
            {
                PRIORITY[i][j] += points;
            }
        }
    }
}

void compute_priority(int catOrKitten)
{
    int piece = catOrKitten == 1 ? MY_COLOR : MY_COLOR + 2;

    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (BOARD[i][j] > 0 && BOARD[i][j] % 2 != MY_COLOR % 2)
            {
                if (BOARD[i][j] > 2)
                    add_priority(i, j, 6);
                else
                    add_priority(i, j, 4);
            }
        }
    }

    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (BOARD[i][j] > 0 && BOARD[i][j] % 2 == MY_COLOR % 2 && BOARD[i][j] <= piece)
            {
                add_priority(i, j, -4);
            }
        }
    }

    int board_copy[BOUND][BOUND];
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (BOARD[i][j] == 0 && if_straight(board_copy, i, j, catOrKitten))
            {
                PRIORITY[i][j] += 50;
            }
        }
    }
}

int pick_position()
{
    int row, col;
    int max = 0;
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (BOARD[i][j] == 0 && PRIORITY[i][j] > max)
            {
                max = PRIORITY[i][j];
                row = i;
                col = j;
            }
        }
    }

    return row * 6 + col + 1;
}

int *pick_or_not()
{
    int *record = malloc(sizeof(int) * 3);

    // 橫向
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (BOARD[i][j] > 0 && BOARD[i][j + 1] > 0 && BOARD[i][j + 2] > 0 && BOARD[i][j] % 2 == MY_COLOR % 2 && BOARD[i][j + 1] % 2 == MY_COLOR % 2 && BOARD[i][j + 2] % 2 == MY_COLOR % 2)
            {
                record[0] = i * 6 + j + 1;
                record[1] = record[0] + 1;
                record[2] = record[1] + 1;

                return record;
            }
        }
    }

    // 直向
    for (int j = 0; j < BOUND; ++j)
    {
        for (int i = 0; i < BOUND - 2; ++i)
        {
            if (BOARD[i][j] > 0 && BOARD[i + 1][j] > 0 && BOARD[i + 2][j] > 0 && BOARD[i][j] % 2 == MY_COLOR % 2 && BOARD[i + 1][j] % 2 == MY_COLOR % 2 && BOARD[i + 2][j] % 2 == MY_COLOR % 2)
            {
                record[0] = i * 6 + j + 1;
                record[1] = record[0] + 6;
                record[2] = record[1] + 6;

                return record;
            }
        }
    }

    // 斜向
    for (int i = 0; i < BOUND - 2; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (BOARD[i][j] > 0 && BOARD[i + 1][j + 1] > 0 && BOARD[i + 2][j + 2] > 0 && BOARD[i][j] % 2 == MY_COLOR % 2 && BOARD[i + 1][j + 1] % 2 == MY_COLOR % 2 && BOARD[i + 2][j + 2] % 2 == MY_COLOR % 2)
            {
                record[0] = i * 6 + j + 1;
                record[1] = record[0] + 7;
                record[2] = record[1] + 7;

                return record;
            }
        }
    }
    for (int i = 2; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (BOARD[i][j] > 0 && BOARD[i - 1][j + 1] > 0 && BOARD[i - 2][j + 2] > 0 && BOARD[i][j] % 2 == MY_COLOR % 2 && BOARD[i - 1][j + 1] % 2 == MY_COLOR % 2 && BOARD[i - 2][j + 2] % 2 == MY_COLOR % 2)
            {
                record[0] = (i - 2) * 6 + j + 2 + 1;
                record[1] = record[0] + 5;
                record[2] = record[1] + 5;

                return record;
            }
        }
    }

    // 8隻貓
    int pos;
    int value = 0;
    int count = 0;
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (BOARD[i][j] == MY_COLOR || BOARD[i][j] == MY_COLOR + 2)
            {
                count++;
                if (BOARD[i][j] > value)
                {
                    value = BOARD[i][j];
                    pos = i * 6 + j + 1;
                }
            }
        }
    }
    if (count > 7)
    {
        record[0] = pos;
        FULL = 1;
        return record;
    }

    free(record);
    return NULL;
}

void swap_position(int board[BOUND][BOUND], int piece, int r1, int c1, int r2, int c2)
{
    if (r1 < 0 || r1 >= BOUND || c1 < 0 || c1 >= BOUND)
    {
        return;
    }
    else if (r2 < 0 || r2 >= BOUND || c2 < 0 || c2 >= BOUND)
    {
        board[r1][c1] = 0;
    }
    else if (piece >= board[r1][c1] && board[r2][c2] == 0)
    {
        board[r2][c2] = board[r1][c1];
        board[r1][c1] = 0;
    }
}

void put_cat(int board[BOUND][BOUND], int row, int col, int catOrKitten)
{
    int piece = catOrKitten == 1 ? MY_COLOR : MY_COLOR + 2;
    board[row][col] = piece;

    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (i != 0 || j != 0)
                swap_position(board, piece, row + i, col + j, row + i * 2, col + j * 2);
        }
    }
}

int if_straight(int board_copy[BOUND][BOUND], int row, int col, int catOrKitten)
{
    memcpy(board_copy, BOARD, sizeof(BOARD));
    put_cat(board_copy, row, col, catOrKitten);

    // 橫向
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (board_copy[i][j] > 0 && board_copy[i][j + 1] > 0 && board_copy[i][j + 2] > 0 && board_copy[i][j] % 2 == MY_COLOR % 2 && board_copy[i][j + 1] % 2 == MY_COLOR % 2 && board_copy[i][j + 2] % 2 == MY_COLOR % 2)
            {
                return 1;
            }
        }
    }

    // 直向
    for (int j = 0; j < BOUND; ++j)
    {
        for (int i = 0; i < BOUND - 2; ++i)
        {
            if (board_copy[i][j] > 0 && board_copy[i + 1][j] > 0 && board_copy[i + 2][j] > 0 && board_copy[i][j] % 2 == MY_COLOR % 2 && board_copy[i + 1][j] % 2 == MY_COLOR % 2 && board_copy[i + 2][j] % 2 == MY_COLOR % 2)
            {
                return 1;
            }
        }
    }

    // 斜向
    for (int i = 0; i < BOUND - 2; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (board_copy[i][j] > 0 && board_copy[i + 1][j + 1] > 0 && board_copy[i + 2][j + 2] > 0 && board_copy[i][j] % 2 == MY_COLOR % 2 && board_copy[i + 1][j + 1] % 2 == MY_COLOR % 2 && board_copy[i + 2][j + 2] % 2 == MY_COLOR % 2)
            {
                return 1;
            }
        }
    }
    for (int i = 2; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND - 2; ++j)
        {
            if (board_copy[i][j] > 0 && board_copy[i - 1][j + 1] > 0 && board_copy[i - 2][j + 2] > 0 && board_copy[i][j] % 2 == MY_COLOR % 2 && board_copy[i - 1][j + 1] % 2 == MY_COLOR % 2 && board_copy[i - 2][j + 2] % 2 == MY_COLOR % 2)
            {
                return 1;
            }
        }
    }

    return 0;
}

void print()
{
    printf("Board:\n");
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            printf("%d,", BOARD[i][j]);
        }

        printf("\n");
    }

    printf("\n\nPriority:\n");
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            printf("%2d,", PRIORITY[i][j]);
        }

        printf("\n");
    }
}
