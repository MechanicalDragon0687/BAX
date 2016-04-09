# BootAnim9
Custom boot animations for A9LH


# THIS IS OUTDATED. I'LL UPDATE AS SOON AS POSSIBLE, FOR NOW JUST GO TO http://gbatemp.net/threads/release-a9lh-bootanim9-custom-boot-animations.420202/ FOR THE LATEST INSTRUCTIONS


# How to install
Compile with 'make' and copy the resulting arm9loaderhax.bin to your SD card root directory.
Generate your custom animation(s) and copy it/them to your SD card root as sdmc:/anim and sdmc:/bottom_anim

After displaying the animations, it will load and launch sdmc:/arm9payload.bin, so make sure to have an ARM9 payload sitting there!

# How to generate a custom animation
This process ***requires*** **ffmpeg** with the "transpose" video filter and an animated image with size 400x240 or 320x240.

You can use this command to scale a file, and perform needed operations (as an example):
>ffmpeg -an -i "gif filename" -r 30 -pix_fmt bgr24 -vf "scale=400x240,transpose=1" output.rgb

>*Framerate is recommended to be between 5 and 30, and both animations must have* ***the same framerate***

Copy the resulting output.rgb to sdmc:/anim/anim or sdmc:/anim/bottom_anim, edit your configuration file to reflect framerate changes, and *voila*!

Please note that if you want to create a bottom screen animation, you have to use a 320x240 image instead of 400x240.

## How to set the framerate

Inside the anim directory there should be a 'config' file. The sole purpose of this file is to indicate the framerate of the animations, and you have to indicate it in hex.
For example, if your framerate is 15, then the only char in 'config' should be 0x0F (or ASCII char 15).

# How to change the external payload path (won't provide support for modified payloads whatsoever)
Run 'make clean' inside both the main folder and the external_loader folder.
Change whatever you want in the external_loader folder (guess you might want to change the string that says "/arm9payload.bin").
Run 'make' in the external folder, rename 'external_loader.bin' to 'chain.bin' and run 'xxd -i chain.bin > chain.h'.
Replace the 'chain.h' in the main source directory with the one you just obtained.
Run make in the main directory, and copy the resulting 'arm9loaderhax.bin' to your SD card.
Done!

This'll apply to all versions until I change the way the external payload boots.

In the case that you currently don't have the 'xxd' utility (you should tho) you can use 'bin2c', however some changes will be required.

## Credits

b1l1s for his ctr library and fs functions.
dark-samus and AuroraWright for the chainloader.
chaoskagami for better code overall and TheBaloneyBoy for ability to skip animations.

### Yes, the files are expected to be big, because they're RAW dumps.
