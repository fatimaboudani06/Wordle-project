<img width="1200" height="630" alt="image" src="https://github.com/user-attachments/assets/fb5070ab-e5ae-4329-bd9e-39a39155e97f" />


# Wordle project
This project is a Wordle game with an automatic Solver implemented in C.


## Project Structure 
- wordlesolver.main.c
- words.txt
- wordle game.c
- Wordle Game and Solver Report.pdf

## Project Objective
The goal of this project is to implement a Wordle game and design a solver that can guess the secret word by progressively eliminating invalid candidates based on feedback.

# How It Works
## Wordle Game 
The game selects a secret word from the  word list.
- The player enters a guess at each attempt.
- The game provides feedback for each letter:
  - Correct letter and position (Green ).
  - Correct letter but wrong position(Yellow).
  - Letter not in the word(Gray).
- The game ends when the word is guessed or the attempts are finished.
  
  ## Wordle Solver
- The solver starts by selecting an initial word from a predefined word list.
- After each guess, it receives feedback from the game.
- Based on this feedback, the solver filters the list of possible words.
- Incorrect words are eliminated.
- The solver selects the next word from the remaining candidates.
- This process continues until the correct word is found.
