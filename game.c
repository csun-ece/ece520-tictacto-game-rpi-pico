#include "game.h"
#include "pico/multicore.h"
#include <stdint.h>

// ----------------------------------------
// GPIO setting functions
// ----------------------------------------

/*
The init_gpio function initializes a set of GPIO (General Purpose Input/Output)
pins by performing the following actions:
  - Looping through an array of GpioConfig structures (gpio) of length len.
  - For each GpioConfig structure in the array, it calls the gpio_init function
with the pin_number field as an argument. This initializes the specified
GPIO pin.
  - Then, it calls the gpio_set_dir function with the pin_number field and the
pin_dir field as arguments. This sets the direction (input or output) of
the specified GPIO pin.
*/
// Define a function named "init_gpio" that takes in two arguments, a pointer to
// a GpioConfig structure and the size of the GpioConfig array
void init_gpio(GpioConfig *gpio, size_t len) {
  // Loop through the length of the GpioConfig array
  for (size_t i = 0; i < len; i++) {
    // Initialize the GPIO specified by the pin number in the current GpioConfig
    // structure
    gpio_init(gpio[i].pin_number);
    // Set the direction of the GPIO specified by the pin number and direction
    // in the current GpioConfig structure
    gpio_set_dir(gpio[i].pin_number, gpio[i].pin_dir);
  }
}

// ----------------------------------------
// Button debounce functions
// ----------------------------------------

/*
The function debounce is a hardware-level implementation to filter out
electrical noise from a button press. It checks if the button state has changed
(has_changed), and if the new state is stable (is_stable). If both conditions
are true, it returns true, indicating a valid button press. If either condition
is false, it returns false, indicating an invalid button press.
*/
// Define a function named "debounce" that takes in a constant volatile BtnState
// structure as an argument
bool debounce(const volatile BtnState btn) {
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

/*
The function is_stable is part of a debouncing routine used to filter out
electrical noise from a button press. It takes two arguments: button
(representing the button pin number), and prev_state (representing the previous
state of the button).
It first waits for a specified time DEBOUNCE_DELAY in microseconds (used
to wait for the electrical noise to settle), and then it reads the current state
of the button using the gpio_get function.
If the previous state of the button is HIGH and the current state is also HIGH,
it returns true and outputs a message indicating that the button state is
stable. If the two states do not match, it returns false.
*/
// Define a function named "is_stable" that takes in a constant uint
// (representing the button pin number) and a constant bool (representing the
// previous state of the button)
bool is_stable(const uint button, const bool prev_state) {
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

/*
The function has_changed takes two arguments: prev_state and curr_state, both of
type bool.
It compares the previous state (prev_state) and the current state (curr_state)
of a button and returns true if the previous state is LOW and the current state
is HIGH. This indicates that the button has been pressed (transition from LOW to
HIGH).
If the button state has changed, the function also outputs a message indicating
that the button state has changed. If the two states are the same, it returns
false.
*/
// Define a function named "has_changed" that takes in two arguments, the
// previous state and current state of the button
bool has_changed(bool prev_state, bool curr_state) {
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

/*
The function update_btn_state updates the state of a button. It takes a pointer
to a BtnState struct, btn, as its argument.
The function checks the current state of the button (btn->curr_state) and
updates the previous state (btn->prev_state) accordingly. If the current state
is 0, it sets the previous state to 0. If the current state is 1, it sets the
previous state to 1.
Finally, it updates the current state of the button (btn->curr_state) by calling
the gpio_get function and passing the button pin number (btn->but_pin) as an
argument.
*/
// Define a function named "update_btn_state" that takes in a pointer to a
// volatile BtnState structure
void update_btn_state(volatile BtnState *btn) {
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

// ----------------------------------------
// Board functions
// ----------------------------------------

/*
The function reset_board resets a two-dimensional game board used for a
two-player game (likely Tic Tac Toe). It takes three arguments: current_player
(a pointer to the current player), moves (a pointer to the number of moves
made), and board (a two-dimensional array representing the game board).
The function initializes all the positions on the game board to an empty state
(EMPTY) by looping through all the rows and columns. It sets the number of moves
made to 0 and the current player to X.
Finally, it calls two functions: print_board (to print the newly reset board)
and print_player_turn (to print the current player's turn).
*/
// Define a function named "reset_board" that takes in pointers to the current
// player, number of moves, the game board, and game over flag
void reset_board(char *current_player, uint *moves, char (*board)[COLS],
                 bool *is_game_over) {
  // Print a message indicating that the board is being reset
  printf("Reset board\n");
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

/*
The function get_curr_row takes an integer argument moves and returns the
current row number in a two-dimensional game board, based on the number of moves
made.
It calculates the row number by dividing the number of moves made by 3 and
returning the result as an integer. For example, if moves is 5, get_curr_row
returns 1. This calculation assumes that the game board has 3 ROWS and COLS.
*/
// Define a function named "get_curr_row" that takes in the number of moves made
// in the game
uint get_curr_row(const uint moves) {
  // Return the current row in the game board by dividing the number of moves by
  // the number of rows
  return (uint)(moves) / ROWS;
}

/*
The function get_next_row takes an integer argument moves and returns the next
row number in a two-dimensional game board, based on the number of moves made.
It calculates the next row number by adding 1 to the number of moves made,
dividing the result by 3, and returning the result as an integer. For example,
if moves is 5, get_next_row returns 2. This calculation assumes that the game
board has 3 ROWS and COLS.
*/
// Define a function named "get_next_row" that takes in the number of moves made
// in the game
uint get_next_row(const uint moves) {
  // Return the next row in the game board by dividing the number of moves plus
  // one by the number of rows
  return (uint)(moves + 1) / ROWS;
}

/*
The function get_next_col takes an integer argument moves and returns the next
column number in a two-dimensional game board, based on the number of moves
made.
It calculates the next column number by adding 1 to the number of moves made,
dividing the result by 3, and returning the remainder as an integer. For
example, if moves is 5, get_next_col returns 2. This calculation assumes that
the game board has 3 ROWS and COLS.
*/
// Define a function named "get_next_col" that takes in the number of moves made
// in the game
uint get_next_col(const uint moves) {
  // Return the next column in the game board by taking the remainder of the
  // number of moves plus one divided by the number of columns
  return (uint)(moves + 1) % COLS;
}

/*
The function get_curr_col takes an integer argument moves and returns the
current column number in a two-dimensional game board, based on the number of
moves made.
It calculates the current column number by dividing the number of moves made by
3 and returning the remainder as an integer. For example, if moves is 5,
get_curr_col returns 2. This calculation assumes that the game board has 3
 ROWS and COLS.
*/
// Define a function named "get_curr_col" that takes in the number of moves made
// in the game
uint get_curr_col(const uint moves) {
  // Return the current column in the game board by taking the remainder of the
  // number of moves divided by the number of columns
  return (uint)(moves) % COLS;
}

/*
The function update_position takes a pointer to an integer moves and updates the
current position on a two-dimensional game board based on the number of moves
made.
It first calls the functions get_next_row and get_next_col to calculate the next
row and column numbers in the board, respectively. It then checks if the next
row and column numbers are valid by calling the function is_valid_pos. If the
next position is valid, the function increments the number of moves made. If the
next position is not valid, the function prints a message indicating that the
end of the board has been reached and resets the number of moves to 0, starting
from the top-left corner of the board.
*/
// Increment the move counter to update the current position of game
void update_position(uint *moves) {
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
    // Print message to indicate start of new round
    printf("End of board. Starting from top.\n");
#endif
    // Reset the move counter
    *moves = 0;
  }
}

/*
The function print_curr_pos takes two integers row and col and prints the values
of row and col to the console. The message "Row: %u Col: %u\n" is printed, where
%u represents the values of row and col as unsigned integers.
*/
// Define a function called "print_curr_pos" that takes in two unsigned
// integers, "row" and "col"
void print_curr_pos(const uint row, const uint col) {
  // Print a string "Row: %u Col: %u\n" with the values of "row" and "col"
  // replacing the placeholders %u
  printf("Row: %u Col: %u\n", row, col);
}

/*
The function is_valid_pos takes two integers row and col as arguments and
returns a boolean value indicating if the given row and col are within the valid
range of the two-dimensional array. The function returns true if both row >= 0
and row < ROWS (where ROWS is a constant value), and col >= 0 and col < COLS
(where COLS is a constant value), are true, otherwise it returns false.
*/
// Define a function to check if the row and column values passed as arguments
// are within the limits of the Tic Tac Toe board ROWS and COLS are macro
// constants representing the number of rows and columns in the Tic Tac Toe
// board
bool is_valid_pos(const uint row, const uint col) {
  // Return true if both row and column are greater than or equal to 0 and less
  // than ROWS and COLS respectively
  return (row >= 0 && row < ROWS && col >= 0 && col < COLS);
}

/*
The is_empty_pos function checks if the specified position on the board is
empty, i.e., its value is EMPTY. The position is specified by the row and col
arguments. The function returns true if the position is empty, and false
otherwise.
*/
bool is_empty_pos(uint const row, uint const col, const char (*board)[COLS]) {
  // return true if the cell is empty otherwise return false
  return (board[row][col] == EMPTY);
}

/*
The function update_board updates the tic-tac-toe board with the current
player's symbol (either 'X' or 'O') at the position specified by moves count.
The row and col values are calculated from the moves count using the functions
get_curr_row and get_curr_col. If the calculated position is a valid position on
the board, then the current player's symbol is entered into that position on the
board, and a message indicating this is printed to the console.
*/
// Declare a function named "update_board" that takes in the current player as a
// char, number of moves as an unsigned int, and a 2D character array "board"
void update_board(const char current_player, const uint moves,
                  char (*board)[COLS]) {
  // Calculate the row by calling "get_curr_row" function with the number of
  // moves
  uint row = get_curr_row(moves);
  // Calculate the col by calling "get_curr_col" function with the number of
  // moves
  uint col = get_curr_col(moves);
  // Print the current player and the row and col where the player's input is
  // being entered
  printf("Entering player %c input into row %d col %d\n", current_player, row,
         col);
  // Update the board at the calculated row and col with the current player's
  // input
  board[row][col] = current_player;
}

/*
This function print_board takes a 2D character array board as input and prints
it in a Tic Tac Toe board format. The board is displayed as three rows and three
columns separated by | symbols and lines with + symbols. The value of each cell
of the board is displayed inside each cell.
*/
// Declare a function named "print_board" that takes in a 2D character array
// "board"
void print_board(const char (*board)[COLS]) {
  // Print the first row of the board
  printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
  // Print the separator line
  printf("---+---+---\n");
  // Print the second row of the board
  printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
  // Print the separator line
  printf("---+---+---\n");
  // Print the third row of the board
  printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
}

/*
The function print_player_turn takes a single argument current_player which is a
character and it prints a message indicating whose turn it is to play in the
game. The message includes the value of current_player and the text "turn".
*/
void print_player_turn(const char current_player) {
  // Print the "Player %c turn\n", current_player message
  printf("Player %c turn\n", current_player);
}

// ----------------------------------------
// LED Handle functions
// ----------------------------------------

/*
The update_player_led function updates the state of two LEDs (LED1 and LED2)
based on the current turn of the player in a game of Tic-Tac-Toe. If the current
player is X, then LED1 is set to HIGH and LED2 is set to LOW. If the current
player is O, then LED1 is set to LOW and LED2 is set to HIGH. If the current
player is neither X nor O, then both LED1 and LED2 are set to HIGH.
*/
// Declare a function named "update_player_led" that takes in the current player
// as a char
void update_player_led(const char current_player) {
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

/*
The flash_winner_led function is a function that flashes a LED to indicate the
winner of a game. The function loops continuously and waits for data to be
available in a multicore FIFO. If data is available, the function retrieves the
winner of the game by popping the data from the FIFO. Based on the winner, the
function selects an LED (LED1, LED2, or ONBOARD_LED) to flash. The selected LED
is set to a high state for a specified amount of time (BLINK_LED_DELAY) and then
set to a low state for the same amount of time.
*/
void flash_winner_led() {
  // Declare a variable "winner" of type uint32_t and initialize it to the value
  // of "EMPTY"
  uint32_t winner = (uint32_t)EMPTY;
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

// ----------------------------------------
// Button Handle functions
// ----------------------------------------

/*
The function handle_btn1 increments the value of the moves variable and then
prints the row and column number corresponding to the updated value of moves. It
does this by calling the update_position and print_curr_pos functions and
passing moves as the argument.
*/
void handle_btn1(uint *moves) {
  // Update the position of moves
  update_position(moves);
  // Call the function "get_curr_row" with the parameter "moves" and store the
  // result in a variable "curr_row"
  // Call the function "get_curr_col" with the parameter "moves" and store the
  // result in a variable "curr_col"
  // Call the function "print_curr_pos" with parameters "curr_row" and
  // "curr_col" to print the updated position
  print_curr_pos(get_curr_row(*moves), get_curr_col(*moves));
}

/*
The function handle_btn2 is a handler for an event that occurs when a button
(presumably button 2) is pressed. It does the following:

  - Gets the current row and column of the game board from the value of the
moves argument.
  - If the row and column are not valid (i.e. they are out of bounds), it prints
an error message and returns.
  - If the position is not empty (i.e. there is already a player piece there),
it prints an error message and returns.
  - Updates the board with the current player's piece at the current position.
  - Prints the updated board.
  - Checks if the current player has won the game. If so, it prints a win
message and resets the game.
  - If there is no winner, checks if the game has ended in a tie. If so, it
prints a tie message and resets the game.
  - If the game is not over, it updates the current player, resets the moves
value to 0, and prints the turn for the next player.
*/
void handle_btn2(char *current_player, uint *moves, char (*board)[COLS],
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
    printf("Invalid selection row %d col %d", row, col);
    // Return from the function
    return;
  }

  // Check if the position (row, col) is empty
  if (!is_empty_pos(row, col, board)) {
    // If the position is not empty, print a message "row %d col %d is not
    // empty" with row and col values Print another message "Please select
    // another location."
    printf("row %d col %d is not empty.\n", row, col);
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
    // Print messages "Please press reset button to start the game." and
    // "Waiting for the reset ..."
    printf("\nPlease press reset button to start the game.\n");
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

// ----------------------------------------
// Game status functions
// ----------------------------------------

/*
The function get_new_player takes in a char type argument current_player which
represents the current player. The function returns O if the current player is X
and returns X if the current player is O. The purpose of the function is to
switch the player from one to the other.
*/
char get_new_player(char current_player) {
  // Return the next player symbol
  return (current_player == X) ? O : X;
}

/*
This function is_win takes two arguments, player and board, and returns a
boolean value indicating whether the given player has won the game by checking
the board.
The function checks each row, each column, and both diagonals for the presence
of three consecutive squares filled by the player. If any of the checks returns
true, it means the player has won the game, so the function returns true. If all
the checks fail, the function returns false meaning the player has not won the
game.
*/
// Check if the given player has won the game
bool is_win(const char player, const char (*board)[COLS]) {
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

/*
The function is_tie returns true if all positions of the Tic-tac-toe board
represented by 2D array board are filled with either X or O, and false
otherwise. It checks each position of the board with two nested for loops, and
if there's an empty position, it returns false immediately. If all positions are
filled and the function has not returned yet, it returns true.
*/
bool is_tie(const char (*board)[COLS]) {
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
