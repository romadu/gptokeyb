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

const int maxKeysNoExtendedSymbols = 69;        //number of keys available for interactive text input
const int maxKeysWithSymbols = 96;              //number of keys available for interactive text input with extra symbols
int maxKeys = maxKeysNoExtendedSymbols;
const int maxChars = 20;                        // length of text in characters that can be entered
int character_set[maxKeysWithSymbols];          // keys that can be selected in text input interactive mode
bool character_set_shift[maxKeysWithSymbols];   // indicate which keys require shift

int current_character = 0; 
int current_key[maxChars];                      // current key selected for each key


void initialiseCharacters()
{
    if (textinputinteractive_noautocapitals) {
        current_key[0] = 26; // if environment variable has been set to disable capitalisation of first characters start with all lower case  
    } else {
        current_key[0] = 0; // otherwise start with upper case for 1st character
    }
    for (int ii = 1; ii < maxChars; ii++) { // start with lower case for other character onwards
        current_key[ii] = 26;
    }
}

void initialiseCharacterSet()
{
    character_set[0]=char_to_keycode("a"); //capital letters
    character_set_shift[0]=true;
    character_set[1]=char_to_keycode("b");
    character_set_shift[1]=true;
    character_set[2]=char_to_keycode("c");
    character_set_shift[2]=true;
    character_set[3]=char_to_keycode("d");
    character_set_shift[3]=true;
    character_set[4]=char_to_keycode("e");
    character_set_shift[4]=true;
    character_set[5]=char_to_keycode("f");
    character_set_shift[5]=true;
    character_set[6]=char_to_keycode("g");
    character_set_shift[6]=true;
    character_set[7]=char_to_keycode("h");
    character_set_shift[7]=true;
    character_set[8]=char_to_keycode("i");
    character_set_shift[8]=true;
    character_set[9]=char_to_keycode("j");
    character_set_shift[9]=true;
    character_set[10]=char_to_keycode("k");
    character_set_shift[10]=true;
    character_set[11]=char_to_keycode("l");
    character_set_shift[11]=true;
    character_set[12]=char_to_keycode("m");
    character_set_shift[12]=true;
    character_set[13]=char_to_keycode("n");
    character_set_shift[13]=true;
    character_set[14]=char_to_keycode("o");
    character_set_shift[14]=true;
    character_set[15]=char_to_keycode("p");
    character_set_shift[15]=true;
    character_set[16]=char_to_keycode("q");
    character_set_shift[16]=true;
    character_set[17]=char_to_keycode("r");
    character_set_shift[17]=true;
    character_set[18]=char_to_keycode("s");
    character_set_shift[18]=true;
    character_set[19]=char_to_keycode("t");
    character_set_shift[19]=true;
    character_set[20]=char_to_keycode("u");
    character_set_shift[20]=true;
    character_set[21]=char_to_keycode("v");
    character_set_shift[21]=true;
    character_set[22]=char_to_keycode("w");
    character_set_shift[22]=true;
    character_set[23]=char_to_keycode("x");
    character_set_shift[23]=true;
    character_set[24]=char_to_keycode("y");
    character_set_shift[24]=true;
    character_set[25]=char_to_keycode("z");
    character_set_shift[25]=true;
    character_set[26]=char_to_keycode("a"); //lower case
    character_set_shift[26]=false;
    character_set[27]=char_to_keycode("b");
    character_set_shift[27]=false;
    character_set[28]=char_to_keycode("c");
    character_set_shift[28]=false;
    character_set[29]=char_to_keycode("d");
    character_set_shift[29]=false;
    character_set[30]=char_to_keycode("e");
    character_set_shift[30]=false;  
    character_set[31]=char_to_keycode("f");
    character_set_shift[31]=false;
    character_set[32]=char_to_keycode("g");
    character_set_shift[32]=false;
    character_set[33]=char_to_keycode("h");
    character_set_shift[33]=false;
    character_set[34]=char_to_keycode("i");
    character_set_shift[34]=false;
    character_set[35]=char_to_keycode("j");
    character_set_shift[35]=false;
    character_set[36]=char_to_keycode("k");
    character_set_shift[36]=false;
    character_set[37]=char_to_keycode("l");
    character_set_shift[37]=false;
    character_set[38]=char_to_keycode("m");
    character_set_shift[38]=false;
    character_set[39]=char_to_keycode("n");
    character_set_shift[39]=false;
    character_set[40]=char_to_keycode("o");
    character_set_shift[40]=false;
    character_set[41]=char_to_keycode("p");
    character_set_shift[41]=false;
    character_set[42]=char_to_keycode("q");
    character_set_shift[42]=false;
    character_set[43]=char_to_keycode("r");
    character_set_shift[43]=false;
    character_set[44]=char_to_keycode("s");
    character_set_shift[44]=false;
    character_set[45]=char_to_keycode("t");
    character_set_shift[45]=false;
    character_set[46]=char_to_keycode("u");
    character_set_shift[46]=false;
    character_set[47]=char_to_keycode("v");
    character_set_shift[47]=false;
    character_set[48]=char_to_keycode("w");
    character_set_shift[48]=false;
    character_set[49]=char_to_keycode("x");
    character_set_shift[49]=false;
    character_set[50]=char_to_keycode("y");
    character_set_shift[50]=false;
    character_set[51]=char_to_keycode("z");
    character_set_shift[51]=false;
    character_set[52]=char_to_keycode("0");
    character_set_shift[52]=false;
    character_set[53]=char_to_keycode("1");
    character_set_shift[53]=false;
    character_set[54]=char_to_keycode("2");
    character_set_shift[54]=false;
    character_set[55]=char_to_keycode("3");
    character_set_shift[55]=false;
    character_set[56]=char_to_keycode("4");
    character_set_shift[56]=false;
    character_set[57]=char_to_keycode("5");
    character_set_shift[57]=false;
    character_set[58]=char_to_keycode("6");
    character_set_shift[58]=false;
    character_set[59]=char_to_keycode("7"); 
    character_set_shift[59]=false;
    character_set[60]=char_to_keycode("8"); 
    character_set_shift[60]=false;
    character_set[61]=char_to_keycode("9"); 
    character_set_shift[61]=false;
    character_set[62]=char_to_keycode("space"); 
    character_set_shift[62]=false;
    character_set[63]=char_to_keycode("."); 
    character_set_shift[63]=false;
    character_set[64]=char_to_keycode(","); 
    character_set_shift[64]=false;
    character_set[65]=char_to_keycode("-"); 
    character_set_shift[65]=false;
    character_set[66]=char_to_keycode("_"); 
    character_set_shift[66]=true;
    character_set[67]=char_to_keycode("("); 
    character_set_shift[67]=true;
    character_set[68]=char_to_keycode(")");  
    character_set_shift[68]=true;

    if (textinputinteractive_extrasymbols) {
        maxKeys = maxKeysWithSymbols;
        character_set[69]=char_to_keycode("@");  
        character_set_shift[69]=true;
        character_set[70]=char_to_keycode("#");  
        character_set_shift[70]=true;
        character_set[71]=char_to_keycode("%");  
        character_set_shift[71]=true;
        character_set[72]=char_to_keycode("&");  
        character_set_shift[72]=true;
        character_set[73]=char_to_keycode("*");  
        character_set_shift[73]=true;
        character_set[74]=char_to_keycode("-");  
        character_set_shift[74]=false;
        character_set[75]=char_to_keycode("+");  
        character_set_shift[75]=true;
        character_set[76]=char_to_keycode("!");  
        character_set_shift[76]=true;
        character_set[77]=char_to_keycode("\"");  
        character_set_shift[77]=true;
        character_set[78]=char_to_keycode("\'");  
        character_set_shift[78]=false;
        character_set[79]=char_to_keycode(":");  
        character_set_shift[79]=true;
        character_set[80]=char_to_keycode(";");  
        character_set_shift[80]=false;
        character_set[81]=char_to_keycode("/");  
        character_set_shift[81]=false;
        character_set[82]=char_to_keycode("?");  
        character_set_shift[82]=true;
        character_set[83]=char_to_keycode("~");  
        character_set_shift[83]=true;
        character_set[84]=char_to_keycode("`");  
        character_set_shift[84]=false;
        character_set[85]=char_to_keycode("|");  
        character_set_shift[85]=true;
        character_set[86]=char_to_keycode("{");  
        character_set_shift[86]=true;
        character_set[87]=char_to_keycode("}");  
        character_set_shift[87]=true;
        character_set[88]=char_to_keycode("$");  
        character_set_shift[88]=true;
        character_set[89]=char_to_keycode("^");  
        character_set_shift[89]=true;
        character_set[90]=char_to_keycode("=");  
        character_set_shift[90]=false;
        character_set[91]=char_to_keycode("[");  
        character_set_shift[91]=false;
        character_set[92]=char_to_keycode("]");  
        character_set_shift[92]=false;
        character_set[93]=char_to_keycode("\\");  
        character_set_shift[93]=false;
        character_set[94]=char_to_keycode("<");  
        character_set_shift[94]=true;
        character_set[95]=char_to_keycode(">");  
        character_set_shift[95]=true;
    }
    initialiseCharacters();
}

Uint32 repeatInputCallback(Uint32 interval, void *param)
{
    int key_code = *reinterpret_cast<int*>(param); 
    if (key_code == KEY_UP) {
        prevTextInputKey(true);
        interval = config.key_repeat_interval; // key repeats according to repeat interval
    } else if (key_code == KEY_DOWN) {
        nextTextInputKey(true);
        interval = config.key_repeat_interval; // key repeats according to repeat interval
    } else {
        interval = 0; //turn off timer if invalid keycode
    }
    return (interval);
}

void setInputRepeat(int code, bool is_pressed)
{
    if (is_pressed) {
        state.key_to_repeat = code;
        state.key_repeat_timer_id=SDL_AddTimer(config.key_repeat_interval, repeatInputCallback, &state.key_to_repeat); // for a new repeat, use repeat delay for first time, then switch to repeat interval
    } else {
        SDL_RemoveTimer( state.key_repeat_timer_id );
        state.key_repeat_timer_id=0;
        state.key_to_repeat=0;
    }
}

void addTextInputCharacter()
{
    emitTextInputKey(character_set[current_key[current_character]],character_set_shift[current_key[current_character]]);
}

void removeTextInputCharacter()
{
    emitTextInputKey(KEY_BACKSPACE, false); //delete one character
}

void confirmTextInputCharacter()
{
    emitTextInputKey(KEY_ENTER, false); //emit ENTER to confirm text input
}

void nextTextInputKey(bool SingleIncrease) // enable fast skipping if SingleIncrease = false
{
    removeTextInputCharacter(); //delete character(s)
    if (SingleIncrease) {
        current_key[current_character]++;
    } else {
        current_key[current_character] = current_key[current_character] + 13; // jump forward by half alphabet
    }
    if (current_key[current_character] >= maxKeys) {
        current_key[current_character] = current_key[current_character] - maxKeys;
    } else if ((current_character == 0) && (character_set[current_key[current_character]] == KEY_SPACE)) {
        current_key[current_character]++; //skip space as first character 
    }

    addTextInputCharacter(); //add new character
}

void prevTextInputKey(bool SingleDecrease)
{
    removeTextInputCharacter(); //delete character(s)
    if (SingleDecrease) {
        current_key[current_character]--;
    } else {
        current_key[current_character] = current_key[current_character] - 13; // jump back by half alphabet  
    }
    if (current_key[current_character] < 0) {
        current_key[current_character] = current_key[current_character] + maxKeys;
    } else if ((current_character == 0) && (character_set[current_key[current_character]] == KEY_SPACE)) {
        current_key[current_character]--; //skip space as first character due to weird graphical issue with Exult
    }
    addTextInputCharacter(); //add new character
}

void setupFakeKeyboardMouseDevice(uinput_user_dev& device, int fd)
{
    strncpy(device.name, "Fake Keyboard", UINPUT_MAX_NAME_SIZE);
    device.id.vendor = 0x1234;  /* sample vendor */
    device.id.product = 0x5678; /* sample product */

    for (int i = 0; i < 256; i++) {
        ioctl(fd, UI_SET_KEYBIT, i);
    }

    // Keys or Buttons
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    // Fake mouse
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
}

void handleEventBtnInteractiveKeyboard(const SDL_Event &event, bool is_pressed)
{
    switch (event.cbutton.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: //move back one character
        if (is_pressed) {
            removeTextInputCharacter();
            if (current_character > 0) {
                current_character--;
            } else if (current_character == 0) {
                removeTextInputCharacter();
                initialiseCharacters();
                addTextInputCharacter();
            }
        }
        break; // SDL_CONTROLLER_BUTTON_DPAD_LEFT

    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: //add one more character
        if (is_pressed) {
            if ((character_set[current_key[current_character]] == KEY_SPACE) && (!(textinputinteractive_noautocapitals))) {
                current_key[++current_character] = 0; // use capitals after a space
            } else {
                current_character++;
            }
            if (current_character < maxChars) {
                addTextInputCharacter();
            } else { // reached limit of characters
                confirmTextInputCharacter();
                state.textinputinteractive_mode_active = false;
                printf("text input interactive mode no longer active\n");
            }
        }
        break; //SDL_CONTROLLER_BUTTON_DPAD_RIGHT

    case SDL_CONTROLLER_BUTTON_DPAD_UP: //select previous key
        if (is_pressed) {
            prevTextInputKey(true);  
            setInputRepeat(KEY_UP, true);        
        } else {
            setInputRepeat(KEY_UP, false);
        }
        break; //SDL_CONTROLLER_BUTTON_DPAD_UP

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  //select next key
        if (is_pressed) {
            nextTextInputKey(true);
            setInputRepeat(KEY_DOWN, true);        
        } else {
            setInputRepeat(KEY_DOWN, false);
        }
        break; //SDL_CONTROLLER_BUTTON_DPAD_DOWN

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: //jump back by 13 letters
        if (is_pressed) {
            prevTextInputKey(false); //jump back by 13 letters
            setInputRepeat(KEY_UP, false); //disable key repeat  
        } else {
            setInputRepeat(KEY_UP, false);
        }
        break; //SDL_CONTROLLER_BUTTON_LEFTSHOULDER

    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:  //jump forward by 13 letters
        if (is_pressed) {
            nextTextInputKey(false); //jump forward by 13 letters
            setInputRepeat(KEY_DOWN, false); //disable key repeat        
        } else {
            setInputRepeat(KEY_DOWN, false);
        }
        break; //SDL_CONTROLLER_BUTTON_RIGHTSHOULDER

    case SDL_CONTROLLER_BUTTON_A: //A buttons sends ENTER KEY
        if (is_pressed) {
            confirmTextInputCharacter();
            //disable interactive mode
            state.textinputinteractive_mode_active = false;
            printf("text input interactive mode no longer active\n");
        }
        break; //SDL_CONTROLLER_BUTTON_A

    case SDL_CONTROLLER_BUTTON_LEFTSTICK: // hotkey override
    case SDL_CONTROLLER_BUTTON_BACK: // aka select
        if (is_pressed) { // cancel key input and disable interactive input mode
            for( int ii = 0; ii <= current_character; ii++ ) {
                removeTextInputCharacter(); // delete all characters
                if ((character_set[current_key[current_character]] == KEY_SPACE) && app_exult_adjust) {
                    removeTextInputCharacter(); //remove extra spaces            
                }
            }
            initialiseCharacters(); //reset the character selections ready for new text to be added later
            state.textinputinteractive_mode_active = false;
            printf("text input interactive mode no longer active\n");
        }
        break; //SDL_CONTROLLER_BUTTON_BACK

    case SDL_CONTROLLER_BUTTON_START:
        if (is_pressed) { 
            confirmTextInputCharacter(); // send ENTER key to confirm text entry
            //disable interactive mode
            state.textinputinteractive_mode_active = false;
            printf("text input interactive mode no longer active\n");
        }
        break; //SDL_CONTROLLER_BUTTON_START

    }   //switch (event.cbutton.button) for textinputinteractive_mode_active     
}

#define _UPDATE_BUTTON_STATE(BUTTON) \
    if (is_pressed) { \
        GBTN_ON(BUTTON); \
    } else { \
        GBTN_OFF(BUTTON); \
    }

#define _DPAD_TRIGGER(DIRECTION) \
    _UPDATE_BUTTON_STATE(DIRECTION) \
    emitKey(config.DIRECTION, is_pressed, config.left_modifier); \
    if ((config.DIRECTION ## _repeat && is_pressed && (state.key_to_repeat == 0)) || \
            (!(is_pressed) && (state.key_to_repeat == config.DIRECTION))) { \
        setKeyRepeat(config.DIRECTION, is_pressed); \
    }

#define _BUTTON_TRIGGER(BUTTON) \
    _UPDATE_BUTTON_STATE(BUTTON) \
    if (state.hotkey_pressed) { \
        emitKey(config.BUTTON ## _hk, is_pressed, config.BUTTON ## _hk_modifier); \
        if (is_pressed) { \
            state.BUTTON ## _hk_was_pressed = true; \
            state.hotkey_combo_triggered = true; \
        } else { \
            state.BUTTON ## _hk_was_pressed = false; \
        } \
    } else if (state.BUTTON ## _hk_was_pressed && !(is_pressed)) { \
        emitKey(config.BUTTON ## _hk, is_pressed, config.BUTTON ## _hk_modifier); \
        state.BUTTON ## _hk_was_pressed = false; \
    } else { \
        emitKey(config.BUTTON, is_pressed, config.BUTTON ## _modifier); \
        if ((config.BUTTON ## _repeat && is_pressed && (state.key_to_repeat == 0)) || \
                (!(is_pressed) && (state.key_to_repeat == config.BUTTON))){ \
            setKeyRepeat(config.BUTTON, is_pressed); \
        } \
    }

void handleEventBtnFakeKeyboardMouseDevice(const SDL_Event& event, bool is_pressed)
{
    //config mode (i.e. not textinputinteractive_mode_active)
    switch (event.cbutton.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        if (textinputpreset_mode) { //check if input preset mode is triggered
            state.textinputpresettrigger_jsdevice = event.cdevice.which;
            state.textinputpresettrigger_pressed = is_pressed;
            if (state.start_pressed && state.textinputpresettrigger_pressed)
                break; //hotkey combo triggered
        }
        _DPAD_TRIGGER(left)
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        _DPAD_TRIGGER(up)
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        if (textinputpreset_mode) { //check if input preset enter_press is triggered
            state.textinputconfirmtrigger_jsdevice = event.cdevice.which;
            state.textinputconfirmtrigger_pressed = is_pressed;
            if (state.start_pressed && state.textinputconfirmtrigger_pressed)
                break; //hotkey combo triggered
        }
        _DPAD_TRIGGER(right)
        break;

    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        if (textinputinteractive_mode) {
            state.textinputinteractivetrigger_jsdevice = event.cdevice.which;
            state.textinputinteractivetrigger_pressed = is_pressed;
            if (state.start_pressed && state.textinputinteractivetrigger_pressed)
                break; //hotkey combo triggered
        }
        _DPAD_TRIGGER(down)
        break;

    case SDL_CONTROLLER_BUTTON_A:
        _BUTTON_TRIGGER(a)
        break;

    case SDL_CONTROLLER_BUTTON_B:
        _BUTTON_TRIGGER(b)
        break;

    case SDL_CONTROLLER_BUTTON_X:
        _BUTTON_TRIGGER(x)
        break;

    case SDL_CONTROLLER_BUTTON_Y:
        _BUTTON_TRIGGER(y)
        break;

    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        _BUTTON_TRIGGER(l1)
        break;

    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        _BUTTON_TRIGGER(r1)
        break;

    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        _UPDATE_BUTTON_STATE(l3)
        if ((kill_mode && hotkey_override && (strcmp(hotkey_code, "l3") == 0)) || (textinputpreset_mode && hotkey_override && (strcmp(hotkey_code, "l3") == 0)) || (textinputinteractive_mode && hotkey_override && (strcmp(hotkey_code, "l3") == 0))) {
            state.hotkey_jsdevice = event.cdevice.which;
            state.hotkey_pressed = is_pressed;
        } else if (hotkey_override && (strcmp(hotkey_code, "l3") == 0)) {
            state.hotkey_jsdevice = event.cdevice.which;
            state.hotkey_pressed = is_pressed;            
        }
        if (state.hotkey_pressed && (state.hotkey_jsdevice == event.cdevice.which)) {
            state.hotkey_was_pressed = true; // if hotkey is pressed, note the details of hotkey press in case it is released without triggering a hotkey combo event, since its press will need to be processed

        } else if (state.hotkey_combo_triggered && !(is_pressed)) { 
            state.hotkey_combo_triggered = false; //hotkey combo was pressed; ignore hotkey button release
            state.hotkey_was_pressed = false; //reset hotkey
        } else if (state.hotkey_was_pressed && !(is_pressed)) { 
            state.hotkey_was_pressed = false;
            emitKey(config.l3, true, config.l3_modifier); //key pressed and now released without hotkey trigger so process key press then key release
            SDL_Delay(16);
            emitKey(config.l3, is_pressed, config.l3_modifier);            
            if ((config.l3_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.l3))){
                setKeyRepeat(config.l3, is_pressed);
                //note: hotkey cannot be assigned for key repeat; release key repeat for completeness
            }
        } //hotkey state check prior to emitting key, to avoid conflicts with emitkey and hotkey press        
        else {
            emitKey(config.l3, is_pressed, config.l3_modifier);            
            if ((config.l3_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.l3))){
                setKeyRepeat(config.l3, is_pressed);
            }
        }
        break;

    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        _UPDATE_BUTTON_STATE(r3)
        emitKey(config.r3, is_pressed, config.r3_modifier);
        if ((config.r3_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.r3))){
            setKeyRepeat(config.r3, is_pressed);
        }
        break;

    case SDL_CONTROLLER_BUTTON_GUIDE:
        _UPDATE_BUTTON_STATE(guide)
        if ((kill_mode && !(hotkey_override)) || (kill_mode && hotkey_override && (strcmp(hotkey_code, "guide") == 0)) || (textinputpreset_mode && !(hotkey_override)) || (textinputpreset_mode && (strcmp(hotkey_code, "guide") == 0)) || (textinputinteractive_mode && !(hotkey_override)) || (textinputinteractive_mode && (strcmp(hotkey_code, "guide") == 0))) {
            state.hotkey_jsdevice = event.cdevice.which;
            state.hotkey_pressed = is_pressed;
        } else if (!(hotkey_override)) {
            state.hotkey_jsdevice = event.cdevice.which;
            state.hotkey_pressed = is_pressed;
        }
        if (state.hotkey_pressed && (state.hotkey_jsdevice == event.cdevice.which)) {
            state.hotkey_was_pressed = true; // if hotkey is pressed, note the details of hotkey press in case it is released without triggering a hotkey combo event, since its press will need to be processed

        } else if (state.hotkey_combo_triggered && !(is_pressed)) { 
            state.hotkey_combo_triggered = false; //hotkey combo was pressed; ignore hotkey button release
            state.hotkey_was_pressed = false; //reset hotkey

        } else if (state.hotkey_was_pressed && !(is_pressed)) { 
            state.hotkey_was_pressed = false;
            emitKey(config.guide, true, config.guide_modifier); //key pressed and now released without hotkey trigger so process key press then key release
            SDL_Delay(16);
            emitKey(config.guide, is_pressed, config.guide_modifier);
            if ((config.guide_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.guide))){
                setKeyRepeat(config.guide, is_pressed);
                //note: hotkey cannot be assigned for key repeat; release key repeat for completeness
            }
        } //hotkey state check prior to emitting key, to avoid conflicts with emitkey and hotkey press        
        else {
            emitKey(config.guide, is_pressed, config.guide_modifier);
            if ((config.guide_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.guide))){
                setKeyRepeat(config.guide, is_pressed);
            }
        }
        break;

    case SDL_CONTROLLER_BUTTON_BACK: // aka select
        _UPDATE_BUTTON_STATE(back)
        if (!emuelec_override) {
            if ((kill_mode && !(hotkey_override)) || (kill_mode && hotkey_override && (strcmp(hotkey_code, "back") == 0))) {
                state.hotkey_jsdevice = event.cdevice.which;
                state.hotkey_pressed = is_pressed;
            } else if (!(hotkey_override)) {
                state.hotkey_jsdevice = event.cdevice.which;
                state.hotkey_pressed = is_pressed;
            }
        }

        if (state.hotkey_pressed && (state.hotkey_jsdevice == event.cdevice.which)) {
            state.hotkey_was_pressed = true; // if hotkey is pressed, note the details of hotkey press in case it is released without triggering a hotkey combo event, since its press will need to be processed

        } else if (state.hotkey_combo_triggered && !(is_pressed)) { 
            state.hotkey_combo_triggered = false; //hotkey combo was pressed; ignore hotkey button release
            state.hotkey_was_pressed = false; //reset hotkey

        } else if (state.hotkey_was_pressed && !(is_pressed)) { 
            state.hotkey_was_pressed = false;
            emitKey(config.back, true, config.back_modifier); //key pressed and now released without hotkey trigger so process key press then key release
            SDL_Delay(16);
            emitKey(config.back, is_pressed, config.back_modifier);
            if ((config.back_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.back))){
                setKeyRepeat(config.back, is_pressed);
                //note: hotkey cannot be assigned for key repeat; release key repeat for completeness
            }
        } //hotkey state check prior to emitting key, to avoid conflicts with emitkey and hotkey press        
        else {
            emitKey(config.back, is_pressed, config.back_modifier);
            if ((config.back_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.back))){
                setKeyRepeat(config.back, is_pressed);
            }
        }
        break;

    case SDL_CONTROLLER_BUTTON_START:
        _UPDATE_BUTTON_STATE(start)
        if ((kill_mode) || (textinputpreset_mode) || (textinputinteractive_mode)) {
            state.start_jsdevice = event.cdevice.which;
            state.start_pressed = is_pressed;
        } // start pressed - ready for text input modes if trigger is also pressed
        if (state.start_pressed && (state.start_jsdevice == event.cdevice.which)) { 
            state.start_was_pressed = true; // if start as hotkey is pressed, note the details of start key press in case it is released without triggering a hotkey event, since its press will need to be processed

        } else if (state.start_combo_triggered && !(is_pressed)) { 
            state.start_combo_triggered = false; //ignore start key release if it acted as hotkey
            state.start_was_pressed = false; //reset hotkey

        } else if (state.start_was_pressed && !(is_pressed)) { //key pressed and now released without start trigger so process original key press, pause, then process key release
            state.start_was_pressed = false;
            emitKey(config.start, true, config.start_modifier);
            SDL_Delay(16);
            emitKey(config.start, is_pressed, config.start_modifier);
            //note: start cannot be assigned for key repeat; release key repeat for completeness
            if ((config.start_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.start))){
                setKeyRepeat(config.start, is_pressed);
            }
        } else { //process start key as normal
            emitKey(config.start, is_pressed, config.start_modifier);
            if ((config.start_repeat && is_pressed && (state.key_to_repeat == 0)) || (!(is_pressed) && (state.key_to_repeat == config.start))){
                setKeyRepeat(config.start, is_pressed);
            }
        }
        break;
    } //switch
    if ((kill_mode) && (state.start_pressed && state.hotkey_pressed)) {
        doKillMode();
    } //kill mode 
    else if ((textinputpreset_mode) && (state.textinputpresettrigger_pressed && state.start_pressed)) { //activate input preset mode - send predefined text as a series of keystrokes
        printf("text input preset pressed\n");
        state.start_combo_triggered = true;
        if (state.start_jsdevice == state.textinputpresettrigger_jsdevice) {
            if (config.text_input_preset != NULL) {
                printf("text input processing %s\n", config.text_input_preset);
                processKeys();
            }
        }
        state.textinputpresettrigger_pressed = false; //reset textinputpreset trigger
        state.start_pressed = false;
        state.start_jsdevice = 0;
        state.textinputpresettrigger_jsdevice = 0;
    } //input preset trigger mode (i.e. not kill mode)
    else if ((textinputpreset_mode) && (state.textinputconfirmtrigger_pressed && state.start_pressed)) { //activate input preset confirm mode - send ENTER key
        printf("text input confirm pressed\n");
        state.start_combo_triggered = true;
        if (state.start_jsdevice == state.textinputconfirmtrigger_jsdevice) {
            printf("text input Enter key\n");
            emitKey(char_to_keycode("enter"), true);
            SDL_Delay(15);
            emitKey(char_to_keycode("enter"), false);
        }
        state.textinputconfirmtrigger_pressed = false; //reset textinputpreset confirm trigger
        state.start_pressed = false;
        state.start_jsdevice = 0;
        state.textinputconfirmtrigger_jsdevice = 0;
    } //input confirm trigger mode (i.e. not kill mode)         
    else if ((textinputinteractive_mode) && (state.textinputinteractivetrigger_pressed && state.start_pressed)) { //activate interactive text input mode
        printf("text input interactive pressed\n");
        state.start_combo_triggered = true;
        if (state.start_jsdevice == state.textinputinteractivetrigger_jsdevice) {
            printf("text input interactive mode active\n");
            state.textinputinteractive_mode_active = true;
            SDL_RemoveTimer( state.key_repeat_timer_id ); // disable any active key repeat timer
            current_character = 0;

            addTextInputCharacter();
        }
        state.textinputinteractivetrigger_pressed = false; //reset interactive text input mode trigger
        state.start_pressed = false;
        state.textinputinteractivetrigger_jsdevice = 0;
        state.start_jsdevice = 0;
    }
}

#define _ANALOG_AXIS_POS(ANALOG_VALUE) !_ANALOG_AXIS_ZERO(ANALOG_VALUE) && ((ANALOG_VALUE) > config.deadzone)
#define _ANALOG_AXIS_NEG(ANALOG_VALUE) !_ANALOG_AXIS_ZERO(ANALOG_VALUE) && ((ANALOG_VALUE) < config.deadzone)
#define _ANALOG_AXIS_ZERO(ANALOG_VALUE) (abs(ANALOG_VALUE) < config.deadzone)

// #define _ANALOG_AXIS_POS(ANALOG_VALUE) (ANALOG_VALUE > 0)
// #define _ANALOG_AXIS_NEG(ANALOG_VALUE) (ANALOG_VALUE < 0)
// #define _ANALOG_AXIS_ZERO(ANALOG_VALUE) (ANALOG_VALUE == 0)

#define _ANALOG_AXIS_TRIGGER(STICK, DIRECTION, AXIS, CONDITION_POS, CONDITION_ZERO) \
    handleAnalogTrigger( \
        CONDITION_POS( state.current_ ## STICK ## _ ## AXIS ), \
        state. STICK ## _was_ ## DIRECTION, \
        config.STICK ## _ ## DIRECTION, \
        config.STICK ## _ ## DIRECTION ## _modifier); \
    if ((CONDITION_POS( state.current_ ## STICK ## _ ## AXIS )) && config.STICK ## _ ## DIRECTION ## _repeat && (state.key_to_repeat == 0)) { \
        setKeyRepeat(config.STICK ## _ ## DIRECTION, true); \
    } else if ((CONDITION_ZERO( state.current_ ## STICK ## _ ## AXIS )) && config.STICK ## _ ## DIRECTION ## _repeat && (state.key_to_repeat == config.STICK ## _ ## DIRECTION)) { \
        setKeyRepeat(config.STICK ## _ ## DIRECTION, false); \
    }


void handleEventAxisFakeKeyboardMouseDevice(const SDL_Event &event)
{
    // indicate which axis was moved before checking whether it's assigned as mouse
    bool left_axis_movement = false;
    bool right_axis_movement = false;

    switch (event.caxis.axis) {
    case SDL_CONTROLLER_AXIS_LEFTX:
        state.current_left_analog_x = event.caxis.value;
        left_axis_movement = true;
        break;

    case SDL_CONTROLLER_AXIS_LEFTY:
        state.current_left_analog_y = event.caxis.value;
        left_axis_movement = true;
        break;

    case SDL_CONTROLLER_AXIS_RIGHTX:
        state.current_right_analog_x = event.caxis.value;
        right_axis_movement = true;
        break;

    case SDL_CONTROLLER_AXIS_RIGHTY:
        state.current_right_analog_y = event.caxis.value;
        right_axis_movement = true;
        break;

    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
        state.current_l2 = event.caxis.value;
        break;

    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
        state.current_r2 = event.caxis.value;
        break;
    } // switch (event.caxis.axis)

    // fake mouse
    if (config.left_analog_as_mouse && left_axis_movement) {
        deadzone_calc(
            state.mouseX, state.mouseY,
            state.current_left_analog_x, state.current_left_analog_y);
    } else if (config.right_analog_as_mouse && right_axis_movement) {
        deadzone_calc(
            state.mouseX, state.mouseY,
            state.current_right_analog_x, state.current_right_analog_y);
    } else {
        // Analogs trigger keys
        if (!(state.textinputinteractive_mode_active)) {
            _ANALOG_AXIS_TRIGGER(left_analog, up,    y, _ANALOG_AXIS_NEG, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(left_analog, down,  y, _ANALOG_AXIS_POS, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(left_analog, left,  x, _ANALOG_AXIS_NEG, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(left_analog, right, x, _ANALOG_AXIS_POS, _ANALOG_AXIS_ZERO)

            _ANALOG_AXIS_TRIGGER(right_analog, up,    y, _ANALOG_AXIS_NEG, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(right_analog, down,  y, _ANALOG_AXIS_POS, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(right_analog, left,  x, _ANALOG_AXIS_NEG, _ANALOG_AXIS_ZERO)
            _ANALOG_AXIS_TRIGGER(right_analog, right, x, _ANALOG_AXIS_POS, _ANALOG_AXIS_ZERO)
        } //!(state.textinputinteractive_mode_active)
    } // Analogs trigger keys 

    if (state.hotkey_pressed) {
        handleAnalogTrigger(
            state.current_l2 > config.deadzone_triggers,
            state.l2_hk_was_pressed,
            config.l2_hk,
            config.l2_hk_modifier);
        handleAnalogTrigger(
            state.current_r2 > config.deadzone_triggers,
            state.r2_hk_was_pressed,
            config.r2_hk,
            config.r2_hk_modifier);
        if (state.l2_hk_was_pressed || state.r2_hk_was_pressed)
            state.hotkey_combo_triggered = true;
    } else if (state.l2_hk_was_pressed || state.r2_hk_was_pressed) {
        handleAnalogTrigger(
            state.current_l2 > config.deadzone_triggers,
            state.l2_hk_was_pressed,
            config.l2_hk,
            config.l2_hk_modifier);
        handleAnalogTrigger(
            state.current_r2 > config.deadzone_triggers,
            state.r2_hk_was_pressed,
            config.r2_hk,
            config.r2_hk_modifier);
    } else {
        handleAnalogTrigger(
            state.current_l2 > config.deadzone_triggers,
            state.l2_was_pressed,
            config.l2,
            config.l2_modifier);
        handleAnalogTrigger(
            state.current_r2 > config.deadzone_triggers,
            state.r2_was_pressed,
            config.r2,
            config.r2_modifier);
    }
}
