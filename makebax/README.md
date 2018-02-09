# makebax

###### BAX Animation creator

### Usage

`makebax "input" "output" [-t w] [-c l] [-r n] [-a "Author"] [-d "Description"] [-b bg_color]`

Parameters enclosed in `[ ]` are optional.

 - "input" is the input video file path, the size must have a resolution of either 400x240 (top screen only), 320x240 (bottom screen only) or 720x240 (both screens side-by-side). Input must be a container with at least one video stream supported by libav and must have a constant framerate.

 - "output" is the path of the BAX file that will be created.

 - "t" sets the work pipeline length, larger values make the program consume more memory. Default is 4 (approximately 350MiB used in a worst case scenario).

 - "c" sets the LZ4 compression level, larger values make the program take more CPU time. Default is 9.

 - "r" sets the frame rate. It should be automagically detected but there's some pretty weird formats out there that might need manual adjustment.

 - "a" sets the Author string. It doesn't affect the player at all, but it's nice to include some metadata. Maximum length is 32 ASCII characters (or as much UTF-8 fits in there).

 - "d" is the same as above but for description. Maximum length is 192 ASCII characters.


### Compiling

Requires `pthreads` and  `lz4` (besides the obvious `make` and `cc`).


#### Notes

Although it should be compatible with big endian platforms (PowerPC, Microblaze, etc) this has not been properly tested yet. Please report any issues regarding compilation/usage on big endian platforms.

Please don't use VBR files as input. I'm begging you.
