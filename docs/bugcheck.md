# BUGCHECK!

Something went wrong. That's all you need to know.

For more information, look below...

If you ever encounter one marked with "**report**", make sure to file a bug report.


| Code           | Dumped information       | Description |
| :-------       | :----------------------: | :---------- |
| UNDEFINED      | Registers                | Attempted to execute an invalid instruction - **report** |
| SOFTWARE_INT   | Registers                | Called `svc`. Shouldn't happen within BAX - **report** |
| PREFETCH_ABORT | Registers                | Attempted to execute an instruction from an unmapped/unavailable address - **report** |
| DATA_ABORT     | Registers                | Attempted to read/write data to an unmapped/unavailable address - **report** |
| RESERVED       | Registers                | ????? - **report** |
| OLDARM_PANIC   | None                     | The ARM9 processor has suffered an unrecoverable error - **report** |
| FS_INIT        | Error code               | Failed to initialize FatFS |
| ANIM_TOO_LARGE | BAX size                 | Selected animation is too large - remove it or shrink it somehow |
| ANIM_MALLOC    | BAX size                 | Failed to allocate enough memory for the BAX file - **report** |
| ANIM_NOT_OK    | BAX size and error code  | Sanity check failed - check the error code and **report** |
| FIRM_NOT_FOUND | None                     | Couldn't find a FIRM file to boot |
| FIRM_TOO_LARGE | FIRM size                | The FIRM file is too big - **report** |
| FIRM_MALLOC    | FIRM size                | Couldn't allocate enough memory for the FIRM - **report** |
| FIRM_ERR       | FIRM size and error code | Sanity check failed - check the error code and **report** |
| IRQ_REGISTER   | IRQN, Core               | Tried to enable an invalid IRQ or map an IRQ to an invalid core - **report** |
| IRQ_DEREGISTER | IRQN, Core               | Tried to disable an invalid IRQ or unmap an IRQ from an invalid core- **report** |
