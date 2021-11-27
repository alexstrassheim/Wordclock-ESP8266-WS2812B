#include <Arduino.h>
#include <WClock.h>

void WClock::begin(void)
{
    DEBUG_PRINTLN("Begin WordClock Matrix");
    //FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void WClock::resetAndBlack()
{
    for (int i = 0; i < NUM_LEDS; ++i)
    {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

// https://github.com/FastLED/FastLED/blob/master/examples/XYMatrix/XYMatrix.ino
uint16_t WClock::XY(uint8_t y, uint8_t x)
{
    uint16_t i;
    i = (y * kMatrixWidth) + x;
    return i;
}

void WClock::setLed(int row, int col, boolean on)
{
    if (on)
        //  leds[XY(row,col)] = CRGB::Blue;
        leds[XY(row, col)] = color;
    else
        leds[XY(row, col)] = CRGB::Black;
    FastLED.show();
}

void WClock::updateDisplay(boolean previousframe[11][11], boolean frame[11][11])
{
    for (int r = 0; r < 11; ++r)
    {
        for (int c = 0; c < 11; ++c)
        {
            if (prevframe[r][c] && !frame[r][c])
            {
                setLed(r, c, false);
            }
            else if (!prevframe[r][c] && frame[r][c])
            {
                setLed(r, c, true);
            }
        }
    }
}

void WClock::addWordToFrame(const int theword[3], boolean frame[11][11])
{
    for (int i = 0; i < theword[2]; ++i)
    {
        frame[theword[0]][theword[1] + i] = true;
    }
}

void WClock::showTime(int hours, int minutes, int seconds)
{
    int h = hours;
    //int h2 = h;
    int m = minutes;
    //int s = seconds;

    for (int r = 0; r < 11; r++)
    {
        for (int c = 0; c < 11; c++)
        {
            frame[r][c] = false;
        }
    }

    addWordToFrame(w_es, frame);
    addWordToFrame(w_ist, frame);

    if (m >= 5 && m < 10)
    {
        addWordToFrame(w_minutes[0], frame);
        addWordToFrame(w_nach, frame);
    }
    else if (m >= 10 && m < 15)
    {
        addWordToFrame(w_minutes[1], frame);
        addWordToFrame(w_nach, frame);
    }
    else if (m >= 15 && m < 20)
    {
        addWordToFrame(w_minutes[4], frame);
        addWordToFrame(w_nach, frame);
    }
    else if (m >= 20 && m < 25)
    {
        addWordToFrame(w_minutes[2], frame);
        addWordToFrame(w_nach, frame);
    }
    else if (m >= 25 && m < 30)
    {
        addWordToFrame(w_minutes[0], frame);
        addWordToFrame(w_vor, frame);
        addWordToFrame(w_minutes[5], frame);
    }
    else if (m >= 30 && m < 35)
    {
        addWordToFrame(w_minutes[5], frame);
    }
    else if (m >= 35 && m < 40)
    {
        addWordToFrame(w_minutes[0], frame);
        addWordToFrame(w_nach, frame);
        addWordToFrame(w_minutes[5], frame);
    }
    else if (m >= 40 && m < 45)
    {
        addWordToFrame(w_minutes[2], frame);
        addWordToFrame(w_vor, frame);
    }
    else if (m >= 45 && m < 50)
    {
        addWordToFrame(w_minutes[4], frame);
        addWordToFrame(w_vor, frame);
    }
    else if (m >= 50 && m < 55)
    {
        addWordToFrame(w_minutes[1], frame);
        addWordToFrame(w_vor, frame);
    }
    else if (m >= 55 && m < 60)
    {
        addWordToFrame(w_minutes[0], frame);
        addWordToFrame(w_vor, frame);
    }

    if (h >= 12)
        h -= 12;

    if (m >= 25)
        h++;

    if (h == 12)
        h = 0;

    switch (h)
    {
    case 0:
        addWordToFrame(w_hours[12], frame);
        break;
    case 1:
        if (m > 4)
            addWordToFrame(w_hours[1], frame);
        else
            addWordToFrame(w_hours[0], frame);
        break;
    case 2:
        addWordToFrame(w_hours[2], frame);
        break;
    case 3:
        addWordToFrame(w_hours[3], frame);
        break;
    case 4:
        addWordToFrame(w_hours[4], frame);
        break;
    case 5:
        addWordToFrame(w_hours[5], frame);
        break;
    case 6:
        addWordToFrame(w_hours[6], frame);
        break;
    case 7:
        addWordToFrame(w_hours[7], frame);
        break;
    case 8:
        addWordToFrame(w_hours[8], frame);
        break;
    case 9:
        addWordToFrame(w_hours[9], frame);
        break;
    case 10:
        addWordToFrame(w_hours[10], frame);
        break;
    case 11:
        addWordToFrame(w_hours[11], frame);
        break;
    }

    if (m < 5)
        addWordToFrame(w_uhr, frame);

    int singleMinutes = m % 5;
    switch (singleMinutes)
    {
    case 1:
        addWordToFrame(w_minutes[6], frame);
        break;
    case 2:
        addWordToFrame(w_minutes[6], frame);
        addWordToFrame(w_minutes[7], frame);
        break;
    case 3:
        addWordToFrame(w_minutes[6], frame);
        addWordToFrame(w_minutes[7], frame);
        addWordToFrame(w_minutes[8], frame);
        break;
    case 4:
        addWordToFrame(w_minutes[6], frame);
        addWordToFrame(w_minutes[7], frame);
        addWordToFrame(w_minutes[8], frame);
        addWordToFrame(w_minutes[9], frame);
        break;
    }

    updateDisplay(prevframe, frame);

    for (int r = 0; r < 11; r++)
    {
        for (int c = 0; c < 11; c++)
        {
            prevframe[r][c] = frame[r][c];
        }
    }
}

void WClock::setBrightness(int val)
{
    FastLED.setBrightness(val);
    FastLED.show();
}

void WClock::setColor(int val)
{
    if (oldVal != val)
    {
        oldVal = val;
        //     DEBUG_PRINT("Value: ");
        //   DEBUG_PRINTLN(val);
    }

    color = CHSV(val, 255, 255);

    for (int r = 0; r < 11; r++)
    {
        for (int c = 0; c < 11; c++)
        {
            if (frame[r][c] == true)
            {
                leds[XY(r, c)] = color;
            }
        }
    }
    FastLED.show();
}

void WClock::setRGBColor(int r, int g, int b)
{
    color = CRGB(r, g, b);
    for (int r = 0; r < 11; r++)
    {
        for (int c = 0; c < 11; c++)
        {
            if (frame[r][c] == true)
            {
                leds[XY(r, c)] = color;
            }
        }
    }
    FastLED.show();
}

void WClock::update()
{
    FastLED.show();
}
