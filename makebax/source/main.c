#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wthreads.h"
#include "main.h"

/*
 How all this works together:

    - decompress frames from codec to RGB565 through libavcodec magic
    - delta encoding and transposing (preferably) in-buffer
    - compress the frame with LZ4
    - write the frames to the output BAX file
*/

int main(int argc, char *argv[])
{
    MainState *main_state;
    const char *author = NULL, *info = NULL;
    int workring_width = WORKRING_DEF_SIZE, complvl = LZ4_DEF_COMP, rate = -1, bgc = 0;

    printf("BAXcomp v" BAXCOMP_VERSION "\n");
    if (argc < 3) {
        abort_error(
            "Usage: %s \"input.ivf\" \"output.bax\" [-t w] [-c l] [-r n] "
            "[-b bg_color] [-a \"Author\"] [-d \"Description\"]\n",
            argv[0]);
    }

    if (argc >= 5) {
        for (int i = 3; i < argc; i++) {
            char opt_char;
            if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
                opt_char = argv[i++][1];
                switch(opt_char) {
                    case 't':
                        workring_width = strtol(argv[i], NULL, 0);
                        break;
                    case 'c':
                        complvl = strtol(argv[i], NULL, 0);
                        break;
                    case 'r':
                        rate = strtol(argv[i], NULL, 0);
                        break;
                    case 'b':
                        bgc = strtol(argv[i], NULL, 0);
                        break;
                    case 'a':
                        author = argv[i];
                        break;
                    case 'd':
                        info = argv[i];
                        break;
                    default:
                        abort_error("Unknown option \"-%c\"\n", opt_char);
                }
            }
        }
    }

    if (workring_width < WORKRING_MIN_SIZE || workring_width > WORKRING_MAX_SIZE) {
        printf("WARNING: Invalid pipeline length %d, using %d instead\n",
            workring_width, WORKRING_DEF_SIZE);
        workring_width = WORKRING_DEF_SIZE;
    }

    if (complvl < LZ4_MIN_COMP || complvl > LZ4_MAX_COMP) {
        printf("WARNING: Invalid compression level %d, using %d instead\n",
            complvl, LZ4_DEF_COMP);
        complvl = LZ4_DEF_COMP;
    }

    main_state = malloc(sizeof(MainState));
    assert(main_state != NULL);
    memset(main_state, 0, sizeof(MainState));
    main_state->in_path  = argv[1];
    main_state->out_path = argv[2];

    main_state->author = author;
    main_state->info = info;

    main_state->compression_lvl = complvl;
    main_state->state = -1;

    main_state->background_color = bgc;
    main_state->frame_rate = rate;

    for (int i = 0; i < 3; i++)
        main_state->ring[i] = RingBuffer_Init(workring_width);

    assert(pthread_create(&main_state->thread[0], NULL, VideoDecodeThread, main_state) == 0);
    assert(pthread_create(&main_state->thread[1], NULL, PostProcessThread, main_state) == 0);
    assert(pthread_create(&main_state->thread[2], NULL, LZ4CompressThread, main_state) == 0);
    assert(pthread_create(&main_state->thread[3], NULL, BAXBuildThread, main_state) == 0);

    pthread_join(main_state->thread[0], NULL);
    pthread_join(main_state->thread[1], NULL);
    pthread_join(main_state->thread[2], NULL);
    pthread_join(main_state->thread[3], NULL);

    for (int i = 0; i < 3; i++)
        RingBuffer_Kill(main_state->ring[i]);

    free(main_state);
    printf("\n");
    return 0;
}
