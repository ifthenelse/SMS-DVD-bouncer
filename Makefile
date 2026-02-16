DEVKITSMS ?= $(HOME)/devkitSMS
SDCC      ?= sdcc
CC        ?= cc

TARGET = dvd_bouncer
BUILD  = build
SRC    = src
OBJS   = $(BUILD)/main.rel
IHX2SMS_LOCAL = $(BUILD)/ihx2sms_local
IHX2SMS_SRC   = $(wildcard $(DEVKITSMS)/ihx2sms/src/*.c)

# Match SMSlib's build flags (IY reserved + peep rules) and include dirs
CFLAGS = -mz80 --opt-code-speed \
  --no-optsdcc-in-asm \
  --reserve-regs-iy \
  --peep-file $(DEVKITSMS)/SMSlib/src/peep-rules.txt \
  -I$(DEVKITSMS)/SMSlib -I$(DEVKITSMS)/SMSlib/src

# Add proper linker flags for SMS (match official examples)
LFLAGS = -mz80 --no-std-crt0 --data-loc 0xC000

.PHONY: all clean

all: $(BUILD)/$(TARGET).sms

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/main.rel: $(SRC)/main.c | $(BUILD)
	$(SDCC) $(CFLAGS) -c $< -o $@

# IMPORTANT: link with devkitSMS CRT0 and SMSlib
$(BUILD)/$(TARGET).ihx: $(OBJS)
	$(SDCC) $(LFLAGS) \
	  $(DEVKITSMS)/crt0/crt0_sms.rel \
	  $^ \
	  $(DEVKITSMS)/SMSlib/SMSlib.lib \
	  -o $@

# Build ihx2sms locally so conversion works on macOS even when devkitSMS ships only Linux binaries.
$(IHX2SMS_LOCAL): $(IHX2SMS_SRC) | $(BUILD)
	$(CC) -O2 -std=c99 $(IHX2SMS_SRC) -o $@

# ihx -> sms
$(BUILD)/$(TARGET).sms: $(BUILD)/$(TARGET).ihx $(IHX2SMS_LOCAL)
	$(IHX2SMS_LOCAL) $< $@

clean:
	rm -rf $(BUILD)
