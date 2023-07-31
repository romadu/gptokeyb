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

#ifndef __GPTOKEYB_H__
#define __GPTOKEYB_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <libevdev-1.0/libevdev/libevdev.h>

#include <fcntl.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <vector>

#include <SDL.h>

#define CONFIG_ARG_MAX_BYTES 128
#define SDL_DEFAULT_REPEAT_DELAY 500
#define SDL_DEFAULT_REPEAT_INTERVAL 30

#include "structs.h"

DZ_MODE deadzone_get_mode(const char *str);
void deadzone_calc(int &x, int &y, int in_x, int in_y);

// config.cpp
std::vector<config_option> parseConfigFile(const char* path);
void readConfigFile(const char* config_file);

// keyboard.cpp
void initialiseCharacterSet();
void nextTextInputKey(bool SingleIncrease);
void prevTextInputKey(bool SingleIncrease);

void handleEventBtnInteractiveKeyboard(const SDL_Event &event, bool is_pressed);

void setupFakeKeyboardMouseDevice(uinput_user_dev& device, int fd);
void handleEventBtnFakeKeyboardMouseDevice(const SDL_Event &event, bool is_pressed);
void handleEventAxisFakeKeyboardMouseDevice(const SDL_Event &event);


// input.cpp
bool handleInputEvent(const SDL_Event& event);

// Xbox360.cpp
void setupFakeXbox360Device(uinput_user_dev& device, int fd);
void handleEventBtnFakeXbox360Device(const SDL_Event &event, bool is_pressed);
void handleEventAxisFakeXbox360Device(const SDL_Event &event);

// util.cpp
void emit(int type, int code, int val);
void emitMouseMotion(int x, int y);
void emitAxisMotion(int code, int value);
void emitTextInputKey(int code, bool uppercase);
void emitKey(int code, bool is_pressed, int modifier = 0);
void handleAnalogTrigger(bool is_triggered, bool& was_triggered, int key, int modifier = 0);

short char_to_keycode(const char* str);

void doKillMode();

// gptokeyb.cpp
int applyDeadzone(int value, int deadzone);
void setKeyRepeat(int code, bool is_pressed);
void processKeys();


extern GptokeybConfig config;
extern GptokeybState state;

extern int uinp_fd;
extern uinput_user_dev uidev;

extern bool kill_mode;
extern bool sudo_kill;      //allow sudo kill instead of killall for non-emuelec systems
extern bool pckill_mode;    //emit alt+f4 to close apps on pc during kill mode, if env variable is set
extern bool openbor_mode;
extern bool xbox360_mode;
extern bool textinputpreset_mode; 
extern bool textinputinteractive_mode;
extern bool textinputinteractive_noautocapitals;
extern bool textinputinteractive_extrasymbols;
extern bool app_exult_adjust;

extern char* AppToKill;
extern bool config_mode;
extern bool hotkey_override;
extern bool emuelec_override;
extern char* hotkey_code;

extern const int maxKeysNoExtendedSymbols;  //number of keys available for interactive text input
extern const int maxKeysWithSymbols;        //number of keys available for interactive text input with extra symbols
extern int maxKeys;
extern const int maxChars;                  // length of text in characters that can be entered
extern int character_set[];                 // keys that can be selected in text input interactive mode
extern bool character_set_shift[];          // indicate which keys require shift

extern int current_character; 
extern int current_key[];                   // current key selected for each key

#endif /* __GPTOKEYB_H__ */
