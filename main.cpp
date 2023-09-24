#include "game.hpp"

int main() {

  char board[Game::ROWS][Game::COLS] = {
      {Game::EMPTY, Game::EMPTY, Game::EMPTY},
      {Game::EMPTY, Game::EMPTY, Game::EMPTY},
      {Game::EMPTY, Game::EMPTY, Game::EMPTY}};

  // Set current player to X
  char current_player = Game::X;

  // Set current number of moves initialized to 0
  uint moves = 0;

  bool is_game_over = false;

  // Array of structs of GPIO configurations
  GpioConfig my_gpio[Game::NUMBER_OF_GPIOS] = {
      {Game::LED1, GPIO_OUT}, {Game::LED2, GPIO_OUT},
      {Game::BTN1, GPIO_IN},  {Game::BTN2, GPIO_IN},
      {Game::BTN3, GPIO_IN},  {Game::ONBOARD_LED, GPIO_OUT}};

  // Struct for button 1 state
  volatile BtnState btn1 = {
      .but_pin = Game::BTN1, // Assuming Game::BTN1 is the correct pin number
      .prev_state = false,
      .curr_state = false};

  // Struct for button 2 state
  volatile BtnState btn2 = {
      .but_pin = Game::BTN2, // Assuming Game::BTN2 is the correct pin number
      .prev_state = false,
      .curr_state = false};

  // Struct for button 3 state
  volatile BtnState btn3 = {
      .but_pin = Game::BTN3, // Assuming Game::BTN3 is the correct pin number
      .prev_state = false,
      .curr_state = false};

  Game game; // Create an instance of the Game class

  // Initialize the standard input/output library
  stdio_init_all();
  multicore_launch_core1(Game::flash_winner_led);

  game.init_gpio(my_gpio);

  // Reset the board for Tic-Tac-Toe game.
  game.reset_board(&current_player, &moves, board, &is_game_over);

  while (true) {
    // Update player status led if the game is not over
    if (!is_game_over) {
      game.update_player_led(current_player);

      // Check and handle button 1 press event
      game.update_btn_state(&btn1);
      if (game.debounce(btn1)) {
        game.handle_btn1(&moves);
      }

      // Check and handle button 2 press event
      game.update_btn_state(&btn2);
      if (game.debounce(btn2)) {
        game.handle_btn2(&current_player, &moves, board, &is_game_over);
      }
    }

    // Check and handle button 3 press event
    game.update_btn_state(&btn3);
    if (game.debounce(btn3)) {
      game.reset_board(&current_player, &moves, board, &is_game_over);
    }
  }

  return 0;
}
