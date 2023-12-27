#include "../header/keyboard.h"
#include "../header/console.h"
#include "../header/types.h"
#include "../header/port.h"
#include "../header/shell.h"

#define keyboard_port 0x60

#define esc '\x1b' // hex
#define backspace '\b'
#define tab '\t'
#define enter '\r'
#define delete '\x7f'

// invisible
#define ctrl_left 0
#define ctrl_right 0
#define shift_left 0
#define shift_right 0
#define alt_left 0
#define alt_right 0
#define caps_lock 0

// makecode and breakcode
#define shift_left_make 0x2a
#define shift_right_make 0x36
#define alt_left_make 0x38
#define alt_right_make 0xe038
#define alt_right_break 0xe0b8
#define ctrl_left_make 0x1d
#define ctrl_right_make 0xe01d
#define ctrl_right_break 0xe09d
#define caps_lock_make 0x3a

static _Bool ctrl_status, shift_status, alt_status, caps_lock_status;
static _Bool scancode_0xe0;

ring_buffer keyboard_buffer;

// operate with shift
static char keymap[][2] = {
    {0, 0},     // 0x00=nothing
    {esc, esc}, // 0x01
    {'1', '!'}, // 0x02
    {'2', '@'}, //...
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {backspace, backspace},
    {tab, tab},
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
    {'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    {enter, enter},
    {ctrl_left, ctrl_left},
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '"'},
    {'`', '~'},
    {shift_left, shift_left},
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {shift_right, shift_right},
    {'*', '*'},
    {alt_left, alt_left},
    {' ', ' '},
    {caps_lock, caps_lock}};

static void interrupt_keyboard_handler(isr_reg regs)
{
    // true=down,false=up

    _Bool ctrl_down_last = ctrl_status;
    _Bool shift_down_last = shift_status;
    _Bool caps_lock_down_last = caps_lock_status;

    _Bool break_code;
    uint16_t scancode = inbyte(keyboard_port); // read from keyborad_port

    // console_write_hex(scancode, black, white);

    if (scancode == 0xe0)
    {
        scancode_0xe0 = TRUE; // it has more than 1 scancode, with the start 0xe0
        return;
    }

    if (scancode_0xe0)
    {
        scancode = ((0xe000) | scancode);
        scancode_0xe0 = FALSE; // close that
    }

    break_code = ((scancode & 0x0080) != 0);

    if (break_code) // if it is breakcode..
    {

        uint16_t makecode = (scancode &= 0xff7f);

        if (makecode == ctrl_right_make || makecode == ctrl_left_make)
        {
            ctrl_status = FALSE;
        }
        else if (makecode == shift_left_make || makecode == shift_right_make)
        {
            shift_status = FALSE;
        }
        else if (makecode == alt_left_make || makecode == alt_right_make)
        {
            alt_status = FALSE;
        }

        // capslock need special solution
        return;
    }
    // if it is makecode..
    else if ((scancode > 0x0000 && scancode < 0x003b) || (scancode == alt_right_make) || (scancode == ctrl_right_make)) // keymap
    {
        _Bool shift = FALSE; // you can think the shift is a status for the keymap[][shift]

        // 0-9,`,[,],\,;,/,.,/,,,-,= will not be influenced by caps_lock
        if ((scancode < 0x0e) || (scancode == 0x29) ||
            (scancode == 0x1a) || (scancode == 0x1b) ||
            (scancode == 0x2b) || (scancode == 0x27) ||
            (scancode == 0x28) || (scancode == 0x33) ||
            (scancode == 0x34) || (scancode == 0x35))
        {
            if (shift_down_last)
            {
                shift = TRUE;
            }
        }
        else // a-z
        {
            if (shift_down_last && caps_lock_down_last) // they are down at the same time..
            {
                shift = FALSE;
            }
            else if (shift_down_last || caps_lock_down_last)
            {
                shift = TRUE;
            }
            else
            {
                shift = FALSE;
            }
        }

        uint8_t index = (scancode &= 0x00ff);

        char current_char = keymap[index][shift];

        if (current_char) // value = 0 means they are invisible keys
        {
            if (!ring_buffer_is_full(&keyboard_buffer))
            {
                putchar_into_buffer(&keyboard_buffer, current_char);
                myshell(&keyboard_buffer);
            }
            return;
        }

        if (scancode == ctrl_left_make || scancode == ctrl_right_make)
        {
            ctrl_status = TRUE;
        }
        else if (scancode == shift_left_make || scancode == shift_left_make)
        {
            shift_status = TRUE;
        }
        else if (scancode == alt_left_make || scancode == alt_left_make)
        {
            alt_status = TRUE;
        }
        else if (scancode == caps_lock_make)
        {
            caps_lock_status = !caps_lock_status;
        }
    }
    else
    {
        console_write("unknown key \n");
    }
}

void keyboard_init()
{
    info;
    console_write("keyborad init..\n");
    ring_buffer_init(&keyboard_buffer);
    init_interrupt_func(33, interrupt_keyboard_handler);
    info;
    console_write("keyborad init done\n");

    // for (int i = 0; i < 59; i++)

    //     console_write_hex(keymap[i][1], black, white);
    // }
}