CC = gcc
CFLAGS = -Wall -Wextra -pthread -I./include
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Core sources (everything except main.c)
CORE_SOURCES = $(SRCDIR)/queue.c $(SRCDIR)/thread_pool.c $(SRCDIR)/executor.c \
               $(SRCDIR)/ai_model.c $(SRCDIR)/decomposer.c $(SRCDIR)/dispatcher.c \
               $(SRCDIR)/utils.c

CORE_OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CORE_SOURCES))

all: directories dashboard stress

verify: directories $(BINDIR)/unit_tests.exe
	@echo "Running Internal Logic Verification..."
	@./$(BINDIR)/unit_tests.exe

dashboard: $(BINDIR)/scheduler_dashboard.exe
stress: $(BINDIR)/scheduler_stress.exe

$(BINDIR)/unit_tests.exe: $(CORE_OBJECTS)
	$(CC) $(CFLAGS) $(SRCDIR)/unit_tests.c $^ -o $@

$(BINDIR)/scheduler_dashboard.exe: $(CORE_OBJECTS)
	$(CC) $(CFLAGS) -DAUTO_DASHBOARD $(SRCDIR)/main.c $^ -o $@

$(BINDIR)/scheduler_stress.exe: $(CORE_OBJECTS)
	$(CC) $(CFLAGS) -DAUTO_STRESS $(SRCDIR)/main.c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
