#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f0xx.h"
//#include <time.h>



char ** createArray(int m, int n);
void destroyArray(char** arr);
void initMaze(char** arr, int rows, int cols);
void printMaze(char** arr, int rows, int cols);

void createPath(char **arr, int x, int y, int rows, int cols);

char ** createArray(int m, int n)
{
    char* values = calloc(m*n, sizeof(char));
    char** rows = malloc(m*sizeof(char*));
    for (int i=0; i<m; ++i)
    {
        rows[i] = values + i*n;
    }
    return rows;
}

void destroyArray(char** arr)
{
    free(*arr);
    free(arr);
}

void initMaze(char **arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            arr[i][j] = '|';
        }

    }
    arr[1][1] = ' ';
}

void printMaze(char **arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c", arr[i][j]);
        }
        printf("\n");
    }
}



void createPath(char **arr, int x, int y, int rows, int cols) {
    arr[x][y] = ' ';
    int direction_arr[4] = {0 , 1 ,2, 3};

    for (int i=0; i<4; i++)
    {
        int r = random() & 3;
        int temp = direction_arr[r];
        direction_arr[r] = direction_arr[i];
        direction_arr[i] = temp;
    }

    for (int i=0; i<4; i++)
    {

        int x_direction=0;
        int y_direction=0;
        if (direction_arr[i] == 0) {
            y_direction = -1;
        }
        else if (direction_arr[i] == 1) {
            y_direction = 1;
        }
        else if(direction_arr[i] == 2) {
            x_direction = 1;
        }
        else if(direction_arr[i] == 3) {
            x_direction = -1;
        }


        int x2 = x + (x_direction<<1);
        int y2 = y + (y_direction<<1);
        if ((x2 >= 0 && x2 < rows) && (y2 >= 0 && y2 < cols))
        //if (checkBorders(x2,y2, rows, cols))
        {
            if (arr[x2][y2] == '|')
            {

                arr[x2-x_direction][y2-y_direction] = ' ';
                createPath(arr, x2,y2, rows, cols);
            }
        }
    }


}





char** generate_maze(int num_rows, int num_cols) {
    //printf("Hello, World!\n");
    //srand( time(0) );



    char** arr = createArray(num_rows,num_cols);
    initMaze(arr, num_rows, num_cols);
    createPath(arr, 1, 1, num_rows, num_cols);

    //printMaze(arr, num_rows, num_cols);

    return arr;

    //printf("%c", arr[0][0]);

    /*
    destroyArray(arr);

    return 0;
    */
}
