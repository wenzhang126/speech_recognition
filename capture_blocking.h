#ifndef CAPTURE_BLOCKING_H
#define CAPTURE_BLOCKING_H
#define SAMPLE_RATE  (16000)
#define WINDOW_SIZE 20 //ms
#define FRAMES_PER_BUFFER (WINDOW_SIZE * SAMPLE_RATE / 1000)
#define HOP_SIZE (WINDOW_SIZE / 2)
#define NUM_CHANNELS    (1)
/* #define DITHER_FLAG     (paDitherOff)  */
#define DITHER_FLAG     (0) /**/

/* @todo Underflow and overflow is disabled until we fix priming of blocking write. */
#define CHECK_OVERFLOW  (0)
#define CHECK_UNDERFLOW  (0)

/* Select sample format. */
#define PA_SAMPLE_TYPE  paInt16
#define SAMPLE_SIZE (2)
#define SAMPLE_SILENCE  (0)
#define CLEAR(a) memset( (a), 0,  FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
#define PRINTF_S_FORMAT "%d"

typedef short SAMPLE;

int main(void);
void write2file(cv::Mat mat);

#endif
