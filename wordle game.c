#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define WORDLEN 5
#define MAXWORDS 5000

int loadDictionary(char words[][WORDLEN+1]) {
    FILE *f = fopen("words.txt", "r");
    char line[100];
    int count = 0;
    int len, i;
    int ok;

    if (!f) {
        printf("Cannot open words.txt\n");
        return 0;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        len = strlen(line);

        if (line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (strlen(line) == WORDLEN) {
            ok = 1;
            for (i = 0; i < WORDLEN; i++) {
                if (!isalpha(line[i]))
                    ok = 0;
                line[i] = tolower(line[i]);
            }
            if (ok) {
                strcpy(words[count], line);
                count++;
            }
        }
    }

    fclose(f);
    return count;
}

int exists(char words[][WORDLEN+1], int n, char guess[]) {
    int i;
    for (i = 0; i < n; i++) {
        if (strcmp(words[i], guess) == 0)
            return 1;
    }
    return 0;
}

void feedback(char target[], char guess[]) {
    int used[WORDLEN] = {0};
    int fb[WORDLEN] = {0};
    int i, j;

    // Greens
    for (i = 0; i < WORDLEN; i++) {
        if (guess[i] == target[i]) {
            fb[i] = 2;
            used[i] = 1;
        }
    }

    // Yellows
    for (i = 0; i < WORDLEN; i++) {
        if (fb[i] == 0) {
            for (j = 0; j < WORDLEN; j++) {
                if (!used[j] && guess[i] == target[j]) {
                    fb[i] = 1;
                    used[j] = 1;
                    break;
                }
            }
        }
    }

    // Print
   for (i = 0; i < WORDLEN; i++) {
    if (fb[i] == 2)      printf("G ");
    else if (fb[i] == 1) printf("Y ");
    else                 printf(". ");
}

    printf("   %s\n", guess);
}

int main() {
    srand(time(NULL));

    char words[MAXWORDS][WORDLEN+1];
    char target[WORDLEN+1];
    char guess[100];
    int n, attempt, i;

    n = loadDictionary(words);
    if (n == 0) {
        printf("Dictionary empty or missing.\n");
        return 1;
    }

    printf("Loaded %d words.\n", n);

    strcpy(target, words[rand() % n]);

    printf("=== WORDLE GAME ===\n");

    for (attempt = 1; attempt <= 6; attempt++) {
        printf("Attempt %d/6: ", attempt);
        scanf("%s", guess);

        if (strlen(guess) != WORDLEN) {
            printf("Word must be 5 letters.\n");
            attempt--;
            continue;
        }

        for (i = 0; i < WORDLEN; i++)
            guess[i] = tolower(guess[i]);

        if (!exists(words, n, guess)) {
            printf("Word not in dictionary.\n");
            attempt--;
            continue;
        }

        feedback(target, guess);

        if (strcmp(target, guess) == 0) {
            printf("You WIN! \n The word was '%s'.\n", target);
            return 0;
        }
    }

    printf("You lost! The word was '%s'.\n", target);

    return 0;
}
