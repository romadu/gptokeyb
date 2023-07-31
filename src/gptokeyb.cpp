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

int uinp_fd = -1;
uinput_user_dev uidev;

bool kill_mode = false;
bool sudo_kill = false; //allow sudo kill instead of killall for non-emuelec systems
bool pckill_mode = false; //emit alt+f4 to close apps on pc during kill mode, if env variable is set
bool openbor_mode = false;
bool xbox360_mode = false;
bool textinputpreset_mode = false; 
bool textinputinteractive_mode = false;
bool textinputinteractive_noautocapitals = false;
bool textinputinteractive_extrasymbols = false;
bool app_exult_adjust = false;

char* AppToKill;
bool config_mode = false;
bool hotkey_override = false;
bool emuelec_override = false;
char* hotkey_code;

GptokeybConfig config;
GptokeybState state;

int applyDeadzone(int value, int deadzone)
{
    if (std::abs(value) > deadzone) {
        return value;
    } else {
        return 0;
    }
}


void processKeys()
{
    int lenText = strlen(config.text_input_preset);
    char str[2];
    char lowerstr[2];
    char upperstr[2];
    char lowerchar;
    char upperchar;
    bool uppercase = false;
    for (int ii = 0; ii < lenText; ii++) {  
        if (config.text_input_preset[ii] != '\0') {
            memcpy( str, &config.text_input_preset[ii], 1 );        
            str[1] = '\0';

            lowerchar = std::tolower(config.text_input_preset[ii], std::locale());
            upperchar = std::toupper(config.text_input_preset[ii], std::locale());

            memcpy( upperstr, &upperchar, 1 );        
            upperstr[1] = '\0';
            memcpy( lowerstr, &lowerchar, 1 );        
            lowerstr[1] = '\0';
            uppercase = (strcmp(upperstr,str) == 0);

            int code = char_to_keycode(lowerstr);

            if (strcmp(str, " ") == 0) {
                    code = KEY_SPACE;
                    uppercase = false;
            } else if (strcmp(str, "_") == 0) {
                    code = KEY_MINUS;
                    uppercase = true;
            } else if (strcmp(str, "-") == 0) {
                    code = KEY_MINUS;
                    uppercase = true;
            } else if (strcmp(str, ".") == 0) {
                    code = KEY_DOT;
                    uppercase = false;
            } else if (strcmp(str, ",") == 0) {
                    code = KEY_COMMA;
                    uppercase = false;
            }
            
            emitTextInputKey(code, uppercase);
        } // if valid character
    } //for
}

Uint32 repeatKeyCallback(Uint32 interval, void *param)
{
        //timerCallback requires pointer parameter, but passing pointer to key_code for analog sticks doesn't work
        int key_code = *reinterpret_cast<int*>(param); 
        emitKey(key_code, false);
        emitKey(key_code, true); 
        interval = config.key_repeat_interval; // key repeats according to repeat interval; initial interval is set to delay
        return(interval);
}
void setKeyRepeat(int code, bool is_pressed)
{
    if (is_pressed) {
        state.key_to_repeat=code;
        state.key_repeat_timer_id=SDL_AddTimer(config.key_repeat_delay, repeatKeyCallback, &state.key_to_repeat); // for a new repeat, use repeat delay for first time, then switch to repeat interval
    } else {
        SDL_RemoveTimer( state.key_repeat_timer_id );
        state.key_repeat_timer_id=0;
        state.key_to_repeat=0;
    }
}

int main(int argc, char* argv[])
{
    const char* config_file = nullptr;

    config_mode = true;
    config_file = "/emuelec/configs/gptokeyb/default.gptk";

    // Add hotkey environment variable if available
    if (char* env_hotkey = SDL_getenv("HOTKEY")) {
        hotkey_override = true;
        hotkey_code = env_hotkey;
    }
    // Run in EmuELEC mode
    if (SDL_getenv("EMUELEC")) {
        emuelec_override = true;
    }

    // Add textinput_preset environment variable if available
    if (char* env_textinput = SDL_getenv("TEXTINPUTPRESET")) {
        textinputpreset_mode = true;
        config.text_input_preset = env_textinput;
    }

    // Add textinput_interactive environment variable if available
    if (char* env_textinput_interactive = SDL_getenv("TEXTINPUTINTERACTIVE")) {
        if (strcmp(env_textinput_interactive,"Y") == 0) {
            textinputinteractive_mode = true;
            state.textinputinteractive_mode_active = false;
        }
    }

    // Add pc alt+f4 exit environment variable if available
    if (char* env_pckill_mode = SDL_getenv("PCKILLMODE")) {
        if (strcmp(env_pckill_mode,"Y") == 0) {
            pckill_mode = true;
        }
    }

    if (argc > 1) {
        config_mode = false;
        config_file = "";
    }

    for( int ii = 1; ii < argc; ii++ )
    {      
        if (strcmp(argv[ii], "xbox360") == 0) {
            xbox360_mode = true;
        } else if (strcmp(argv[ii], "textinput") == 0) {
            textinputinteractive_mode = true;
            state.textinputinteractive_mode_active = false;
        } else if (strcmp(argv[ii], "-c") == 0) {
            if (ii + 1 < argc) { 
                config_mode = true;
                config_file = argv[++ii];
            } else {
                config_mode = true;
                config_file = "/emuelec/configs/gptokeyb/default.gptk";
            }
        } else if (strcmp(argv[ii], "-hotkey") == 0) {
            if (ii + 1 < argc) {
                hotkey_override = true;
                hotkey_code = argv[++ii];
            }
        } else if ((strcmp(argv[ii], "1") == 0) || (strcmp(argv[ii], "-1") == 0) || (strcmp(argv[ii], "-k") == 0)) {
            if (ii + 1 < argc) { 
                kill_mode = true;
                AppToKill = argv[++ii];
            }
        } else if ((strcmp(argv[ii], "-sudokill") == 0)) {
            if (ii + 1 < argc) { 
                kill_mode = true;
                sudo_kill = true;
                AppToKill = argv[++ii];
                if (strcmp(AppToKill, "exult") == 0) { // special adjustment for Exult, which adds double spaces during text input
                    app_exult_adjust = true;
                }
            }
            
        } 
    }

    // Add textinput_interactive mode, check for extra options via environment variable if available
    if (textinputinteractive_mode) {
        if (char* env_textinput_nocaps = SDL_getenv("TEXTINPUTNOAUTOCAPITALS")) { // don't automatically use capitals for first letter or after space
            if (strcmp(env_textinput_nocaps,"Y") == 0) {
                textinputinteractive_noautocapitals = true;
            }
        }
        if (char* env_textinput_extrasymbols = SDL_getenv("TEXTINPUTADDEXTRASYMBOLS")) { // extended characters set for interactive text input mode
            if (strcmp(env_textinput_extrasymbols,"Y") == 0) {
                textinputinteractive_extrasymbols = true;
            }
        }    
    }


    // SDL initialization and main loop
    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init() failed: %s\n", SDL_GetError());
        return -1;
    }

    // Create fake input device (not needed in kill mode)
    //if (!kill_mode) {  
    if (config_mode || xbox360_mode || textinputinteractive_mode) { // initialise device, even in kill mode, now that kill mode will work with config & xbox modes
        uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (uinp_fd < 0) {
            printf("Unable to open /dev/uinput\n");
            return -1;
        }

        // Intialize the uInput device to NULL
        memset(&uidev, 0, sizeof(uidev));
        uidev.id.version = 1;
        uidev.id.bustype = BUS_USB;

        if (xbox360_mode) {
            printf("Running in Fake Xbox 360 Mode\n");
            setupFakeXbox360Device(uidev, uinp_fd);
        } else {
            printf("Running in Fake Keyboard mode\n");
            setupFakeKeyboardMouseDevice(uidev, uinp_fd);

            // if we are in config mode, read the file
            if (config_mode) {
                printf("Using ConfigFile %s\n", config_file);
                readConfigFile(config_file);
            }
            // if we are in textinput mode, note the text preset
            if (textinputpreset_mode) {
                if (config.text_input_preset != NULL) {
                    printf("text input preset is %s\n", config.text_input_preset);
                } else {
                    printf("text input preset is not set\n");
                    //textinputpreset_mode = false;   removed so that Enter key can be pressed
                }
            } 
        }
                        // if we are in textinputinteractive mode, initialise the character set
        if (textinputinteractive_mode) {
            initialiseCharacterSet();
            printf("interactive text input mode available\n");
            if (textinputinteractive_noautocapitals)
                printf("interactive text input mode without auto-capitals\n");
            if (textinputinteractive_extrasymbols)
                printf("interactive text input mode includes extra symbols\n");
        
        }
        // Create input device into input sub-system
        write(uinp_fd, &uidev, sizeof(uidev));

        if (ioctl(uinp_fd, UI_DEV_CREATE)) {
            printf("Unable to create UINPUT device.");
            return -1;
        }
    }

    if (const char* db_file = SDL_getenv("SDL_GAMECONTROLLERCONFIG_FILE")) {
        SDL_GameControllerAddMappingsFromFile(db_file);
    }

    SDL_Event event;
    bool running = true;
    int mouse_x = 0;
    int mouse_y = 0;
    float slow_scale = (100.0 / (float)(config.mouse_slow_scale));

    while (running) {
        if (state.mouseX != 0 || state.mouseY != 0 || (config.dpad_as_mouse && GBTN_CHECK_BTN(DPAD))) {
            while (running && SDL_PollEvent(&event)) {
                running = handleInputEvent(event);
            }

            mouse_x = state.mouseX;
            mouse_y = state.mouseY;
            if (config.dpad_as_mouse) {
                mouse_x -= (GBTN_CHECK_BTN(LEFT)  ? config.dpad_mouse_step : 0);
                mouse_x += (GBTN_CHECK_BTN(RIGHT) ? config.dpad_mouse_step : 0);
                mouse_y -= (GBTN_CHECK_BTN(UP)    ? config.dpad_mouse_step : 0);
                mouse_y += (GBTN_CHECK_BTN(DOWN)  ? config.dpad_mouse_step : 0);
            }

            if (config.mouse_slow_button && GBTN_CHECK(config.mouse_slow_button)) {
                mouse_x = (int)((float)(mouse_x) / slow_scale);
                mouse_y = (int)((float)(mouse_y) / slow_scale);
            }

            emitMouseMotion(mouse_x, mouse_y);
            SDL_Delay(config.fake_mouse_delay);
        } else {
            if (!SDL_WaitEvent(&event)) {
                printf("SDL_WaitEvent() failed: %s\n", SDL_GetError());
                return -1;
            }

            running = handleInputEvent(event);
        }
    }
    SDL_RemoveTimer( state.key_repeat_timer_id );
    SDL_Quit();

    /*
        * Give userspace some time to read the events before we destroy the
        * device with UI_DEV_DESTROY.
        */
    sleep(1);

    /* Clean up */
    ioctl(uinp_fd, UI_DEV_DESTROY);
    close(uinp_fd);
    return 0;
}
