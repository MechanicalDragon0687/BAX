# BUG!

Something went wrong. _Really wrong_.

Errors marked in **bold** are unexpected bugs that must be reported if found, including information like current system setup, BAX version, error information, file that caused the error (if any), etc.

Those not marked in bold are bugs arising from either user error or invalid BAX files. They should provide enough information to pinpoint the source of the problem and be able to deal with it individually.


| Code                | Dumped information             | Description |
| :-------            | :----------------------------: | :---------- |
| **UNDEFINED**       | Registers                      | Attempted to execute an invalid instruction |
| **SOFT INTERRUPT**  | Registers                      | Called `svc`. Shouldn't happen within BAX |
| **PREFETCH ABORT**  | Registers                      | Attempted to execute an instruction from an invalid address |
| **DATA ABORT**      | Registers                      | Attempted to access data on an invalid address |

| **OLDARM_PANIC**    | Registers                      | The ARM9 processor suffered an unrecoverable error |

| **PXICMD_HANDLER**  | Command ID and argcount        | Received an unknown PXI command |

| **FS_INIT**         | FF error code                  | Failed to initialize FatFS |
| FS_FILEOPEN         | Path and FF error code         | Failed to open a file |
| FS_FILEREAD         | Path and data lengths          | Failed to read a file |
| FS_FILESETPOS       | Path, size and position        | Failed to seek a file |
| FS_DIROPEN          | Path and FF error code         | Failed to open a directory |

| **ANIM_ALLOC**      | Size and attempt count         | Failed to allocate space in the heap |
| ANIM_VALIDATE       | Error code and size            | Attempted to load an invalid animation |
| **ANIM_DECOMPRESS** | Path, index, result and sizes  | Failed to decompress an animation frame |

| FIRM_TOO_LARGE      | FIRM size                      | The FIRM file is too large |
| **FIRM_ALLOC**      | FIRM size                      | Couldn't allocate enough memory for the FIRM |
| **FIRM_VALIDATE**   | FIRM size and error code       | Tried to boot an invalid FIRM |
