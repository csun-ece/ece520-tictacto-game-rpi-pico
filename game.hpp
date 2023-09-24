#ifndef GAME_HPP
#define GAME_HPP

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <cstddef>
#include <cstdio>

struct GpioConfig {
  uint pin_number;
  uint pin_dir;
};

struct BtnState {
  uint but_pin;
  bool prev_state;
  bool curr_state;
};

class Game {
public:
  static const int ROWS = 3;
  static const int COLS = 3;
  static const char EMPTY = ' ';
  static const char X = 'X';
  static const char O = 'O';
  static const int DEBOUNCE_DELAY = 200000;
  static const int BLINK_LED_DELAY = 500;
  static const int HIGH = 1;
  static const int LOW = 0;
  static const int ONBOARD_LED = 25;
  static const int LED1 = 15;
  static const int LED2 = 14;
  static const int BTN1 = 16;
  static const int BTN2 = 18;
  static const int BTN3 = 19;
  static const size_t NUMBER_OF_GPIOS = 6;

  Game();
  void init_gpio(GpioConfig (&gpio)[NUMBER_OF_GPIOS]);
  bool debounce(const volatile BtnState &btn);
  bool is_stable(const uint button, const bool prev_state);
  bool has_changed(bool prev_state, bool curr_state);
  void update_btn_state(volatile BtnState *btn);
  void reset_board(char *current_player, uint *moves, char (*board)[COLS],
                   bool *is_game_over);
  uint get_curr_row(const uint moves);
  uint get_next_row(const uint moves);
  uint get_next_col(const uint moves);
  uint get_curr_col(const uint moves);
  void update_position(uint *moves);
  void print_curr_pos(const uint row, const uint col);
  bool is_valid_pos(const uint row, const uint col);
  bool is_empty_pos(uint const row, uint const col, const char (*board)[COLS]);
  void update_board(const char current_player, const uint moves,
                    char (*board)[COLS]);
  void print_board(const char (*board)[COLS]);
  void print_player_turn(const char current_player);
  void update_player_led(const char current_player);
  void handle_btn1(uint *moves);
  void handle_btn2(char *current_player, uint *moves, char (*board)[COLS],
                   bool *is_game_over);
  char get_new_player(char current_player);
  bool is_win(const char player, const char (*board)[COLS]);
  bool is_tie(const char (*board)[COLS]);

  static void flash_winner_led();
};

#endif // GAME_HPP
