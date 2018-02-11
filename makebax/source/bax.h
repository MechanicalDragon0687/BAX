#ifndef BAX_H
#define BAX_H

typedef struct BAX_Header BAX_Header;
typedef struct BAX_FrameInfo BAX_FrameInfo;

BAX_Header *BAX_Init(int count, int rate, uint16_t bg, int offset, int width, const char *auth, const char *desc);
void BAX_Kill(BAX_Header *hdr);

BAX_FrameInfo *BAX_FrameInfoInit(BAX_Header *hdr);
void BAX_FrameInfoSet(BAX_FrameInfo *info, int frame, size_t offset, size_t size);
void BAX_FrameInfoKill(BAX_FrameInfo *info);

size_t BAX_DataStart(BAX_Header *hdr);
size_t BAX_HeaderSize(void);
size_t BAX_FrameInfoSize(BAX_Header *hdr);

#endif
