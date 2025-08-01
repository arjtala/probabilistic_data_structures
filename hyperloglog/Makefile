CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c99 -O3 -march=native -flto -ffast-math -mtune=native -I../lib -I../bloom_filter
LDFLAGS = -flto
LOCAL_HEADERS = $(wildcard *.h)
SHARED_HEADERS = $(wildcard ../lib/*.h)
BLOOM_HEADERS = $(wildcard ../bloom_filter/*.h)
HEADERS = $(BLOOM_HEADERS) $(LOCAL_HEADERS) $(SHARED_HEADERS)
SRCS = $(wildcard *.c)
LIBS = $(filter-out tests.c, $(SRCS))
BUILD_DIR = build
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIBS))
DEPS = $(OBJ)

# Library name
LIB = hll.a

# Test executable
TEST_EXEC = $(BUILD_DIR)/tests

# Default target - build the library (only if we have source files)
all:
	@if [ -n "$(LIBS)" ]; then \
		$(MAKE) $(LIB); \
	else \
		echo "No library source files found. Only headers and tests.c detected."; \
		echo "Run 'make test' to build the test executable."; \
	fi

# Build the static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# Build object files (automatically depends on all headers)
$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Test target - build and run tests
test: $(TEST_EXEC)

# Build test executable
$(TEST_EXEC): tests.c $(HEADERS) $(OBJ)
	@mkdir -p $(BUILD_DIR)
	@if [ -n "$(LIBS)" ]; then \
		$(MAKE) $(LIB) && $(CC) $(CFLAGS) $(LDFLAGS) tests.c -L. -lproject -o $@ -lm; \
	else \
		$(CC) $(CFLAGS) $(LDFLAGS) tests.c -o $@ -lm; \
	fi

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(LIB)
	rm -f *.o $(LIB) $(TEST_EXEC)

# Rebuild everything
rebuild: clean all

# Phony targets
.PHONY: all test clean rebuild

# Show detected files (useful for debugging)
show:
	@echo "Headers: $(HEADERS)"
	@echo "Sources: $(SRCS)"
	@echo "Library sources: $(LIBS)"
	@echo "Objects: $(OBJ)"

print-targets:
	@echo "Binary output: $(TEST_EXEC)"
	@echo "Library output: $(LIB)"
