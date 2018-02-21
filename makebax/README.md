# makebax

###### BAX Animation creator

### Usage

`makebax "input.ivf" "output.bax" [-s n] [-c l] [-b back_color] [-a "Author"] [-i "Info"]`

Parameters enclosed in `[ ]` are optional.

 - `input.ivf` is the input IVF file path, the size must have a resolution of either 400x240 (top screen only), 320x240 (bottom screen only) or 720x240 (both screens side-by-side). Input must be an IVF container with a VP8/VP9 stream.

 - `output.bax` is the path to the BAX file that will be created.

 - `s` sets the block size. `n` is the number of frames that are processed per iteration. The higher the number, the more memory is used on runtime (and the less often disk accesses are required). Defaults to 48.

 - `c` sets the compression level. Should be between 1 (lowest) and 16 (highest). Defaults to 6.

 - `b` sets the background color. `bg_color` should be the 16-bit integer representation of an RGB565 color. Defaults to zero (black). Refer to [this site](http://www.barth-dev.de/online/rgb565-color-picker/) for more information.

 - `a` sets the Author string. It doesn't affect the player at all, but it's nice to include some metadata. Maximum length is 32 ASCII characters (or as much UTF-8 fits in there).

 - `i` is the same as above but for Information. Maximum length is 192 ASCII characters.


### What on earth is an IVF file?

[IVF](https://wiki.multimedia.cx/index.php/IVF) is a simple container for VP8/VP9 streams, unable to contain more than one stream. It was chosen as the input format due to it being both simplicity and straightforward to work with.

You can transcode any video supported by `ffmpeg` into IVF by running `ffmpeg -i "input.video" -f ivf "output.ivf"`.

If your source video resolution isn't the required you can also do `ffmpeg -i "input.video" -f ivf -vf scale=<WIDTH>:240,fps=<FRAMERATE> "output.ivf"`, with "WIDTH" being either 400, 320 or 720 and "FRAMERATE" being between 1 and 60.

While having libav* support would be ideal, it's less than fit due to it's programming complexity (easy to make mistakes with) and labyrinth of dependencies (requires X11 libs, so it's unusable from servers, f.e.).


### Compiling

Requires a C++14 compatible compiler with thread support, `OpenMP`, `lz4` and `libvpx`.


#### Notes

Although it should be compatible with big endian platforms (PowerPC, Microblaze, etc) this has not been properly tested yet. Please report any issues regarding compilation/usage on big endian platforms.

For best performance, the block size should be an integer multiple of the number of available hardware threads (logical processors). This is why the default block size is 48: it's a multiple of the most common PO2 and MO6 multithread setups.
