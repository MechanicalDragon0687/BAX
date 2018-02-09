#ifndef WTHREADS_H
#define WTHREADS_H

void *VideoDecodeThread(void *main_state);
void *PostProcessThread(void *main_state);
void *LZ4CompressThread(void *main_state);
void *BAXBuildThread(void *main_state);

#endif
