//
//  main.c
//  macRogAuraCore
//
//  Created by Nick on 6/29/20.
//  Copyright © 2020 Nick. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>

//  ---------------------------------------
//  Constant declarations
//  ---------------------------------------

#define MESSAGE_LENGTH 17
#define MAX_NUM_MESSAGES 6
#define MAX_NUM_COLORS 4
#define MAX_NUM_SCALARS 4
#define MAX_FUNCNAME_LEN 32

//  ---------------------------------------
//  Function macros
//  ---------------------------------------

int verbose = 0;
#define V(x) if(!verbose); else x

//  ---------------------------------------
//  Data structures
//  ---------------------------------------

typedef struct {
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
} Color;

typedef struct {
    Color colors[MAX_NUM_COLORS];
    int scalars[MAX_NUM_SCALARS];
} Arguments;

typedef struct {
    int nMessages;
    uint8_t messages[MAX_NUM_MESSAGES][MESSAGE_LENGTH];
    int setAndApply;
} Messages;

typedef struct {
    const char *NAME;
    const char *name;
    int min;
    int max;
} ScalarDef;

typedef struct {
    const char *szName;
    void (*function)(Arguments *args, Messages *outputs);
    int nColors;
    int nScalars;
    ScalarDef scalars[MAX_NUM_SCALARS];
} FunctionRecord;

//  ---------------------------------------
//  USB protocol for keyboard
//  ---------------------------------------

const uint8_t SPEED_BYTE_VALUES[] = { 0xe1, 0xeb, 0xf5 };

uint8_t speedByteValue(int speed) {
    return SPEED_BYTE_VALUES[speed - 1];
}

const int BRIGHTNESS_OFFSET = 4;
uint8_t MESSAGE_BRIGHTNESS[MESSAGE_LENGTH] = { 0x5a, 0xba, 0xc5, 0xc4 };
uint8_t MESSAGE_SET[MESSAGE_LENGTH] = { 0x5d, 0xb5 };
uint8_t MESSAGE_APPLY[MESSAGE_LENGTH] = { 0x5d, 0xb4 };
uint8_t MESSAGE_INITIALIZE_KEYBOARD[MESSAGE_LENGTH] = { 0x5a, 0x41, 0x53, 0x55, 0x53, 0x20, 0x54, 0x65, 0x63, 0x68, 0x2e, 0x49, 0x6e, 0x63, 0x2e, 0x00 };

void initMessage(uint8_t *msg) {
    memset(msg, 0, MESSAGE_LENGTH);
    msg[0] = 0x5d;
    msg[1] = 0xb3;
}

void single_static(Arguments *args, Messages *outputs) {
    V(printf("single_static\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[4] = args->colors[0].nRed;
    m[5] = args->colors[0].nGreen;
    m[6] = args->colors[0].nBlue;
}

void single_breathing(Arguments *args, Messages *outputs) {
    V(printf("single_breathing\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[3] = 1;
    m[4] = args->colors[0].nRed;
    m[5] = args->colors[0].nGreen;
    m[6] = args->colors[0].nBlue;
    m[7] = speedByteValue(args->scalars[0]);
    m[9] = 1;
    m[10] = args->colors[1].nRed;
    m[11] = args->colors[1].nGreen;
    m[12] = args->colors[1].nBlue;
}

void single_colorcycle(Arguments *args, Messages *outputs) {
    V(printf("single_colorcycle\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[3] = 2;
    m[4] = 0xff;
    m[7] = speedByteValue(args->scalars[0]);
}

void multi_static(Arguments *args, Messages *outputs) {
    V(printf("multi_static\n"));
    outputs->nMessages = 4;
    for (int i = 0; i < 4; ++i) {
        uint8_t *m = outputs->messages[i];
        initMessage(m);
        m[2] = i + 1;
        m[4] = args->colors[i].nRed;
        m[5] = args->colors[i].nGreen;
        m[6] = args->colors[i].nBlue;
        m[7] = 0xeb;
    }
}

void multi_breathing(Arguments *args, Messages *outputs) {
    V(printf("multi_breathing\n"));
    outputs->nMessages = 4;
    for (int i = 0; i < 4; ++i) {
        uint8_t *m = outputs->messages[i];
        initMessage(m);
        m[2] = i + 1;
        m[3] = 1;
        m[4] = args->colors[i].nRed;
        m[5] = args->colors[i].nGreen;
        m[6] = args->colors[i].nBlue;
        m[7] = speedByteValue(args->scalars[0]);
    }
}

void set_brightness(Arguments *args, Messages *outputs) {
    V(printf("single_static\n"));
    memcpy(outputs->messages[0], MESSAGE_BRIGHTNESS, MESSAGE_LENGTH);
    outputs->messages[0][BRIGHTNESS_OFFSET] = args->scalars[0];
    outputs->nMessages = 1;
    outputs->setAndApply = 0;
}

void initialize_keyboard(Arguments *args, Messages *outputs) {
    V(printf("initialize_keyboard\n"));
    memcpy(outputs->messages[0], MESSAGE_INITIALIZE_KEYBOARD, MESSAGE_LENGTH);
    outputs->nMessages = 1;
    outputs->setAndApply = 0;
}

// Colors
const uint8_t RED[] = { 0xff, 0x00, 0x00 };
const uint8_t GREEN[] = { 0x00, 0xff, 0x00 };
const uint8_t BLUE[] = { 0x00, 0x00, 0xff };
const uint8_t YELLOW[] = { 0xff, 0xff, 0x00 };
const uint8_t GOLD[] = { 0xff, 0x8C, 0x00 };
const uint8_t CYAN[] = { 0x00, 0xff, 0xff };
const uint8_t MAGENTA[] = { 0xff, 0x00, 0xff };
const uint8_t WHITE[] = { 0xff, 0xff, 0xff };
const uint8_t BLACK[] = { 0x00, 0x00, 0x00 };

void red(Arguments *args, Messages *messages) {
    memcpy(args->colors, RED, 3);
    single_static(args, messages);
}

void green(Arguments *args, Messages *messages) {
    memcpy(args->colors, GREEN, 3);
    single_static(args, messages);
}

void blue(Arguments *args, Messages *messages) {
    memcpy(args->colors, BLUE, 3);
    single_static(args, messages);
}

void yellow(Arguments *args, Messages *messages) {
    memcpy(args->colors, YELLOW, 3);
    single_static(args, messages);
}

void gold(Arguments *args, Messages *messages) {
    memcpy(args->colors, GOLD, 3);
    single_static(args, messages);
}

void cyan(Arguments *args, Messages *messages) {
    memcpy(args->colors, CYAN, 3);
    single_static(args, messages);
}

void magenta(Arguments *args, Messages *messages) {
    memcpy(args->colors, MAGENTA, 3);
    single_static(args, messages);
}

void white(Arguments *args, Messages *messages) {
    memcpy(args->colors, WHITE, 3);
    single_static(args, messages);
}

void black(Arguments *args, Messages *messages) {
    memcpy(args->colors, BLACK, 3);
    single_static(args, messages);
}

void rainbow(Arguments *args, Messages *messages) {
    memcpy(&(args->colors[0]), RED, 3);
    memcpy(&(args->colors[1]), YELLOW, 3);
    memcpy(&(args->colors[2]), CYAN, 3);
    memcpy(&(args->colors[3]), MAGENTA, 3);
    multi_static(args, messages);
}

void on(Arguments *args, Messages *messages) {
    memcpy(args->colors, RED, 3);
    single_static(args, messages);
}

void off(Arguments *args, Messages *messages) {
    memcpy(args->colors, BLACK, 3);
    single_static(args, messages);
}

//  ---------------------------------------
//  Parse the arguments
//  ---------------------------------------

#define SPEED { "SPEED", "speed", 1, 3 }
#define BRIGHTNESS { "BRIGHTNESS", "brightness", 0, 3 }

const FunctionRecord FUNCTION_RECORDS[] = {
    { "initialize_keyboard", &initialize_keyboard, 0, 0 },
    { "brightness", &set_brightness, 0, 1, {BRIGHTNESS} },
    { "on", &red, 0, 0 },
    { "off", &black, 0, 0 },
    { "single_static", &single_static, 1, 0 },
    { "single_breathing", &single_breathing, 2, 1, {SPEED} },
    { "single_colorcycle", &single_colorcycle, 0, 1, {SPEED} },
    { "multi_static", &multi_static, 4, 0 },
    { "multi_breathing", &multi_breathing, 4, 1, {SPEED} },
    { "red", &red, 0, 0 },
    { "green", &green, 0, 0 },
    { "blue", &blue, 0, 0 },
    { "yellow", &yellow, 0, 0 },
    { "gold", &gold, 0, 0 },
    { "cyan", &cyan, 0, 0 },
    { "magenta", &magenta, 0, 0 },
    { "white", &white, 0, 0 },
    { "black", &black, 0, 0 },
    { "rainbow", &rainbow, 0, 0 },
};

const int NUM_FUNCTION_RECORDS = (int)(sizeof(FUNCTION_RECORDS) / sizeof(FUNCTION_RECORDS[0]));

void usage() {
    printf("macRogAuraCore - RGB keyboard control for Asus ROG laptops\n");
    printf("(c) 2020 black.dragon74 aka Nick\n\n");
    printf("Usage:\n");
    printf("   macRogAuraCore COMMAND ARGUMENTS\n\n");
    printf("COMMAND should be one of:\n");
    for (int i = 0; i < NUM_FUNCTION_RECORDS; ++i) {
        printf("   %s\n", FUNCTION_RECORDS[i].szName);
    }
}

int parseColor(char *arg, Color *pResult) {
    V(printf("parse color %s\n", arg));
    uint32_t v = 0;
    if (strlen(arg) != 6) goto fail;
    for (int i = 0; i < 6; ++i) {
        if (!isxdigit(arg[i])) goto fail;
    }
    v = (uint32_t)strtol(arg, 0, 16);
    if (errno == ERANGE) goto fail;
    pResult->nRed = (v >> 16) & 0xff;
    pResult->nGreen = (v >> 8) & 0xff;
    pResult->nBlue = v & 0xff;
    V(printf("interpreted color %d %d %d\n", pResult->nRed, pResult->nGreen, pResult->nBlue));
    return 0;
fail:
    fprintf(stderr, "Could not interpret color parameter value %s\n", arg);
    fprintf(stderr, "Please give this value as a six-character hex string like ff0000.\n");
    return -1;
}

int parseScalar(char *arg, ScalarDef type, int *pResult) {
    V(printf("parse speed %s\n", arg));
    long nSpeed = strtol(arg, 0, 0);
    if (errno == ERANGE || nSpeed < type.min || nSpeed > type.max) {
        fprintf(stderr, "Could not interpret %s parameter value %s\n"
                "Please give this value as an integer from %d to %d.\n",
                type.name, arg, type.min, type.max);
        return -1;
    }
    *pResult = (int)nSpeed;
    return 0;
}

int parseArguments(int argc, char **argv, Messages *messages) {
    int                   nRetval;
    Arguments             args;
    int                   nArgs         = 0;
    const FunctionRecord *pDesiredFunc  = 0;
    int                   nColors       = 0;
    int                   nScalars      = 0;

    // check for command line options
    while ((nRetval = getopt(argc, argv, "v")) != -1) {
        switch (nRetval) {
        case 'v':
            verbose = 1;
            break;
        default: /* '?' */
            usage();
            return -1;
        }
    }
    nArgs = argc - optind;

    // identify the function the user has asked for
    if (nArgs > 0) {
        for (int i = 0; i < NUM_FUNCTION_RECORDS; ++i) {
            if (!strncmp(argv[optind], FUNCTION_RECORDS[i].szName, MAX_FUNCNAME_LEN)) {
                pDesiredFunc = &(FUNCTION_RECORDS[i]);
                break;
            }
        }
    }
    if (!pDesiredFunc) {
        usage();
        return -1;
    }
    // check that the function signature is satisfied
    if (nArgs != (1 + pDesiredFunc->nColors + pDesiredFunc->nScalars)) {
        usage();
        printf("\nFunction %s takes ", pDesiredFunc->szName);
        if (pDesiredFunc->nColors > 0) {
            if (pDesiredFunc->nScalars == 1) {
                printf("%d color(s) and a speed", pDesiredFunc->nColors);
            } else if (pDesiredFunc->nScalars) {
                printf("%d color(s) and %d integers",
                       pDesiredFunc->nColors, pDesiredFunc->nScalars);
            } else {
                printf("%d color(s)", pDesiredFunc->nColors);
            }
        } else {
            if (pDesiredFunc->nScalars == 1) {
                const ScalarDef *d = pDesiredFunc->scalars;
                printf("a single integer from %d to %d", d->min, d->max);
            } else if (pDesiredFunc->nScalars) {
                printf("%d integers", pDesiredFunc->nScalars);
            } else {
                printf("no arguments");
            }
        }
        printf(":\n   rogauracore %s ", pDesiredFunc->szName);
        for (int i = 0; i < pDesiredFunc->nColors; i++) {
            printf("COLOR%d ", i+1);
        }
        for (int i = 0; i < pDesiredFunc->nScalars; i++) {
            printf("%s ", pDesiredFunc->scalars[i].NAME);
        }
        printf("\n\nCOLOR argument(s) should be given as hex values like ff0000\n");
        printf("SPEED argument should be given as an integer: 1, 2, or 3\n");
        return -1;
    }
    // parse the argument values
    for (int i = optind + 1; i < argc; ++i) {
        if (nColors < pDesiredFunc->nColors) {
            nRetval = parseColor(argv[i], &(args.colors[nColors]));
            if (nRetval < 0) return -1;
            nColors++;
        } else if (nScalars < pDesiredFunc->nScalars) {
            nRetval = parseScalar(argv[i], pDesiredFunc->scalars[nScalars],
                                  &(args.scalars[nScalars]));
            if (nRetval < 0) return -1;
            nScalars++;
        }
    }
    V(printf("args:\n"));
    for (int i = 0; i < MAX_NUM_COLORS; ++i) {
        V(printf("color%d %d %d %d\n", i + 1, args.colors[i].nRed, args.colors[i].nGreen, args.colors[i].nBlue));
    }
    for (int i = 0; i < pDesiredFunc->nScalars; ++i) {
      V(printf("%s %d\n", pDesiredFunc->scalars[i].name, args.scalars[i]));
    }
    // call the function the user wants
    pDesiredFunc->function(&args, messages);
    V(printf("constructed %d messages:\n", messages->nMessages));
    for (int i = 0; i < messages->nMessages; ++i) {
        V(printf("message %d: ", i));
        for (int j = 0; j < MESSAGE_LENGTH; j++)
        {
            V(printf("%02x ", messages->messages[i][j]));
        }
        V(printf("\n"));
    }
    return 0;
}

//  ---------------------------------------
//  IOHIDDevice interface
//  ---------------------------------------

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key) {
    CFTypeRef ref;
    int32_t value;
    
    ref = IOHIDDeviceGetProperty(device, key);
    if (ref) {
        if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
            CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
            return value;
        }
    }
    return 0;
}

static int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len) {
    CFStringRef str;
    
    if (!len)
        return 0;
    
    str = (CFStringRef) IOHIDDeviceGetProperty(device, prop);
    
    buf[0] = 0;
    
    if (str) {
        CFIndex str_len = CFStringGetLength(str);
        CFRange range;
        CFIndex used_buf_len;
        CFIndex chars_copied;
        
        len --;
        
        range.location = 0;
        range.length = ((size_t) str_len > len)? len: (size_t) str_len;
        chars_copied = CFStringGetBytes(str,
                                        range,
                                        kCFStringEncodingUTF32LE,
                                        (char) '?',
                                        FALSE,
                                        (UInt8*)buf,
                                        len * sizeof(wchar_t),
                                        &used_buf_len);
        
        if (chars_copied == len)
            buf[len] = 0; /* len is already decremented above */
        else
            buf[chars_copied] = 0;
        
        return 0;
    }
    else
        return -1;
    
}

// Vendor specific constants
const uint16_t ASUS_VENDOR_ID = 0x0b05;
const uint16_t ASUS_PRODUCT_IDS[] = { 0x1854, 0x1869, 0x1866, 0x19b6, 0x1932 };
const int NUM_ASUS_PRODUCTS = (int)(sizeof(ASUS_PRODUCT_IDS) / sizeof(ASUS_PRODUCT_IDS[0]));

int handleUsb(Messages *pMessages) {
    IOHIDManagerRef tIOHIDManagerRef = NULL;
    IOHIDDeviceRef *tIOHIDDeviceRefs = NULL;
    CFSetRef deviceCFSetRef= NULL;
    
    // Create an IOHIDManager reference
    tIOHIDManagerRef = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!tIOHIDManagerRef) goto exit;
    
    // Set the HID device matching dictionary
    IOHIDManagerSetDeviceMatching(tIOHIDManagerRef, NULL);
    
    // Now open the IOHIDManager reference
    IOReturn tIOReturn = IOHIDManagerOpen(tIOHIDManagerRef, kIOHIDOptionsTypeNone);
    if (tIOReturn == kIOReturnError) goto exit;
    
    // Copy out the devices
    deviceCFSetRef = IOHIDManagerCopyDevices(tIOHIDManagerRef);
    if (!deviceCFSetRef) goto exit;
    
    // Get the count of devices on the USB HID
    CFIndex deviceCount = CFSetGetCount(deviceCFSetRef);
    
    // convert the list into a C array so we can iterate easily
    tIOHIDDeviceRefs = calloc(deviceCount, sizeof(IOHIDDeviceRef));
    CFSetGetValues(deviceCFSetRef, (const void **) tIOHIDDeviceRefs);
    
    for (int i = 0; i < deviceCount; i++) {
        IOHIDDeviceRef dev = tIOHIDDeviceRefs[i];
        int dev_vid = get_int_property(dev, CFSTR(kIOHIDVendorIDKey));
        int dev_pid = get_int_property(dev, CFSTR(kIOHIDProductIDKey));
        if (dev_vid == ASUS_VENDOR_ID) {
            for (int i = 0; i < NUM_ASUS_PRODUCTS; ++i)
            {
                if (dev_pid == ASUS_PRODUCT_IDS[i]) {
                    // We got our device
                    wchar_t buf[256];
                    get_string_property(dev, CFSTR(kIOHIDProductKey), buf, 256);
                    printf("Found ROG Aura keyboard: %p\n", dev);
                    printf("  Name: %ls\n", buf);
                    printf("  VendorID: %04x\n", dev_vid);
                    printf("  ProductID: %04x\n", dev_pid);
                    
                    // Send the messages
                    for (int i = 0; i < pMessages->nMessages; ++i) {
                        IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, pMessages->messages[i][0], pMessages->messages[i], MESSAGE_LENGTH);
                    }
                    
                    if (pMessages->setAndApply) {
                        IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, MESSAGE_SET[0], MESSAGE_SET, MESSAGE_LENGTH);
                        IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, MESSAGE_APPLY[0], MESSAGE_APPLY, MESSAGE_LENGTH);
                    }
                    
                    V(printf("sent %d messages\n", pMessages->nMessages + 2));
                }
            }
        }
    }
    
exit:
    if (deviceCFSetRef) CFRelease(deviceCFSetRef);
    if (tIOHIDDeviceRefs) free(tIOHIDDeviceRefs);
    if (tIOHIDManagerRef) CFRelease(tIOHIDManagerRef);
    return 0;
}

//  ---------------------------------------
//  Driver function
//  ---------------------------------------

int main(int argc, char** argv) {
    Messages pMessages;
    pMessages.setAndApply = 1;
    
    if (parseArguments(argc, (char**)argv, &pMessages) == 0) {
        return handleUsb(&pMessages);
    }
    
    return -1;
}
