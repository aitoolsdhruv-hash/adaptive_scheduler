CC = gcc
CFLAGS = -Wall -Wextra -pthread -I./include
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Core sources (everything except main.c and unit_tests.c)
CORE_SOURCES = $(SRCDIR)/queue.c $(SRCDIR)/thread_pool.c $(SRCDIR)/executor.c \
               $(SRCDIR)/ai_model.c $(SRCDIR)/decomposer.c $(SRCDIR)/dispatcher.c \
               $(SRCDIR)/utils.c

CORE_OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CORE_SOURCES))

all: directories dashboard stress unit_tests

directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

dashboard: $(BINDIR)/scheduler_dashboard.exe

stress: $(BINDIR)/scheduler_stress.exe

unit_tests: $(BINDIR)/unit_tests.exe

$(BINDIR)/scheduler_dashboard.exe: $(SRCDIR)/main.c $(CORE_OBJECTS)
	$(CC) $(CFLAGS) -DAUTO_DASHBOARD $^ -o $@

$(BINDIR)/scheduler_stress.exe: $(SRCDIR)/main.c $(CORE_OBJECTS)
	$(CC) $(CFLAGS) -DAUTO_STRESS $^ -o $@

$(BINDIR)/unit_tests.exe: $(SRCDIR)/unit_tests.c $(CORE_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

verify: unit_tests
	@echo "Running Internal Logic Verification..."
	@./$(BINDIR)/unit_tests.exe

clean:
	rm -rf $(OBJDIR) $(BINDIR)
