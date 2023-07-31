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
* Spaghetti code incoming, beware :)
*/

#include "gptokeyb.h"

#include <cmath>

/* Code based on:
 * https://github.com/Minimuino/thumbstick-deadzones
 */


float map_range(float value, float old_min, float old_max, float new_min, float new_max) {
    return (new_min + (new_max - new_min) * (value - old_min) / (old_max - old_min));
}
float get_sign(float value) { return ((value < 0.0)? (-1.0) : (1.0)); }

// Basic vector 2d class for better analog deadzone code
class Vector2D {
public:
    float x;
    float y;
    Vector2D() : x(0.0), y(0.0) {}
    Vector2D(float _x, float _y) : x(_x), y(_y) {}
    Vector2D(const Vector2D &vec) : x(vec.x), y(vec.y) {}

    float magnitude() const { return sqrt(pow(x, 2.0) + pow(y, 2.0)); }
};

Vector2D dz_axial(const Vector2D &stick_input, float deadzone) {
    Vector2D result(stick_input);
    if (abs(result.x) < deadzone)
        result.x = 0.0;
    if (abs(result.y) < deadzone)
        result.y = 0.0;
    return result;
}

Vector2D dz_radial(const Vector2D &stick_input, float deadzone) {
    float input_magnitude = stick_input.magnitude();

    if (input_magnitude < deadzone)
        return Vector2D(0.0, 0.0);
    else
        return Vector2D(stick_input);
}

Vector2D dz_scaled_radial(const Vector2D &stick_input, float deadzone) {
    float input_magnitude = stick_input.magnitude();
    if (input_magnitude < deadzone)
        return Vector2D(0.0, 0.0);

    Vector2D result(stick_input);
    float range_scale = map_range(input_magnitude, deadzone, 1.0, 0.0, 1.0);
    result.x /= input_magnitude;
    result.y /= input_magnitude;
    result.x *= range_scale;
    result.y *= range_scale;
    return result;
}

Vector2D dz_sloped_axial(const Vector2D &stick_input, float deadzone) {
    float deadzone_x = deadzone * abs(stick_input.y);
    float deadzone_y = deadzone * abs(stick_input.x);
    Vector2D result(stick_input);

    if (abs(result.x) < deadzone_x)
        result.x = 0.0;
    if (abs(result.y) < deadzone_y)
        result.y = 0.0;

    return result;
}

Vector2D dz_sloped_scaled_axial(const Vector2D &stick_input, float deadzone) {
    float deadzone_x = deadzone * abs(stick_input.y);
    float deadzone_y = deadzone * abs(stick_input.x);
    Vector2D result(0.0, 0.0);

    Vector2D sign(get_sign(stick_input.x), get_sign(stick_input.y));

    if (abs(stick_input.x) > deadzone_x)
        result.x = sign.x * map_range(abs(stick_input.x), deadzone_x, 1.0, 0.0, 1.0);
    if (abs(stick_input.y) > deadzone_y)
        result.y = sign.y * map_range(abs(stick_input.y), deadzone_y, 1.0, 0.0, 1.0);

    return result;
}

Vector2D dz_hybrid(const Vector2D &stick_input, float deadzone) {
    float input_magnitude = stick_input.magnitude();

    if (input_magnitude < deadzone)
        return Vector2D(0.0, 0.0);

    Vector2D partial_output = dz_scaled_radial(stick_input, deadzone);

    Vector2D final_output = dz_sloped_scaled_axial(partial_output, deadzone);

    return final_output;
}


DZ_MODE deadzone_get_mode(const char *str)
{
    if (strcmp(str, "axial") == 0)
        return DZ_AXIAL;

    else if (strcmp(str, "radial") == 0)
        return DZ_RADIAL;

    else if (strcmp(str, "scaled_radial") == 0)
        return DZ_SCALED_RADIAL;

    else if (strcmp(str, "sloped_axial") == 0)
        return DZ_SLOPED_AXIAL;

    else if (strcmp(str, "sloped_scaled_axial") == 0)
        return DZ_SLOPED_SCALED_AXIAL;

    else if (strcmp(str, "hybrid") == 0)
        return DZ_HYBRID;

    else if (strcmp(str, "default") == 0)
        return DZ_DEFAULT;

    // default
    return DZ_DEFAULT;
}

void dz_default(int &x, int &y, int in_x, int in_y)
{
    // Basic bitch deadzone code
    x = applyDeadzone(in_x, config.deadzone_x) / config.fake_mouse_scale;
    y = applyDeadzone(in_y, config.deadzone_y) / config.fake_mouse_scale;
}

void deadzone_calc(int &x, int &y, int in_x, int in_y)
{
    Vector2D stick_input((float)(in_x) / 32768.0, (float)(in_y) / 32768.0);
    Vector2D stick_output;
    float dz = (float)(config.deadzone) / 32768.0;

    switch(config.deadzone_mode)
    {
    case DZ_AXIAL:
        stick_output = dz_axial(stick_input, dz);
        break;
    case DZ_RADIAL:
        stick_output = dz_radial(stick_input, dz);
        break;
    case DZ_SCALED_RADIAL:
        stick_output = dz_scaled_radial(stick_input, dz);
        break;
    case DZ_SLOPED_AXIAL:
        stick_output = dz_sloped_axial(stick_input, dz);
        break;
    case DZ_SLOPED_SCALED_AXIAL:
        stick_output = dz_sloped_scaled_axial(stick_input, dz);
        break;
    case DZ_HYBRID:
        stick_output = dz_hybrid(stick_input, dz);
        break;

    default:
    case DZ_DEFAULT:
        dz_default(x, y, in_x, in_y);
        return;
    }

    x = (int)(stick_output.x * config.deadzone_scale);
    y = (int)(stick_output.y * config.deadzone_scale);
}
