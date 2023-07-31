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

#include "gptokeyb.h"

std::vector<config_option> parseConfigFile(const char* path)
{
    std::vector<config_option> result;

    FILE* fp;

    if ((fp = fopen(path, "r+")) == NULL) {
        perror("fopen()");
        return result;
    }

    while (1) {
        result.emplace_back();
        auto& co = result.back();

        if (fscanf(fp, "%s = %s", &co.key[0], &co.value[0]) != 2) {
            if (feof(fp)) {
                break;
            }

            if (co.key[0] == '#') {
                while (fgetc(fp) != '\n') {
                    // Do nothing (to move the cursor to the end of the line).
                }
                result.pop_back();
                continue;
            }

            perror("fscanf()");
            result.pop_back();
            continue;
        }
    }

    fclose(fp);

    return result;
}


// UGLY, but works.
#define _KEY_CONFIG_EXTRA(KEY) \
    if (strcmp(co.value, "add_alt") == 0) { config.KEY ## _modifier |= KEY_LEFTALT; } \
    else if (strcmp(co.value, "add_ctrl") == 0) { config.KEY ## _modifier |= KEY_LEFTCTRL; } \
    else if (strcmp(co.value, "add_shift") == 0) { config.KEY ## _modifier |= KEY_LEFTSHIFT; } \
    else { config.KEY = char_to_keycode(co.value); }

#define _KEY_CONFIG_EXTRA_W_REPEAT(KEY) \
    if (strcmp(co.value, "repeat") == 0) { config.KEY ## _repeat = true; } else _KEY_CONFIG_EXTRA(KEY)

#define _KEY_CONFIG_EXTRA_MS_W_REPEAT(KEY) \
    if (strcmp(co.value, "mouse_slow") == 0) { config.KEY = 0; config.mouse_slow_button = (GBTN_ ## KEY); } else _KEY_CONFIG_EXTRA_W_REPEAT(KEY)

#define _KEY_CONFIG(KEY) \
    (strcmp(co.key, #KEY) == 0) { _KEY_CONFIG_EXTRA(KEY) }

#define _KEY_CONFIG_RPT(KEY) \
    (strcmp(co.key, #KEY) == 0) { _KEY_CONFIG_EXTRA_W_REPEAT(KEY) }

#define _KEY_CONFIG_MS_RPT(KEY) \
    (strcmp(co.key, #KEY) == 0) { _KEY_CONFIG_EXTRA_MS_W_REPEAT(KEY) }

#define _KEY_CONFIG_MM(KEY, KEY_BASE) \
    (strcmp(co.key, #KEY) == 0) { \
        if (strcmp(co.value, "mouse_movement_up") == 0) { \
            config.KEY_BASE ## _as_mouse = true; \
        } else { _KEY_CONFIG_EXTRA_W_REPEAT(KEY) } \
    }

#define _KEY_CONFIG_HK(KEY) \
    _KEY_CONFIG_MS_RPT(KEY) else if _KEY_CONFIG(KEY ## _hk)

#define _KEY_CONFIG_ATOI(KEY) \
    (strcmp(co.key, #KEY) == 0) { config.KEY = atoi(co.value); }

#define _KEY2_CONFIG_ATOI(KEY1, KEY2) \
    (strcmp(co.key, #KEY1) == 0) { config.KEY2 = atoi(co.value); }

#define _KEY_CONFIG_SPECIAL(KEY) \
    (strcmp(co.key, #KEY) == 0)


void readConfigFile(const char* config_file)
{
    const auto parsedConfig = parseConfigFile(config_file);
    for (const auto& co : parsedConfig) {
        if _KEY_CONFIG_RPT(back)            // Back/Select button
        else if _KEY_CONFIG_RPT(guide)      // Guide button
        else if _KEY_CONFIG_RPT(start)      // Start button
        else if _KEY_CONFIG_HK(a)           // A button, Hotkey + A
        else if _KEY_CONFIG_HK(b)           // B button, Hotkey + B
        else if _KEY_CONFIG_HK(x)           // X button, Hotkey + A
        else if _KEY_CONFIG_HK(y)           // Y button, Hotkey + Y
        else if _KEY_CONFIG_HK(l1)          // L1 button, Hotkey + L1
        else if _KEY_CONFIG_HK(l2)          // L1 button, Hotkey + L2
        else if _KEY_CONFIG_RPT(l3)         // L3 button
        else if _KEY_CONFIG_HK(r1)          // R1 button, Hotkey + R1
        else if _KEY_CONFIG_HK(r2)          // R2 button, Hotkey + R2
        else if _KEY_CONFIG_RPT(r3)         // R3 button
        else if _KEY_CONFIG_MM(up, dpad)    // Up dpad
        else if _KEY_CONFIG_RPT(down)       // Down dpad
        else if _KEY_CONFIG_RPT(left)       // Left dpad
        else if _KEY_CONFIG_RPT(right)      // Right dpad
        else if _KEY_CONFIG_MM(left_analog_up, left_analog)
        else if _KEY_CONFIG_RPT(left_analog_down)
        else if _KEY_CONFIG_RPT(left_analog_left)
        else if _KEY_CONFIG_RPT(left_analog_right)
        else if _KEY_CONFIG_MM(right_analog_up, right_analog)
        else if _KEY_CONFIG_RPT(right_analog_down)
        else if _KEY_CONFIG_RPT(right_analog_left)
        else if _KEY_CONFIG_RPT(right_analog_right)
        // Various settings
        else if _KEY_CONFIG_SPECIAL(deadzone_mode) { config.deadzone_mode = deadzone_get_mode(co.value); }
        else if _KEY_CONFIG_ATOI(deadzone)
        else if _KEY_CONFIG_ATOI(deadzone_scale)
        // An alias for fake_mouse_delay
        else if _KEY2_CONFIG_ATOI(deadzone_delay, fake_mouse_delay)

        else if _KEY_CONFIG_ATOI(deadzone_y)
        else if _KEY_CONFIG_ATOI(deadzone_x)
        else if _KEY_CONFIG_ATOI(deadzone_triggers)
        else if _KEY_CONFIG_ATOI(dpad_mouse_step)
        else if _KEY_CONFIG_ATOI(mouse_slow_scale)
        else if _KEY2_CONFIG_ATOI(mouse_scale, fake_mouse_scale)
        else if _KEY2_CONFIG_ATOI(mouse_delay, fake_mouse_delay)
        else if _KEY2_CONFIG_ATOI(repeat_delay, key_repeat_delay)
        else if _KEY2_CONFIG_ATOI(repeat_interval, key_repeat_interval)
    }

    // Gotta clear these
    if (config.dpad_as_mouse) {
        config.up = 0;
        config.up_repeat = false;
        config.down = 0;
        config.down_repeat = false;
        config.left = 0;
        config.left_repeat = false;
        config.right = 0;
        config.right_repeat = false;
    }

    if (config.mouse_slow_scale > 100)
        config.mouse_slow_scale = 100;

    if (config.mouse_slow_scale <= 0)
        config.mouse_slow_scale = 1;
}
