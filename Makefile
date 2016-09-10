TARGETS = librapid_timer.a
LIBOBJS = $(patsubst %.c,%.o,$(SOURCE)) 
SOURCE = $(wildcard src/*.c)
OBJSDIR = .objs
CFLAG = -Wall 

OFFSET=\033[41G
SUCCESS_COLOR=\033[1;34m
FAILURE_COLOR=\033[1;31m
RESET=\033[0m
CLEAR=\033[H\033[J


all: $(TARGETS) 

%.o: %.c
	@if (gcc $(CFLAG) -c $< -o $@);then \
		echo "$@ $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)";\
	else \
		echo "$@ $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
		exit 1;\
	fi

$(TARGETS):$(LIBOBJS)
	@if (ar rcs $@ $^);then \
		echo "$@ $(OFFSET)$(SUCCESS_COLOR)[ ok ]$(RESET)";\
	else \
		echo "$@ $(OFFSET)$(FAILURE_COLOR)[ failed ]$(RESET)";\
		exit 1; \
	fi
	@mkdir $(OBJSDIR) 
	@mv src/*.o $(OBJSDIR)/



clean:
	rm -fr $(TARGETS) 
	rm -fr $(OBJSDIR)

