#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOUNDARY 6

int remove_record[3] = {0, 0, 0};
int board[BOUNDARY][BOUNDARY];
int priority[BOUNDARY][BOUNDARY];
int myColor = 0;
int CatOrKitten = 0, PutLocation = 0, TakeLocation = 0;

int check_horizontal()
{
    int count = 0;
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            if (board[i][j] % 2 == myColor && board[i][j] != 0)
            {
                remove_record[count++] = i * BOUNDARY + j + 1;
            }
            else
                count = 0;

            if (count >= 3)
                return 1;
        }
    }

    return 0;
}

int check_vertical()
{
    int count = 0;
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            if (board[j][i] % 2 == myColor && board[j][i] != 0)
            {
                remove_record[count++] = j * BOUNDARY + i + 1;
            }
            else
                count = 0;

            if (count >= 3)
                return 1;
        }
    }

    return 0;
}

int check_slash()
{
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            // /方向
            if (board[i][j] != 0 && board[i][j] % 2 == myColor && i + 1 < BOUNDARY && i - 1 >= 0 && j + 1 < BOUNDARY && j - 1 >= 0)
            {
                // /方向
                if (board[i + 1][j - 1] != 0 && board[i - 1][j + 1] != 0 && board[i + 1][j - 1] % 2 == myColor && board[i - 1][j + 1] % 2 == myColor)
                {
                    remove_record[0] = (i - 1) * 6 + j + 1 + 1;
                    remove_record[1] = (i) * 6 + j + 1;
                    remove_record[2] = (i + 1) * 6 + j - 1 + 1;
                    return 1;
                }
                // \方向
                else if (board[i - 1][j - 1] != 0 && board[i + 1][j + 1] != 0 && board[i - 1][j - 1] % 2 == myColor && board[i + 1][j + 1] % 2 == myColor)
                {
                    remove_record[0] = (i - 1) * 6 + j - 1 + 1;
                    remove_record[1] = (i) * 6 + j + 1;
                    remove_record[2] = (i + 1) * 6 + j + 1 + 1;
                    return 1;
                }
            }
        }
    }

    return 0;
}

int find()
{
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            if (board[i][j] != 0 && board[i][j] % 2 == myColor)
                return (i * BOUNDARY + j + 1);
        }
    }

    return 0;
}

void push_cat(int row, int col, int CatorKitten)
{
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            int r = row + i, c = col + j;
            if ((i == 0 && j == 0) || r < 0 || r >= BOUNDARY || c < 0 || c >= BOUNDARY || board[r][c] == 0)
                continue;

            // push
            int r1 = r + i, c1 = c + j;
            if (r1 < 0 || r1 > BOUNDARY || c1 < 0 || c1 > BOUNDARY)
            {
                board[r][c] = 0;
            }
            else if (board[r1][c1] == 0 && CatorKitten >= board[r][c])
            {
                board[r1][c1] = board[r][c];
                board[r][c] = 0;
            }
        }
    }
}

void compute_priority()
{
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            if (board[i][j] != 0 && (board[i][j] % 2 != myColor))
            {
                // add priority
                for (int r = -1; r < 2; ++r)
                {
                    for (int c = -1; c < 2; ++c)
                    {
                        int row = i + r, col = j + c;
                        if ((r == 0 && c == 0) || row < 0 || row >= BOUNDARY || col < 0 || col >= BOUNDARY)
                            continue;
                        priority[row][col]++;
                    }
                }
            }
        }
    }
}

int pick_and_put()
{
    int row = 3, col = 3;
    int max_p = 0;
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            if (board[i][j] == 0 && priority[i][j] > max_p)
            {
                row = i;
                col = j;
                max_p = priority[i][j];
            }
        }
    }

    PutLocation = row * 6 + col + 1;
    if (CatOrKitten == 1 && myColor == 0)
    {
        board[row][col] = 2;
    }
    else if (CatOrKitten == 1 && myColor == 1)
    {
        board[row][col] = 1;
    }
    else if (CatOrKitten == 2 && myColor == 0)
    {
        board[row][col] = 4;
    }
    else
    {
        board[row][col] = 3;
    }

    push_cat(row, col, CatOrKitten * 2);

    return PutLocation;
}

void print_board()
{
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            printf("%d,", board[i][j]);
        }

        printf("\n");
    }

    printf("\n=================================\n");

    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            printf("%d,", priority[i][j]);
        }

        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    char *color = argv[1];
    char *board_file = argv[2];
    char *step_file = argv[3];

    // color
    if (!strcmp(color, "gray"))
        myColor = 1;
    else
        myColor = 0;

    printf("color: %d\n\n", myColor);

    // board input
    FILE *myboard = fopen(board_file, "r");

    // board position & initial priority
    for (int i = 0; i < BOUNDARY; ++i)
    {
        for (int j = 0; j < BOUNDARY; ++j)
        {
            fscanf(myboard, "%d,", &board[i][j]);
            priority[i][j] = 0;
        }
    }

    // cat number//
    int cat, kitten;
    int o_cat, o_kitten;
    int g_cat, g_kitten;
    fscanf(myboard, "%d,", &g_kitten);
    fscanf(myboard, "%d,", &g_cat);
    fscanf(myboard, "%d,", &o_kitten);
    fscanf(myboard, "%d,", &o_cat);
    fclose(myboard);

    if (myColor)
    {
        cat = g_cat;
        kitten = g_kitten;
    }
    else
    {
        cat = o_cat;
        kitten = o_kitten;
    }

    // output step
    FILE *my_step = fopen(step_file, "w");

    // CatOrKitten
    if (kitten)
        CatOrKitten = 2;
    else
        CatOrKitten = 1;
    fprintf(my_step, "%d,", CatOrKitten);

    // push cat
    compute_priority();
    PutLocation = pick_and_put();
    fprintf(my_step, "%d,\n", PutLocation);

    // remove
    if (check_horizontal())
    {
        fprintf(my_step, "%d,%d,%d,\n", remove_record[0], remove_record[1], remove_record[2]);
    }
    else if (check_vertical())
    {
        fprintf(my_step, "%d,%d,%d,\n", remove_record[0], remove_record[1], remove_record[2]);
    }
    else if (check_slash())
    {
        fprintf(my_step, "%d,%d,%d,\n", remove_record[0], remove_record[1], remove_record[2]);
    }
    else if ((cat + kitten) <= 1)
    {
        fprintf(my_step, "%d,\n", find());
    }
    else
    {
        TakeLocation = 0;
        fprintf(my_step, "%d,\n", TakeLocation);
    }

    print_board();

    fclose(my_step);
    return 0;
}