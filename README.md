# BootAnim9
Custom boot animations for A9LH

## How to install

Place the downloaded/compiled payload in root and place the payload to chainload as "/anim/arm9payload.bin".
A special exception is made with [Luma3DS](https://github.com/AuroraWright/Luma3DS), its payload *must* be saved as "/anim/luma.bin".

## How to create an animation

*Requires ffmpeg to be in PATH and ImageMagick to be installed.*

Use @Docmudkipz' script, [makeanim](https://github.com/Docmudkipz/makeanim).
Follow the instructions in the script, and make sure to have a file source readable by ffmpeg (GIF/H.264/VP8/etc).

## How to install animations

Create directories called '0' through '9' in the "/anim/" folder. Place whatever animations you downloaded/created inside said folders, making sure not to leave spaces between them (for example, you can't place animations in folder '0' and '2' without placing any in folder '1').
The randomizer will pick one of your animations and play it back on each coldboot (not when rebooting).

###### Personal code is licensed under the GPL v2.0 license (NOT 3.0 OR ANY THAT COME AFTER IT), everything under "/source/fatfs" is licensed either under FatFS' license (MIT) or GPLv2

## Credits

chaoskagami for better code overall and TheBaloneyBoy for the ability to skip animations
Docmudkipz for makeanim
d0k3 for his [compression](https://github.com/d0k3/ban9comp) implementation
b1l1s for his ctr library full of useful snippets and Normmatt for sdmmc functions
\#Cakey for useful tips and pointers, as always
