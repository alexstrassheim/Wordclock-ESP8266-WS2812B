#ifndef WClock_h
#define WClock_h

#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

/*
 * Turn debugging on or off
 */
#define WClock_DEBUG

/*
 * Define where DEBUG output will be printed
 */
#define DEBUG_PRINTER Serial

#ifdef WClock_DEBUG
    #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
    #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
    #define DEBUG_PRINT(...) {}
    #define DEBUG_PRINTLN(...) {}
#endif

/*
 * Number of used LEDs for WordClock
 */
#define DATA_PIN        D2
#define NUM_LEDS        121
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B
#define BRIGHTNESS      255

class WClock {
    public:
        // WClock(int LED_PIN, int NUM_LEDS);
        void begin(void);
        void drawHeart();
        void drawMatrix();
        void resetAndBlack();
        void update();

        uint16_t XY( uint8_t y, uint8_t x);
        void setLed(int row, int col, boolean on);
        void updateDisplay(boolean previousframe[11][11], boolean frame[11][11]);
        void addWordToFrame(const int theword[3], boolean frame[11][11]);

        void showTime(int hours, int minutes, int seconds);
        void setBrightness(int val);
        void setColor(int val);
        void setRGBColor(int r, int g, int b);
    private:
        CRGB leds[NUM_LEDS];
        boolean frame[11][11];
        boolean prevframe[11][11];
        const uint8_t kMatrixWidth = 11;
        const uint8_t kMatrixHeight = 11;

        int hours = 0, minutes = 0, seconds = 0;

        CRGB color = CRGB( 50, 100, 150);

        int oldVal = 0;

        // Wörter
        // Format: { column index, start index, length }
        const int w_funk[3] =       {3, 3, 4};

        const int w_es[3] =         {10, 0, 2};
        const int w_ist[3] =        {10, 3, 3};
        const int w_vor[3] =        {7, 0, 3};
        const int w_nach[3] =       {7, 7, 4};
        const int w_uhr[3] =        {1, 8, 3};

        const int w_hours[13][3] = {
            {5, 0, 3 }, // Ein
            {5, 0, 4 }, // Eins
            {5, 7, 4}, // Zwei
            {4, 0, 4}, // Drei
            {4, 7, 4}, // Vier
            {6, 7 ,4}, // Fünf
            {3, 0, 5}, // Sechs
            {2, 0, 6}, // Sieben
            {3, 7, 4 }, // Acht
            {1, 3, 4}, // Neun
            {1, 0, 4}, // Zehn
            {6, 5, 3}, // Elf
            {2, 6, 5}, // Zwölf
        };
        const int w_minutes[10][3] = {
            {10, 7 ,4}, // Fünf
            {9, 0, 4}, // Zehn
            {9, 4, 7}, // Zwanzig
            {8, 0, 11}, // Dreiviertel
            {8, 4, 7}, // Viertel
            {6, 0, 4}, // Halb
            {0, 3, 1}, // eins
            {0, 4, 1}, // zwei
            {0, 6, 1}, // drei
            {0, 7, 1} // vier
        };
};

#endif

