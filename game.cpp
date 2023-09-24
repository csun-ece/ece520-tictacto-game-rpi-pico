#include "game.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

Game::Game() {
  // Initialize any necessary game variables or setup here
  // For example, you can initialize the game board and other variables.
}

// void Game::init_gpio(GpioConfig *gpio, size_t len) {
void Game::init_gpio(GpioConfig (&gpio)[NUMBER_OF_GPIOS]) {
  // Loop through the length of the GpioConfig array
  for (size_t i = 0; i < Game::NUMBER_OF_GPIOS; i++) {
    // Initialize the GPIO specified by the pin number in the current GpioConfig
    // structure
    gpio_init(gpio[i].pin_number);
    // Set the direction of the GPIO specified by the pin number and direction
    // in the current GpioConfig structure
    gpio_set_dir(gpio[i].pin_number, gpio[i].pin_dir);
  }
}

bool Game::debounce(const volatile BtnState& btn) {
  // Check if the previous state and current state of the button have changed
  if (has_changed(btn.prev_state, btn.curr_state)) {
    // Check if the current state of the button is stable
    if (is_stable(btn.but_pin, btn.curr_state)) {
      // Return true if the state has changed and is stable
      return true;
    }
  }
  // Return false if state has not changed or is not stable
  return false;
}

bool Game::is_stable(const uint button, const bool prev_state) {
  // Wait for a specific amount of time (DEBOUNCE_DELAY)
  sleep_us(DEBOUNCE_DELAY);
  // Get the current state of the button
  uint current_state = gpio_get(button);
  // Check if the previous state and current state of the button are both high
  if (prev_state == HIGH && current_state == HIGH) {
    // Optionally print a message if the button state is stable (only if the
    // preprocessor macro "VERBOSE" is defined)
#ifdef VERBOSE
    printf("Button state is stable\n");
#endif
    // Return true if the button state is stable
    return true;
  }
  // Return false if the button state is not stable
  return false;
}

bool Game::has_changed(bool prev_state, bool curr_state) {
  // Check if the previous state is LOW and the current state is HIGH
  bool changed = (prev_state == LOW && curr_state == HIGH);
  // Optionally print a message if the button state has changed (only if the
  // preprocessor macro "VERBOSE" is defined)
#ifdef VERBOSE
  if (changed) {
    printf("Button state changed\n");
  }
#endif
  // Return true if the button state has changed, false otherwise
  return changed;
}

void Game::update_btn_state(volatile BtnState *btn) {
  // Check if the current state of the button is 0
  if (btn->curr_state == 0) {
    // If so, set the previous state of the button to 0
    btn->prev_state = 0;
  }
  // Check if the current state of the button is 1
  else if (btn->curr_state == 1) {
    // If so, set the previous state of the button to 1
    btn->prev_state = 1;
  }
  // Update the current state of the button by reading the button pin
  btn->curr_state = gpio_get(btn->but_pin);
}

void Game::reset_board(char *current_player, uint *moves, char (*board)[COLS],
                       bool *is_game_over) {
  // Print a message indicating that the board is being reset
  cout << "Reset board" << endl;

  // Loop over the rows and columns of the game board
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      // Set each cell of the game board to the "EMPTY" value
      board[i][j] = EMPTY;
    }
  }

  // Reset the number of moves to 0
  *moves = 0;

  // Reset the current player to "X"
  *current_player = X;

  // Reset the game over flag to false
  *is_game_over = false;

  // Call the function "print_board" with the game board as an argument
  print_board(board);

  // Call the function "print_player_turn" with the current player as an
  // argument
  print_player_turn(*current_player);

  // Call the function "multicore_fifo_push_blocking" with "EMPTY" as an
  // argument
  multicore_fifo_push_blocking(EMPTY);
}

uint Game::get_curr_row(const uint moves) {
  // Return the current row in the game board by dividing the number of moves by
  // the number of rows
  return static_cast<uint>(moves) / ROWS;
}

uint Game::get_next_row(const uint moves) {
  // Return the next row in the game board by dividing the number of moves plus
  // one by the number of rows
  return static_cast<uint>(moves + 1) / ROWS;
}

uint Game::get_next_col(const uint moves) {
  // Return the next column in the game board by taking the remainder of the
  // number of moves plus one divided by the number of columns
  return static_cast<uint>(moves + 1) % COLS;
}

uint Game::get_curr_col(const uint moves) {
  // Return the current column in the game board by taking the remainder of the
  // number of moves divided by the number of columns
  return static_cast<uint>(moves) % COLS;
}

void Game::update_position(uint *moves) {
  // Calculate the next row and column of the board
  uint next_row = get_next_row(*moves);
  uint next_col = get_next_col(*moves);
  // Check if the next position is within the valid range of the board
  if (is_valid_pos(next_row, next_col)) {
    // Increment the move counter
    (*moves)++;
  } else {
    // If the end of the board is reached, start again from the top
#ifdef VERBOSE
    // Print message to indicate start of a new round
    printf("End of board. Starting from top.\n");
#endif
    // Reset the move counter
    *moves = 0;
  }
}

void Game::print_curr_pos(const uint row, const uint col) {
  // Print a string "Row: %u Col: %u\n" with the values of "row" and "col"
  // replacing the placeholders %u
  cout << "Row: " << row << " Col: " << col << endl;
}

bool Game::is_valid_pos(const uint row, const uint col) {
  // Return true if both row and column are greater than or equal to 0 and less
  // than ROWS and COLS respectively
  return (row < ROWS && col < COLS);
}

bool Game::is_empty_pos(const uint row, const uint col,
                        const char (*board)[COLS]) {
  // Return true if the cell is empty (contains the EMPTY character) otherwise
  // return false
  return (board[row][col] == EMPTY);
}

void Game::update_board(const char current_player, const uint moves,
                        char (*board)[COLS]) {
  // Calculate the row by calling "get_curr_row" function with the number of
  // moves
  uint row = get_curr_row(moves);
  // Calculate the col by calling "get_curr_col" function with the number of
  // moves
  uint col = get_curr_col(moves);
  // Print the current player and the row and col where the player's input is
  // being entered
  cout << "Entering player " << current_player << " input into row " << row << " col " << col << endl;
  // Update the board at the calculated row and col with the current player's
  // input
  board[row][col] = current_player;
}

void Game::print_board(const char (*board)[COLS]) {
  // ANSI escape codes to clear the console and move the cursor to the top
  cout << "\e[1;1H\e[2J";

  // Print the board using a loop to avoid duplicating code
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      // Print the cell value
      cout << " "  << board[i][j] << " ";
      // Print vertical separators except for the last column
      if (j < COLS - 1) {
        cout << "|"; 
      }
    }
    // Print horizontal separators except for the last row
    if (i < ROWS - 1) {
      cout << endl << "---+---+---" << endl;
    } else {
      cout << endl;
    }
  }
}

void Game::print_player_turn(const char current_player) {
  // Print the "Player %c turn" message
  cout << "Player " << current_player << " turn" << endl;
}

void Game::update_player_led(const char current_player) {
  // Check if the current player is "X"
  if (current_player == X) {
    // Turn on LED1 and turn off LED2 if the current player is "X"
    gpio_put(LED1, HIGH);
    gpio_put(LED2, LOW);
  }
  // Check if the current player is "O"
  else if (current_player == O) {
    // Turn off LED1 and turn on LED2 if the current player is "O"
    gpio_put(LED1, LOW);
    gpio_put(LED2, HIGH);
  }
  // If the current player is neither "X" nor "O"
  else {
    // Turn off both LED1 and LED2
    gpio_put(LED1, LOW);
    gpio_put(LED2, LOW);
  }
}

void Game::flash_winner_led() {
  // Declare a variable "winner" of type uint32_t and initialize it to the value
  // of "EMPTY"
  uint32_t winner = static_cast<uint32_t>(EMPTY);
  // Declare a variable "led_pin" of type uint and initialize it to the value of
  // "ONBOARD_LED"
  uint led_pin = ONBOARD_LED;

  // Start an infinite loop
  while (true) {

    // Check if the function "multicore_fifo_rvalid()" returns true
    if (multicore_fifo_rvalid()) {
      // Pop a value from the "multicore_fifo" and store it in the variable
      // "winner"
      winner = multicore_fifo_pop_blocking();
    }

    // Check if the value stored in "winner" is equal to 'X'
    if ((char)winner == X) {
      // If yes, set the value of "led_pin" to "LED1"
      led_pin = LED1;
      // Check if the value stored in "winner" is equal to 'O'
    } else if ((char)winner == O) {
      // If yes, set the value of "led_pin" to "LED2"
      led_pin = LED2;
    } else {
      // If the value of "winner" is neither 'X' nor 'O', set the value of
      // "led_pin" to "ONBOARD_LED"
      led_pin = ONBOARD_LED;
    }
    // Set the value of the "led_pin" to "HIGH"
    gpio_put(led_pin, HIGH);
    // Sleep for "BLINK_LED_DELAY" milliseconds
    sleep_ms(BLINK_LED_DELAY);
    // Set the value of the "led_pin" to "LOW"
    gpio_put(led_pin, LOW);
    // Sleep for "BLINK_LED_DELAY" milliseconds
    sleep_ms(BLINK_LED_DELAY);
  }
}

void Game::handle_btn1(uint *moves) {
  // Update the position of moves
  update_position(moves);
  // Call the function "get_curr_row" with the parameter "moves" and store the
  // result in a variable "curr_row"
  uint curr_row = get_curr_row(*moves);
  // Call the function "get_curr_col" with the parameter "moves" and store the
  // result in a variable "curr_col"
  uint curr_col = get_curr_col(*moves);
  // Call the function "print_curr_pos" with parameters "curr_row" and
  // "curr_col" to print the updated position
  print_curr_pos(curr_row, curr_col);
}

void Game::handle_btn2(char *current_player, uint *moves, char (*board)[COLS],
                       bool *is_game_over) {
  // Call the function "get_curr_row" with the parameter "moves" and store the
  // result in a variable "row"
  uint row = get_curr_row(*moves);
  // Call the function "get_curr_col" with the parameter "moves" and store the
  // result in a variable "col"
  uint col = get_curr_col(*moves);

  // Check if the position (row, col) is valid
  if (!is_valid_pos(row, col)) {
    // If the position is not valid, print a message "Invalid selection row %d
    // col %d" with row and col values
    printf("Invalid selection row %d col %d\n", row, col);
    // Return from the function
    return;
  }

  // Check if the position (row, col) is empty
  if (!is_empty_pos(row, col, board)) {
    // If the position is not empty, print a message "row %d col %d is not
    // empty" with row and col values
    printf("row %d col %d is not empty.\n", row, col);
    // Print another message "Please select another location."
    printf("Please select another location.\n");
    // Return from the function
    return;
  }

  // Call the function "update_board" with parameters *current_player, *moves,
  // board to update the board
  update_board(*current_player, *moves, board);
  // Call the function "print_board" with parameter board to print the board
  print_board(board);

  // Check if there's a win
  if (is_win(*current_player, board)) {
    // If there's a win, print a message "Player %c wins!" with *current_player
    printf("Player %c wins!\n", *current_player);
    // Call the function "multicore_fifo_push_blocking" with parameter
    // *current_player to push the winner
    multicore_fifo_push_blocking(*current_player);
    // Set *is_game_over to true
    *is_game_over = true;
    // Print messages "Please press the reset button to start the game." and
    // "Waiting for the reset ..."
    printf("\nPlease press the reset button to start the game.\n");
    printf("\nWaiting for the reset ...\n");
  } else if (is_tie(board)) {
    // If it's a tie game, print the message "Tie game!"
    printf("Tie game!\n");
    // Call the function "reset_board" with parameters "current_player",
    // "moves", "board", and "is_game_over"
    reset_board(current_player, moves, board, is_game_over);
  } else {
    // If there's no win or tie, set *moves to 0
    *moves = 0;
    // Call the function "get_new_player" with parameter *current_player and
    // store the result in *current_player
    *current_player = get_new_player(*current_player);

    // Call the function `print_player_turn` to print which player's turn it is
    print_player_turn(*current_player);
  }
}

char Game::get_new_player(char current_player) {
  // Return the next player symbol
  return (current_player == X) ? O : X;
}

bool Game::is_win(const char player, const char (*board)[COLS]) {
  // Check rows
  for (int i = 0; i < ROWS; i++) {
    // Check if all elements in the row are equal to the player
    if (board[i][0] == player && board[i][1] == player &&
        board[i][2] == player) {
      // Player wins
      return true;
    }
  }
  // Check columns
  for (int j = 0; j < COLS; j++) {
    // Check if all elements in the column are equal to the player
    if (board[0][j] == player && board[1][j] == player &&
        board[2][j] == player) {
      // Player wins
      return true;
    }
  }
  // Check diagonals
  // Check if all elements in the first diagonal are equal to the player
  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
    // Player wins
    return true;
  }
  // Check if all elements in the second diagonal are equal to the player
  if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
    // Player wins
    return true;
  }
  // Player has not won
  return false;
}

bool Game::is_tie(const char (*board)[COLS]) {
  // Iterate over each row of the board
  for (int i = 0; i < ROWS; i++) {
    // Iterate over each column of the board
    for (int j = 0; j < COLS; j++) {
      // Check if the current cell is empty
      if (board[i][j] == EMPTY) {
        // Return false if the cell is empty
        return false;
      }
    }
  }
  // Return true if all cells are filled
  return true;
}
