#pragma once
#include <common.h>
#include <anim.h>
#include <vq.h>

void bax_stream_fq(u32 tgt, vq *fq);
bool bax_cvt_fq(vq *wq, vq (*fq)[ANIM_TARGETS]);
u32 bax_cvt_wq(void *anim, u32 size, vq *wq);
