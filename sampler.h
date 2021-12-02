#ifndef sampler_h_
#define sampler_h_

#include "Arduino.h"
#include "AudioStream.h"

class Sampler : public AudioStream
{
public:
  Sampler(void) : AudioStream(0, NULL), playing(0) { }
  void play(const uint16_t *data);
  void stop(void);
  bool isPlaying(void) { return playing; }
  virtual void update(void);
private:
  const uint16_t *next;
  const uint16_t *beginning;
  uint32_t length;
  int16_t prior;
  volatile uint8_t playing;
};

#endif
