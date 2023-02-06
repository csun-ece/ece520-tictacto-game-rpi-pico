#ifndef __GAME_H__
#define __GAME_H__

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stddef.h>
#include <stdio.h>

#define ROWS 3                // Number of rows in table
#define COLS 3                // Number of col in table
#define EMPTY ' '             // Default value for empty cells
#define X 'X'                 // Player 1 symbol
#define O 'O'                 // Player 2 symbol
#define DEBOUNCE_DELAY 200000 // Debouncing delay in microseconds
#define BLINK_LED_DELAY 500   // Blink led delay in miliseconds
#define HIGH 1
#define LOW 0

// Define the pin numbers for the on-board LED, LED1, LED2, BTN1, BTN2, and BTN3
#define ONBOARD_LED 25
#define LED1 15
#define LED2 14
#define BTN1 16
#define BTN2 18
#define BTN3 19

// Struct for storing GPIO configuration information
// @field pin_number the number of the pin
// @field pin_dir the direction of the pin
typedef struct {
  uint pin_number;
  uint pin_dir;
} GpioConfig;

// Struct for storing button state information
// @field but_pin the number of the button pin
// @field prev_state the previous state of the button
// @field curr_state the current state of the button
typedef struct {
  uint but_pin;
  bool prev_state;
  bool curr_state;
} BtnState;


// ----------------------------------------
// GPIO setting functions
// ----------------------------------------

// Initializes the GPIOs
/**
 * @brief Initializes the GPIOs
 *
 * @param gpio Pointer to the GPIO configuration structure
 * @param len Length of the GPIO configuration structure
 */
void init_gpio(GpioConfig *gpio, size_t len);

// ----------------------------------------
// Button debounce functions
// ----------------------------------------

// Debouncing function to ensure the button state is stable
/**
 * @brief Debounces the state of a button to ensure it is stable
 *
 * @param btn Pointer to the button state structure
 *
 * @return true If the button state is stable
 * @return false If the button state is not stable
 */
bool debounce(const volatile BtnState btn);

/**
 * @brief Returns whether the button state is stable or not
 *
 * @param button Index of the button
 * @param prev_state Previous state of the button
 *
 * @return true If the button state is stable
 * @return false If the button state is not stable
 */
bool is_stable(const uint button, const bool prev_state);

/**
 * @brief Returns whether the button state has changed or not
 *
 * @param prev_state Previous state of the button
 * @param curr_state Current state of the button
 *
 * @return true If the button state has changed
 * @return false If the button state has not changed
 */
bool has_changed(bool prev_state, bool curr_state);

/**
 * @brief Updates the button state
 *
 * @param btn Pointer to the button state structure
 */
void update_btn_state(volatile BtnState *btn);

// ----------------------------------------
// Board functions
// ----------------------------------------

// Function to update the tic-tac-toe board
/**
 * @brief Resets the board and updates the current player, moves and
 * is_game_over variables
 *
 * @param current_player Pointer to the current player
 * @param moves Pointer to the number of moves
 * @param board Pointer to the tic-tac-toe board
 * @param is_game_over Pointer to the flag indicating whether the game is over
 * or not
 */
void reset_board(char *current_player, uint *moves, char (*board)[COLS],
                 bool *is_game_over);

/**
 * @brief Returns the current row based on the number of moves
 *
 * @param moves Number of moves
 *
 * @return Current row
 */
uint get_curr_row(const uint moves);

/**
 * @brief Returns the next row based on the number of moves
 *
 * @param moves Number of moves
 *
 * @return Next row
 */
uint get_next_row(const uint moves);

/**
 * @brief Returns the next column based on the number of moves
 *
 * @param moves Number of moves
 *
 * @return Next column
 */
uint get_next_col(const uint moves);

/**
 * @brief Get the current column from the number of moves made.
 *
 * @param moves The number of moves made.
 *
 * @return The current column.
 */
uint get_curr_col(const uint moves);

/**
 * @brief Update the current position by incrementing the number of moves made.
 *
 * @param moves The number of moves made.
 */
void update_position(uint *moves);

/**
 * @brief Print the current position on the tic-tac-toe board.
 *
 * @param row The current row.
 * @param col The current column.
 */
void print_curr_pos(const uint row, const uint col);

/**
 * @brief Check if a position on the tic-tac-toe board is valid.
 *
 * @param row The current row.
 * @param col The current column.
 *
 * @return true if the position is valid, false otherwise.
 */
bool is_valid_pos(const uint row, const uint col);

/**
 * @brief Check if a position on the tic-tac-toe board is empty.
 *
 * @param row The current row.
 * @param col The current column.
 * @param board The tic-tac-toe board.
 *
 * @return true if the position is empty, false otherwise.
 */
bool is_empty_pos(uint const row, uint const col, const char (*board)[COLS]);

/**
 * @brief Update the tic-tac-toe board with the current player's move.
 *
 * @param current_player The current player.
 * @param moves The number of moves made.
 * @param board The tic-tac-toe board.
 */
void update_board(const char current_player, const uint moves,
                  char (*board)[COLS]);

/**
 * @brief Print the tic-tac-toe board.
 *
 * @param board The tic-tac-toe board.
 */
void print_board(const char (*board)[COLS]);

/**
 * @brief Print the current player's turn.
 *
 * @param current_player The current player.
 */
void print_player_turn(const char current_player);

/**
 * @brief Update the LED indicating the current player.
 *
 * @param current_player The current player.
 */
void update_player_led(const char current_player);

/**
 * @brief Flash the LED indicating the winner.
 */
void flash_winner_led();

/**
 * @brief Handle button 1 press.
 *
 * @param moves The number of moves made.
 */
void handle_btn1(uint *moves);

/**
 * @brief Handle button 2 press.
 *
 * @param current_player The current player.
 * @param moves The number of moves made.
 * @param board The tic-tac-toe board.
 * @param is_game_over Flag indicating if the game is over.
 */
void handle_btn2(char *current_player, uint *moves, char (*board)[COLS],
                 bool *is_game_over);

// ----------------------------------------
// Game status functions
// ----------------------------------------

/**
 * @brief Get the new player's character.
 *
 * @param current_player The current player's character
 * @return The new player's character.
 */
char get_new_player(char current_player);

/**
 * @brief Check if the given player has won the game.
 *
 * @param player The current player's character
 * @param board The current state of the tic-tac-toe board.
 * @return True if the player has won, False otherwise.
 */
bool is_win(const char player, const char (*board)[COLS]);

/**
 * @brief Check if the game is tied.
 *
 * @param board The current state of the tic-tac-toe board.
 * @return True if the game is tied, False otherwise.
 */
bool is_tie(const char (*board)[COLS]);

#endif