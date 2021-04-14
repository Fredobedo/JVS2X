#ifdef JVS2KEYBOARD
#include "USB_KEYBOARD/usb_keyboard.h"

/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/
static const uint8_t PROGMEM endpoint_config_table[] = {
	1, EP_TYPE_INTERRUPT_IN,   EP_SIZE(KEYBOARD_SIZE)  | KEYBOARD_P1_BUFFER,
	1, EP_TYPE_INTERRUPT_IN,   EP_SIZE(KEYBOARD_SIZE)  | KEYBOARD_P2_BUFFER,
	1, EP_TYPE_INTERRUPT_IN,   EP_SIZE(DEBUG_TX_SIZE) | DEBUG_TX_BUFFER
};

/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/
// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!
static const uint8_t PROGMEM device_descriptor[] = {
	18,									// bLength
	1,									// bDescriptorType
	0x10, 0x01,							// bcdUSB
	0,									// bDeviceClass
	0,									// bDeviceSubClass
	0,									// bDeviceProtocol
	ENDPOINT0_SIZE,						// bMaxPacketSize0
	LSB(VENDOR_ID), MSB(VENDOR_ID),		// idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),	// idProduct
	0x00, 0x01,							// bcdDevice
	1,									// iManufacturer
	2,									// iProduct
	0,									// iSerialNumber
	1									// bNumConfigurations
};

static const uint8_t PROGMEM debug_hid_report_descriptor[]   = {
	0x06, 0x31, 0xFF,		// Usage Page 0xFF31 (vendor defined)
	0x09, 0x74,				// Usage 0x74
	0xA1, 0x53,				// Collection 0x53
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,		// logical maximum = 255
	0x95, DEBUG_TX_SIZE,	// report count
	0x09, 0x75,				// usage
	0x81, 0x02,				// Input (array)
	0xC0					// end collection
};

// Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
static const uint8_t PROGMEM keyboard_hid_report_desc[] = {
        0x05, 0x01,             //  Usage Page (Generic Desktop),
        0x09, 0x06,             //  Usage (Keyboard),
        0xA1, 0x01,             //  Collection (Application),
        0x75, 0x01,             //  Report Size (1),
        0x95, 0x08,             //  Report Count (8),
        0x05, 0x07,             //  Usage Page (Key Codes),
        0x19, 0xE0,             //  Usage Minimum (224),
        0x29, 0xE7,             //  Usage Maximum (231),
        0x15, 0x00,             //  Logical Minimum (0),
        0x25, 0x01,             //  Logical Maximum (1),
        0x81, 0x02,             //  Input (Data, Variable, Absolute), ;Modifier byte
        0x95, 0x08,             //  Report Count (8),
        0x75, 0x01,             //  Report Size (1),
        0x15, 0x00,             //  Logical Minimum (0),
        0x25, 0x01,             //  Logical Maximum (1),
        0x05, 0x0C,             //  Usage Page (Consumer),
        0x09, 0xE9,             //  Usage (Volume Increment),
        0x09, 0xEA,             //  Usage (Volume Decrement),
        0x09, 0xE2,             //  Usage (Mute),
        0x09, 0xCD,             //  Usage (Play/Pause),
        0x09, 0xB5,             //  Usage (Scan Next Track),
        0x09, 0xB6,             //  Usage (Scan Previous Track),
        0x09, 0xB7,             //  Usage (Stop),
        0x09, 0xB8,             //  Usage (Eject),
        0x81, 0x02,             //  Input (Data, Variable, Absolute), ;Media keys
        0x95, 0x05,             //  Report Count (5),
        0x75, 0x01,             //  Report Size (1),
        0x05, 0x08,             //  Usage Page (LEDs),
        0x19, 0x01,             //  Usage Minimum (1),
        0x29, 0x05,             //  Usage Maximum (5),
        0x91, 0x02,             //  Output (Data, Variable, Absolute), ;LED report
        0x95, 0x01,             //  Report Count (1),
        0x75, 0x03,             //  Report Size (3),
        0x91, 0x03,             //  Output (Constant),                 ;LED report padding
        0x95, 0x06,             //  Report Count (6),
        0x75, 0x08,             //  Report Size (8),
        0x15, 0x00,             //  Logical Minimum (0),
        0x25, 0x7F,             //  Logical Maximum(104),
        0x05, 0x07,             //  Usage Page (Key Codes),
        0x19, 0x00,             //  Usage Minimum (0),
        0x29, 0x7F,             //  Usage Maximum (104),
        0x81, 0x00,             //  Input (Data, Array),                ;Normal keys
        0xc0                    // End Collection
};


#define KEYBOARD_P1_HID_DESC_OFFSET	    (9	+9)
#define KEYBOARD_P2_HID_DESC_OFFSET	    (9	+9+9+7  +9)
#define DEBUG_HID_DESC_OFFSET	        (9	+9+9+7  +9+9+7  +9)
#define CONFIG1_DESC_SIZE		        (9	+9+9+7  +9+9+7  +9+9+7)

static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 							    // bLength;
	2,							    // bDescriptorType; (2 -> = configuration)
	LSB(CONFIG1_DESC_SIZE),			// wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	NUM_INTERFACE,				    // bNumInterfaces
	1,							    // bConfigurationValue
	0,							    // iConfiguration
	0x80,							// bmAttributes
	50,								// bMaxPower
	
	// ---------------------------------------------
	// --- KEYBOARD 1 INTERFACE + HID + ENDPOINT ----
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	// ---------------------------------------------
	9,                                      // bLength
	4,                                      // bDescriptorType
	KEYBOARD_P1_INTERFACE,                  // bInterfaceNumber
	0,                                      // bAlternateSetting
	1,                                      // bNumEndpoints
	0x03,                                   // bInterfaceClass (0x03 = HID)
	0x01,                                   // bInterfaceSubClass (0x01 = Boot)
	0x01,                                   // bInterfaceProtocol (0x01 = Keyboard)
	3,                                      // iInterface
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,                                      // bLength
	0x21,                                   // bDescriptorType
	0x11, 0x01,                             // bcdHID
	0x13,                                      // bCountryCode
	1,                                      // bNumDescriptors
	0x22,                                   // bDescriptorType
	sizeof(keyboard_hid_report_desc),       // wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,                                      // bLength
	5,                                      // bDescriptorType
	KEYBOARD_P1_ENDPOINT | 0x80,            // bEndpointAddress
	0x03,                                   // bmAttributes (0x03=intr)
	KEYBOARD_SIZE, 0,                       // wMaxPacketSize
	KEYBOARD_INTERVAL,                      // bInterval					            

	// ---------------------------------------------
	// --- KEYBOARD 2 INTERFACE + HID + ENDPOINT ----
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	// ---------------------------------------------
	9,                                      // bLength
	4,                                      // bDescriptorType
	KEYBOARD_P2_INTERFACE,                  // bInterfaceNumber
	0,                                      // bAlternateSetting
	1,                                      // bNumEndpoints
	0x03,                                   // bInterfaceClass (0x03 = HID)
	0x01,                                   // bInterfaceSubClass (0x01 = Boot)
	0x01,                                   // bInterfaceProtocol (0x01 = Keyboard)
	4,                                      // iInterface
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,                                      // bLength
	0x21,                                   // bDescriptorType
	0x11, 0x01,                             // bcdHID
	0x13,                                      // bCountryCode
	1,                                      // bNumDescriptors
	0x22,                                   // bDescriptorType
	sizeof(keyboard_hid_report_desc),       // wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,                                      // bLength
	5,                                      // bDescriptorType
	KEYBOARD_P2_ENDPOINT | 0x80,            // bEndpointAddress
	0x03,                                   // bmAttributes (0x03=intr)
	KEYBOARD_SIZE, 0,                       // wMaxPacketSize
	KEYBOARD_INTERVAL,                      // bInterval	

#ifdef JVSDEBUG
	// ---------------------------------------------
	// --- DEBUG INTERFACE + HID + ENDPOINT     ----
	// ---------------------------------------------
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,								// bLength
	4,								// bDescriptorType		(4 -> = interface)
	DEBUG_TX_INTERFACE,				// bInterfaceNumber
	0,								// bAlternateSetting
	1,								// bNumEndpoints
	0x03,							// bInterfaceClass 		(0x03 = HID)
	0x00,							// bInterfaceSubClass
	0x00,							// bInterfaceProtocol
	5,								// iInterface
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,								// bLength
	0x21,							// bDescriptorType		(21 -> = HID)
	0x11, 0x01,						// bcdHID
	0,								// bCountryCode
	1,								// bNumDescriptors
	0x22,							// bDescriptorType		(22 -> = report)
	sizeof(debug_hid_report_descriptor),// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,								// bLength
	5,								// bDescriptorType		(5 -> = endpoint)
	DEBUG_TX_ENDPOINT | 0x80,		// bEndpointAddress
	0x03,							// bmAttributes 		(0x03=intr)
	DEBUG_TX_SIZE, 0,				// wMaxPacketSize
	1								// bInterval
#endif
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};

// Get_Descriptor request with wIndex = 0 -> The device returns the String descriptor containing codes for different languages.
// Here, we only support 1 language 0x0409 (English US)
static const struct usb_string_descriptor_struct PROGMEM langDesc = {
	4,
	0x03,
	{0x0409}
};
static const struct usb_string_descriptor_struct PROGMEM manufacturerDesc = {
	sizeof(STR_MANUFACTURER),
	0x03,
	{STR_MANUFACTURER}
};
static const struct usb_string_descriptor_struct PROGMEM productDesc = {
	sizeof(STR_PRODUCT),
	0x03,
	{STR_PRODUCT}
};
static const struct usb_string_descriptor_struct PROGMEM controller1Desc = {
	sizeof(STR_KEYBOARD_P1),
	0x03,
	{STR_KEYBOARD_P1}
};
static const struct usb_string_descriptor_struct PROGMEM controller2Desc = {
	sizeof(STR_KEYBOARD_P2),
	0x03,
	{STR_KEYBOARD_P2}
};
static const struct usb_string_descriptor_struct PROGMEM debugDesc = {
	sizeof(STR_DEBUG_INTERFACE),
	0x03,
	{STR_DEBUG_INTERFACE}
};


// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct descriptor_list_struct {
	uint16_t	wValue;
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
		{0x2100, KEYBOARD_P1_INTERFACE, config1_descriptor + KEYBOARD_P1_HID_DESC_OFFSET, 9},
		{0x2200, KEYBOARD_P1_INTERFACE, keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
		{0x2100, KEYBOARD_P2_INTERFACE, config1_descriptor + KEYBOARD_P2_HID_DESC_OFFSET, 9},
		{0x2200, KEYBOARD_P2_INTERFACE, keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
		{0x2100, DEBUG_TX_INTERFACE, config1_descriptor + DEBUG_HID_DESC_OFFSET, 9},
		{0x2200, DEBUG_TX_INTERFACE, debug_hid_report_descriptor, sizeof(debug_hid_report_descriptor)},
	{0x0300, 0x0000, (const uint8_t *)&langDesc, 4},
	{0x0301, 0x0409, (const uint8_t *)&manufacturerDesc, sizeof(STR_MANUFACTURER)},
	{0x0302, 0x0409, (const uint8_t *)&productDesc, sizeof(STR_PRODUCT)},
	{0x0303, 0x0409, (const uint8_t *)&controller1Desc, sizeof(STR_KEYBOARD_P1)},
	{0x0304, 0x0409, (const uint8_t *)&controller2Desc, sizeof(STR_KEYBOARD_P2)},
	{0x0305, 0x0409, (const uint8_t *)&debugDesc, sizeof(STR_DEBUG_INTERFACE)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))

/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/
// zero when we are not configured, non-zero when enumerated
volatile uint8_t usb_configuration USBSTATE;
volatile uint8_t usb_suspended USBSTATE;

// byte0: which modifier keys are currently pressed
//  1=left ctrl,    2=left shift,   4=left alt,    8=left gui
//  16=right ctrl, 32=right shift, 64=right alt, 128=right gui
// byte1: media keys (TODO: document these)
// bytes2-7: which keys are currently pressed, up to 6 keys may be down at once
uint8_t keyboard_P1_state[8] USBSTATE;
uint8_t keyboard_P2_state[8] USBSTATE;

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t keyboard_P1_protocol USBSTATE;
static uint8_t keyboard_P2_protocol USBSTATE;

// the idle configuration, how often we send the report to the
// host (ms * 4) even when it hasn't changed
static uint8_t keyboard_P1_idle_config USBSTATE;
static uint8_t keyboard_P2_idle_config USBSTATE;

// count until idle timeout
uint8_t keyboard_P1_idle_count USBSTATE;
uint8_t keyboard_P2_idle_count USBSTATE;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
volatile uint8_t keyboard_P1_leds USBSTATE;
volatile uint8_t keyboard_P2_leds USBSTATE;

/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/
// initialize USB
void usb_init(void) {
	HW_CONFIG();
	USB_FREEZE();				// enable USB
	PLL_CONFIG();				// config PLL
	while (!(PLLCSR & (1<<PLOCK)));		// wait for PLL lock
	USB_CONFIG();				// start USB clock
	UDCON = 0;				// enable attach resistor
	usb_configuration = 0;

	keyboard_P1_state[0] = 0;
	keyboard_P1_state[1] = 0;
	keyboard_P1_state[2] = 0;
	keyboard_P1_state[3] = 0;
	keyboard_P1_state[4] = 0;
	keyboard_P1_state[5] = 0;
	keyboard_P1_state[6] = 0;
	keyboard_P1_state[7] = 0;
	keyboard_P1_protocol = 1;
	keyboard_P1_idle_config = 125;
	keyboard_P1_idle_count = 0;
	keyboard_P1_leds = 0;

	keyboard_P2_state[0] = 0;
	keyboard_P2_state[1] = 0;
	keyboard_P2_state[2] = 0;
	keyboard_P2_state[3] = 0;
	keyboard_P2_state[4] = 0;
	keyboard_P2_state[5] = 0;
	keyboard_P2_state[6] = 0;
	keyboard_P2_state[7] = 0;
	keyboard_P2_protocol = 1;
	keyboard_P2_idle_config = 125;
	keyboard_P2_idle_count = 0;
	keyboard_P2_leds = 0;
	
	UDIEN = (1<<EORSTE)|(1<<SOFE);
	sei();
}

void usb_shutdown(void)
{
	UDIEN = 0;	// disable interrupts
	UDCON = 1;	// disconnect attach resistor
	USBCON = 0;	// shut off USB periperal
	PLLCSR = 0;	// shut off PLL
	usb_configuration = 0;
	usb_suspended = 1;
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void) {
	return usb_configuration;
}

//usb_controller_state_t usb_controller_p1;
//usb_controller_state_t usb_controller_p2;
//usb_controller_state_t USB_CONTROLLER_UNASSIGNED;

/*
inline void usbKeyboardResetState(usb_keyboard_state_t controller_state) {
	memcpy_P(&controller_state, &controller_idle_state, sizeof(usb_controller_state_t));
}
*/

/*
int8_t usbControllerP1SendReport() {
	uint8_t intr_state, timeout, i;

	if (!usb_configuration) return -1;
	intr_state = SREG;
	cli();
	UENUM = KEYBOARD_P1_ENDPOINT;
	timeout = UDFNUML + 50;
	while (1) {
		// are we ready to transmit?
		if (UEINTX & (1<<RWAL)) break;
		SREG = intr_state;
		// has the USB gone offline?
		if (!usb_configuration) return -1;
		// have we waited too long?
		if (UDFNUML == timeout) return -1;
		// get ready to try checking again
		// get ready to try checking again_list
		intr_state = SREG;
		cli();
		UENUM = KEYBOARD_P1_ENDPOINT;
	}

	for (i=0; i<sizeof(usb_keyboard_state_t); i++) {
		UEDATX = ((uint8_t*)&usb_keyboard_p1)[i];
	}

	UEINTX = 0x3A;
	SREG = intr_state;
	return 0;
}


int8_t usbControllerP2SendReport() {
	uint8_t intr_state, timeout, i;

	if (!usb_configuration) return -1;
	intr_state = SREG;
	cli();
	UENUM = KEYBOARD_P2_ENDPOINT;
	timeout = UDFNUML + 50;
	while (1) {
		// are we ready to transmit?
		if (UEINTX & (1 << RWAL)) break;
		SREG = intr_state;
		// has the USB gone offline?
		if (!usb_configuration) return -1;
		// have we waited too long?
		if (UDFNUML == timeout) return -1;
		// get ready to try checking again
		// get ready to try checking again_list
		intr_state = SREG;
		cli();
		UENUM = KEYBOARD_P2_ENDPOINT;
	}

	for (i = 0; i < sizeof(usb_keyboard_state_t); i++) {
		UEDATX = ((uint8_t*)&usb_keyboard_p2)[i];
	}

	UEINTX = 0x3A;
	SREG = intr_state;
	return 0;
}
*/

// the time remaining before we transmit any partially full
// packet, or send a zero length packet.
static volatile uint8_t debug_flush_timer = 0;

// transmit a character.  0 returned on success, -1 on error
int8_t usb_debug_putchar(uint8_t c)
{
	static uint8_t previous_timeout = 0;
	uint8_t timeout, intr_state;

	// if we're not online (enumerated and configured), error
	if (!usb_configuration) return -1;
	// interrupts are disabled so these functions can be
	// used from the main program or interrupt context,
	// even both in the same program!
	intr_state = SREG;
	cli();
	UENUM = DEBUG_TX_ENDPOINT;
	// if we gave up due to timeout before, don't wait again
	if (previous_timeout) {
		if (!(UEINTX & (1 << RWAL))) {
			SREG = intr_state;
			return -1;
		}
		previous_timeout = 0;
	}
	// wait for the FIFO to be ready to accept data
	timeout = UDFNUML + 4;
	while (1) {
		// are we ready to transmit?
		if (UEINTX & (1 << RWAL)) break;
		SREG = intr_state;
		// have we waited too long?
		if (UDFNUML == timeout) {
			previous_timeout = 1;
			return -1;
		}
		// has the USB gone offline?
		if (!usb_configuration) return -1;
		// get ready to try checking again
		intr_state = SREG;
		cli();
		UENUM = DEBUG_TX_ENDPOINT;
	}
	// actually write the byte into the FIFO
	UEDATX = c;
	// if this completed a packet, transmit it now!
	if (!(UEINTX & (1 << RWAL))) {
		UEINTX = 0x3A;
		debug_flush_timer = 0;
	}
	else {
		debug_flush_timer = 2;
	}
	SREG = intr_state;
	return 0;
}


// immediately transmit any buffered output.
void usb_debug_flush_output(void)
{
	uint8_t intr_state;

	intr_state = SREG;
	cli();
	if (debug_flush_timer) {
		UENUM = DEBUG_TX_ENDPOINT;
		while ((UEINTX & (1 << RWAL))) {
			UEDATX = 0;
		}
		UEINTX = 0x3A;
		debug_flush_timer = 0;
	}
	SREG = intr_state;
}

// ---------------------------------------------------------
// Private Functions called by USB General Interrupt request
ISR(USB_GEN_vect)
{
	uint8_t intbits;

	intbits = UDINT;
	UDINT = 0;
	if (intbits & (1<<EORSTI)) {
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1<<RXSTPE);
		usb_configuration = 0;
	}
}

// Misc functions to wait for ready and send/receive packets
static inline void usb_wait_in_ready(void)
{
	while (!(UEINTX & (1<<TXINI))) ;
}
static inline void usb_send_in(void)
{
	UEINTX = ~(1<<TXINI);
}
static inline void usb_wait_receive_out(void)
{
	while (!(UEINTX & (1<<RXOUTI))) ;
}
static inline void usb_ack_out(void)
{
	UEINTX = ~(1<<RXOUTI);
}

// ---------------------------------------------------------------
// USB Endpoint Pipe Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
ISR(USB_COM_vect)
{
	uint8_t intbits;
	const uint8_t *list;
	const uint8_t *cfg;
	uint8_t i, n, len, en;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t desc_val;
	const uint8_t *desc_addr;
	uint8_t	desc_length;

	UENUM = 0;
	intbits = UEINTX;
	if (intbits & (1<<RXSTPI)) {
		bmRequestType = UEDATX;
		bRequest = UEDATX;
		wValue = UEDATX;
		wValue |= (UEDATX << 8);
		wIndex = UEDATX;
		wIndex |= (UEDATX << 8);
		wLength = UEDATX;
		wLength |= (UEDATX << 8);

		UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
		if (bRequest == GET_DESCRIPTOR) {
			list = (const uint8_t *)descriptor_list;
			for (i=0; ; i++) {
				if (i >= NUM_DESC_LIST) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
					return;
				}
				desc_val = pgm_read_word(list);
				if (desc_val != wValue) {
					list += sizeof(struct descriptor_list_struct);
					continue;
				}
				//2 bytes later, we have wIndex
				list += 2;
				desc_val = pgm_read_word(list);
				if (desc_val != wIndex) {
					list += sizeof(struct descriptor_list_struct)-2;
					continue;
				}
				list += 2;
				desc_addr = (const uint8_t *)pgm_read_word(list);
				list += 2;
				desc_length = pgm_read_byte(list);
				break;
			}
			len = (wLength < 256) ? wLength : 255;
			if (len > desc_length) len = desc_length;
			do {
				// wait for host ready for IN packet
				do {
					i = UEINTX;
				} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
				if (i & (1<<RXOUTI)) return;	// abort
				// send IN packet
				n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) {
					UEDATX = pgm_read_byte(desc_addr++);
				}
				len -= n;
				usb_send_in();
			} while (len || n == ENDPOINT0_SIZE);
			return;
		}
		if (bRequest == SET_ADDRESS) {
			usb_send_in();
			usb_wait_in_ready();
			UDADDR = wValue | (1<<ADDEN);
			return;
		}
		if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_configuration = wValue;
			usb_send_in();
			cfg = endpoint_config_table;
			for (i=1; i< NUM_ENDPOINT +1; i++) {
				UENUM = i;
				en = pgm_read_byte(cfg++);
				UECONX = en;
				if (en) {
					UECFG0X = pgm_read_byte(cfg++);
					UECFG1X = pgm_read_byte(cfg++);
				}
			}
			UERST = 0x1E;
			UERST = 0;
			return;
		}
		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			usb_wait_in_ready();
			UEDATX = usb_configuration;
			usb_send_in();
			return;
		}

		if (bRequest == GET_STATUS) {
			usb_wait_in_ready();
			i = 0;
			#ifdef SUPPORT_ENDPOINT_HALT
			if (bmRequestType == 0x82) {
				UENUM = wIndex;
				if (UECONX & (1<<STALLRQ)) i = 1;
				UENUM = 0;
			}
			#endif
			UEDATX = i;
			UEDATX = 0;
			usb_send_in();
			return;
		}
		#ifdef SUPPORT_ENDPOINT_HALT
		if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
		  && bmRequestType == 0x02 && wValue == 0) {
			i = wIndex & 0x7F;
			if (i >= 1 && i <= MAX_ENDPOINT) {
				usb_send_in();
				UENUM = i;
				if (bRequest == SET_FEATURE) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);
				} else {
					UECONX = (1<<STALLRQC)|(1<<RSTDT)|(1<<EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		#endif
		if (wIndex == KEYBOARD_P1_INTERFACE) {
				if (bmRequestType == 0xA1) {
						if (bRequest == HID_GET_REPORT) {
								usb_wait_in_ready();
								//len = keyboard_protocol ? sizeof(keyboard_keys) : 8;
								for (i=0; i < 8; i++) {
										UEDATX = keyboard_P1_state[i];
								}
								usb_send_in();
								return;
						}
						if (bRequest == HID_GET_IDLE) {
								usb_wait_in_ready();
								UEDATX = keyboard_P1_idle_config;
								usb_send_in();
								return;
						}
						if (bRequest == HID_GET_PROTOCOL) {
								usb_wait_in_ready();
								UEDATX = keyboard_P1_protocol;
								usb_send_in();
								return;
						}
				}
				if (bmRequestType == 0x21) {
						if (bRequest == HID_SET_REPORT) {
								usb_wait_receive_out();
								keyboard_P1_leds = UEDATX;
								usb_ack_out();
								usb_send_in();
								return;
						}
						if (bRequest == HID_SET_IDLE) {
								keyboard_P1_idle_config = (wValue >> 8);
								keyboard_P1_idle_count = 0;
								//usb_wait_in_ready();
								usb_send_in();
								return;
						}
						if (bRequest == HID_SET_PROTOCOL) {
								keyboard_P1_protocol = wValue;
								//usb_wait_in_ready();
								usb_send_in();
								return;
						}
				}
		}
		if (wIndex == KEYBOARD_P2_INTERFACE) {
				if (bmRequestType == 0xA1) {
						if (bRequest == HID_GET_REPORT) {
								usb_wait_in_ready();
								//len = keyboard_protocol ? sizeof(keyboard_keys) : 8;
								for (i=0; i < 8; i++) {
										UEDATX = keyboard_P2_state[i];
								}
								usb_send_in();
								return;
						}
						if (bRequest == HID_GET_IDLE) {
								usb_wait_in_ready();
								UEDATX = keyboard_P2_idle_config;
								usb_send_in();
								return;
						}
						if (bRequest == HID_GET_PROTOCOL) {
								usb_wait_in_ready();
								UEDATX = keyboard_P2_protocol;
								usb_send_in();
								return;
						}
				}
				if (bmRequestType == 0x21) {
						if (bRequest == HID_SET_REPORT) {
								usb_wait_receive_out();
								keyboard_P2_leds = UEDATX;
								usb_ack_out();
								usb_send_in();
								return;
						}
						if (bRequest == HID_SET_IDLE) {
								keyboard_P2_idle_config = (wValue >> 8);
								keyboard_P2_idle_count = 0;
								//usb_wait_in_ready();
								usb_send_in();
								return;
						}
						if (bRequest == HID_SET_PROTOCOL) {
								keyboard_P2_protocol = wValue;
								//usb_wait_in_ready();
								usb_send_in();
								return;
						}
				}
		}

	}
	UECONX = (1<<STALLRQ) | (1<<EPEN);	// stall
}

#endif
