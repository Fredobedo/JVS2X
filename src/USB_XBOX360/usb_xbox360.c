#ifdef JVS2XBOX
#include "USB_PS3/usb_ps3.h"



/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/
static const uint8_t PROGMEM endpoint_config_table[] = {
	1, EP_TYPE_INTERRUPT_IN,   EP_SIZE(CONTROLLER_SIZE)  | CONTROLLER_P1_BUFFER,
	1, EP_TYPE_INTERRUPT_IN,   EP_SIZE(CONTROLLER_SIZE)  | CONTROLLER_P2_BUFFER,
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
	0x12,                             // bLength
	0x01,                             // bDescriptorType (Device)
	0x00, 0x02,                       // bcdUSB 2.00
	0xFF,                             // bDeviceClass 
	0xFF,                             // bDeviceSubClass 
	0xFF,                             // bDeviceProtocol 
	ENDPOINT0_SIZE,					  // bMaxPacketSize0
	LSB(VENDOR_ID), MSB(VENDOR_ID),	  // idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID), // idProduct
	0x14, 0x01,                       // bcdDevice 2.14
	0x01,                             // iManufacturer (String Index)
	0x02,                             // iProduct (String Index)
	0x03,                             // iSerialNumber (String Index)
	0x01                              // bNumConfigurations 1
};
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


#define CONTROLLER_P1_HID_DESC_OFFSET	(9	+9)
#define CONTROLLER_P2_HID_DESC_OFFSET	(9	+9+9+7  +9)
#define DEBUG_HID_DESC_OFFSET	        (9	+9+9+7  +9+9+7  +9)
#define CONFIG1_DESC_SIZE		        (9	+9+9+7  +9+9+7  +9+9+7)

static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	0x09,                           // bLength
	0x02,                           // bDescriptorType (Configuration)
	LSB(CONFIG1_DESC_SIZE),			// wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	NUM_INTERFACE,				    // bNumInterfaces
	0x01,                           // bConfigurationValue
	0x00,                           // iConfiguration (String Index)
	0xA0,                           // bmAttributes Remote Wakeup
	0xFA,                           // bMaxPower 500mA
	
	// ---------------------------------------------
	// --- CONTROLLER 1 INTERFACE + HID + ENDPOINT ----
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	// ---------------------------------------------	
	0x09,                           // bLength
	0x04,                           // bDescriptorType (Interface)
	CONTROLLER_P1_INTERFACE,		// bInterfaceNumber
	0x00,                           // bAlternateSetting
	0x02,                           // bNumEndpoints 2
	0xFF,                           // bInterfaceClass
	0x5D,                           // bInterfaceSubClass
	0x01,                           // bInterfaceProtocol
	0x00,                           // iInterface (String Index)
	// HID interface descriptor, HID 1.11 spec, section 6.2.1	
	0x11,                           // bLength
	0x21,                           // bDescriptorType (HID)
	0x00, 0x01,                     // bcdHID 1.00
	0x01,                           // bCountryCode
	0x25,                           // bNumDescriptors 
	0x81,                           // bDescriptorType[0] (Unknown 0x81)
	0x14, 0x00,                     // wDescriptorLength[0] 20
	0x00,                           // bDescriptorType[1] (Unknown 0x00)
	0x00, 0x00,                     // wDescriptorLength[1] 0
	0x13,                           // bDescriptorType[2] (Unknown 0x13)
	0x01, 0x08,                     // wDescriptorLength[2] 2049
	0x00,                           // bDescriptorType[3] (Unknown 0x00)
	0x00, 
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	0x07,                           // bLength
	0x05,                           // bDescriptorType (Endpoint)
	CONTROLLER_P1_ENDPOINT | 0x80,	// bEndpointAddress
	0x03,                           // bmAttributes (Interrupt)
	CONTROLLER_SIZE, 0,			    // wMaxPacketSize
	0x02,                           // bInterval (unit depends on device speed)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	0x07,                           // bLength
	0x05,                           // bDescriptorType (Endpoint)
	0x01,                           // bEndpointAddress (OUT/H2D)
	0x03,                           // bmAttributes (Interrupt)
	CONTROLLER_SIZE, 0,			    // wMaxPacketSize
	0x08,                           // bInterval 8 (unit depends on device speed)

	// ---------------------------------------------
	// --- ??? TBC ???? CONTROLLER 2 INTERFACE + HID + ENDPOINT ----
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	// ---------------------------------------------	
	0x09,                           // bLength
	0x04,                           // bDescriptorType (Interface)
	CONTROLLER_P2_INTERFACE,		// bInterfaceNumber
	0x00,                           // bAlternateSetting
	0x04,                           // bNumEndpoints 4
	0xFF,                           // bInterfaceClass
	0x5D,                           // bInterfaceSubClass
	0x03,                           // bInterfaceProtocol
	0x00,                           // iInterface (String Index)
	// HID interface descriptor, HID 1.11 spec, section 6.2.1	
	0x1B,                           // bLength
	0x21,                           // bDescriptorType (HID)
	0x00, 0x01,                     // bcdHID 1.00
	0x01,                           // bCountryCode
	0x01,                           // bNumDescriptors
	0x82,                           // bDescriptorType[0] (Unknown 0x82)
	0x40, 0x01,                     // wDescriptorLength[0] 320
	0x02, 0x20, 0x16, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07,                           // bLength
	0x05,                           // bDescriptorType (Endpoint)
	CONTROLLER_P2_ENDPOINT | 0x80,	// bEndpointAddress
	0x03,                           // bmAttributes (Interrupt)
	CONTROLLER_SIZE, 0,			    // wMaxPacketSize
	0x02,                           // bInterval 2 (unit depends on device speed)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x07,                           // bLength
	0x05,                           // bDescriptorType (Endpoint)
	0x02,                           // bEndpointAddress (OUT/H2D)
	0x03,                           // bmAttributes (Interrupt)
	0x20, 0x00,                     // wMaxPacketSize 32
	0x04,                           // bInterval 4 (unit depends on device speed)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x07,                          // bLength
	0x05,                          // bDescriptorType (Endpoint)
	0x83,                          // bEndpointAddress (IN/D2H)
	0x03,                          // bmAttributes (Interrupt)
	0x20, 0x00,                    // wMaxPacketSize 32
	0x40,                          // bInterval 64 (unit depends on device speed)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x07,                          // bLength
	0x05,                          // bDescriptorType (Endpoint)
	0x03,                          // bEndpointAddress (OUT/H2D)
	0x03,                          // bmAttributes (Interrupt)
	0x20, 0x00,                    // wMaxPacketSize 32
	0x10,                          // bInterval 16 (unit depends on device speed)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x09,                          // bLength
	0x04,                          // bDescriptorType (Interface)
	0x02,                          // bInterfaceNumber 2
	0x00,                          // bAlternateSetting
	0x01,                          // bNumEndpoints 1
	0xFF,                          // bInterfaceClass
	0x5D,                          // bInterfaceSubClass
	0x02,                          // bInterfaceProtocol
	0x00,                          // iInterface (String Index)
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x09,                          // bLength
	0x21,                          // bDescriptorType (HID)
	0x00, 0x01,                    // bcdHID 1.00
	0x01,                          // bCountryCode
	0x22,                          // bNumDescriptors
	0x84,                          // bDescriptorType[0] (Unknown 0x84)
	0x07, 0x00,                    // wDescriptorLength[0] 7
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13	
	0x07,                          // bLength
	0x05,                          // bDescriptorType (Endpoint)
	0x84,                          // bEndpointAddress (IN/D2H)
	0x03,                          // bmAttributes (Interrupt)
	0x20, 0x00,                    // wMaxPacketSize 32
	0x10,                          // bInterval 16 (unit depends on device speed)
	// ---------------------------------------------	
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	// ---------------------------------------------	
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x03,        // bInterfaceNumber 3
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	0xFF,        // bInterfaceClass
	0xFD,        // bInterfaceSubClass
	0x13,        // bInterfaceProtocol
	0x04,        // iInterface (String Index)
	
	0x06,        // bLength
	0x41,        // bDescriptorType (Unknown)
	0x00, 0x01, 0x01, 0x03
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
	sizeof(STR_MANUFACTURER_SONY),
	0x03,
	{STR_MANUFACTURER_SONY}
};
static const struct usb_string_descriptor_struct PROGMEM productDesc = {
	sizeof(STR_PRODUCT_SIXAXIS),
	0x03,
	{STR_PRODUCT_SIXAXIS}
};
static const struct usb_string_descriptor_struct PROGMEM controller1Desc = {
	sizeof(STR_CONTROLLER_P1),
	0x03,
	{STR_CONTROLLER_P1}
};
static const struct usb_string_descriptor_struct PROGMEM controller2Desc = {
	sizeof(STR_CONTROLLER_P2),
	0x03,
	{STR_CONTROLLER_P2}
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
		{0x2100, CONTROLLER_P1_INTERFACE, config1_descriptor + CONTROLLER_P1_HID_DESC_OFFSET, 9},
		{0x2200, CONTROLLER_P1_INTERFACE, controller_hid_report_descriptor, sizeof(controller_hid_report_descriptor)},
		{0x2100, CONTROLLER_P2_INTERFACE, config1_descriptor + CONTROLLER_P2_HID_DESC_OFFSET, 9},
		{0x2200, CONTROLLER_P2_INTERFACE, controller_hid_report_descriptor, sizeof(controller_hid_report_descriptor)},
		{0x2100, DEBUG_TX_INTERFACE, config1_descriptor + DEBUG_HID_DESC_OFFSET, 9},
		{0x2200, DEBUG_TX_INTERFACE, debug_hid_report_descriptor, sizeof(debug_hid_report_descriptor)},
	{0x0300, 0x0000, (const uint8_t *)&langDesc, 4},
	{0x0301, 0x0409, (const uint8_t *)&manufacturerDesc, sizeof(STR_MANUFACTURER_SONY)},
	{0x0302, 0x0409, (const uint8_t *)&productDesc, sizeof(STR_PRODUCT_SIXAXIS)},
	{0x0303, 0x0409, (const uint8_t *)&controller1Desc, sizeof(STR_CONTROLLER_P1)},
	{0x0304, 0x0409, (const uint8_t *)&controller2Desc, sizeof(STR_CONTROLLER_P2)},
	{0x0305, 0x0409, (const uint8_t *)&debugDesc, sizeof(STR_DEBUG_INTERFACE)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))

/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/
// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration = 0;

static const usb_controller_state_t PROGMEM controller_idle_state = {
	.triangle_btn = 0, .square_btn = 0, .cross_btn = 0, .circle_btn = 0,
	.l1_btn = 0, .r1_btn = 0, .l2_btn = 0, .r2_btn = 0,
	.select_btn = 0, .start_btn = 0, .ps_btn = 0,
	.direction = 0x08,
	.left_stick_axis_x = 0x80, .left_stick_axis_y = 0x80, .right_stick_axis_x = 0x80, .right_stick_axis_y = 0x80,
	.unknown = {0x00, 0x00, 0x00, 0x00},
	.circle_axis = 0x00, .cross_axis = 0x00, .square_axis = 0x00, .triangle_axis = 0x00,
	.l1_axis = 0x00, .r1_axis = 0x00, .l2_axis = 0x00, .r2_axis = 0x00
};

/*
 * Series of bytes that appear in control packets right after the HID
 * descriptor is sent to the host. They where discovered by tracing output
 * from a Madcatz SF4 Joystick. Sending these bytes makes the PS button work. */
static const uint8_t PROGMEM magic_init_bytes[] = {
	0x21, 0x26, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00
};

static uint8_t controller_P1_idle_config = 0;
static uint8_t controller_P2_idle_config = 0;

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t controller_P1_protocol = 1;
static uint8_t controller_P2_protocol = 1;


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
	UDIEN = (1<<EORSTE)|(1<<SOFE);
	sei();
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void) {
	return usb_configuration;
}

usb_controller_state_t usb_controller_p1;
usb_controller_state_t usb_controller_p2;
usb_controller_state_t USB_CONTROLLER_UNASSIGNED;

inline void usbControllerResetState(usb_controller_state_t controller_state) {
	memcpy_P(&controller_state, &controller_idle_state, sizeof(usb_controller_state_t));
}

int8_t usbControllerP1SendReport() {
	uint8_t intr_state, timeout, i;

	if (!usb_configuration) return -1;
	intr_state = SREG;
	cli();
	UENUM = CONTROLLER_P1_ENDPOINT;
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
		UENUM = CONTROLLER_P1_ENDPOINT;
	}

	for (i=0; i<sizeof(usb_controller_state_t); i++) {
		UEDATX = ((uint8_t*)&usb_controller_p1)[i];
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
	UENUM = CONTROLLER_P2_ENDPOINT;
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
		UENUM = CONTROLLER_P2_ENDPOINT;
	}

	for (i = 0; i < sizeof(usb_controller_state_t); i++) {
		UEDATX = ((uint8_t*)&usb_controller_p2)[i];
	}

	UEINTX = 0x3A;
	SREG = intr_state;
	return 0;
}


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
		if (wIndex == CONTROLLER_P1_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					if(wValue==0x0300){
						usb_wait_in_ready();
						for (i=0; i<sizeof(magic_init_bytes); i++) {
							UEDATX = pgm_read_byte(&magic_init_bytes[i]);
						}
					usb_send_in();
					}
					return;
				}
				if (bRequest == HID_GET_IDLE) {
					usb_wait_in_ready();
					UEDATX = controller_P1_idle_config;
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_PROTOCOL) {
					usb_wait_in_ready();
					UEDATX = controller_P1_protocol;
					usb_send_in();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if (bRequest == HID_SET_REPORT) {
					usb_wait_receive_out();
					usb_ack_out();
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_IDLE) {
					controller_P1_idle_config = (wValue >> 8);
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_PROTOCOL) {
					controller_P1_protocol = wValue;
					usb_send_in();
					return;
				}
			}
		}
		if (wIndex == CONTROLLER_P2_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();

					for (i = 0; i < sizeof(magic_init_bytes); i++) {
						UEDATX = pgm_read_byte(&magic_init_bytes[i]);
					}

					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_IDLE) {
					usb_wait_in_ready();
					UEDATX = controller_P2_idle_config;
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_PROTOCOL) {
					usb_wait_in_ready();
					UEDATX = controller_P2_protocol;
					usb_send_in();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if (bRequest == HID_SET_REPORT) {
					usb_wait_receive_out();
					usb_ack_out();
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_IDLE) {
					controller_P2_idle_config = (wValue >> 8);
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_PROTOCOL) {
					controller_P2_protocol = wValue;
					usb_send_in();
					return;
				}
			}
		}
	}
	UECONX = (1<<STALLRQ) | (1<<EPEN);	// stall
}

#endif
