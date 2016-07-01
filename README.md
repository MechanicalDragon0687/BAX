# BootAnim9 by Wolfvak

# Features

- Plays animations on boot
- Includes chainloader
- Animations can be RAW or compressed (QuickLZ + delta encoding, by @d0k3)
- Configurable framerate (1 - 30, both animations must have the same framerate)
- Luma-style reboot patching (no need for a pathchanger)

## How to install

Place the downloaded/compiled payload and create a folder called "anim" on root and place the payload to
chainload as "/anim/arm9payload.bin".

### How to install animations

Create directories called `0` through `9` in the "/anim/" folder. Place whatever animations
you downloaded/created inside said folders, making sure not to leave spaces between them
(for example, you can't place animations in folder `0` and `2` without placing any in folder `1`).

The randomizer will pick one of your animations and play it back on each coldboot (not when rebooting).

For example, a tree layout may be:

SD card root: - arm9loaderhax.bin
              - anim/     
			              - 0/     
						           - anim
			                       - bottom_anim
								   - config.txt

			              - 1/
						           - anim

                          - arm9payload.bin (can be your CFW of choice or a bootloader)

In the case above, there're two animations (0 and 1), animation 0 has both animations (top and bottom)
and a config file, while animation 1 has only a top screen animation and no config.

#### Credits

- chaoskagami and TheBaloneyBoy for code improvements

- Docmudkipz for makeanim

- d0k3 for his [compression](https://github.com/d0k3/ban9comp) implementation

- \#Cakey for useful tips and pointers, as always

- Normmatt for sdmmc.c/h, ChaN for FatFS

- Everyone else who supported me through these two awesome months! (yes, I'm looking at you /r/Nintendohacks!)

- Special shoutout to [Housey](https://github.com/housebreaker) for creating some epic music that helped me focus during some
rough times. You can find his awesome stuff [here](https://soundcloud.com/housebreaker1)

- As well as [A_Random_Guy](https://gbatemp.net/members/a_random_guy.366070/) for asking me to make FB9 and kind of
getting me started on the 3DS scene (apparently he came up with the name too, I don't remember).
