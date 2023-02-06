#include "game.h"

#define NUMBER_OF_GPIOS 6 // Number of GPIOs used in this project
// Main function
int main() {
  //  initializes a 2D array board with dimensions ROWS x COLS with all elements
  //  set to the constant EMPTY.
  char board[ROWS][COLS] = {
      {EMPTY, EMPTY, EMPTY}, 
      {EMPTY, EMPTY, EMPTY}, 
      {EMPTY, EMPTY, EMPTY}};

  // Set current player to X
  char current_player = X;
  // Set current number of moves initialized to 0
  uint moves = 0;

  bool is_game_over = false;
  // Array of structs of GPIO configurations
  GpioConfig my_gpio[NUMBER_OF_GPIOS] = {{LED1, GPIO_OUT},
                                         {LED2, GPIO_OUT},
                                         {BTN1, GPIO_IN},
                                         {BTN2, GPIO_IN},
                                         {BTN3, GPIO_IN},
                                         {ONBOARD_LED, GPIO_OUT},
                                         };

  // Struct for button 1 state
  volatile BtnState btn1 = {
      .but_pin = BTN1,
      .prev_state = false,
      .curr_state = false,
  };

  // Struct for button 2 state
  volatile BtnState btn2 = {
      .but_pin = BTN2,
      .prev_state = false,
      .curr_state = false,
  };

  // Struct for button 3 state
  volatile BtnState btn3 = {
      .but_pin = BTN3,
      .prev_state = false,
      .curr_state = false,
  };

  // Initialize the standard input/output library
  stdio_init_all();
  multicore_launch_core1(flash_winner_led);
  // Set GPIOs for our program
  init_gpio(my_gpio, NUMBER_OF_GPIOS);
  // Reset the board for Tic-Tac-Toe game.
  reset_board(&current_player, &moves, board, &is_game_over);

  while (true) {

    // Update player status led
    if (!is_game_over) {
      update_player_led(current_player);
      // Update button 1 state
      update_btn_state(&btn1);
      // Check if button 1 was pressed and debounced
      if (debounce(btn1)) {
        // Handle button 1 press event
        handle_btn1(&moves);
      }
      // Update button 2 state
      update_btn_state(&btn2);
      // Check if button 2 was pressed and debounced
      if (debounce(btn2)) {
        // Handle button 2 press event
        handle_btn2(&current_player, &moves, board, &is_game_over);
      }
    }
    // Update button 3 state
    update_btn_state(&btn3);
    // Check if button 3 was pressed and debounced
    if (debounce(btn3)) {
      // Handle button 3 press event
      reset_board(&current_player, &moves, board, &is_game_over);
    }
  }

  return 0;
}