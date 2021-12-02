#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "sampler.h"
#include "wav.h"

// GUItool: begin automatically generated code
Sampler                  playMem4; //xy=103.88333129882812,522.88330078125
Sampler                  playMem1;       //xy=114.88333129882812,323.8833312988281
Sampler                  playMem3; //xy=115.88333129882812,453.8833312988281
Sampler                  playMem2;  //xy=118.88333129882812,388.8833312988281
AudioMixer4              mixer1;         //xy=443.8833312988281,421.8833312988281
AudioOutputI2S           i2s1;           //xy=729.88330078125,456.8833312988281
AudioConnection          patchCord1(playMem4, 0, mixer1, 3);
AudioConnection          patchCord2(playMem1, 0, mixer1, 0);
AudioConnection          patchCord3(playMem3, 0, mixer1, 2);
AudioConnection          patchCord4(playMem2, 0, mixer1, 1);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=476.8833312988281,714.8833312988281
// GUItool: end automatically generated code

IntervalTimer clk;

#define TFT_RST 1
#define TFT_SCK 2
#define TFT_MISO 3
#define TFT_MOSI 4
#define TFT_CS 5
#define TFT_DC 6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

#define AUDIO_MEMORY 8

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};
byte rowPins[ROWS] = { 9, 10, 11, 12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {24, 25, 26}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const char hex[16] = "0123456789ABCDEF";
const char *hex_2[256] = {
  "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
  "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
  "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
  "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
  "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
  "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
  "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
  "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
  "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
  "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
  "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
  "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
  "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
  "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
  "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
  "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

const int ledPin = 13;

uint16_t *samples[4] = {NULL, NULL, NULL, NULL};

int sequences[4][16] = {
  { 0, -1, -1, -1,  0, -1, -1, -1,  0, -1, -1, -1,  0, -1, -1,  0},
  {-1, -1,  1, -1, -1, -1,  1,  1, -1, -1,  1, -1, -1,  1,  1,  1},
  {-1,  3, -1,  3,  2,  3, -1,  3},
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};
int sequence_lengths[4] = {
  16, 16, 8, 16,
};
int sequence_poses[4] = {
  0, 0, 0, 0,
};

int cursor_x, cursor_y;
const int ui_padding = 1;
const int ui_padding_2 = ui_padding * 2;
const int ui_padding_4 = ui_padding * 4;
const int ui_text_height = 14;
const int ui_text_width = 12;
const int ui_sampler_width = ui_text_width * 4;
int frame_offset, frame_height, frame_lines;

int ui_color = ILI9341_WHITE;
int bg_color = ILI9341_BLACK;
int hi_color = ILI9341_YELLOW;

const int max_name = 16;
const int n_samplers = 4;
Sampler *samplers[n_samplers] = {
  &playMem1, &playMem2, &playMem3, &playMem4
};
char sampler_names[n_samplers][max_name];

volatile unsigned int i = 0;

int strncmp(char *a, char *b, int len)
{
  for (int i = 0; i < 0; i++)
  {
    if (a[i] != b[i])
    {
      return i;
    }
  }
  return 0;
}

void handleKeypad()
{
  int color = ui_color;
  if (keypad.getKeys())
  {
    for (int i = 0; i < 4; ++i)
    {
      if (keypad.key[i].stateChanged && (keypad.key[i].kstate == PRESSED || keypad.key[i].kstate == HOLD))
      {
        switch (keypad.key[i].kchar)
        {
        case '#':
          clk.end();
          break;
        case '*':
          clk.begin(processTrigger, 200000);
          break;
        case '6':
          cursor_y -= 16;
          break;
        case '4':
          cursor_y += 16;
          break;
        case '2':
          cursor_x -= 12;
          break;
        case '8':
          cursor_x += 12;
          break;
        }
      }
    }
  }
  tft.fillRect(cursor_x, cursor_y, 12, 14, color);
}

void drawFrame()
{
  frame_offset = ui_text_height + ui_padding_2;
  frame_height = tft.height() - frame_offset - ui_padding_4;
  frame_lines = 14;
  tft.setRotation(1);
  tft.fillScreen(bg_color);
  tft.setTextColor(ui_color, bg_color);
  tft.setTextSize(2);
  tft.setCursor(ui_padding, ui_padding);
  tft.setCursor(ui_padding, ui_padding);
  tft.println("Pattern 0");
  tft.drawRect(ui_padding, frame_offset, tft.width() - ui_padding_2, frame_height, ui_color);
}

void drawPattern(int i, int sampler)
{
  int xoff = (ui_padding_4 + ui_sampler_width) * i + ui_padding_4;

  int start = 0;

  // update the scroll position
  if (sequence_poses[sampler] + 1 >= start + frame_lines)
  {
    start += sequence_poses[sampler] - start - frame_lines + 1;
  }
  else if (sequence_poses[sampler] < start)
  {
    start = sequence_poses[sampler];
  }
  
  int end = min(sequence_lengths[sampler], start + frame_lines);
  for (int i = start, j = 0, sample = -1; i < end; ++i, ++j)
  {
    tft.setTextColor(ui_color, bg_color);
    if (i == sequence_poses[sampler])
    {
      tft.setTextColor(hi_color, bg_color);
    }
    // i = sequence position, j = display position
    sample = sequences[sampler][i];
    tft.setCursor(xoff + ui_padding_2, frame_offset + ui_padding_2 + j * (ui_text_height + ui_padding_2));
    if (sample == -1)
    {
      tft.print("--");
    }
    else
    {
      //tft.print(hex[(sample >> 4) & 0x0F]);
      //tft.print(hex[sample & 0x0F]);
      tft.print(hex_2[sample]);
    }
    tft.print("--");
  }
  tft.drawRect(xoff, frame_offset, ui_sampler_width + ui_padding_4, frame_height, ui_color);
}

void setup()
{
  // setup tft display
  tft.begin();
  drawFrame();

  cursor_x = 0;
  cursor_y = 16;

  AudioMemory(AUDIO_MEMORY);
  AudioNoInterrupts();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.0);

  //playSdWav1.frequency(28);
  clk.begin(processTrigger, 200000);
  //crash.begin(playCrash, 1200000);

  mixer1.gain(0, 0.50);
  mixer1.gain(1, 0.50);
  mixer1.gain(2, 0.15);
  mixer1.gain(3, 0.15);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  randomSeed(analogRead(0));

  SD.begin(BUILTIN_SDCARD);

  loadSample("kick.wav", samples[0]);
  loadSample("SNARE.wav", samples[1]);
  loadSample("CRASH.wav", samples[2]);
  loadSample("HAT.wav", samples[3]);

  for (int s = 0; s < n_samplers; ++s)
  {
    sprintf(sampler_names[s], "Sampler %d", s+1);
    /*for (int i = 0; i < sequence_lengths[s]; ++i)
    {
      sequences[s][i] = random(-1, 4);
    }*/
  }

  //pinMode(7, INPUT);
  //pinMode(13, OUTPUT);
  AudioInterrupts();
}

void loop()
{
  int knob = analogRead(A1);
  float vol = (float)knob / 1030.0;
  sgtl5000_1.volume(vol);

  //tft.fillScreen(ILI9341_BLACK);
  //tft.fillRect(0, 0, 48, 14, ILI9341_BLACK);
  tft.setCursor(ui_padding, ui_padding);
  //tft.println(vol);

  drawPattern(0, 0);
  drawPattern(1, 1);
  drawPattern(2, 2);
  drawPattern(3, 3);
  drawPattern(4, 3);
  drawPattern(5, 3);

  handleKeypad();
  /*char key = keypad.getKey();
  if (key != NO_KEY) {
    //tft.fillRect(0, 16, 12, 14, ILI9341_BLACK);
    tft.setCursor(0, 16);
    tft.println(key);
  }*/

  //delay(150);
}

void processTrigger()
{

  AudioNoInterrupts();

  int sample = -1;
  for (int s = 0; s < n_samplers; ++s)
  //for (int s = 0; s < 1; ++s)
  {
    sample = sequences[s][sequence_poses[s]];
    if (sample > -1)
    {
      samplers[s]->play(samples[sample]);
    }
    sequence_poses[s] = (sequence_poses[s] + 1) % sequence_lengths[s];
  }

  AudioInterrupts();
}
