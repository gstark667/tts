#ifndef pattern_h_
#define pattern_h_

#include "widget.h"

#include <Arduino.h>

class Pattern: public Widget
{
private:
  int m_curSeq;
  int16_t **m_sequences;
  uint16_t *m_lengths, *m_poses;
  int16_t m_num;

public:
  Pattern(int16_t **sequences, uint16_t *lengths, uint16_t *poses, uint16_t num):
    m_sequences(sequences), m_lengths(lengths), m_poses(poses), m_num(num), m_curSeq(0) {};

  void up();
  void down();
  void left();
  void right();

  void init();
  void draw();

private:
  void drawPattern(int i, int sampler, int selected);
};

#endif
