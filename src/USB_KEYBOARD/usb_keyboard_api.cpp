#ifdef JVS2KEYBOARD
#include "USB_KEYBOARD/usb_keyboard_api.h"
#include "TRACE/hid_trace.h"

usb_keyboard_class::usb_keyboard_class(uint8_t keyboard_report_data[8], uint8_t KEYBOARD_ENDPOINT, uint8_t keyboard_idle_count)
{
    this->keyboard_report_data=keyboard_report_data;
    this->KEYBOARD_ENDPOINT=KEYBOARD_ENDPOINT;
    this->keyboard_idle_count=keyboard_idle_count;
}


void usb_keyboard_class::write_key(KEYCODE_TYPE keycode)
{
	keyboard_report_data[0] = keycode_to_modifier(keycode);
	keyboard_report_data[1] = 0;
	keyboard_report_data[2] = keycode_to_key(keycode);
	keyboard_report_data[3] = 0;
	keyboard_report_data[4] = 0;
	keyboard_report_data[5] = 0;
	keyboard_report_data[6] = 0;
	keyboard_report_data[7] = 0;
	send_now();
	keyboard_report_data[0] = 0;
	keyboard_report_data[2] = 0;
	send_now();
}

uint8_t usb_keyboard_class::keycode_to_modifier(KEYCODE_TYPE keycode)
{
	uint8_t modifier=0;

	#ifdef SHIFT_MASK
	if (keycode & SHIFT_MASK) modifier |= MODIFIERKEY_SHIFT;
	#endif
	#ifdef ALTGR_MASK
	if (keycode & ALTGR_MASK) modifier |= MODIFIERKEY_RIGHT_ALT;
	#endif
	#ifdef RCTRL_MASK
	if (keycode & RCTRL_MASK) modifier |= MODIFIERKEY_RIGHT_CTRL;
	#endif
	return modifier;
}

uint8_t usb_keyboard_class::keycode_to_key(KEYCODE_TYPE keycode)
{
	uint8_t key = keycode & 0x3F;
	#ifdef KEY_NON_US_100
	if (key == KEY_NON_US_100) key = 100;
	#endif
	return key;
}



void usb_keyboard_class::set_modifier(uint16_t c)
{
	keyboard_report_data[0] = (uint8_t)c;
}
void usb_keyboard_class::set_key1(uint8_t c)
{
	keyboard_report_data[2] = c;
}
void usb_keyboard_class::set_key2(uint8_t c)
{
	keyboard_report_data[3] = c;
}
void usb_keyboard_class::set_key3(uint8_t c)
{
	keyboard_report_data[4] = c;
}
void usb_keyboard_class::set_key4(uint8_t c)
{
	keyboard_report_data[5] = c;
}
void usb_keyboard_class::set_key5(uint8_t c)
{
	keyboard_report_data[6] = c;
}
void usb_keyboard_class::set_key6(uint8_t c)
{
	keyboard_report_data[7] = c;
}


void usb_keyboard_class::send_now(void)
{
        uint8_t intr_state, timeout;

        if (!usb_configuration) return;
        intr_state = SREG;
        cli();
        UENUM = KEYBOARD_ENDPOINT;
        timeout = UDFNUML + 50;
        while (1) {
                // are we ready to transmit?
                if (UEINTX & (1<<RWAL)) break;
                SREG = intr_state;
                // has the USB gone offline?
                if (!usb_configuration) return;
                // have we waited too long?
                if (UDFNUML == timeout) return;
                // get ready to try checking again
                intr_state = SREG;
                cli();
                UENUM = KEYBOARD_ENDPOINT;
        }
        UEDATX = keyboard_report_data[0];
        UEDATX = keyboard_report_data[1];
        UEDATX = keyboard_report_data[2];
        UEDATX = keyboard_report_data[3];
        UEDATX = keyboard_report_data[4];
        UEDATX = keyboard_report_data[5];
        UEDATX = keyboard_report_data[6];
        UEDATX = keyboard_report_data[7];
        UEINTX = 0x3A;
        keyboard_idle_count = 0;
        SREG = intr_state;

		//releaseAll();
}


void usb_keyboard_class::press(uint16_t n)
{
	uint8_t key, mod, msb, modrestore=0;

	msb = n >> 8;
        if (msb >= 0xC2) {
                if (msb <= 0xDF) {
                        n = (n & 0x3F) | ((uint16_t)(msb & 0x1F) << 6);
                } else if (msb == 0xF0) {
                        presskey(n, 0);
                        return;
                } else if (msb == 0xE0) {
                        presskey(0, n);
                        return;
                } else if (msb == 0xE2) {
                        //press_system_key(n);
                        return;
                } else if (msb >= 0xE4 && msb <= 0xE7) {
                        //press_consumer_key(n & 0x3FF);
                        return;
                } else {
                        return;
                }
        }
		
	
	KEYCODE_TYPE keycode = unicode_to_keycode(n);
	if (!keycode) return;

	TRACE_ARGS_P(1, "press: keycode=%d\n", keycode);
	mod = keycode_to_modifier(keycode);
	key = keycode_to_key(keycode);
	presskey(key, mod | modrestore);
}

void usb_keyboard_class::release(uint16_t n)
{
	uint8_t key, mod, msb;

	msb = n >> 8;
        if (msb >= 0xC2) {
                if (msb <= 0xDF) {
                        n = (n & 0x3F) | ((uint16_t)(msb & 0x1F) << 6);
                } else if (msb == 0xF0) {
                        releasekey(n, 0);
                        return;
                } else if (msb == 0xE0) {
                        releasekey(0, n);
                        return;
                } else if (msb == 0xE2) {
                        //release_system_key(n);
                        return;
                } else if (msb >= 0xE4 && msb <= 0xE7) {
                        //release_consumer_key(n & 0x3FF);
                        return;
                } else {
                        return;
                }
        }
	KEYCODE_TYPE keycode = unicode_to_keycode(n);
	if (!keycode) return;
	mod = keycode_to_modifier(keycode);
	key = keycode_to_key(keycode);
	releasekey(key, mod);
}

void usb_keyboard_class::write_keycode(KEYCODE_TYPE keycode)
{
	if (!keycode) return;
	#ifdef DEADKEYS_MASK
	KEYCODE_TYPE deadkeycode = deadkey_to_keycode(keycode);
	if (deadkeycode) write_key(deadkeycode);
	#endif
	write_key(keycode);
}

KEYCODE_TYPE usb_keyboard_class::deadkey_to_keycode(KEYCODE_TYPE keycode)
{
	#ifdef DEADKEYS_MASK
	keycode &= DEADKEYS_MASK;
	if (keycode == 0) return 0;
	#ifdef ACUTE_ACCENT_BITS
	if (keycode == ACUTE_ACCENT_BITS) return DEADKEY_ACUTE_ACCENT;
	#endif
	#ifdef CEDILLA_BITS
	if (keycode == CEDILLA_BITS) return DEADKEY_CEDILLA;
	#endif
	#ifdef CIRCUMFLEX_BITS
	if (keycode == CIRCUMFLEX_BITS) return DEADKEY_CIRCUMFLEX;
	#endif
	#ifdef DIAERESIS_BITS
	if (keycode == DIAERESIS_BITS) return DEADKEY_DIAERESIS;
	#endif
	#ifdef GRAVE_ACCENT_BITS
	if (keycode == GRAVE_ACCENT_BITS) return DEADKEY_GRAVE_ACCENT;
	#endif
	#ifdef TILDE_BITS
	if (keycode == TILDE_BITS) return DEADKEY_TILDE;
	#endif
	#ifdef RING_ABOVE_BITS
	if (keycode == RING_ABOVE_BITS) return DEADKEY_RING_ABOVE;
	#endif
	#endif // DEADKEYS_MASK
	return 0;
}

KEYCODE_TYPE usb_keyboard_class::unicode_to_keycode(uint16_t cpoint)
{
	// Unicode code points beyond U+FFFF are not supported
	// technically this input should probably be called UCS-2
	if (cpoint < 32) {
		if (cpoint == 10) return KEY_ENTER & KEYCODE_MASK;
		if (cpoint == 11) return KEY_TAB & KEYCODE_MASK;
		return 0;
	}
	if (cpoint < 128) {
		if (sizeof(KEYCODE_TYPE) == 1) {
			return pgm_read_byte(keycodes_ascii + (cpoint - 0x20));
		} else if (sizeof(KEYCODE_TYPE) == 2) {
			return pgm_read_word(keycodes_ascii + (cpoint - 0x20));
		}
		return 0;
	}
	#ifdef ISO_8859_1_A0
	if (cpoint <= 0xA0) return 0;
	if (cpoint < 0x100) {
		if (sizeof(KEYCODE_TYPE) == 1) {
			return pgm_read_byte(keycodes_iso_8859_1 + (cpoint - 0xA0));
		} else if (sizeof(KEYCODE_TYPE) == 2) {
			return pgm_read_word(keycodes_iso_8859_1 + (cpoint - 0xA0));
		}
		return 0;
	}
	#endif
	//#ifdef UNICODE_20AC
	//if (cpoint == 0x20AC) return UNICODE_20AC & 0x3FFF;
	//#endif
	#ifdef KEYCODE_EXTRA00
	if (cpoint == UNICODE_EXTRA00) return (KEYCODE_EXTRA00) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA01
	if (cpoint == UNICODE_EXTRA01) return (KEYCODE_EXTRA01) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA02
	if (cpoint == UNICODE_EXTRA02) return (KEYCODE_EXTRA02) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA03
	if (cpoint == UNICODE_EXTRA03) return (KEYCODE_EXTRA03) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA04
	if (cpoint == UNICODE_EXTRA04) return (KEYCODE_EXTRA04) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA05
	if (cpoint == UNICODE_EXTRA05) return (KEYCODE_EXTRA05) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA06
	if (cpoint == UNICODE_EXTRA06) return (KEYCODE_EXTRA06) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA07
	if (cpoint == UNICODE_EXTRA07) return (KEYCODE_EXTRA07) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA08
	if (cpoint == UNICODE_EXTRA08) return (KEYCODE_EXTRA08) & 0x3FFF;
	#endif
	#ifdef KEYCODE_EXTRA09
	if (cpoint == UNICODE_EXTRA09) return KEYCODE_EXTRA09 & 0x3FFF;
	#endif
	return 0;
}


void usb_keyboard_class::presskey(uint8_t key, uint8_t modifier)
{
	TRACE_ARGS_P(1, "presskey: key=%d, modifier=%d\n", key, modifier);
	uint8_t i;
    // Modifier is global for all 6 possible key codes
	if (modifier) {
		if ((keyboard_report_data[0] & modifier) != modifier)
			keyboard_report_data[0] |= modifier;
	}

    // Check if key is not already in report
    bool found=false;
    if (key) {
		for (i=2; i < 8; i++) {
			if (keyboard_report_data[i] == key) 
            {
                found=true;
                break;        
            }
		}

        // Ok let's see if there is still place to add the key
        if(!found){
            for (i=2; i < 8; i++) {
                if (keyboard_report_data[i] == 0) {
                    keyboard_report_data[i] = key;
                    break;
                }
            }
        }
	}
}




void usb_keyboard_class::releasekey(uint8_t key, uint8_t modifier)
{
	uint8_t i;

	if (modifier) {
		if ((keyboard_report_data[0] & modifier) != 0) {
			keyboard_report_data[0] &= ~modifier;
		}
	}
	if (key) {
		for (i=2; i < 8; i++) {
			if (keyboard_report_data[i] == key) {
				keyboard_report_data[i] = 0;
			}
		}
	}
}

void usb_keyboard_class::releaseAll(void)
{
	uint8_t i, anybits;

	anybits = keyboard_report_data[0];
	for (i=2; i < 8; i++) {
		anybits |= keyboard_report_data[i];
		keyboard_report_data[i] = 0;
	}
	if (!anybits) return;
	keyboard_report_data[0] = 0;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
usb_keyboard_class Keyboard_P1 = usb_keyboard_class(keyboard_P1_state, KEYBOARD_P1_ENDPOINT, keyboard_P1_idle_count);
usb_keyboard_class Keyboard_P2 = usb_keyboard_class(keyboard_P2_state, KEYBOARD_P2_ENDPOINT, keyboard_P2_idle_count);

#endif