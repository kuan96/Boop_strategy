#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOUND 6

int MY_COLOR;

int within_boundary(int row, int col)
{
    if (row >= 0 && row < BOUND && col >= 0 && col < BOUND)
        return 1;

    return 0;
}

int pushable(int piece1, int piece2)
{
    // p1 推得動 P2
    if (piece1 >= piece2)
        return 1;
    else if (piece1 > 2 && piece2 > 2)
        return 1;
    else if (piece1 <= 2 && piece2 <= 2)
        return 1;
    else
        return 0;
}

int same_color(int piece1, int piece2)
{
    if (piece1 == 0 || piece2 == 0)
        return 0;
    if (piece1 % 2 != piece2 % 2)
        return 0;
    return 1;
}

void init(char *color, char *board_file, int board[BOUND][BOUND], int priority[BOUND][BOUND], int cat_kitten_num[2][2]);
int cat_or_kitten(int cat_kitten_num[2][2], char *step_file);
void choose_location(int board[BOUND][BOUND], int priority[BOUND][BOUND], int cat_kitten_num[2][2], int catOrKitten);
void put(int board[BOUND][BOUND], int row, int col, int piece);
void pick(int board[BOUND][BOUND], int write_to_file, char *step_file);
void print(int board[BOUND][BOUND]);

int main(int argc, char *argv[])
{
    char *color = argv[1];
    char *board_file = argv[2];
    char *step_file = argv[3];

    int board[BOUND][BOUND] = {{0}};
    int priority[BOUND][BOUND] = {{0}};
    int piece[2][2] = {{1, 3}, {2, 4}};
    int cat_kitten_num[2][2] = {{0}};

    init(color, board_file, board, priority, cat_kitten_num);

    int catOrKitten;
    catOrKitten = cat_or_kitten(cat_kitten_num, step_file);

    int putLocation;

    put(board, 1, 1, 2);
    pick(board, 1, step_file);

    print(board);

    return 0;
}

void init(char *color, char *board_file, int board[BOUND][BOUND], int priority[BOUND][BOUND], int cat_kitten_num[2][2])
{
    // 判斷顏色
    if (!strcmp(color, "gray"))
    {
        MY_COLOR = 0;
    }
    else
    {
        MY_COLOR = 1;
    }

    // scan 棋盤
    FILE *myBoard = fopen(board_file, "r");
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            fscanf(myBoard, "%d,", &board[i][j]);
        }
    }

    // 輸入大貓小貓數量
    int o_kitten_num, o_cat_num;
    int g_kitten_num, g_cat_num;
    fscanf(myBoard, "%d,", &cat_kitten_num[0][0]);
    fscanf(myBoard, "%d,", &cat_kitten_num[0][1]);
    fscanf(myBoard, "%d,", &cat_kitten_num[1][0]);
    fscanf(myBoard, "%d,", &cat_kitten_num[1][1]);

    fclose(myBoard);

    // 初始化 priority
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (i == 0 || i == 5 || j == 0 || j == 5)
            {
                priority[i][j] = 0;
            }
            else if (i == 1 || i == 4 || j == 1 || j == 4)
            {
                priority[i][j] = 1;
            }
            else
            {
                priority[i][j] = 2;
            }
        }
    }
}

int cat_or_kitten(int cat_kitten_num[2][2], char *step_file)
{
    FILE *myStep = fopen(step_file, "a");
    int catOrKitten;

    if (cat_kitten_num[MY_COLOR][0] > 0)
    {
        catOrKitten = 2;
    }
    else
    {
        catOrKitten = 1;
    }

    fprintf(myStep, "%d,", catOrKitten);
    fclose(myStep);

    return catOrKitten;
}

void put(int board[BOUND][BOUND], int row, int col, int piece)
{
    board[row][col] = piece;

    int r1, c1, r2, c2;
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (i == 0 && j == 0)
                continue;
            r1 = row + i, c1 = col + j;
            r2 = r1 + i, c2 = c1 + j;

            if (!within_boundary(r1, c1))
                continue;
            if (!pushable(piece, board[r1][c1]))
                continue;

            if (!within_boundary(r2, c2))
            {
                board[r1][c1] = 0;
            }
            else if (board[r2][c2] == 0)
            {
                board[r2][c2] = board[r1][c1];
                board[r1][c1] = 0;
            }
        }
    }
}

void pick(int board[BOUND][BOUND], int write_to_file, char *step_file)
{
    int remove_location[3] = {0};
    int pick_flag = 0;

    int r, c, r1, c1, r2, c2;
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            for (int m = -1; m < 2; ++m)
            {
                for (int n = -1; n < 2; ++n)
                {
                    if (m == 0 && n == 0)
                        continue;
                    r = i, c = j;
                    r1 = r + m, c1 = c + n;
                    r2 = r - m, c2 = c - n;
                    if (!within_boundary(r1, c1) || !within_boundary(r2, c2))
                        continue;
                    if (same_color(board[r][c], board[r1][c1]) && same_color(board[r][c], board[r2][c2]))
                    {
                        remove_location[0] = r * 6 + c + 1;
                        remove_location[1] = r1 * 6 + c1 + 1;
                        remove_location[2] = r2 * 6 + c2 + 1;
                        board[r][c] = 0;
                        board[r1][c1] = 0;
                        board[r2][c2] = 0;
                        pick_flag = 1;
                    }
                }
            }
        }
    }

    int count = 0;
    int row, col;
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            if (board[i][j] != 0 && board[i][j] % 2 != MY_COLOR % 2)
            {
                count++;
                row = i, col = j;
            }
        }
    }
    if (count > 7)
    {
        remove_location[0] = row * 6 + col + 1;
        pick_flag = 2;
    }

    if (write_to_file)
    {
        FILE *myStep = fopen(step_file, "a");
        switch (pick_flag)
        {
        case 0:
            fprintf(myStep, "%d,\n", 0);
            break;
        case 1:
            fprintf(myStep, "%d,%d,%d,\n", remove_location[0], remove_location[1], remove_location[2]);
            break;
        case 2:
            fprintf(myStep, "%d,\n", remove_location[0]);
            break;
        default:
            break;
        }
        fclose(myStep);
    }
}

void print(int board[BOUND][BOUND])
{
    for (int i = 0; i < BOUND; ++i)
    {
        for (int j = 0; j < BOUND; ++j)
        {
            printf("%2d", board[i][j]);
        }

        printf("\n");
    }
}
