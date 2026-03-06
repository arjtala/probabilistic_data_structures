CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c99 -O3 -march=native -flto -ffast-math -mtune=native
LDFLAGS = -flto
BUILD_DIR = build

# Headers
HEADERS = $(wildcard lib/*.h) $(wildcard hyperloglog/*.h) $(wildcard bloom_filter/*.h)

# Source files
SRCS = lib/hash.c lib/bitarray.c lib/utilities.c hyperloglog/hll.c bloom_filter/bloom.c

# Object files
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Include paths
INCLUDES = -Ilib -Ihyperloglog -Ibloom_filter

# Combined static library
LIB = $(BUILD_DIR)/libpds.a

# Test executables
TEST_HLL = $(BUILD_DIR)/test_hll
TEST_BLOOM = $(BUILD_DIR)/test_bloom

# Default target
all: $(LIB)

# Static library
$(LIB): $(OBJ)
	ar rcs $@ $^

# Build object files
$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Test targets
test: test-hll test-bloom

test-hll: $(TEST_HLL)
	./$(TEST_HLL) 10 "hello" phrases/phrases.txt

test-bloom: $(TEST_BLOOM)
	./$(TEST_BLOOM)

$(TEST_HLL): hyperloglog/tests.c $(OBJ) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) hyperloglog/tests.c $(OBJ) -o $@ -lm

$(TEST_BLOOM): bloom_filter/tests.c $(OBJ) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) bloom_filter/tests.c $(OBJ) -o $@ -lm

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

.PHONY: all test test-hll test-bloom clean rebuild show

show:
	@echo "Headers: $(HEADERS)"
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJ)"
