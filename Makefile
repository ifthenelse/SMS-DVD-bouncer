DEVKITSMS ?= $(HOME)/devkitSMS
SDCC      ?= sdcc

TARGET = dvd_bouncer
BUILD  = build
SRC    = src
OBJS   = $(BUILD)/main.rel

# Match SMSlib’s build flags (IY reserved + peep rules) and include dirs
CFLAGS = -mz80 --opt-code-speed \
  --reserve-regs-iy \
  --peep-file $(DEVKITSMS)/SMSlib/src/peep-rules.txt \
  -I$(DEVKITSMS)/SMSlib -I$(DEVKITSMS)/SMSlib/src

LFLAGS = -mz80 --no-std-crt0

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

# ihx -> sms
$(BUILD)/$(TARGET).sms: $(BUILD)/$(TARGET).ihx
	$(DEVKITSMS)/ihx2sms/ihx2sms $< $@

clean:
	rm -rf $(BUILD)