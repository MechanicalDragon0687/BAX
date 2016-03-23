# BootAnim9
Custom boot animations for A9LH

# How to install
Compile with 'make' and copy the resulting arm9loaderhax.bin to your SD card root directory.
Generate your custom animation(s) and copy it/them to your SD card root as sdmc:/anim and sdmc:/bottom_anim

After displaying the animations, it will load and launch sdmc:/arm9payload.bin, so make sure to have an ARM9 payload sitting there!

# How to generate a custom animation
This process ***requires*** **ffmpeg** with the "transpose" video filter.

You can use the command 
>ffmpeg -i "nameofanimation.gif" -s 400x240 -r 20 -pix_fmt bgr24 -vf "transpose=1" output.rgb

Copy the resulting output.rgb to sdmc:/anim, and *voila*!

Please note that if you want to create a bottom screen animation, you have to set 320x240 instead of 400x240 and you should copy it to sdmc:/bottom_anim.

## Yes, the files can be expected to be big. I limited both files' size to 64MB so if the sum of your animations is higher than 64MB, it will skip the animations.
