#ifdef JVS2KEYBOARD
#ifndef USBKEYBOARD_H_
#define USBKEYBOARD_H_

#include <Arduino.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/* Enable C linkage for C++ Compilers: */
#ifdef __cplusplus
extern "C" {
#endif

//uint8_t not_used;
#define NOT_USED                               0

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/
// Unicode encoded strings
#define STR_MANUFACTURER	    L"Generic"
#define STR_PRODUCT 		    L"JVS2X Keyboard"
#define STR_KEYBOARD_P1		    L"JVS2X Keyboard."
#define STR_KEYBOARD_P2		    L"JVS2X Keyboard.."
#define STR_DEBUG_INTERFACE		L"JVS2X DEBUG Interface" 

#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x0487

#define SUPPORT_ENDPOINT_HALT

//Test For MiSter
#define TWO_KEYBOARD 

#ifdef JVSDEBUG
	#ifdef TWO_KEYBOARD
		#define NUM_INTERFACE			3
		#define NUM_ENDPOINT			3

		#define DEBUG_TX_INTERFACE	    2
		#define DEBUG_TX_ENDPOINT	    3
	#else
		#define NUM_INTERFACE			2
		#define NUM_ENDPOINT			2

		#define DEBUG_TX_INTERFACE	    1
		#define DEBUG_TX_ENDPOINT	    2
	#endif
#else
	#ifdef TWO_KEYBOARD
		#define NUM_INTERFACE			2
		#define NUM_ENDPOINT			2
	#else
		#define NUM_INTERFACE			1
		#define NUM_ENDPOINT			1
	#endif
#endif

#define ENDPOINT0_SIZE	       64

#define KEYBOARD_P1_INTERFACE	0
#define KEYBOARD_P1_ENDPOINT    1

#define KEYBOARD_P2_INTERFACE	1
#define KEYBOARD_P2_ENDPOINT  	2



#define KEYBOARD_SIZE		   8
#define DEBUG_TX_SIZE		   32

#define KEYBOARD_P1_BUFFER   EP_DOUBLE_BUFFER
#define KEYBOARD_P2_BUFFER   EP_DOUBLE_BUFFER
#define DEBUG_TX_BUFFER		   EP_DOUBLE_BUFFER

#define KEYBOARD_INTERVAL       1

#define EP_TYPE_CONTROL			0x00
#define EP_TYPE_BULK_IN			0x81
#define EP_TYPE_BULK_OUT		0x80
#define EP_TYPE_INTERRUPT_IN	0xC1
#define EP_TYPE_INTERRUPT_OUT	0xC0
#define EP_TYPE_ISOCHRONOUS_IN	0x41
#define EP_TYPE_ISOCHRONOUS_OUT	0x40

#define EP_SINGLE_BUFFER		0x02
#define EP_DOUBLE_BUFFER		0x06

#define EP_SIZE(s)	((s) == 64 ? 0x30 :	\
			((s) == 32 ? 0x20 :	\
			((s) == 16 ? 0x10 :	\
			             0x00)))

#define MAX_ENDPOINT		4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

#if defined(__AVR_AT90USB162__)
#define HW_CONFIG()
#define PLL_CONFIG() (PLLCSR = ((1<<PLLE)|(1<<PLLP0)))
#define USB_CONFIG() (USBCON = (1<<USBE))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_ATmega32U4__)
#define HW_CONFIG() (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x12)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB646__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x1A)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB1286__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x16)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#endif

// standard control endpoint request types
#define GET_STATUS				0
#define CLEAR_FEATURE			1
#define SET_FEATURE				3
#define SET_ADDRESS				5
#define GET_DESCRIPTOR			6
#define GET_CONFIGURATION		8
#define SET_CONFIGURATION		9
#define GET_INTERFACE			10
#define SET_INTERFACE			11
// HID (human interface device)
#define HID_GET_REPORT			1
#define HID_GET_IDLE			2
#define HID_GET_PROTOCOL		3
#define HID_SET_REPORT			9
#define HID_SET_IDLE			10
#define HID_SET_PROTOCOL		11
// CDC (communication class device)
#define CDC_SET_LINE_CODING		0x20
#define CDC_GET_LINE_CODING		0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22

#define USBSTATE __attribute__ ((section (".noinit")))

void usb_init(void);			// initialize everything
uint8_t usb_configured(void);		// is the USB port configured

extern volatile uint8_t usb_configuration;
extern volatile uint8_t usb_suspended;

int8_t usb_debug_putchar(uint8_t c);	// transmit a character
void usb_debug_flush_output(void);	// immediately transmit any buffered output

extern uint8_t keyboard_P1_state[8];
extern uint8_t keyboard_P2_state[8];

extern uint8_t keyboard_P1_idle_count;
extern uint8_t keyboard_P2_idle_count;

extern volatile uint8_t keyboard_P1_leds;
extern volatile uint8_t keyboard_P2_leds;


#ifdef __cplusplus
} // extern "C"
#endif

#endif
#endif

