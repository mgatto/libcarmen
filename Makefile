CC       = cc
CFLAGS   = -std=c17 -Wall -Wextra -pedantic -O2
INCLUDES = -Iinclude -Ivendor/stb -Ivendor/utf8 -Ivendor/cjson -Ivendor/toml-c

BUILD_DIR = build
DIST_DIR  = dist
VERSION   = 0.1.0

# --------------------------------------------------------------------------- #
#  Platform detection
# --------------------------------------------------------------------------- #

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  SHARED_EXT   = dylib
  SHARED_FLAGS = -dynamiclib -install_name @rpath/libcarmen.dylib
  RPATH_FLAGS  = -Wl,-rpath,@loader_path
else
  SHARED_EXT   = so
  SHARED_FLAGS = -shared -Wl,-soname,libcarmen.so.0
  RPATH_FLAGS  = -Wl,-rpath,\$$ORIGIN
endif

# --------------------------------------------------------------------------- #
#  Library sources
# --------------------------------------------------------------------------- #

LIB_SRCS = src/utf8.c src/site.c src/connection.c src/city.c src/game_world.c \
           src/seed_data_islamic.c src/villain.c src/artifact.c src/case.c \
           src/session.c src/settings.c src/i18n.c \
           vendor/cjson/cJSON.c
LIB_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(filter src/%.c,$(LIB_SRCS))) \
           $(BUILD_DIR)/cJSON.o

# Library artifacts
STATIC_LIB = $(BUILD_DIR)/libcarmen.a
SHARED_LIB = $(BUILD_DIR)/libcarmen.$(SHARED_EXT)

# Demo binary (link against the static lib)
TRAIL_DEMO     = $(BUILD_DIR)/trail_demo
TRAIL_DEMO_OBJ = $(BUILD_DIR)/trail_demo.o

# --------------------------------------------------------------------------- #
#  Default: build the static library and the demo binary
# --------------------------------------------------------------------------- #

all: $(STATIC_LIB) $(TRAIL_DEMO)

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
#  Demo binary
# --------------------------------------------------------------------------- #

$(TRAIL_DEMO): $(TRAIL_DEMO_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TRAIL_DEMO_OBJ) $(STATIC_LIB)

$(TRAIL_DEMO_OBJ): examples/trail_demo.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/cJSON.o: vendor/cjson/cJSON.c | $(BUILD_DIR)
	$(CC) -std=c17 -O2 -Ivendor/cjson -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --------------------------------------------------------------------------- #
#  Distributable bundle
#
#  Assemble a self-contained, runnable folder that survives `make clean`:
#  a demo linked dynamically against the shipped shared lib (found at runtime
#  via rpath), the locales it reads from the CWD, and an editable settings
#  file. Re-running `make dist` never clobbers an edited settings.toml.
# --------------------------------------------------------------------------- #

DIST_DEMO = $(DIST_DIR)/trail_demo

dist: $(SHARED_LIB) $(TRAIL_DEMO_OBJ)
	mkdir -p $(DIST_DIR)/locales
	$(CC) $(CFLAGS) -o $(DIST_DEMO) $(TRAIL_DEMO_OBJ) \
	    -L$(BUILD_DIR) -lcarmen $(RPATH_FLAGS)
	cp -f $(SHARED_LIB) $(DIST_DIR)/
	cp -f locales/*.json $(DIST_DIR)/locales/
	cp -n examples/settings.default.toml $(DIST_DIR)/settings.toml || true
	@echo ""
	@echo "Bundle ready: run it with"
	@echo "  cd $(DIST_DIR) && ./trail_demo en settings.toml"

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf $(DIST_DIR)

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
                 include/carmen/game_world.h \
                 include/carmen/seed_data_islamic.h \
                 include/carmen/villain.h include/carmen/artifact.h \
                 include/carmen/case.h include/carmen/session.h \
                 include/carmen/settings.h include/carmen/i18n.h

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
            $(TEST_DIR)/test_game_world $(TEST_DIR)/test_carmen_scenarios \
            $(TEST_DIR)/test_artifact $(TEST_DIR)/test_case $(TEST_DIR)/test_session \
            $(TEST_DIR)/test_settings $(TEST_DIR)/test_villain

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

$(TEST_DIR)/test_artifact: test/test_artifact.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_case: test/test_case.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_session: test/test_session.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_settings: test/test_settings.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^

$(TEST_DIR)/test_villain: test/test_villain.c $(LIB_SRCS) $(UNITY_SRC) | $(TEST_DIR)
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
           $(COV_DIR)/test_game_world $(COV_DIR)/test_carmen_scenarios \
           $(COV_DIR)/test_artifact $(COV_DIR)/test_case $(COV_DIR)/test_session \
           $(COV_DIR)/test_settings $(COV_DIR)/test_villain

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

$(COV_DIR)/test_artifact: test/test_artifact.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

$(COV_DIR)/test_case: test/test_case.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

$(COV_DIR)/test_session: test/test_session.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

$(COV_DIR)/test_settings: test/test_settings.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

$(COV_DIR)/test_villain: test/test_villain.c $(LIB_SRCS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS) $(UNITY_SRC)

.PHONY: all lib dist clean distclean test coverage install uninstall
