TARGETS = librapid_timer.a
LIBOBJS = $(patsubst %.c,%.o,$(SOURCE)) 
SOURCE = $(wildcard src/*.c)
OBJSDIR = .objs
CFLAG = -Wall 
LFLAG = 

ifeq ($(coverage), yes)
	CFLAG +=  -fprofile-arcs -ftest-coverage
	LFLAG +=  -lgcov
endif

DEMO_SOURCE = $(wildcard example/*.c)
DEMO_OBJS = $(patsubst %.c,%.o,$(DEMO_SOURCE))
DEMO_TARGETS = $(patsubst %.c,%,$(notdir $(DEMO_SOURCE)))

TEST_SOURCE = $(wildcard test/*.c)
TEST_OBJS = $(patsubst %.c,%.o,$(TEST_SOURCE))
TEST_TARGETS = $(patsubst %.c,%,$(notdir $(TEST_SOURCE)))

OFFSET=\033[41G
SUCCESS_COLOR=\033[1;34m
FAILURE_COLOR=\033[1;31m
RESET=\033[0m
CLEAR=\033[H\033[J


all: $(TARGETS) example test

%.o: %.c
	@if (gcc $(CFLAG) -c $< -o $@);then \
		echo -e "$@ $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)";\
	else \
		echo -e "$@ $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
		exit 1;\
	fi

$(TARGETS):$(LIBOBJS)
	@if (ar rcs $@ $^);then \
		echo -e "$@ $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)";\
	else \
		echo -e "$@ $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
		exit 1; \
	fi
	@mkdir $(OBJSDIR) 
	@mv src/*.o $(OBJSDIR)/

example:FORCE
	@$(foreach DEMO_TARGET,$(DEMO_TARGETS),\
		if (gcc $(CFLAG) -c example/$(DEMO_TARGET).c -o example/$(DEMO_TARGET).o);then \
			echo -e "example/$(DEMO_TARGET).o $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)"; \
		else \
			echo -e "example/$(DEMO_TARGET).o $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
			exit 1; \
		fi; \
		if (gcc $(CFLAG) $(LFLAG) example/$(DEMO_TARGET).o librapid_timer.a -o example/$(DEMO_TARGET)); then \
			echo -e "example/$(DEMO_TARGET) $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)"; \
		else \
			echo -e "example/$(DEMO_TARGET) $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
			exit 1; \
		fi; \
	)

test:FORCE
	@$(foreach TEST_TARGET,$(TEST_TARGETS),\
		if (gcc $(CFLAG) -c test/$(TEST_TARGET).c -o test/$(TEST_TARGET).o);then \
			echo -e "test/$(TEST_TARGET).o $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)"; \
		else \
			echo -e "test/$(TEST_TARGET).o $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
			exit 1; \
		fi; \
		if (gcc $(CFLAG) $(LFLAG) test/$(TEST_TARGET).o librapid_timer.a -o test/$(TEST_TARGET)); then \
			echo -e "test/$(TEST_TARGET) $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)"; \
		else \
			echo -e "test/$(TEST_TARGET) $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
			exit 1; \
		fi; \
	)

clean:
	rm -fr $(TARGETS) 
	rm -fr $(OBJSDIR)
	rm -fr $(DEMO_OBJS)
	rm -fr $(DEMO_TARGETS)
	rm -fr $(TEST_OBJS)
	rm -fr $(TEST_TARGETS)

FORCE:
