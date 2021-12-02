#include <Arduino.h>
#include "sampler.h"
#include "utility/dspinst.h"


void Sampler::play(const uint16_t *data)
{
  //uint32_t format;

  playing = 0;
  prior = 0;
  //format = *data++;
  next = data;
  beginning = data;
  ///length = format & 0xFFFFFF;
  length = data[0];
  //playing = format >> 24;
  playing = 1;
}

void Sampler::stop(void)
{
  playing = 0;
}

void Sampler::update(void)
{
  audio_block_t *block;
  const uint16_t *in;
  int16_t *out;
  uint32_t tmp32, consumed;
  int16_t s0, s1, s2, s3, s4;
  int i;

  if (!playing) return;
  block = allocate();
  if (block == NULL) return;

  out = block->data;
  in = next;
  s0 = prior;

  for (i=0; i < AUDIO_BLOCK_SAMPLES; i += 2) {
    tmp32 = *in++;
    *out++ = (int16_t)(tmp32 & 65535);
    *out++ = (int16_t)(tmp32 >> 16);
  }
  consumed = AUDIO_BLOCK_SAMPLES;

  prior = s0;
  next = in;
  if (length > consumed) {
    length -= consumed;
  } else {
    playing = 0;
  }
  transmit(block);
  release(block);
}
