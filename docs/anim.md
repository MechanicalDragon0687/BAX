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
| 0x18           | 0x04   | Column offset from start |
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

  - none so far

