###### THIS REPO SHOULD BE CONSIDERED BROKEN AND OBSOLETE. I'M CURRENTLY WORKING ON A NEW VERSION OF BA9 BUT UNTIL THEN DON'T USE SELF-COMPILED BINARIES BECAUSE THEY HAVE BEEN BROKEN. USE THE LATEST RELEASE (KEEP IN MIND THE NEW NAMING DOESNT APPLY TO THIS VERSION)

###### (Check https://github.com/Wolfvak/BootAnim for more details)

# BootAnim9 by Wolfvak

# Features

- Plays animations on boot
- Includes chainloader
- Animations can be RAW or compressed (QuickLZ + delta encoding, by @d0k3)
- Configurable framerate (1 - 30, both animations must have the same framerate)
- Luma-style reboot patching (no need for a pathchanger)

## How to install

Create a folder on your SD card called `anim`, move your current `arm9loaderhax.bin` to `anim` and rename it to `arm9payload.bin`.
Extract the BootAnim9 zip to the SD and install any animations you want (16 is the maximum amount, more are possible though).

### How to install animations

Create directories called whatever you want and place either "anim" or "bottom_anim" in there (or both). `config.txt` should
be in the same folder, of course.

The randomizer will pick one of your animations and play it back on each coldboot (so it won't play again when rebooting).

For example, a tree layout may be:

SD card root:

			- arm9loaderhax.bin (this is BA9 itself, the .bin file from the zip)

			- anim/

				- calibrator (this file is used for timing purposes)

				- mh4/

					- anim

					- bottom_anim

					- config.txt

				- doom/

					- anim

				- arm9payload.bin (your CFW/bootloader/payload)


In the case above, there're two animations "mh4" has both animations (top and bottom)
and a config file, while "doom" has only a top screen animation and no config.

#### Credits

- chaoskagami and TheBaloneyBoy for code contributions

- Docmudkipz for [makeanim](https://github.com/docmudkipz/makeanim)

- d0k3 for his [compression](https://github.com/d0k3/ban9comp) implementation

- Normmatt for sdmmc.c/h, ChaN for FatFS

- gemarcano for start.s

- bilis for his amazing [qemu](https://github.com/b1l1s/qemu) fork

- \#Cakey for useful tips and pointers, as always

- Everyone else who supported me through these four awesome months! (yes, I'm looking at you /r/Nintendohacks!)

- Special shoutout to [Housey](https://github.com/housebreaker) for creating some epic music that helped me focus during some
rough times. You can find his awesome stuff [here](https://soundcloud.com/housebreaker1)

- As well as [A\_Random\_Guy](https://gbatemp.net/members/a_random_guy.366070/) for asking me to make [FB9](http://github.com/Wolfvak/FakeBrick9)
and kind of getting me started on the 3DS scene (apparently he came up with the name too, I don't remember).
