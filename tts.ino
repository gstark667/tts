#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Keypad.h>

#include "sampler.h"
#include "wav.h"

#include "pattern.h"

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

const int ledPin = 13;

uint16_t *samples[4] = {NULL, NULL, NULL, NULL};

static int16_t sequences[4][16] = {
  { 0, -1, -1, -1,  0, -1, -1, -1,  0, -1, -1, -1,  0, -1, -1,  0},
  {-1, -1,  1, -1, -1, -1,  1,  1, -1, -1,  1, -1, -1,  1,  1,  1},
  {-1,  3, -1,  3,  2,  3, -1,  3, -1,  3, -1,  3,  2,  3, -1,  3},
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};
static uint16_t sequence_lengths[4] = {
  16, 16, 8, 16,
};
static uint16_t sequence_poses[4] = {
  0, 0, 0, 0,
};

const int max_name = 16;
const uint16_t n_samplers = 4;
Sampler *samplers[n_samplers] = {
  &playMem1, &playMem2, &playMem3, &playMem4
};
char sampler_names[n_samplers][max_name];

Widget *cur;

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
    for (int i = 0; i < 10; ++i)
    {
      if (keypad.key[i].stateChanged && (keypad.key[i].kstate == PRESSED))
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
          cursor_y--;
          cur->up();
          break;
        case '4':
          cursor_y++;
          cur->down();
          break;
        case '2':
          cursor_x--;
          cur->left();
          break;
        case '8':
          cursor_x++;
          cur->right();
          break;
        }
      }
    }
  }
}

void setup()
{
  // setup tft display
  tft.begin();

  int16_t **seqs = malloc(sizeof(int16_t) * 4);
  for (int i = 0; i < 4; ++i)
  {
    seqs[i] = malloc(sizeof(int16_t) * 16);
    for (int j = 0; j < 16; ++j)
    {
      seqs[i][j] = sequences[i][j];
    }
  }

  cur = new Pattern(seqs, sequence_lengths, sequence_poses, n_samplers);
  cur->init();

  cursor_x = 0;
  cursor_y = 0;

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
    for (int i = 0; i < sequence_lengths[s]; ++i)
    {
      sequences[s][i] = random(-1, 4);
    }
  }

  AudioInterrupts();
}

void loop()
{
  int knob = analogRead(A1);
  float vol = (float)knob / 1030.0;
  sgtl5000_1.volume(vol);

  //tft.fillScreen(ILI9341_BLACK);
  //tft.fillRect(0, 0, 48, 14, ILI9341_BLACK);
  /*tft.setTextColor(ui_color, bg_color);
  tft.setCursor(tft.width() - ui_padding - 4 * ui_text_width, 0);
  tft.print(AudioProcessorUsage());
  tft.setCursor(128, 0);
  tft.print(cursor_x);
  tft.print(",");
  tft.print(cursor_y);*/

  handleKeypad();
  
  cur->draw();
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
