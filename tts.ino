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
bool running;

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

int16_t **sequences;

static int16_t example_data[4][16] = {
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

void play()
{
  clk.begin(processTrigger, 200000);
  running = true;
}

void pause()
{
  clk.end();
  running = false;
}

void stop()
{
  clk.end();
  running = false;
  for (int i = 0; i < n_samplers; ++i)
  {
    sequence_poses[i] = 0;
  }
}

void handleKeypad()
{
  int color = ui_color;
  if (keypad.getKeys())
  {
    int idx = keypad.findInList('*');
    if (idx > -1 && keypad.key[i].kstate == PRESSED || keypad.key[i].kstate == HOLD)
    {
      cur->shift(true);
    }
    else
    {
      cur->shift(false);
    }
    
    for (int i = 0; i < 10; ++i)
    {
      if (keypad.key[i].stateChanged && (keypad.key[i].kstate == PRESSED))
      {
        switch (keypad.key[i].kchar)
        {
        case '3':
          play();
          break;
        case '9':
          pause();
          break;
        case '6':
          cur->up();
          break;
        case '4':
          cur->down();
          break;
        case '2':
          cur->left();
          break;
        case '8':
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

  sequences = malloc(sizeof(int16_t) * 4);
  for (int i = 0; i < 4; ++i)
  {
    sequences[i] = malloc(sizeof(int16_t) * 16);
    for (int j = 0; j < 16; ++j)
    {
      sequences[i][j] = example_data[i][j];
    }
  }

  cur = new Pattern(sequences, sequence_lengths, sequence_poses, n_samplers);
  cur->init();

  AudioMemory(AUDIO_MEMORY);
  AudioNoInterrupts();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.0);

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
  }

  AudioInterrupts();
}

void loop()
{
  int knob = analogRead(A1);
  float vol = (float)knob / 1030.0;
  sgtl5000_1.volume(vol);

  handleKeypad();
  
  cur->draw(running);
}

void processTrigger()
{

  AudioNoInterrupts();

  int sample = -1;
  for (int s = 0; s < n_samplers; ++s)
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
