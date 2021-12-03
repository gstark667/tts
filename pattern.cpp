#include "pattern.h"


void Pattern::up()
{
  
}

void Pattern::down()
{
  
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

void Pattern::init()
{
  drawFrame();
}

void Pattern::draw()
{
  for (int i = 0; i < m_num; ++i)
  {
    drawPattern(i, i, m_curSeq);
  }
}

void Pattern::drawPattern(int i, int sampler, int selected)
{
  int xoff = (ui_padding_4 + ui_sampler_width) * i + ui_padding_4;

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
  if (selected == i)
  {
    color = sl_color;
  }
  
  int end = min(m_lengths[sampler], start + frame_lines);
  for (int i = start, j = 0, sample = -1; i < end; ++i, ++j)
  {
    tft.setTextColor(color, bg_color);
    if (i == m_poses[sampler])
    {
      tft.setTextColor(hi_color, bg_color);
    }
    // i = sequence position, j = display position
    sample = m_sequences[sampler][i];
    tft.setCursor(xoff + ui_padding_2, frame_offset + ui_padding_2 + j * (ui_text_height + ui_padding_2));
    if (sample == -1)
    {
      tft.print("--");
    }
    else
    {
      tft.print(hex_2[sample]);
    }
    tft.print("--");
  }
  tft.drawRect(xoff, frame_offset, ui_sampler_width + ui_padding_4, frame_height, ui_color);
}
