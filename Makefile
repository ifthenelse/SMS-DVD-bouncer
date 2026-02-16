DEVKITSMS ?= $(HOME)/devkitSMS
SDCC      ?= sdcc
CC        ?= cc
CPPCHECK  ?= cppcheck

TARGET = dvd_bouncer
BUILD  = build
SRC    = src
OBJS   = $(BUILD)/main.rel
IHX2SMS_LOCAL = $(BUILD)/ihx2sms_local
IHX2SMS_SRC   = $(wildcard $(DEVKITSMS)/ihx2sms/src/*.c)
LINT_SOURCES  = $(wildcard $(SRC)/*.c)

# Match SMSlib's build flags (IY reserved + peep rules) and include dirs
CFLAGS = -mz80 --opt-code-speed \
  --no-optsdcc-in-asm \
  --reserve-regs-iy \
  --peep-file $(DEVKITSMS)/SMSlib/src/peep-rules.txt \
  -I$(DEVKITSMS)/SMSlib -I$(DEVKITSMS)/SMSlib/src

# Add proper linker flags for SMS (match official examples)
LFLAGS = -mz80 --no-std-crt0 --data-loc 0xC000

CPPCHECK_FLAGS = \
	--enable=warning,style,performance,portability \
	--std=c99 \
	--error-exitcode=1 \
	--quiet \
	--force \
	--suppress=missingInclude \
	--suppress=missingIncludeSystem \
	-D__sfr= \
	-D'__at(x)='

.PHONY: all clean lint

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

lint:
	@command -v $(CPPCHECK) >/dev/null 2>&1 || { \
		echo "cppcheck is required for linting. Install with: brew install cppcheck"; \
		exit 1; \
	}
	$(CPPCHECK) $(CPPCHECK_FLAGS) $(LINT_SOURCES)

clean:
	rm -rf $(BUILD)
