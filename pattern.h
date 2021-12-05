#ifndef pattern_h_
#define pattern_h_

#include "widget.h"

#include <Arduino.h>

class Pattern: public Widget
{
private:
  int m_curSeq, m_curNote;
  int16_t **m_sequences;
  uint16_t *m_lengths, *m_poses;
  int16_t m_num;
  int m_slColor;
  bool m_shift;

public:
  Pattern(int16_t **sequences, uint16_t *lengths, uint16_t *poses, uint16_t num):
    m_sequences(sequences), m_lengths(lengths), m_poses(poses), m_num(num), m_curSeq(0), m_slColor(sl_color) {};

  void up();
  void down();
  void left();
  void right();
  void shift(bool s);

  void init();
  void draw(bool running);

private:
  void drawPattern(int i, int sampler, int selected, bool running);
};

#endif
