#ifdef JVS2KEYBOARD
#ifndef USBKEYBOARDAPI_H_
#define USBKEYBOARDAPI_H_

#include <Arduino.h>
#include <keylayouts.h>
#include "USB_KEYBOARD/usb_keyboard.h"
class usb_keyboard_class 
{
	public:
        usb_keyboard_class(uint8_t keyboard_report_data[8], uint8_t KEYBOARD_ENDPOINT, uint8_t keyboard_idle_count);
        void write_unicode(uint16_t unicode) { write_keycode(unicode_to_keycode(unicode)); }
        void send_now(void);
        void press(uint16_t n);
        void release(uint16_t n);
        void releaseAll(void);
        void set_key1(uint8_t);
        void set_key2(uint8_t);
        void set_key3(uint8_t);
        void set_key4(uint8_t);
        void set_key5(uint8_t);
        void set_key6(uint8_t);
    private:
        KEYCODE_TYPE unicode_to_keycode(uint16_t unicode);
        uint8_t keycode_to_modifier(KEYCODE_TYPE keycode);
        uint8_t keycode_to_key(KEYCODE_TYPE keycode);
        void set_modifier(uint16_t);
        void presskey(uint8_t key, uint8_t modifier);
        void releasekey(uint8_t key, uint8_t modifier);
        void write_keycode(KEYCODE_TYPE key);
        void write_key(KEYCODE_TYPE code);
	    KEYCODE_TYPE deadkey_to_keycode(KEYCODE_TYPE keycode);

        uint8_t* keyboard_report_data;
        uint8_t KEYBOARD_ENDPOINT;
        uint8_t keyboard_idle_count;
};

extern usb_keyboard_class Keyboard_P1;
extern usb_keyboard_class Keyboard_P2;
extern usb_keyboard_class KEYBOARD_UNASSIGNED;

#endif
#endif