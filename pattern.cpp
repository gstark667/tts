#include "pattern.h"

void Pattern::up()
{
  if (m_shift)
  {
    m_sequences[m_curSeq][m_curNote] = constrain(m_sequences[m_curSeq][m_curNote] + 1, -1, 3);
  }
  else
  {
    m_curNote = constrain(m_curNote - 1, 0, m_lengths[m_curSeq] - 1);
  }
}

void Pattern::down()
{
  if (m_shift)
  {
    m_sequences[m_curSeq][m_curNote] = constrain(m_sequences[m_curSeq][m_curNote] - 1, -1, 3);
  }
  else
  {
    m_curNote = constrain(m_curNote + 1, 0, m_lengths[m_curSeq] - 1);
  }
}

void Pattern::left()
{
  m_curSeq--;
  if (m_curSeq < 0)
  {
    m_curSeq = 0;
  }
}

void Pattern::right()
{
  m_curSeq++;
  if (m_curSeq >= m_num)
  {
    m_curSeq = m_num - 1;
  }
}

void Pattern::shift(bool s)
{
  m_shift = s;
  if (m_shift)
  {
    m_slColor = pk_color;
  }
  else
  {
    m_slColor = hi_color;
  }
}

void Pattern::init()
{
  drawFrame();
}

void Pattern::draw(bool running)
{
  for (int i = 0; i < m_num; ++i)
  {
    drawPattern(i, i, m_curSeq, running);
  }
}

void Pattern::drawPattern(int n, int sampler, int selected, bool running)
{
  int xoff = (ui_padding_4 + ui_sampler_width) * n + ui_padding_4;
  int curNote = constrain(m_curNote, 0, m_lengths[m_curSeq] - 1);

  int start = 0;

  // update the scroll position
  if (m_poses[sampler] + 1 >= start + frame_lines)
  {
    start += m_poses[sampler] - start - frame_lines + 1;
  }
  else if (m_poses[sampler] < start)
  {
    start = m_poses[sampler];
  }

  int color = ui_color;
  if (selected == n)
  {
    color = sl_color;
  }
  
  int end = min(m_lengths[sampler], start + frame_lines);
  for (int i = start, j = 0, sample = -1; i < end; ++i, ++j)
  {
    if ((i == m_poses[sampler] && running) || (selected == n && i == curNote))
    {
      tft.setTextColor(m_slColor, bg_color);
    }
    else
    {
      tft.setTextColor(color, bg_color);
    }

    sample = m_sequences[sampler][i];
    tft.setCursor(xoff + ui_padding_2, frame_offset + ui_padding_2 + j * (ui_text_height));

    tft.print(hex_2[sample]);
    tft.print("--");
  }
  tft.drawRect(xoff, frame_offset, ui_sampler_width + ui_padding_4, frame_height, ui_color);
}
