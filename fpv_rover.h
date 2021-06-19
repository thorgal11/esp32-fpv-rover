#ifndef FPV_ROVER_H
#define FPV_ROVER_H

#include <Arduino.h>

typedef struct {
  int keyL;
  int keyArrowUp;
  int keyArrowLeft;
  int keyArrowRight;
  int keyArrowDown;
} Buttons;


class FPV_Rover
{
  private:
  unsigned long last_motor_change = 0;
  Buttons buttons;
  byte l_pin1, l_pin2, r_pin1, r_pin2, led_pin;
  int led_power, l_track_real, l_track_target, r_track_real, r_track_target;
  
  void change_rover_movement();
  void move_track(int track, int speed);
  int set_tracks_target(int l_speed, int r_speed);
  void attach_pwm_channel_pin(byte pin, int ch);
  
  public:
  FPV_Rover(byte a1, byte a2, byte b1, byte b2, byte l);

  void setButtons(Buttons webButtons);
  
  void move_forwards(int speed);
  void move_forwards_left(int speed);
  void move_forwards_right(int speed);
  void turn_left(int speed);
  void turn_right(int speed);
  void move_backwards(int speed);
  void move_backwards_left(int speed);
  void move_backwards_right(int speed);
  void stop_move();
  
  void set_led(int power);
  int get_led();
  
  int adjust_track_speed();
};

#endif
