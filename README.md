# BootAnim9
Custom boot animations for A9LH

# How to install
Compile with 'make' and copy the resulting arm9loaderhax.bin to your SD card root directory.
Generate your custom animation(s) and copy it/them to your SD card root as sdmc:/anim and sdmc:/bottom_anim

After displaying the animations, it will load and launch sdmc:/arm9payload.bin, so make sure to have an ARM9 payload sitting there!

# How to generate a custom animation
This process ***requires*** **ffmpeg** with the "transpose" video filter and an animated image with size 400x240 or 320x240.

You can use the command:
>ffmpeg -i "nameofanimation_withcorrectsize.gif" -r *framerate* -pix_fmt bgr24 -vf "transpose=1" output.rgb

>*Framerate is recommended to be between 5 and 15, and both animations must have* ***the same framerate***

>*Best results are obtained with framerate 10*

Copy the resulting output.rgb to sdmc:/anim/anim or sdmc:/anim/bottom_anim, edit your configuration file to reflect framerate changes, and *voila*!

Please note that if you want to create a bottom screen animation, you have to use a 320x240 image instead of 400x240.

# How to set framerate

Inside the anim directory there should be a 'config' file. The sole purpose of this file is to indicate the framerate of **

# Credits

b1l1s for his ctr library and fs functions
AuroraWright for the chainloader

## Yes, the files are expected to be big, because they're RAW dumps. If the sum of your animations is higher than 50MB, it will skip the animations.
