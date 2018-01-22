# BAX Animation Format


## Main Header

| Offset         | Length | Description |
| :------------: | :----: | :---------- |
| 0x00           | 0x03   | Header magic, always ASCII "BAX" |
| 0x03           | 0x01   | Header version |
| 0x04           | 0x04   | Animation special flags (detailed below) |
| 0x08           | 0x04   | Frame count |
| 0x0C           | 0x04   | Frame rate  |
| 0x10           | 0x02   | Background color |
| 0x12           | 0x06   | Reserved |
| 0x18           | 0x04   | Row offset from start |
| 0x1C           | 0x04   | Frame width |
| 0x20           | 0x20   | ASCII author |
| 0x40           | 0xC0   | ASCII description |
| 0x100          | 0x08\*n | Frame information |
| 0x100 + 0x08\*n | ???    | Compressed frames |


## Frame information header

| Offset | Length | Description |
| :----: | :----: | :---------- |
| 0x00   | 0x04   | Frame offset from start of file |
| 0x04   | 0x04   | Compressed frame size |


### Animation special flags:

  - bit0 set -> stereoscopic animation



### Tricks:

Since both screens have the same height (240px), they can be treated as a single, contiguous 720px width screen,  this allows dual-screen animations within a single video stream.

  - If both screens are to be used, the row offset should be set to 0 and the width to 720.

  - If only the top screen is to be used, the row offset should be set to 0 and the width to 400.

  - If only the bottom screen is to be used, then the row offset parameter can be set to 400 and the width to 240.

  - If your animation only takes up a chunk of the screen and has a fixed background, you could tweak the row offset and width parameters to make it smaller.

  - In any other case, feel free to experiment!
