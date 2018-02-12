# makebax

###### BAX Animation creator

### Usage

`makebax "input.ivf" "output.bax" [-s n] [-c l] [-b back_color] [-a "Author"] [-i "Info"]`

Parameters enclosed in `[ ]` are optional.

 - `input.ivf` is the input IVF file path, the size must have a resolution of either 400x240 (top screen only), 320x240 (bottom screen only) or 720x240 (both screens side-by-side). Input must be an IVF container with a VP8/VP9 stream.

 - `output.bax` is the path to the BAX file that will be created.

 - `s` sets the block size. `n` is the number of frames that are processed in one go. The higher the number, the more memory is used on runtime.

 - `c` sets the compression level. Values of `l` higher than 9 invoke the HC which takes considerably longer time than lower compression.

 - `b` sets the background color. `bg_color` should be the 16-bit integer representation of an RGB565 color. Defaults to zero (black).

 - `a` sets the Author string. It doesn't affect the player at all, but it's nice to include some metadata. Maximum length is 32 ASCII characters (or as much UTF-8 fits in there).

 - `i` is the same as above but for Information. Maximum length is 192 ASCII characters.


### Compiling

Requires a C++14 compatible compiler with thread support, `OpenMP`, `lz4` and `libvpx`.


#### Notes

THE FOLLOWING STATEMENT IS NOT TRUE

THE PREVIOUS STATEMENT IS TRUE

Although it should be compatible with big endian platforms (PowerPC, Microblaze, etc) this has not been properly tested yet. Please report any issues regarding compilation/usage on big endian platforms.
