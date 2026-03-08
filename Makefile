CC       = cc
CFLAGS   = -std=c17 -Wall -Wextra -pedantic -O2
INCLUDES = -Iinclude -Ivendor/stb -Ivendor/utf8

BUILD_DIR = build
VERSION   = 0.1.0

# --------------------------------------------------------------------------- #
#  Platform detection
# --------------------------------------------------------------------------- #

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  SHARED_EXT   = dylib
  SHARED_FLAGS = -dynamiclib -install_name @rpath/libcarmen.dylib
else
  SHARED_EXT   = so
  SHARED_FLAGS = -shared -Wl,-soname,libcarmen.so.0
endif

# --------------------------------------------------------------------------- #
#  Library sources
# --------------------------------------------------------------------------- #

LIB_SRCS = src/utf8.c src/site.c src/connection.c src/city.c src/game_world.c \
           src/seed_data.c src/seed_data_islamic.c src/villain.c
LIB_OBJS = $(LIB_SRCS:src/%.c=$(BUILD_DIR)/%.o)

# Library artifacts
STATIC_LIB = $(BUILD_DIR)/libcarmen.a
SHARED_LIB = $(BUILD_DIR)/libcarmen.$(SHARED_EXT)

# Demo binaries (link against the static lib)
TARGET     = $(BUILD_DIR)/carmen
TARGET_OBJ = $(BUILD_DIR)/carmen.o

TRAIL_DEMO     = $(BUILD_DIR)/trail_demo
TRAIL_DEMO_OBJ = $(BUILD_DIR)/trail_demo.o

# --------------------------------------------------------------------------- #
#  Default: build the static library and the demo binary
# --------------------------------------------------------------------------- #

all: $(STATIC_LIB) $(TARGET) $(TRAIL_DEMO)

# --------------------------------------------------------------------------- #
#  Library targets
# --------------------------------------------------------------------------- #

$(STATIC_LIB): $(LIB_OBJS) | $(BUILD_DIR)
	ar rcs $@ $^

$(SHARED_LIB): $(LIB_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SHARED_FLAGS) -fPIC -fvisibility=hidden \
	    -o $@ $(LIB_SRCS)

lib: $(STATIC_LIB) $(SHARED_LIB)

# --------------------------------------------------------------------------- #
#  Demo binaries
# --------------------------------------------------------------------------- #

$(TARGET): $(TARGET_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TARGET_OBJ) $(STATIC_LIB)

$(TARGET_OBJ): examples/carmen.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(TRAIL_DEMO): $(TRAIL_DEMO_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TRAIL_DEMO_OBJ) $(STATIC_LIB)

$(TRAIL_DEMO_OBJ): examples/trail_demo.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

# --------------------------------------------------------------------------- #
#  pkg-config
# --------------------------------------------------------------------------- #

PREFIX ?= /usr/local

$(BUILD_DIR)/carmen.pc: carmen.pc.in | $(BUILD_DIR)
	sed 's|@PREFIX@|$(PREFIX)|g; s|@VERSION@|$(VERSION)|g' $< > $@

# --------------------------------------------------------------------------- #
#  Install / Uninstall
# --------------------------------------------------------------------------- #

PUBLIC_HEADERS = include/carmen/carmen.h include/carmen/carmen_export.h \
                 include/carmen/carmen_version.h include/carmen/utf8.h \
                 include/carmen/clue.h include/carmen/site.h \
                 include/carmen/connection.h include/carmen/city.h \
                 include/carmen/game_world.h include/carmen/seed_data.h \
                 include/carmen/seed_data_islamic.h \
                 include/carmen/villain.h

INCLUDEDIR   = $(PREFIX)/include/carmen
LIBDIR       = $(PREFIX)/lib
PKGCONFIGDIR = $(LIBDIR)/pkgconfig

install: $(STATIC_LIB) $(SHARED_LIB) $(BUILD_DIR)/carmen.pc
	install -d $(INCLUDEDIR)
	install -m 644 $(PUBLIC_HEADERS) $(INCLUDEDIR)
	install -d $(LIBDIR)
	install -m 644 $(STATIC_LIB) $(LIBDIR)
	install -m 755 $(SHARED_LIB) $(LIBDIR)
	install -d $(PKGCONFIGDIR)
	install -m 644 $(BUILD_DIR)/carmen.pc $(PKGCONFIGDIR)

uninstall:
	rm -rf $(INCLUDEDIR)
	rm -f  $(LIBDIR)/libcarmen.a
	rm -f  $(LIBDIR)/libcarmen.dylib $(LIBDIR)/libcarmen.so
	rm -f  $(PKGCONFIGDIR)/carmen.pc

# --------------------------------------------------------------------------- #
#  Unit Tests (Unity)
# --------------------------------------------------------------------------- #

UNITY_SRC  = vendor/unity/unity.c
UNITY_INC  = -Ivendor/unity
TEST_FLAGS = -std=c17 -Wall -Wextra -pedantic -O0 -g -DUNITY_INCLUDE_DOUBLE

TEST_DIR  = $(BUILD_DIR)/test
TEST_BINS = $(TEST_DIR)/test_site $(TEST_DIR)/test_connection $(TEST_DIR)/test_city \
            $(TEST_DIR)/test_game_world $(TEST_DIR)/test_carmen_scenarios

test: $(TEST_BINS)
	@echo "========================================"
	@echo "  Running all test suites"
	@echo "========================================"
	@fail=0; \
	for t in $(TEST_BINS); do \
		echo ""; \
		echo "--- $$t ---"; \
		./$$t || fail=1; \
	done; \
	echo ""; \
	if [ $$fail -eq 0 ]; then \
		echo "========================================"; \
		echo "  ALL SUITES PASSED"; \
		echo "========================================"; \
	else \
		echo "========================================"; \
		echo "  SOME TESTS FAILED"; \
		echo "========================================"; \
		exit 1; \
	fi

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

$(TEST_DIR)/test_site: test/test_site.c src/site.c src/utf8.c $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_connection: test/test_connection.c src/connection.c src/utf8.c $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_city: test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_game_world: test/test_game_world.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_carmen_scenarios: test/test_carmen_scenarios.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

# --------------------------------------------------------------------------- #
#  Code Coverage  (requires lcov:  brew install lcov)
# --------------------------------------------------------------------------- #

COV_FLAGS = -std=c17 -Wall -Wextra -pedantic -O0 -g --coverage -DUNITY_INCLUDE_DOUBLE
COV_DIR   = $(BUILD_DIR)/coverage
LCOV_GCOV = --gcov-tool /usr/bin/gcov

LLVM_COV_GCOV := $(shell xcrun -f llvm-cov 2>/dev/null)
ifneq ($(LLVM_COV_GCOV),)
  LCOV_GCOV = --gcov-tool $(COV_DIR)/llvm-gcov.sh
endif

COV_BINS = $(COV_DIR)/test_site $(COV_DIR)/test_connection $(COV_DIR)/test_city \
           $(COV_DIR)/test_game_world $(COV_DIR)/test_carmen_scenarios

coverage: $(COV_BINS)
	@echo "========================================"
	@echo "  Running tests with coverage"
	@echo "========================================"
	@for t in $(COV_BINS); do echo "--- $$t ---"; ./$$t || true; done
	@echo ""
	@echo "Collecting coverage data..."
	lcov --capture --directory $(COV_DIR) \
	     $(LCOV_GCOV) \
	     --output-file $(COV_DIR)/coverage.info \
	     --ignore-errors inconsistent 2>/dev/null || \
	lcov --capture --directory $(COV_DIR) \
	     $(LCOV_GCOV) \
	     --output-file $(COV_DIR)/coverage.info
	lcov --remove $(COV_DIR)/coverage.info \
	     '*/vendor/*' '*/test/*' '/usr/*' '/Library/*' '/opt/*' \
	     --output-file $(COV_DIR)/coverage_filtered.info \
	     --ignore-errors unused 2>/dev/null || \
	lcov --remove $(COV_DIR)/coverage.info \
	     '*/vendor/*' '*/test/*' '/usr/*' '/Library/*' '/opt/*' \
	     --output-file $(COV_DIR)/coverage_filtered.info
	genhtml $(COV_DIR)/coverage_filtered.info \
	        --output-directory $(COV_DIR)/html
	@echo ""
	@echo "========================================"
	@echo "  Coverage report: $(COV_DIR)/html/index.html"
	@echo "========================================"
	@lcov --summary $(COV_DIR)/coverage_filtered.info 2>&1 | tail -4

$(COV_DIR)/llvm-gcov.sh: | $(COV_DIR)
	@printf '#!/bin/sh\nexec xcrun llvm-cov gcov "$$@"\n' > $@
	@chmod +x $@

$(COV_DIR):
	mkdir -p $(COV_DIR)

$(COV_DIR)/test_site: test/test_site.c src/site.c src/utf8.c $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/site.c src/utf8.c $(UNITY_SRC)

$(COV_DIR)/test_connection: test/test_connection.c src/connection.c src/utf8.c $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/connection.c src/utf8.c $(UNITY_SRC)

$(COV_DIR)/test_city: test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/city.c src/site.c src/connection.c src/utf8.c $(UNITY_SRC)

$(COV_DIR)/test_game_world: test/test_game_world.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

$(COV_DIR)/test_carmen_scenarios: test/test_carmen_scenarios.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

.PHONY: all lib clean test coverage install uninstall
