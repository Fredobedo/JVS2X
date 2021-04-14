#ifdef JVS2PS3
#ifndef USBPS3_H_
#define USBPS3_H_

#include <Arduino.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


/* Enable C linkage for C++ Compilers: */
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/
// Unicode encoded strings
#define STR_MANUFACTURER_SONY	L"Generic"
#define STR_PRODUCT_SIXAXIS		L"JVS2X Controller"
#define STR_CONTROLLER_P1		L"JVS2X Controller."
#define STR_CONTROLLER_P2		L"JVS2X Controller.."
#define STR_DEBUG_INTERFACE		L"JVS2X DEBUG Interface" 

#define VENDOR_ID		0x10C4
#define PRODUCT_ID		0x82C0

#define SUPPORT_ENDPOINT_HALT

#ifdef JVSDEBUG
	#define NUM_INTERFACE			3
	#define NUM_ENDPOINT			3
#else
	#define NUM_INTERFACE			2
	#define NUM_ENDPOINT			2
#endif

#define ENDPOINT0_SIZE	       64

#define CONTROLLER_P1_INTERFACE	0
#define CONTROLLER_P1_ENDPOINT  1

#define CONTROLLER_P2_INTERFACE	1
#define CONTROLLER_P2_ENDPOINT  2

#define DEBUG_TX_INTERFACE	    2
#define DEBUG_TX_ENDPOINT	    3

#define CONTROLLER_SIZE		   64
#define DEBUG_TX_SIZE		   32

#define CONTROLLER_P1_BUFFER   EP_DOUBLE_BUFFER
#define CONTROLLER_P2_BUFFER   EP_DOUBLE_BUFFER
#define DEBUG_TX_BUFFER		   EP_DOUBLE_BUFFER

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

void usb_init(void);			// initialize everything
uint8_t usb_configured(void);		// is the USB port configured


int8_t usb_debug_putchar(uint8_t c);	// transmit a character
void usb_debug_flush_output(void);	// immediately transmit any buffered output

//-------+------------
//BUTTON | PS3
//-------+------------
// 1     | square_btn
// 2     | cross_btn
// 3     | circle_btn
// 4     | triangle_btn
// 5     | l1_btn
// 6     | r1_btn
// 7     | l2_btn
// 8     | r2_btn
// 9     | select_btn
// 10    | start_btn
// 11    | Left Thumbstick  -> PS: not used for now here
// 12    | Right Thumbstick -> PS: not used for now here
// 13    | ps_btn
//-------+------------
typedef struct {
    //----------------------------------
	// --- DIGITAL CONTROLS          ---
	//----------------------------------
	// digital buttons, 0 = off, 1 = on
	uint8_t square_btn : 1;
	uint8_t cross_btn : 1;
	uint8_t circle_btn : 1;
	uint8_t triangle_btn : 1;

	uint8_t l1_btn : 1;
	uint8_t r1_btn : 1;
	uint8_t l2_btn : 1;
	uint8_t r2_btn : 1;

	uint8_t select_btn : 1;
	uint8_t start_btn : 1;
	uint8_t : 2;
	uint8_t ps_btn : 1;
	uint8_t : 3;

	// digital direction, use the dir_* constants(enum)
	// 8 = center, 0 = up, 1 = up/right, 2 = right, 3 = right/down
	// 4 = down, 5 = down/left, 6 = left, 7 = left/up
	uint8_t direction;

	//----------------------------------
    // --- ANALOG CONTROLS           ---
    //----------------------------------

	// left and right analog sticks, 0x00 left/up, 0x80 middle, 0xff right/down
	uint8_t left_stick_axis_x;
	uint8_t left_stick_axis_y;
	uint8_t right_stick_axis_x;
	uint8_t right_stick_axis_y;

	uint8_t unknown[4];

	// button axis, 0x00 = unpressed, 0xff = fully pressed
	uint8_t triangle_axis;
	uint8_t circle_axis;
	uint8_t cross_axis;
	uint8_t square_axis;

	uint8_t l1_axis;
	uint8_t r1_axis;
	uint8_t l2_axis;
	uint8_t r2_axis;
} usb_controller_state_t; 

extern usb_controller_state_t usb_controller_p1;
extern usb_controller_state_t usb_controller_p2;

void usbControllerResetState(usb_controller_state_t gamepad_state);

int8_t usbControllerP1SendReport();
int8_t usbControllerP2SendReport();


#ifdef __cplusplus
} // extern "C"
#endif

#endif
#endif

