#include <SD.h>
#include "wav.h"

const char *c_riff = "RIFF";
const char *c_wave = "WAVE";
const char *c_fmt = "fmt";
const char *c_data = "data";

int32_t bytesToInt(char *bytes, int len)
{
  int32_t output = 0;
  for (int i = 0; i < len; ++i)
  {
    output += ((int)bytes[i]) << i * 8;
  }
  return output;
}

// WAV file reader based on format spec from http://www.topherlee.com/software/pcm-tut-wavformat.html
void loadSample(char *path, uint16_t *&data)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
    return;

  char header[4];
  file.readBytes(header, 4); // read magic bits
  if (strncmp(header, c_riff, 4)) return; // check them

  file.readBytes(header, 4); // read file size
  //int32_t totalSize = bytesToInt(header, 4);

  file.readBytes(header, 4); // read file type header
  if (strncmp(header, c_wave, 4)) return; // check them

  file.readBytes(header, 4); // read file type header
  if (strncmp(header, c_fmt, 3)) return; // check them

  file.readBytes(header, 4); // read the format size? (not really important?)
  file.readBytes(header, 2); // read the format? should be PCM/1?

  file.readBytes(header, 2); // read the channels
  int channels = (int)bytesToInt(header, 2);
  if (channels != 2) return; // TODO: support multiple formats

  file.readBytes(header, 4); // read the sample rate
  int32_t sampleRate = bytesToInt(header, 4);
  if (sampleRate != 44100) return; // TODO: resample to 44100 if needed

  file.readBytes(header, 4); // read the bytes per second
  //int32_t bps = bytesToInt(header, 4);

  file.readBytes(header, 2); // read the bytes per frame
  int bpf = (int)bytesToInt(header, 2);

  file.readBytes(header, 2); // read the bits per sample
  //int bps = bytesToInt(header, 2);

  file.readBytes(header, 4); // read the data header
  if (strncmp(header, c_data, 4)) return;

  file.readBytes(header, 4); // read the data size
  int32_t dataSize = bytesToInt(header, 4);

  int32_t frames = dataSize / bpf;
  //if (frames != 10905) return;

  if (data) extmem_free(data);
  data = (uint16_t*)extmem_malloc((frames + 1) * sizeof(int));
  //data[0] = (129 << 24) | frames;
  data[0] = frames;
  char *tmp = (char*)malloc(bpf);
  for (int32_t i = 1; i < frames + 1; ++i)
  {
    file.readBytes(tmp, bpf);
    data[i] = bytesToInt(tmp, 2);
  }
  free(tmp);

  file.close();
}
