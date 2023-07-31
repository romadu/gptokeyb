/* Copyright (c) 2021-2023
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
#
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
#
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
#
* Authored by: Kris Henriksen <krishenriksen.work@gmail.com>
#
* AnberPorts-Keyboard-Mouse
* 
* Part of the code is from from https://github.com/krishenriksen/AnberPorts/blob/master/AnberPorts-Keyboard-Mouse/main.c (mostly the fake keyboard)
* Fake Xbox code from: https://github.com/Emanem/js2xbox
* 
* Modified (badly) by: Shanti Gilbert for EmuELEC
* Modified further by: Nikolai Wuttke for EmuELEC (Added support for SDL and the SDLGameControllerdb.txt)
* Modified further by: Jacob Smith
* 
* Any help improving this code would be greatly appreciated! 
* 
* DONE: Xbox360 mode: Fix triggers so that they report from 0 to 255 like real Xbox triggers
*       Xbox360 mode: Figure out why the axis are not correctly labeled?  SDL_CONTROLLER_AXIS_RIGHTX / SDL_CONTROLLER_AXIS_RIGHTY / SDL_CONTROLLER_AXIS_TRIGGERLEFT / SDL_CONTROLLER_AXIS_TRIGGERRIGHT
*       Keyboard mode: Add a config file option to load mappings from.
*       add L2/R2 triggers
* 
*/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

enum DZ_MODE {
    DZ_DEFAULT,
    DZ_AXIAL,
    DZ_RADIAL,
    DZ_SCALED_RADIAL,
    DZ_SLOPED_AXIAL,
    DZ_SLOPED_SCALED_AXIAL,
    DZ_HYBRID,
};


#define GBTN_NONE 0


#define GBTN_A      (1 <<  0)
#define GBTN_B      (1 <<  1)
#define GBTN_X      (1 <<  2)
#define GBTN_Y      (1 <<  3)
#define GBTN_R1     (1 <<  4)
#define GBTN_R2     (1 <<  5)
#define GBTN_R3     (1 <<  6)
#define GBTN_L1     (1 <<  7)
#define GBTN_L2     (1 <<  8)
#define GBTN_L3     (1 <<  9)
#define GBTN_UP     (1 << 10)
#define GBTN_DOWN   (1 << 11)
#define GBTN_LEFT   (1 << 12)
#define GBTN_RIGHT  (1 << 13)
#define GBTN_START  (1 << 14)
#define GBTN_BACK   (1 << 15)
#define GBTN_GUIDE  (1 << 16)


#define GBTN_a      GBTN_A
#define GBTN_b      GBTN_B
#define GBTN_x      GBTN_X
#define GBTN_y      GBTN_Y
#define GBTN_r1     GBTN_R1
#define GBTN_r2     GBTN_R2
#define GBTN_r3     GBTN_R3
#define GBTN_l1     GBTN_L1
#define GBTN_l2     GBTN_L2
#define GBTN_l3     GBTN_L3
#define GBTN_up     GBTN_UP
#define GBTN_down   GBTN_DOWN
#define GBTN_left   GBTN_LEFT
#define GBTN_right  GBTN_RIGHT
#define GBTN_start  GBTN_START
#define GBTN_back   GBTN_BACK
#define GBTN_guide  GBTN_GUIDE


#define GBTN_DPAD (GBTN_UP|GBTN_DOWN|GBTN_LEFT|GBTN_RIGHT)

#define GBTN_ON(BUTTON) { state.button_state |= (GBTN_ ## BUTTON); }
#define GBTN_OFF(BUTTON) { state.button_state &= ~(GBTN_ ## BUTTON); }
#define GBTN_CHECK_BTN(BUTTON) (state.button_state & (GBTN_ ## BUTTON))
#define GBTN_CHECK(BUTTON) (state.button_state & (BUTTON))

struct GptokeybState
{
    int hotkey_jsdevice;
    int start_jsdevice;
    int textinputinteractivetrigger_jsdevice; // to trigger text input interactive
    int textinputpresettrigger_jsdevice; // to trigger text input preset
    int textinputconfirmtrigger_jsdevice; // to trigger text input confirm via Enter key
    int mouseX = 0;
    int mouseY = 0;
    int current_left_analog_x = 0;
    int current_left_analog_y = 0;
    int current_right_analog_x = 0;
    int current_right_analog_y = 0;
    int current_l2 = 0;
    int current_r2 = 0;
    bool hotkey_pressed = false; // current state of hotkey
    bool hotkey_was_pressed = false; // indicates hotkey button has been pressed, and key may need to be processed on button's release, if hotkey combo isn't triggered
    bool start_pressed = false;
    bool start_was_pressed = false; // indicates start button has been pressed, and key may need to be processed on button's release, if start combo isn't triggered
    bool textinputinteractive_mode_active = false;
    bool textinputinteractivetrigger_pressed = false;
    bool textinputpresettrigger_pressed = false;
    bool textinputconfirmtrigger_pressed = false;
    bool left_analog_was_up = false;
    bool left_analog_was_down = false;
    bool left_analog_was_left = false;
    bool left_analog_was_right = false;
    bool right_analog_was_up = false;
    bool right_analog_was_down = false;
    bool right_analog_was_left = false;
    bool right_analog_was_right = false;
    bool a_hk_was_pressed = false;
    bool b_hk_was_pressed = false;
    bool x_hk_was_pressed = false;
    bool y_hk_was_pressed = false;
    bool l1_hk_was_pressed = false;
    bool r1_hk_was_pressed = false;
    bool l2_was_pressed = false;
    bool l2_hk_was_pressed = false;
    bool r2_was_pressed = false;
    bool r2_hk_was_pressed = false;
    bool hotkey_combo_triggered = false; //keep track of whether a hotkey combo was pressed; if so, don't send hotkey key when hotkey is released
    bool start_combo_triggered = false; //keep track of whether a start combo was pressed; if so, don't send start key when start is released
    short key_to_repeat = 0;
    uint button_state = GBTN_NONE;
    SDL_TimerID key_repeat_timer_id = 0;
};


struct GptokeybConfig
{
    short back = KEY_ESC;
    bool back_repeat = false;
    short back_modifier = 0;
    short start = KEY_ENTER;
    bool start_repeat = false;
    short start_modifier = 0;
    short guide = KEY_ENTER;
    bool guide_repeat = false;
    short guide_modifier = 0;
    short a = KEY_X;
    short a_hk = KEY_ENTER;
    bool a_repeat = false;
    short a_modifier = 0;
    short a_hk_modifier = 0;
    short b = KEY_Z;
    short b_hk = KEY_ESC;
    bool b_repeat = false;
    short b_modifier = 0;
    short b_hk_modifier = 0;
    short x = KEY_C;
    short x_hk = KEY_C;
    bool x_repeat = false;
    short x_modifier = 0;
    short x_hk_modifier = 0;
    short y = KEY_A;
    short y_hk = KEY_A;
    bool y_repeat = false;
    short y_modifier = 0;
    short y_hk_modifier = 0;
    short l1 = KEY_RIGHTSHIFT;
    short l1_hk = KEY_ESC;
    bool l1_repeat = false;
    short l1_modifier = 0;
    short l1_hk_modifier = 0;
    short l2 = KEY_HOME;
    short l2_hk = KEY_HOME;
    bool l2_repeat = false;
    short l2_modifier = 0;
    short l2_hk_modifier = 0;
    short l3 = BTN_LEFT;
    bool l3_repeat = false;
    short l3_modifier = 0;
    short r1 = KEY_LEFTSHIFT;
    short r1_hk = KEY_ENTER;
    bool r1_repeat = false;
    short r1_modifier = 0;
    short r1_hk_modifier = 0;
    short r2 = KEY_END;
    short r2_hk = KEY_END;
    bool r2_repeat = false;
    short r2_modifier = 0;
    short r2_hk_modifier = 0;
    short r3 = BTN_RIGHT;
    bool r3_repeat = false;
    short r3_modifier = 0;
    short up = KEY_UP;
    bool up_repeat = false;
    short up_modifier = 0;
    short down = KEY_DOWN;
    bool down_repeat = false;
    short down_modifier = 0;
    short left = KEY_LEFT;
    bool left_repeat = false;
    short left_modifier = 0;
    short right = KEY_RIGHT;
    bool right_repeat = false;
    short right_modifier = 0;

    bool left_analog_as_mouse = false;
    bool right_analog_as_mouse = false;
    bool dpad_as_mouse = false;

    short left_analog_up = KEY_W;
    bool left_analog_up_repeat = false;
    short left_analog_up_modifier = 0;
    short left_analog_down = KEY_S;
    bool left_analog_down_repeat = false;
    short left_analog_down_modifier = 0;
    short left_analog_left = KEY_A;
    bool left_analog_left_repeat = false;
    short left_analog_left_modifier = 0;
    short left_analog_right = KEY_D;
    bool left_analog_right_repeat = false;
    short left_analog_right_modifier = 0;
    short right_analog_up = KEY_END;
    bool right_analog_up_repeat = false;
    short right_analog_up_modifier = 0;
    short right_analog_down = KEY_HOME;
    bool right_analog_down_repeat = false;
    short right_analog_down_modifier = 0;
    short right_analog_left = KEY_LEFT;
    bool right_analog_left_repeat = false;
    short right_analog_left_modifier = 0;
    short right_analog_right = KEY_RIGHT;
    bool right_analog_right_repeat = false;
    short right_analog_right_modifier = 0;

    int mouse_slow_scale = 50;
    uint mouse_slow_button = GBTN_NONE;

    DZ_MODE deadzone_mode = DZ_DEFAULT;
    int deadzone_scale = 512;
    int dpad_mouse_step = 5;
    int deadzone   = 15000;
    int deadzone_y = 15000;
    int deadzone_x = 15000;
    int deadzone_triggers = 3000;

    int fake_mouse_scale = 512;
    int fake_mouse_delay = 16;

    Uint32 key_repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL * 2; 
    Uint32 key_repeat_delay = SDL_DEFAULT_REPEAT_DELAY; 

    char* text_input_preset;
};


struct config_option
{
    char key[CONFIG_ARG_MAX_BYTES];
    char value[CONFIG_ARG_MAX_BYTES];
};

#endif /* __STRUCTS_H__ */
