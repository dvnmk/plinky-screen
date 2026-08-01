#include "M5Cardputer.h"
#include <MIDI.h>

#define RX_PIN 4
#define TX_PIN 2

#define MARGIN_X 10
#define TITLE_Y 20
#define MESSAGE_Y 60

#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001F
#define COLOR_GREEN     0x07E0
#define COLOR_CYAN      0x07FF   // turquoise
#define COLOR_RED       0xF800
#define COLOR_VIOLET    0xF81F   // magenta/violet
#define COLOR_YELLOW    0xFFE0
#define COLOR_WHITE     0xFFFF

HardwareSerial UART(2);

MIDI_CREATE_INSTANCE(HardwareSerial, UART, MIDI);

struct CCEntry {
    byte number;
    const char* name;
	  uint16_t color;
};

CCEntry ccTable[] = {
	  {38,  "FOO",        COLOR_CYAN},

    {32,  "Volume",        COLOR_CYAN},
		{28,  "Stereo",        COLOR_YELLOW},
		{29,  "Delay Send",        COLOR_BLUE},
    {30,  "Reverb Send",     COLOR_VIOLET},
		{31,  "Toad Factor",     COLOR_RED},

		//		{36,  "Audio In",     COLOR_RED},

		{25,  "Octave",        COLOR_BLUE},
		{24,  "Pitch",        COLOR_BLUE},
		{23,  "Glide",        COLOR_BLUE},

		{34,  "Start",     COLOR_CYAN},
    {35,  "Length",     COLOR_CYAN},
		{33,  "Timestretch",     COLOR_CYAN},


    {36,  "Chorus",     COLOR_GREEN},
    {26,  "Subosc",     COLOR_GREEN},
    {27,  "Wavefold",     COLOR_GREEN},		

    {16,  "Highpass",     COLOR_YELLOW},
    {17,  "Cutoff",     COLOR_YELLOW},
    {18,  "Resonance",     COLOR_YELLOW},

    {19,  "Attack",     COLOR_RED},
    {20,  "Decay",     COLOR_RED},
    {21,  "Sustain",     COLOR_RED},
    {22,  "Release",     COLOR_RED},								

		//   {1,  "X",     COLOR_RED},
		//    {2,  "Y",     COLOR_RED},			

};

struct MixCCEntry {
    uint8_t mainCC;
    uint8_t rangeCC;
    const char* name;
};

MixCCEntry mixParams[] = {
    {16, 44, "Bass EQ / compressor"},
    {17, 45, "Mid EQ / compressor"},
    {18, 46, "Treble EQ / compressor"},
    {19, 47, "Tape dry"},
    {20, 48, "Audio in dry"},
    {21, 49, "Tape delay send"},
    {22, 50, "Tape reverb send"},
    {23, 51, "Audio in delay send"},
    {24, 52, "Audio in reverb send"},
    {25, 53, "Reverb shimmer"},
    {26, 54, "Reverb feedback"},
    {27, 55, "Delay time"},
    {28, 56, "Delay feedback"},
    {29, 57, "Reverb return"},
    {30, 58, "Delay return"},
    {31, 59, "Output level"},
    {32, 60, "Audio in sidechain"},
    {33, 61, "Reverb sidechain"},
    {34, 62, "Delay sidechain"}
};

const int ccCount = sizeof(ccTable) / sizeof(ccTable[0]);

CCEntry* getCCEntry(byte cc)
{
    for (int i = 0; i < ccCount; i++) {
        if (ccTable[i].number == cc) {
            return &ccTable[i];
        }
    }

    return nullptr;
}
void handleNoteOn(byte channel, byte note, byte velocity)
{
    M5Cardputer.Display.fillRect(0, 50, 240, 100, COLOR_BLACK);

    M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(COLOR_GREEN);

    M5Cardputer.Display.printf(
        "NOTE %d",
        note
    );

    M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y + 35);
    M5Cardputer.Display.setTextColor(COLOR_WHITE);

    M5Cardputer.Display.printf(
        "VEL %d",
        velocity
    );
}


void handleNoteOff(byte channel, byte note, byte velocity)
{
    M5Cardputer.Display.fillRect(0, 50, 240, 100, COLOR_BLACK);

    M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y);
    M5Cardputer.Display.setTextSize(3);
    M5Cardputer.Display.setTextColor(COLOR_RED);

    M5Cardputer.Display.printf(
        "NOTE %d",
        note
    );

    M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y + 35);
    M5Cardputer.Display.setTextColor(COLOR_WHITE);

    M5Cardputer.Display.printf(
        "OFF %d",
        velocity
    );
}

void drawTitle()
{
    M5Cardputer.Display.setCursor(MARGIN_X, TITLE_Y);
    M5Cardputer.Display.setTextSize(2);
    M5Cardputer.Display.print("PLINKY 12 BLOCKS");
}


const char* getMixParameter(byte control)
{
    if (control >= 16 && control <= 34) {
        static const char* names[] = {
            "Bass EQ",
            "Mid EQ",
            "Treble EQ",
            "Tape dry",
            "Audio in dry",
            "Tape delay",
            "Tape reverb",
            "Audio in delay",
            "Audio in reverb",
            "Reverb shimmer",
            "Reverb feedback",
            "Delay time",
            "Delay feedback",
            "Reverb return",
            "Delay return",
            "Output level",
            "Audio in sidechain",
            "Reverb sidechain",
            "Delay sidechain"
        };

        return names[control - 16];
    }

    if (control >= 44 && control <= 62) {
        static const char* names[] = {
            "Bass EQ",
            "Mid EQ",
            "Treble EQ",
            "Tape dry",
            "Audio in dry",
            "Tape delay",
            "Tape reverb",
            "Audio in delay",
            "Audio in reverb",
            "Reverb shimmer",
            "Reverb feedback",
            "Delay time",
            "Delay feedback",
            "Reverb return",
            "Delay return",
            "Output level",
            "Audio in sidechain",
            "Reverb sidechain",
            "Delay sidechain"
        };

        return names[control - 44];
    }

    return nullptr;
}

void handleControlChange(byte channel, byte control, byte value)
{
    M5Cardputer.Display.fillRect(0, 50, 240, 100, COLOR_BLACK);

    M5Cardputer.Display.setTextSize(3);

    const char* name = nullptr;
    CCEntry* entry = nullptr;

    if (channel == 13) {
        name = getMixParameter(control);
    } else {
        entry = getCCEntry(control);
    }

    M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y);

    if (channel == 13 && name != nullptr) {

        M5Cardputer.Display.setTextColor(COLOR_CYAN);
        M5Cardputer.Display.print(name);

    } else if (channel != 13 && entry != nullptr) {

        M5Cardputer.Display.setTextColor(entry->color);
        M5Cardputer.Display.print(entry->name);

    } else {

        M5Cardputer.Display.setTextColor(YELLOW);
        M5Cardputer.Display.printf("CC %d", control);
    }

    String val = String(value);

    int16_t width = M5Cardputer.Display.textWidth(val);

    M5Cardputer.Display.setCursor(
        240 - MARGIN_X - width,
        MESSAGE_Y + 35
    );

    M5Cardputer.Display.setTextColor(WHITE);
    M5Cardputer.Display.print(val);
}

/* void handleControlChange(byte channel, byte control, byte value) */
/* { */
/*     CCEntry* entry = getCCEntry(control); */

/*     M5Cardputer.Display.fillRect(0, 50, 240, 100, COLOR_BLACK); */

/*     M5Cardputer.Display.setTextSize(3); */

/*     if (entry != nullptr) { */

/*         // CC name */
/*         M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y); */
/*         M5Cardputer.Display.setTextColor(entry->color); */
/*         M5Cardputer.Display.print(entry->name); */

/*     } else { */

/*         // Unknown CC - display number */
/*         M5Cardputer.Display.setCursor(MARGIN_X, MESSAGE_Y); */
/*         M5Cardputer.Display.setTextColor(YELLOW); */
/*         M5Cardputer.Display.printf("CC %d", control); */
/*     } */

/*     // Value (always displayed) */
/*     String val = String(value); */

/*     int16_t width = M5Cardputer.Display.textWidth(val); */

/*     M5Cardputer.Display.setCursor( */
/*         240 - MARGIN_X - width, */
/*         MESSAGE_Y + 35 */
/*     ); */

/*     M5Cardputer.Display.setTextColor(WHITE); */
/*     M5Cardputer.Display.print(val); */
/* } */


void setup()
{
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(2);

		M5Cardputer.Display.clear();

		drawTitle();
		
    UART.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

		//		MIDI.setHandleNoteOn(handleNoteOn);
		//		MIDI.setHandleNoteOff(handleNoteOff);
		MIDI.setHandleControlChange(handleControlChange);
		
    MIDI.begin(MIDI_CHANNEL_OMNI);


 
}


void loop()
{
    MIDI.read();
}
