/*
 * The program that loads the maze will find the way to the exit and display the result
 *
 * Written by Daryush Tsydzik
 */

#include <stdio.h>
#include <stdlib.h>

#define SIZE 31

int load_maze(const char *filename, char ***labirynth);
int solve_maze(char **maze, int x, int y);
void free_maze(char **maze);

int main() {
    char *filename = malloc(SIZE * sizeof(char));
    if (filename == NULL) {
        printf("Failed to allocate memory");
        return 8;
    }

    printf("Enter file name: ");
    scanf("%30s", filename);

    char **labyrinth = NULL;
    switch (load_maze(filename, &labyrinth)) {
        case 2:
            printf("Couldn't open file");
            free(filename);
            filename = NULL;
            return 4;
        case 3:
            printf("File corrupted");
            free(filename);
            filename = NULL;
            return 6;
        case 4:
            printf("Failed to allocate memory");
            free(filename);
            filename = NULL;
            return 8;
        default:
            free(filename);
            filename = NULL;
            break;
    }

    int x = 0, y = 0;
    while (*(labyrinth + y) != NULL && *(*(labyrinth + y) + x) != 'a') {
        x = 0;
        while (*(*(labyrinth + y) + x) != '\0') {
            if (*(*(labyrinth + y) + x) == 'a') {
                y--;
                break;
            }
            x++;
        }
        y++;
    }

    switch (solve_maze(labyrinth, x, y)) {
        case 0:
            printf("Couldn't find path");
            break;
        case -1:
            printf("File corrupted");
            free_maze(labyrinth);
            return 6;
        default:
            y = 0;
            while (*(labyrinth + y) != NULL) {
                printf("%s\n", *(labyrinth + y));
                y++;
            }
            break;
    }

    free_maze(labyrinth);

    return 0;
}

int load_maze(const char *filename, char ***labirynth) {
    if (filename == NULL || labirynth == NULL)
        return 1;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 2;

    // find height and check characters
    unsigned int height = 0, width = 0, i = 0;
    char ch;
    while (!feof(file)) {
        ch = (char)fgetc(file);
        if (ch == '\n' || feof(file)) {
            if (height && i - 1 != width || height == 0 && width == 0) {
                fclose(file);
                return 3;
            }
            height++, i = 0;
        }
        else if (ch != '#' && ch != ' ' && ch != 'a' && ch != 'b') {
            fclose(file);
            return 3;
        }
        else if (!height)
            width++;
        i++;
    }
    width++;

    fseek(file, 0, SEEK_SET);

    // alloc memory for labyrinth
    *labirynth = malloc((height + 1) * sizeof(char*));
    if (*labirynth == NULL) {
        fclose(file);
        return 4;
    }
    *(*labirynth + height) = NULL;

    i = 0;
    while (i < height) {
        // alloc memory for row
        *(*labirynth + i) = malloc(width * sizeof(char));
        if (*(*labirynth + i) == NULL) {
            for (unsigned int j = 0; j < i; j++) {
                free(*(*labirynth + j));
                *(*labirynth + j) = NULL;
            }
            free(*labirynth);
            *labirynth = NULL;
            fclose(file);
            return 4;
        }

        // read row to 2D labyrinth
        unsigned int j;
        for (j = 0; j < width - 1; j++) {
            *(*(*labirynth + i) + j) = (char) fgetc(file);
        }
        fgetc(file);
        *(*(*labirynth + i) + j) = '\0';
        i++;
    }

    fclose (file);

    // check borders
    i = 0;
    while (i < height) {
        if (i == 0 || i == height - 1) {
            for (unsigned int j = 0; j < width - 1; j++)
                if (*(*(*labirynth + i) + j) != '#') {
                    free_maze(*labirynth);
                    return 3;
                }
        }
        else if (**(*labirynth + i) != '#' || *(*(*labirynth + i) + width - 2) != '#'){
            free_maze(*labirynth);
            return 3;
        }
        i++;
    }

    return 0;
}

int solve_maze(char **maze, int x, int y) {
    if (maze == NULL || *maze == NULL || x < 1 || y < 1)
        return -1;

    int height = 0, width = 0, k, b = 0;

    // find height and width
    while (*(maze + height) != NULL)
        if (!height++)
            while (*(*maze + width) != '\0') width++;

    // check
    for (int i = 0; i < height; i++) {
        if (i == 0 || i == height - 1) {
            for (int j = 0; j < width - 1; j++)
                if (*(*(maze + i) + j) != '#')
                    return -1;
        }
        else if (**(maze + i) != '#' || *(*(maze + i) + width - 1) != '#')
            return -1;
        for (int j = 0; j < width; j++) {
            if (*(*(maze + i) + j) != '#' && *(*(maze + i) + j) != ' ' && *(*(maze + i) + j) != 'a' && *(*(maze + i) + j) != 'b')
                return -1;

            b += *(*(maze + i) + j) == 'b';
        }
    }
    if (b != 1) return -1;

    if (x > width - 2 || y > height - 2)
        return -1;

    while (*(*(maze + y) + x) != 'b') {

        if (*(*(maze + y) + x - 1) == 'b') {
            if (*(*(maze + y) + x) != 'a')
                *(*(maze + y) + x) = '*';
            x -= 1;
            continue;
        }
        else if (*(*(maze + y - 1) + x) == 'b') {
            if (*(*(maze + y) + x) != 'a')
                *(*(maze + y) + x) = '*';
            y -= 1;
            continue;
        }
        else if (*(*(maze + y) + x + 1) == 'b') {
            if (*(*(maze + y) + x) != 'a')
                *(*(maze + y) + x) = '*';
            x += 1;
            continue;
        }
        else if (*(*(maze + y + 1) + x) == 'b') {
            if (*(*(maze + y) + x) != 'a')
                *(*(maze + y) + x) = '*';
            y += 1;
            continue;
        }

        k = 0;
        if (*(*(maze + y) + x - 1) != ' ') k++;
        if (*(*(maze + y - 1) + x) != ' ') k++;
        if (*(*(maze + y) + x + 1) != ' ') k++;
        if (*(*(maze + y + 1) + x) != ' ') k++;

        if (k == 4) {

            // clear
            if (*(*(maze + y) + x - 1) == 'X') *(*(maze + y) + x - 1) = ' ';
            if (*(*(maze + y - 1) + x) == 'X') *(*(maze + y - 1) + x) = ' ';
            if (*(*(maze + y) + x + 1) == 'X') *(*(maze + y) + x + 1) = ' ';
            if (*(*(maze + y + 1) + x) == 'X') *(*(maze + y + 1) + x) = ' ';

            // way back
            if (*(*(maze + y) + x - 1) == '*') *(*(maze + y) + x) = 'X',k = 1, x -= 1;
            else if (*(*(maze + y - 1) + x) == '*') *(*(maze + y) + x) = 'X',k = 1, y -= 1;
            else if (*(*(maze + y) + x + 1) == '*') *(*(maze + y) + x) = 'X',k = 1, x += 1;
            else if (*(*(maze + y + 1) + x) == '*') *(*(maze + y) + x) = 'X',k = 1, y += 1;

            // if back
            if (k == 1)
                *(*(maze + y) + x) = ' ';
            else return 0;

        }
        else {

            // go
            if (*(*(maze + y) + x) != 'a')
                *(*(maze + y) + x) = '*';

            if (*(*(maze + y) + x - 1) == ' ') x -= 1;
            else if (*(*(maze + y - 1) + x) == ' ') y -= 1;
            else if (*(*(maze + y) + x + 1) == ' ') x += 1;
            else if (*(*(maze + y + 1) + x) == ' ') y += 1;

        }
    }

    // clear path
    int last_x = x, last_y = y;
    while (*(*(maze + y) + x) != 'a') {

        // clear
        if (*(*(maze + y) + x - 1) == 'X') *(*(maze + y) + x - 1) = ' ';
        if (*(*(maze + y - 1) + x) == 'X') *(*(maze + y - 1) + x) = ' ';
        if (*(*(maze + y) + x + 1) == 'X') *(*(maze + y) + x + 1) = ' ';
        if (*(*(maze + y + 1) + x) == 'X') *(*(maze + y + 1) + x) = ' ';

        // way back
        if (x - 1 != last_x && (*(*(maze + y) + x - 1) == '*' || *(*(maze + y) + x - 1) == 'a')) last_x = x, last_y = y, x -= 1;
        else if (y - 1 != last_y && (*(*(maze + y - 1) + x) == '*' || *(*(maze + y - 1) + x) == 'a')) last_y = y, last_x = x, y -= 1;
        else if (x + 1 != last_x && (*(*(maze + y) + x + 1) == '*' || *(*(maze + y) + x + 1) == 'a')) last_x = x, last_y = y, x += 1;
        else if (y + 1 != last_y && (*(*(maze + y + 1) + x) == '*' || *(*(maze + y + 1) + x) == 'a')) last_y = y, last_x = x, y += 1;

    }

        return 1;
}

void free_maze(char **maze) {
    if (maze != NULL) {
        int i;
        for (i = 0; *(maze + i) != NULL; i++) {
            free(*(maze + i));
            *(maze + i) = NULL;
        }
        free(*(maze + i));
        free(maze);
    }
}
