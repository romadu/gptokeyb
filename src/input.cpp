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

bool handleInputEvent(const SDL_Event& event)
{
    // Main input loop
    switch (event.type) {
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        {
            const bool is_pressed = event.type == SDL_CONTROLLERBUTTONDOWN;

            if (state.textinputinteractive_mode_active) {
                handleEventBtnInteractiveKeyboard(event, is_pressed);
            } else if (xbox360_mode) {
                handleEventBtnFakeXbox360Device(event, is_pressed);

            } else {
                handleEventBtnFakeKeyboardMouseDevice(event, is_pressed);
            }
        }
        break;

    case SDL_CONTROLLERAXISMOTION:
        if (xbox360_mode) {
            handleEventAxisFakeXbox360Device(event);
        } else {
            handleEventAxisFakeKeyboardMouseDevice(event);
        }
        break;

    case SDL_CONTROLLERDEVICEADDED:
        if (xbox360_mode == true || config_mode == true) {
            SDL_GameControllerOpen(0);

            // SDL_GameController* controller = SDL_GameControllerOpen(0);
            // if (controller) {
            //     const char *name = SDL_GameControllerNameForIndex(0);
            //     printf("Joystick %i has game controller name '%s'\n", 0, name);
            // }

        } else {
            SDL_GameControllerOpen(event.cdevice.which);
        }
        break;

    case SDL_CONTROLLERDEVICEREMOVED:
        if (SDL_GameController* controller = SDL_GameControllerFromInstanceID(event.cdevice.which)) {
            SDL_GameControllerClose(controller);
        }
        break;

    case SDL_QUIT:
        return false;
        break;
    }

    return true;
}
