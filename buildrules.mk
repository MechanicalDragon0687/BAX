
.PHONY: all
all: $(TARGET).elf

.PHONY: clean
clean:
	@rm -rf $(BUILD) $(TARGET).elf

$(TARGET).elf: $(SOURCE_OUTPUT) $(COMMON_OUTPUT)
	@mkdir -p "$(@D)"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p "$(@D)"
	@echo "\t[$(PROCESSOR)] $<"
	@$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.common.c.o: $(COMMON)/%.c
	@mkdir -p "$(@D)"
	@echo "\t[$(PROCESSOR)] $<"
	@$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p "$(@D)"
	@echo "\t[$(PROCESSOR)] $<"
	@$(CC) -c $(ASFLAGS) -o $@ $<

$(BUILD)/%.common.s.o: $(COMMON)/%.s
	@mkdir -p "$(@D)"
	@echo "\t[$(PROCESSOR)] $<"
	@$(CC) -c $(ASFLAGS) -o $@ $<
