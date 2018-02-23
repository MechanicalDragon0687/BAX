# BAX
###### Successor to BootAnim9

### What is this?

BAX is a homebrew program for the Nintendo 3DS (compatible with all models) that allows you to display video streams on load.

Thanks to boot-time entrypoints, this can be done on boot, giving a similar impression to the loading animation of a PC or a mobile smartphone.

Please note that while it's designed to boot another program, it does _not_ allow both itself and the other to run concurrently, but rather sequentially. Therefore, any time that is "taken" by loading/playing the animation file will not be somehow "given" to the next program (f.e. independently of the time taken by the animation, the FIRM will take the same amount of time to load).



### How do I use it?

Download the BAX FIRM from the release archive, and set up your bootloader so that it's the first thing that gets loaded (or at least as early as possible).

Copy any BAX animation files to `sdmc:/bax/` ("bax" folder on the SD card). BAX will randomly choose one animation from all animations found in the folder.

Put the FIRM to be loaded afterwards (CFW, secondary bootloader, Linux, etc) on `sdmc:/bax/boot.firm`. If for some reason you want to keep the FIRM elsewhere (easier to maintain/update, etc) you can also create a file `sdmc:/bax/boot.txt` that contains the plaintext ASCII path to the FIRM (f.e. `boot.txt` can contain the text `"sdmc:/firm/cfw.firm"` without the quotes). Compatibility appears to be perfect, although further testing is required.

If you want to skip the animation playback, press `X` during playback. The animation will halt until you let go, giving you time to press any other necessary buttons until the next FIRM is loaded and executed.

***WARNING: BAX can not be booted from a FIRM partition or an NTRboot cart. Don't even try.***



### What does the name even mean?

**B**oot **A**nim X/10 (since it's the successor to BootAnim9, even though the 9 had nothing to do with the version).



### How do I create my own animations?

Read `/makebax/README.md`.



#### Example setups:

B9S:

 - boot9strap installed on FIRM0.
 - BAX in `sdmc:/boot.firm`.
 - Luma3DS in `sdmc:/bax/boot.firm`.
 - Other FIRMs in `sdmc:/luma/payloads`.


FB3DS:

 - fastboot3DS installed on FIRM0.
 - BAX in `sdmc:/firm/bax.firm` (with fastboot3DS default bootslot pointing to this).
 - Rei-Six in `sdmc:/bax/boot.firm`.
 - Other FIRMs in `sdmc:/firm`, selectable through fastboot3DS if desired.


These are only examples, you can mix and match as you wish - the only requirement is to have BAX somewhere in the boot process, preferably as early as possible.



### I get a "BUG!" thing, some text and a weird error code! What do I do?

Read `docs/bug.md`.



### Why would I use this?

I honestly have no idea. Do I really have to tell you what you can and can't do?
Just give it a shot - if you like it, keep it. Don't like it - remove it.



#### Other notes:

Due to how Horizon/NATIVE_FIRM works, if you reboot from AGB_FIRM (GBA Virtual Console) the save game **will not** be saved until you reboot back into it again.

BAX performs no GPU initialization, only register tweaks and assumes the GPU has already been initialized by the time it boots.

Any B9S-compatible loaders _should_ do this, given the proper bit is set in the FIRM (it certainly is in BAX), but I can not realistically provide support for every existing loader out there.
