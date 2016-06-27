# BootAnim9 - v0.6

## Features

Play animations on boot:

- Animations can be RAW or compressed (LZ + delta encoding, by @d0k3)
- Configurable framerate (1 - 30, both animations must have the same framerate)
- Luma3DS payload patching (doesn't need pathchanger, it's done automatically)

## How to install

Place the downloaded/compiled payload in root and place the payload to chainload as "/anim/arm9payload.bin".
A special exception is made with [Luma3DS](https://github.com/AuroraWright/Luma3DS), the payload *must* be saved as "/anim/luma.bin".

It also supports loading BootCTR9 - just drop "arm9bootloader.bin" into the SD card root as well as its configuration file and you're good to go.
Please note that BootCTR9 will only load if neither "arm9payload.bin" nor "luma.bin" exist.

## How to create an animation

*Requires ffmpeg to be in PATH and ImageMagick to be installed.*

Use @Docmudkipz' script, [makeanim](https://github.com/Docmudkipz/makeanim) (requires PowerShell on Windows or dash-compatible interpreter).
Follow the instructions in the script, and make sure to have a file source readable by ffmpeg (GIF/H.264/VP8/etc).

## How to install animations

Create directories called '0' through '9' in the "/anim/" folder. Place whatever animations you downloaded/created inside said folders, making sure not to leave spaces between them (for example, you can't place animations in folder '0' and '2' without placing any in folder '1').
The randomizer will pick one of your animations and play it back on each coldboot (not when rebooting).

For example, a tree layout may be:

SD card root: - arm9loaderhax.bin
              - anim/     
			              - 0/     
						           - anim
			                       - bottom_anim
								   - config

			              - 1/
						           - anim
								   - config

                          - arm9payload.bin (taken from some other place)

In the case above, there're two animations (0 and 1), animation 0 has both animations (top and bottom) and a config file, while animation 1 has only a top screen animation.

###### Personal code is licensed under the GPL v2.0 license, everything under "/source/fatfs" is licensed either under FatFS' license (MIT) or GPLv2

## Credits

chaoskagami for better code overall and TheBaloneyBoy for the ability to skip animations

Docmudkipz for makeanim

d0k3 for his [compression](https://github.com/d0k3/ban9comp) implementation

\#Cakey for useful tips and pointers, as always

b1l1s for his ctr library full of useful fb snippets and Normmatt for sdmmc

Everyone else who supported me through these two awesome months! (yes, I'm looking at you /r/Nintendohax)

Special shoutout to [Housey](https://github.com/housebreaker) for creating some epic music that helped me focus during some rough times. You can find his awesome stuff [here](https://soundcloud.com/housebreaker1)

As well as [A_Random_Guy](https://gbatemp.net/members/a_random_guy.366070/) for asking me to make FB9 and getting me into this beautiful mess :P
