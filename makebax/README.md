# makebax

###### BAX Animation creator

### Usage

`makebax "input.ivf" "output.bax" [-t w] [-c l] [-r n] [-b bg_color] [-a "Author"] [-d "Description"]`

Parameters enclosed in `[ ]` are optional.

 - `input.ivf` is the input IVF file path, the size must have a resolution of either 400x240 (top screen only), 320x240 (bottom screen only) or 720x240 (both screens side-by-side). Input must be an IVF container with a VP8/VP9 FourCC.

 - `output` is the path of the BAX file that will be created.

 - `t` sets the work pipeline length, larger values make the program consume more memory. Default is 4 (approximately 350MiB used in a worst case scenario).

 - `c` sets the LZ4 compression level, larger values make the program take more CPU time. Default is 9.

 - `r` sets the frame rate. It should be automagically detected but it's provided as an option just in case.

 - `b` sets the background color. `bg_color` should be the 16-bit integer representation of an RGB565 color. Defaults to zero (black).

 - `a` sets the Author string. It doesn't affect the player at all, but it's nice to include some metadata. Maximum length is 32 ASCII characters (or as much UTF-8 fits in there).

 - `d` is the same as above but for description. Maximum length is 192 ASCII characters.


### Compiling

Requires `pthreads`, `lz4` and `libvpx` (besides the obvious `make` and `cc`).


#### Notes

Although it should be compatible with big endian platforms (PowerPC, Microblaze, etc) this has not been properly tested yet. Please report any issues regarding compilation/usage on big endian platforms.
