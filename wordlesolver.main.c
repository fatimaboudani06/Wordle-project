#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORDS 1000

//the structs
typedef struct word {
   char theword[6] ;//For storing letters + place for the null character
   int word_length ;//Number of letters in the word
}word;

typedef struct wordlist{
    int count ;//Number of words currently in the list
    word words[MAX_WORDS];//An array of type word, that stores all possible words
}wordlist;
typedef struct choice{
    word theword;//The word that was guessed
    char feedback[5];//An array of five elements representing the state of each character
}choice;

typedef struct gamestate{
   int num_guesses;//Number of attempts
   wordlist words_remaining;//Words remaining after each filter
   choice guesses[6] ;//An array for storing previous guesses with feedback
}gamestate;

// the functions
void init_wordlist(wordlist *wl){
    wl->count =0;
}

void init_gamestate(gamestate *gs){
    gs->num_guesses =0;
    init_wordlist(&gs->words_remaining);
}
void record_guess(gamestate *gs ,const char feedback[5],const char *guess){
    int i = gs->num_guesses;
    //storage the current guess
   strcpy(gs->guesses[i].theword.theword, guess);

    //storage the word length
    gs->guesses[i].theword.word_length = strlen(guess);

    //storage the state of each char
   for(int k = 0; k < 5; k++)
    gs->guesses[i].feedback[k] = feedback[k];

    //increase the number of gusses
    gs->num_guesses++;
}

void compute_feedback(const char *secret, const char *guess, char out[5]) {
    int i, j;

    // Count how many times each letter appears in the secret word
    int secret_count[26] = {0};

    // Step 1: Set everything to gray '.' and count letters in the secret
    for(i = 0; i < 5; i++) {
        out[i] = '.';                     // Default: wrong letter
        secret_count[secret[i] - 'a']++;   // Count occurrences
    }

    // Step 2: Mark GREEN letters first (correct position)
    for(i = 0; i < 5; i++) {
        if(guess[i] == secret[i]) {
            out[i] = 'G';                  // Green
            secret_count[guess[i] - 'a']--; // Use this letter once
        }
    }

    // Step 3: Mark YELLOW letters (right letter, wrong position)
    for(i = 0; i < 5; i++) {
        if(out[i] == 'G')
            continue;   // Skip already-green letters

        if(secret_count[guess[i] - 'a'] > 0) {
            out[i] = 'Y';                  // Yellow
            secret_count[guess[i] - 'a']--; // Use this letter once
        }
    }
}

int load_wordlist_from_file(wordlist *wl){
     int i = 0 ;
     char c;
     char no[100];
     FILE *f = fopen("words.txt","r");

    if (f == NULL){
        printf("erreur");
        return 1;
    }
     wl->count = 0;
  while ((c = fgetc(f)) != EOF) {

        if (c == '\n') {
            no[i]= '\0';
            if (i == 5) {
                strcpy(wl->words[wl->count].theword, no);
                wl->words[wl->count].word_length = 5;
                wl->count++;
            }

            i = 0;  // start the new word
        }
        else {

            no[i] = c;
            i++;
        }
    }

    fclose(f);
    return 0;
}

// This function checks if one candidate word is compatible
// with the guess and its feedback.
bool is_consistent_with_choice(char *candidate, choice *ch) {

    char *guess = ch->theword.theword;
    char *fb = ch->feedback;

    // check each letter from position 0 to 4
    for (int i = 0; i < 5; i++) {

        // Green : letter must be same and same place
        if (fb[i] == 'G') {
            if (candidate[i] != guess[i]) {
                return false;
            }
        }

        // Yellow :  letter exists but different position
        else if (fb[i] == 'Y') {

            // should NOT be in the same place
            if (candidate[i] == guess[i]) {
                return false;
            }

            // must exist in another position
            bool found = false;
            for (int j = 0; j < 5; j++) {
                if (candidate[j] == guess[i] && j != i) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }

        // '.'  : letter does NOT exist anywhere
        else if (fb[i] == '.') {

            for (int j = 0; j < 5; j++) {
                if (candidate[j] == guess[i]) {
                    return false;
                }
            }
        }
    }

    return true;
}

// This function removes all words that are NOT consistent
// with the last guess and its feedback.
void filter_words_remaining(wordlist *wl, choice *ch) {

    int newCount = 0;

    for (int i = 0; i < wl->count; i++) {

        char *candidate = wl->words[i].theword;

        // keep the word only if it's valid
        if (is_consistent_with_choice(candidate, ch)) {
            wl->words[newCount] = wl->words[i];
            newCount++;
        }
    }

    wl->count = newCount; // update number of remaining words
}


int pick_best_guess_index(wordlist *wl) {
    if (wl->count == 0) return -1;

    int idx = rand() % wl->count;
    return idx;
}
// for colors
void print_colored_letter(char letter, char status) {
    // GREEN
    if (status == 'G') {
        printf("\x1b[42m\x1b[30m %c \x1b[0m", letter);
    }
    // YELLOW
    else if (status == 'Y') {
        printf("\x1b[43m\x1b[30m %c \x1b[0m", letter);
    }
    // GRAY
    else {
        printf("\x1b[47m\x1b[30m %c \x1b[0m", letter);
    }
}





int main() {

    srand(time(NULL));

    wordlist wl;
    load_wordlist_from_file(&wl);
    gamestate gs;
    gs.num_guesses = 0;
    gs.words_remaining = wl;

    // choose a random word

    int secret_index = rand() % wl.count;
    char *secret = wl.words[secret_index].theword;

    printf("=== WORDLE ===\n");

    for (int attempt = 0; attempt < 6; attempt++) {
        char guess[10];
        char feedback[5];


     int best_index = pick_best_guess_index(&gs.words_remaining);
        strcpy(guess, gs.words_remaining.words[best_index].theword);
        printf("Attempt %d: %s\n", attempt + 1, guess);


        if (strlen(guess) != 5) {
            printf("Word must be 5 letters!\n");
            attempt--;
            continue;
        }

        compute_feedback(secret, guess, feedback);

        strcpy(gs.guesses[gs.num_guesses].theword.theword, guess);
        gs.guesses[gs.num_guesses].theword.word_length = 5;
        for (int i = 0; i < 5; i++)
            gs.guesses[gs.num_guesses].feedback[i] = feedback[i];
        gs.num_guesses++;

        printf("Feedback: ");
for (int i = 0; i < 5; i++) {
    print_colored_letter(guess[i], feedback[i]);
}
printf("\n");

        // فلترة الكلمات المتبقية
        filter_words_remaining(&gs.words_remaining, &gs.guesses[gs.num_guesses - 1]);
        printf("Remaining words: ");
        for (int i = 0; i < gs.words_remaining.count; i++) {
            printf("%s ", gs.words_remaining.words[i].theword);
        }
        printf("\n");


        if (strcmp(guess, secret) == 0) {
            printf("Congratulations! You found the word.\n");
            break;
        }
    }

    printf("The secret word was: %s\n", secret);
    return 0;
}
