#include "fpv_rover.h"

#define MOTOR_CHANGE_DELAY 5
#define LEFT 0
#define RIGHT 1


FPV_Rover::FPV_Rover(byte a1, byte a2, byte b1, byte b2, byte l) {
  
  l_pin1 = a1;
  l_pin2 = a2;
  r_pin1 = b1;
  r_pin2 = b2;

  led_pin = l;
  led_power = 0;

  l_track_real = 0;
  l_track_target = 0;
  r_track_real = 0;
  r_track_target = 0;

  attach_pwm_channel_pin(l_pin1, 12);
  attach_pwm_channel_pin(l_pin2, 13);
  attach_pwm_channel_pin(r_pin1, 14);
  attach_pwm_channel_pin(r_pin2, 15);
  attach_pwm_channel_pin(led_pin, 11);

  Buttons buttons = {0,0,0,0,0};
}


// set buttons received by webinterface
void FPV_Rover::setButtons(Buttons webButtons) {
  int changeDirection = 0;

  if (buttons.keyArrowUp != webButtons.keyArrowUp) {
    buttons.keyArrowUp = webButtons.keyArrowUp;
    changeDirection = 1;
  }
  if (buttons.keyArrowDown != webButtons.keyArrowDown) {
    buttons.keyArrowDown = webButtons.keyArrowDown;
    changeDirection = 1;
  }
  if (buttons.keyArrowLeft != webButtons.keyArrowLeft) {
    buttons.keyArrowLeft = webButtons.keyArrowLeft;
    changeDirection = 1;
  }
  if (buttons.keyArrowRight != webButtons.keyArrowRight) {
    buttons.keyArrowRight = webButtons.keyArrowRight;
    changeDirection = 1;
  }

  if (changeDirection) {
    change_rover_movement();
  }

  if (buttons.keyL != webButtons.keyL) {
    buttons.keyL = webButtons.keyL;
    if (buttons.keyL) {
      if (!get_led()) {
        set_led(255);
      }
      else {
        set_led(0);
      }
    }
  }
}


// if received buttons affect the movement direction
void FPV_Rover::change_rover_movement() {
  int keyArrowUp = buttons.keyArrowUp;
  int keyArrowDown = buttons.keyArrowDown;
  int keyArrowLeft = buttons.keyArrowLeft;
  int keyArrowRight = buttons.keyArrowRight;

  int sum = keyArrowUp + keyArrowLeft + keyArrowDown + keyArrowRight;

  if ((sum > 2) || (sum == 0) || (keyArrowLeft && keyArrowRight) || (keyArrowUp && keyArrowDown)) {
    stop_move();
  }
  else if (keyArrowUp) {
    if (keyArrowLeft) {
      move_forwards_left(255);
    }
    else if (keyArrowRight) {
      move_forwards_right(255);
    }
    else {
      move_forwards(255);
    }
  }
  else if (keyArrowDown) {
    if (keyArrowLeft) {
      move_backwards_left(255);
    }
    else if (keyArrowRight) {
      move_backwards_right(255);
    }
    else {
      move_backwards(255);
    }
  }
  else if (keyArrowLeft) {
    turn_left(255);
  }
  else if (keyArrowRight) {
    turn_right(255);
  }
}


// give new target speeds to tracks for recognized movements
void FPV_Rover::move_forwards(int speed) { 
  set_tracks_target(speed, speed * (0.7)); 
}

void FPV_Rover::move_forwards_left(int speed) {
  set_tracks_target(speed * 0.3, speed);
}

void FPV_Rover::move_forwards_right(int speed) {
  set_tracks_target(speed, speed * 0.3);
}

void FPV_Rover::turn_left(int speed) {
  set_tracks_target(speed * (-1), speed);
}

void FPV_Rover::turn_right(int speed) {
  set_tracks_target(speed, speed * (-1));
}

void FPV_Rover::move_backwards(int speed) {
  set_tracks_target(speed * (-1), speed * (-0.7));
}

void FPV_Rover::move_backwards_left(int speed) {
  set_tracks_target(speed * (-0.3), speed * (-1));

}

void FPV_Rover::move_backwards_right(int speed) {
  set_tracks_target(speed * (-1), speed * (-0.3));
}


// set new track target speeds
int FPV_Rover::set_tracks_target(int l_speed, int r_speed) {
 
  if ((l_speed >= -255) && (l_speed <= 255)) {
    l_track_target = l_speed;
  }
  if ((r_speed >= -255) && (r_speed <= 255)) {
    r_track_target = r_speed;
  }

  return 1;
}


// updates speed of tracks gradually to target speed
int FPV_Rover::adjust_track_speed() {

  if (millis() < last_motor_change + MOTOR_CHANGE_DELAY) {
    return 1;
  }

  int l_diff = l_track_target - l_track_real;
  int r_diff = r_track_target - r_track_real;

  if ((l_diff == 0) && (r_diff == 0)) {
    return 0;
  }

  if (l_diff > 0) {
    if (l_diff > 20) { l_track_real += 20; }
    else { l_track_real += l_diff; }
  }
  if (l_diff < 0) {
    if (l_diff < -20) { l_track_real -= 20; }
    else { l_track_real += l_diff; }
  }

  if (r_diff > 0) {
    if (r_diff > 20) { r_track_real += 20; }
    else { r_track_real += r_diff; }
  }
  if (r_diff < 0) {
    if (r_diff < -20) { r_track_real -= 20; }
    else { r_track_real += r_diff; }
  }

  move_track(LEFT, l_track_real); 
  move_track(RIGHT, r_track_real); 

  last_motor_change = millis();
  
  return 1;
}


// instantly stopping tracks
void FPV_Rover::stop_move() {
  l_track_real = 0;
  l_track_target = 0;
  r_track_real = 0;
  r_track_target = 0;
  
  ledcWrite(l_pin1, 0);
  ledcWrite(l_pin2, 0);
  ledcWrite(r_pin1, 0);
  ledcWrite(r_pin2, 0);
}


// change speed of track
void FPV_Rover::move_track(int track, int speed) {

    int t1 = l_pin1;
    int t2 = l_pin2;

    if (track == RIGHT) {
      t1 = r_pin1;
      t2 = r_pin2;
    }
  
    if (speed > 0) { // forwards
      ledcWrite(t1, speed);
      ledcWrite(t2, 0);
    }
    if (speed < 0) { // backwards
      ledcWrite(t1, 0);
      ledcWrite(t2, speed*(-1));
    }
    if (speed == 0) { // stop
      ledcWrite(t1, 0);
      ledcWrite(t2, 0);
    }
}


void FPV_Rover::attach_pwm_channel_pin(byte pin, int ch) {
  ledcAttachPin(pin, ch); 
  ledcSetup(ch, 4000, 8);
}


void FPV_Rover::set_led(int power) {
  if (power >= 0 && power <= 255) {
    led_power = power;
    ledcWrite(11, power);
  }
}


int FPV_Rover::get_led() {
  return led_power;
}
