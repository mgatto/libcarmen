CC       = cc
CFLAGS   = -std=c17 -Wall -Wextra -pedantic -O2
LDLIBS   = -lm
INCLUDES = -I$(GEN_DIR) -Iinclude -Isrc -Ivendor/stb -Ivendor/utf8 -Ivendor/cjson \
           -Ivendor/toml-c -Ivendor/fribidi -Ivendor/fribidi/lib

FRIBIDI_CFLAGS = -DHAVE_CONFIG_H

# Auto-generate header dependency files (.d) alongside each object so that
# editing a header rebuilds exactly the objects that include it.  -MP adds
# phony targets for each header so deleting one never breaks the build.
DEPFLAGS = -MMD -MP

BUILD_DIR = build
DIST_DIR  = dist

# The top-level VERSION file is the single source of truth for the version
# (see doc/versioning.md). Make and CMake both read it, and both regenerate
# the public carmen_version.h header from it via tools/gen_version.sh.
VERSION  := $(shell cat VERSION)
SOMAJOR  := $(firstword $(subst ., ,$(VERSION)))

# Distributable demo tarball naming: libcarmen-demo-<version>-macos-<arch>.tar.gz
ARCH        := $(shell uname -m)
PKG_NAME     = libcarmen-demo-$(VERSION)-macos-$(ARCH)
PKG_TARBALL  = $(PKG_NAME).tar.gz

# --------------------------------------------------------------------------- #
#  Platform detection
# --------------------------------------------------------------------------- #

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  SHARED_EXT      = dylib
  SHARED_REAL     = libcarmen.$(VERSION).dylib
  SHARED_SONAME   = libcarmen.$(SOMAJOR).dylib
  SHARED_LINKNAME = libcarmen.dylib
  SHARED_FLAGS    = -dynamiclib -install_name @rpath/$(SHARED_SONAME) \
                    -compatibility_version $(SOMAJOR) -current_version $(VERSION)
  RPATH_FLAGS     = -Wl,-rpath,@loader_path
else
  SHARED_EXT      = so
  SHARED_REAL     = libcarmen.so.$(VERSION)
  SHARED_SONAME   = libcarmen.so.$(SOMAJOR)
  SHARED_LINKNAME = libcarmen.so
  SHARED_FLAGS    = -shared -Wl,-soname,$(SHARED_SONAME)
  RPATH_FLAGS     = -Wl,-rpath,\$$ORIGIN
endif

# --------------------------------------------------------------------------- #
#  Vendored GNU FriBidi (LGPL-2.1+); see vendor/fribidi/README.vendor
# --------------------------------------------------------------------------- #

FRIBIDI_SRCS = vendor/fribidi/lib/fribidi.c \
               vendor/fribidi/lib/fribidi-arabic.c \
               vendor/fribidi/lib/fribidi-bidi.c \
               vendor/fribidi/lib/fribidi-bidi-types.c \
               vendor/fribidi/lib/fribidi-char-sets.c \
               vendor/fribidi/lib/fribidi-char-sets-cap-rtl.c \
               vendor/fribidi/lib/fribidi-char-sets-cp1255.c \
               vendor/fribidi/lib/fribidi-char-sets-cp1256.c \
               vendor/fribidi/lib/fribidi-char-sets-iso8859-6.c \
               vendor/fribidi/lib/fribidi-char-sets-iso8859-8.c \
               vendor/fribidi/lib/fribidi-char-sets-utf8.c \
               vendor/fribidi/lib/fribidi-deprecated.c \
               vendor/fribidi/lib/fribidi-joining.c \
               vendor/fribidi/lib/fribidi-joining-types.c \
               vendor/fribidi/lib/fribidi-mirroring.c \
               vendor/fribidi/lib/fribidi-brackets.c \
               vendor/fribidi/lib/fribidi-run.c \
               vendor/fribidi/lib/fribidi-shape.c

FRIBIDI_OBJS = $(patsubst vendor/fribidi/lib/%.c,$(BUILD_DIR)/fribidi_%.o,$(FRIBIDI_SRCS))

# --------------------------------------------------------------------------- #
#  Library sources
# --------------------------------------------------------------------------- #

LIB_SRCS = src/utf8.c src/site.c src/connection.c src/city.c src/game_world.c \
           src/connection_gen.c \
           src/villain.c src/artifact.c src/case.c \
           src/session.c src/settings.c src/save.c src/i18n.c \
           vendor/cjson/cJSON.c $(FRIBIDI_SRCS)

# Built-in world: generated at build time from the preset by tools/gen_world.
# LIB_SRCS_ALL is the full library source set (adds the generated world) used
# for the shared lib and for test/coverage binaries that link everything.
PRESET        = presets/islamic.jsonc
GEN_DIR       = $(BUILD_DIR)/generated
GEN_VERSION_H = $(GEN_DIR)/carmen_version.h
GEN_WORLD_SRC = $(GEN_DIR)/world_islamic_generated.c
GEN_WORLD     = $(BUILD_DIR)/gen_world
LIB_SRCS_ALL  = $(LIB_SRCS) $(GEN_WORLD_SRC)

LIB_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(filter src/%.c,$(LIB_SRCS))) \
           $(BUILD_DIR)/cJSON.o \
           $(FRIBIDI_OBJS) \
           $(GEN_DIR)/world_islamic_generated.o

# Library artifacts
STATIC_LIB = $(BUILD_DIR)/libcarmen.a
SHARED_LIB = $(BUILD_DIR)/$(SHARED_REAL)

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

$(SHARED_LIB): $(LIB_SRCS_ALL) | $(BUILD_DIR) $(GEN_VERSION_H)
	$(CC) $(CFLAGS) $(FRIBIDI_CFLAGS) $(INCLUDES) $(SHARED_FLAGS) -fPIC -fvisibility=hidden \
	    -o $@ $(LIB_SRCS_ALL) $(LDLIBS)
	ln -sf $(SHARED_REAL) $(BUILD_DIR)/$(SHARED_SONAME)
	ln -sf $(SHARED_REAL) $(BUILD_DIR)/$(SHARED_LINKNAME)

lib: $(STATIC_LIB) $(SHARED_LIB)

# --------------------------------------------------------------------------- #
#  Demo binary
# --------------------------------------------------------------------------- #

$(TRAIL_DEMO): $(TRAIL_DEMO_OBJ) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TRAIL_DEMO_OBJ) $(STATIC_LIB) $(LDLIBS)

$(TRAIL_DEMO_OBJ): examples/trail_demo.c | $(BUILD_DIR) $(GEN_VERSION_H)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR) $(GEN_VERSION_H)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/cJSON.o: vendor/cjson/cJSON.c | $(BUILD_DIR)
	$(CC) -std=c17 -O2 $(DEPFLAGS) -Ivendor/cjson -c -o $@ $<

$(BUILD_DIR)/fribidi_%.o: vendor/fribidi/lib/%.c | $(BUILD_DIR)
	$(CC) -std=c17 -Wall -Wextra -O2 $(DEPFLAGS) $(FRIBIDI_CFLAGS) \
	    -Ivendor/fribidi -Ivendor/fribidi/lib -c -o $@ $<

# --------------------------------------------------------------------------- #
#  Built-in world code generation
#
#  presets/islamic.jsonc is the single source of truth for the built-in world.
#  gen_world validates it and emits the C that defines
#  carmen_world_build_islamic(); editing the preset regenerates the source, so
#  a bad preset fails the build.
# --------------------------------------------------------------------------- #

$(GEN_WORLD): tools/gen_world.c vendor/cjson/cJSON.c | $(BUILD_DIR)
	$(CC) -std=c17 -Wall -Wextra -pedantic -O2 -Iinclude -Ivendor/cjson \
	    -o $@ tools/gen_world.c vendor/cjson/cJSON.c

$(GEN_WORLD_SRC): $(PRESET) $(GEN_WORLD) | $(GEN_DIR)
	$(GEN_WORLD) $(PRESET) $@

$(GEN_DIR)/world_islamic_generated.o: $(GEN_WORLD_SRC) | $(GEN_DIR) $(GEN_VERSION_H)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c -o $@ $<

# Generated public header: carmen_version.h is derived from the VERSION file by
# tools/gen_version.sh and never committed (see doc/versioning.md). It lives in
# $(GEN_DIR), which -I comes first, so carmen.h's quoted #include resolves here.
$(GEN_VERSION_H): VERSION tools/gen_version.sh | $(GEN_DIR)
	sh tools/gen_version.sh VERSION $@

$(GEN_DIR):
	mkdir -p $(GEN_DIR)

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
	    -L$(BUILD_DIR) -lcarmen $(RPATH_FLAGS) $(LDLIBS)
	cp -f $(SHARED_LIB) $(DIST_DIR)/
	ln -sf $(SHARED_REAL) $(DIST_DIR)/$(SHARED_SONAME)
	cp -f locales/*.json $(DIST_DIR)/locales/
	cp -n examples/settings.default.toml $(DIST_DIR)/settings.toml || true
	@echo ""
	@echo "Bundle ready: run it with"
	@echo "  cd $(DIST_DIR) && ./trail_demo en settings.toml"

# --------------------------------------------------------------------------- #
#  macOS demo tarball
#
#  Roll the self-contained dist/ bundle into a single versioned, arch-tagged
#  .tar.gz for macOS. We stage a renamed copy under build/ (rather than using
#  tar --transform / bsdtar -s) so the archive expands into a self-named
#  top-level folder without depending on tar-flavor-specific flags.
#
#  dist/ is rebuilt from scratch first (unlike a plain `make dist`, which
#  preserves an edited settings.toml) so a stale/edited bundle never leaks
#  into the shipped tarball. The recursive `$(MAKE) dist` guarantees the wipe
#  happens before dist/ is reassembled -- a prerequisite would build too early.
# --------------------------------------------------------------------------- #

package:
	rm -rf $(DIST_DIR)
	$(MAKE) dist
	rm -rf $(BUILD_DIR)/$(PKG_NAME)
	cp -R $(DIST_DIR) $(BUILD_DIR)/$(PKG_NAME)
	cp -f LICENSE $(BUILD_DIR)/$(PKG_NAME)/
	printf '%s\n' \
	    'libcarmen demo ($(VERSION), macos $(ARCH))' \
	    '' \
	    'Run from inside this folder:' \
	    '  ./trail_demo en settings.toml' \
	    '' \
	    'First launch may be blocked by Gatekeeper (unsigned download).' \
	    'If so, clear the quarantine flag once:' \
	    '  xattr -dr com.apple.quarantine .' \
	    > $(BUILD_DIR)/$(PKG_NAME)/README.txt
	tar -czf $(PKG_TARBALL) -C $(BUILD_DIR) $(PKG_NAME)
	@echo ""
	@echo "Package ready: $(PKG_TARBALL)"

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf $(DIST_DIR)
	rm -f $(PKG_TARBALL)

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
                 include/carmen/utf8.h \
                 include/carmen/clue.h include/carmen/site.h \
                 include/carmen/connection.h include/carmen/city.h \
                 include/carmen/game_world.h \
                 include/carmen/world_islamic.h \
                 include/carmen/villain.h include/carmen/artifact.h \
                 include/carmen/case.h include/carmen/session.h \
                 include/carmen/settings.h include/carmen/save.h \
                 include/carmen/i18n.h

INCLUDEDIR   = $(PREFIX)/include/carmen
LIBDIR       = $(PREFIX)/lib
PKGCONFIGDIR = $(LIBDIR)/pkgconfig

install: $(STATIC_LIB) $(SHARED_LIB) $(GEN_VERSION_H) $(BUILD_DIR)/carmen.pc
	install -d $(INCLUDEDIR)
	install -m 644 $(PUBLIC_HEADERS) $(INCLUDEDIR)
	install -m 644 $(GEN_VERSION_H) $(INCLUDEDIR)/carmen_version.h
	install -d $(LIBDIR)
	install -m 644 $(STATIC_LIB) $(LIBDIR)
	install -m 755 $(SHARED_LIB) $(LIBDIR)/$(SHARED_REAL)
	ln -sf $(SHARED_REAL) $(LIBDIR)/$(SHARED_SONAME)
	ln -sf $(SHARED_REAL) $(LIBDIR)/$(SHARED_LINKNAME)
	install -d $(PKGCONFIGDIR)
	install -m 644 $(BUILD_DIR)/carmen.pc $(PKGCONFIGDIR)

uninstall:
	rm -rf $(INCLUDEDIR)
	rm -f  $(LIBDIR)/libcarmen.a
	rm -f  $(LIBDIR)/$(SHARED_REAL) $(LIBDIR)/$(SHARED_SONAME) $(LIBDIR)/$(SHARED_LINKNAME)
	rm -f  $(PKGCONFIGDIR)/carmen.pc

# --------------------------------------------------------------------------- #
#  Unit Tests (Unity)
# --------------------------------------------------------------------------- #

UNITY_SRC  = vendor/unity/unity.c
UNITY_INC  = -Ivendor/unity
TEST_FLAGS = -std=c17 -Wall -Wextra -pedantic -O0 -g -DUNITY_INCLUDE_DOUBLE $(FRIBIDI_CFLAGS)

TEST_DIR  = $(BUILD_DIR)/test
TEST_BINS = $(TEST_DIR)/test_site $(TEST_DIR)/test_connection $(TEST_DIR)/test_city \
            $(TEST_DIR)/test_utf8 $(TEST_DIR)/test_game_world $(TEST_DIR)/test_connection_gen \
            $(TEST_DIR)/test_carmen_scenarios \
            $(TEST_DIR)/test_artifact $(TEST_DIR)/test_case $(TEST_DIR)/test_session \
            $(TEST_DIR)/test_save $(TEST_DIR)/test_settings $(TEST_DIR)/test_villain \
            $(TEST_DIR)/test_world_islamic $(TEST_DIR)/test_i18n

# Presets that must be rejected by the generator (build-time validation).
GEN_REJECT_FIXTURES = bad_json over_cap_sites leftover_routes

test: $(TEST_BINS) $(GEN_WORLD) version-check
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
	echo "--- gen_world rejects invalid presets ---"; \
	for b in $(GEN_REJECT_FIXTURES); do \
		if $(GEN_WORLD) test/fixtures/$$b.jsonc $(BUILD_DIR)/gen_reject_$$b.c >/dev/null 2>&1; then \
			echo "FAIL: gen_world accepted invalid preset $$b"; fail=1; \
		else \
			echo "ok: rejected $$b"; \
		fi; \
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

$(TEST_DIR)/test_site: test/test_site.c src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_site.c src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(TEST_DIR)/test_connection: test/test_connection.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_connection.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(TEST_DIR)/test_city: test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(TEST_DIR)/test_utf8: test/test_utf8.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_utf8.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(TEST_DIR)/test_game_world: test/test_game_world.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_connection_gen: test/test_connection_gen.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_carmen_scenarios: test/test_carmen_scenarios.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_artifact: test/test_artifact.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_case: test/test_case.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_session: test/test_session.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_save: test/test_save.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_settings: test/test_settings.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_villain: test/test_villain.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_world_islamic: test/test_world_islamic.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(TEST_DIR)/test_i18n: test/test_i18n.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(TEST_DIR)
	$(CC) $(TEST_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

# --------------------------------------------------------------------------- #
#  Sanitizers (ASan + UBSan)
#
#  -Wall -Wextra -pedantic catch style/type mistakes, not undefined behavior.
#  test-sanitize rebuilds every test binary with ASan+UBSan instrumentation
#  and is required clean (in addition to plain `test`) before a change to
#  src/ is considered done -- see .cursor/rules/c-safety.mdc.
# --------------------------------------------------------------------------- #

SANITIZE_FLAGS = -std=c17 -Wall -Wextra -pedantic -O0 -g -DUNITY_INCLUDE_DOUBLE \
                  -fsanitize=address,undefined -fno-omit-frame-pointer $(FRIBIDI_CFLAGS)
SANITIZE_DIR   = $(BUILD_DIR)/sanitize
SANITIZE_BINS  = $(SANITIZE_DIR)/test_site $(SANITIZE_DIR)/test_connection $(SANITIZE_DIR)/test_city \
                 $(SANITIZE_DIR)/test_utf8 $(SANITIZE_DIR)/test_game_world $(SANITIZE_DIR)/test_connection_gen \
                 $(SANITIZE_DIR)/test_carmen_scenarios \
                 $(SANITIZE_DIR)/test_artifact $(SANITIZE_DIR)/test_case $(SANITIZE_DIR)/test_session \
                 $(SANITIZE_DIR)/test_save $(SANITIZE_DIR)/test_settings $(SANITIZE_DIR)/test_villain \
                 $(SANITIZE_DIR)/test_world_islamic $(SANITIZE_DIR)/test_i18n

test-sanitize: $(SANITIZE_BINS)
	@echo "========================================"
	@echo "  Running all test suites under ASan+UBSan"
	@echo "========================================"
	@fail=0; \
	for t in $(SANITIZE_BINS); do \
		echo ""; \
		echo "--- $$t ---"; \
		ASAN_OPTIONS=halt_on_error=1 \
		UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
		./$$t || fail=1; \
	done; \
	echo ""; \
	if [ $$fail -eq 0 ]; then \
		echo "========================================"; \
		echo "  ALL SUITES PASSED (ASan+UBSan clean)"; \
		echo "========================================"; \
	else \
		echo "========================================"; \
		echo "  SOME TESTS FAILED UNDER ASan+UBSan"; \
		echo "========================================"; \
		exit 1; \
	fi

$(SANITIZE_DIR):
	mkdir -p $(SANITIZE_DIR)

$(SANITIZE_DIR)/test_site: test/test_site.c src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_site.c src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(SANITIZE_DIR)/test_connection: test/test_connection.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_connection.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(SANITIZE_DIR)/test_city: test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(SANITIZE_DIR)/test_utf8: test/test_utf8.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ test/test_utf8.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(SANITIZE_DIR)/test_game_world: test/test_game_world.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_connection_gen: test/test_connection_gen.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_carmen_scenarios: test/test_carmen_scenarios.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_artifact: test/test_artifact.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_case: test/test_case.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_session: test/test_session.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_save: test/test_save.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_settings: test/test_settings.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_villain: test/test_villain.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_world_islamic: test/test_world_islamic.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

$(SANITIZE_DIR)/test_i18n: test/test_i18n.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(SANITIZE_DIR)
	$(CC) $(SANITIZE_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $^ $(LDLIBS)

# --------------------------------------------------------------------------- #
#  Code Coverage  (requires lcov:  brew install lcov)
# --------------------------------------------------------------------------- #

COV_FLAGS = -std=c17 -Wall -Wextra -pedantic -O0 -g --coverage -DUNITY_INCLUDE_DOUBLE $(FRIBIDI_CFLAGS)
COV_DIR   = $(BUILD_DIR)/coverage
LCOV_GCOV = --gcov-tool /usr/bin/gcov

LLVM_COV_GCOV := $(shell xcrun -f llvm-cov 2>/dev/null)
ifneq ($(LLVM_COV_GCOV),)
  LCOV_GCOV = --gcov-tool $(COV_DIR)/llvm-gcov.sh
endif

COV_BINS = $(COV_DIR)/test_site $(COV_DIR)/test_connection $(COV_DIR)/test_city \
           $(COV_DIR)/test_utf8 $(COV_DIR)/test_game_world $(COV_DIR)/test_connection_gen \
           $(COV_DIR)/test_carmen_scenarios \
           $(COV_DIR)/test_artifact $(COV_DIR)/test_case $(COV_DIR)/test_session \
           $(COV_DIR)/test_save $(COV_DIR)/test_settings $(COV_DIR)/test_villain \
           $(COV_DIR)/test_world_islamic $(COV_DIR)/test_i18n

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

$(COV_DIR)/test_site: test/test_site.c src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/site.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(COV_DIR)/test_connection: test/test_connection.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_city: test/test_city.c src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/city.c src/site.c src/connection.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_utf8: test/test_utf8.c src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< src/utf8.c $(FRIBIDI_OBJS) $(UNITY_SRC)

$(COV_DIR)/test_game_world: test/test_game_world.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_connection_gen: test/test_connection_gen.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_carmen_scenarios: test/test_carmen_scenarios.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_artifact: test/test_artifact.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_case: test/test_case.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_session: test/test_session.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_save: test/test_save.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_settings: test/test_settings.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_villain: test/test_villain.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_world_islamic: test/test_world_islamic.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

$(COV_DIR)/test_i18n: test/test_i18n.c $(LIB_SRCS_ALL) $(UNITY_SRC) | $(COV_DIR)/llvm-gcov.sh
	$(CC) $(COV_FLAGS) $(INCLUDES) $(UNITY_INC) -o $@ $< $(LIB_SRCS_ALL) $(UNITY_SRC) $(LDLIBS)

# --------------------------------------------------------------------------- #
#  Header dependency tracking
#
#  Include the compiler-generated .d files so header edits invalidate the
#  right objects.  The leading '-' silences the first build, before any
#  .d files exist.  These live under $(BUILD_DIR), so `make clean` removes
#  them along with the objects.
# --------------------------------------------------------------------------- #

DEPS = $(LIB_OBJS:.o=.d) $(TRAIL_DEMO_OBJ:.o=.d)
-include $(DEPS)

# --------------------------------------------------------------------------- #
#  Static analysis (SEI CERT C / CWE via GCC -fanalyzer)
#  Needs real GCC (Homebrew gcc-16); Apple's /usr/bin/gcc is clang and
#  silently ignores -fanalyzer.  Vendor sources are exempt per AGENTS.md.
# --------------------------------------------------------------------------- #

ANALYZER_CC   ?= gcc-16
ANALYZE_SRCS   = $(filter src/%.c,$(LIB_SRCS))
ANALYZE_FLAGS  = -fanalyzer -std=c17 -Wall -Wextra -pedantic -DHAVE_CONFIG_H
ifeq ($(UNAME_S),Darwin)
  ANALYZE_SYSROOT := -isysroot $(shell xcrun --show-sdk-path)
endif

analyze: $(GEN_VERSION_H)
	@echo "== $(ANALYZER_CC) -fanalyzer : SEI CERT / CWE audit =="
	@fail=0; \
	for f in $(ANALYZE_SRCS); do \
	    echo "--- $$f ---"; \
	    $(ANALYZER_CC) $(ANALYZE_FLAGS) $(ANALYZE_SYSROOT) $(INCLUDES) \
	        -c $$f -o /dev/null || fail=1; \
	done; \
	exit $$fail

# --------------------------------------------------------------------------- #
#  Version & SONAME parity guards
#
#  version-check flags any drift between the VERSION file, the generated
#  carmen_version.h, and the version $(VERSION) this Makefile resolved (see
#  doc/versioning.md). verify-soname asserts the shared library's recorded
#  name is the .$(SOMAJOR) form so Make output matches CMake's, using the
#  platform-appropriate dumper.
# --------------------------------------------------------------------------- #

version-check: $(GEN_VERSION_H)
	@sh tools/check_version.sh VERSION $(GEN_VERSION_H) $(VERSION)

verify-soname: $(SHARED_LIB)
ifeq ($(UNAME_S),Darwin)
	@otool -D $(SHARED_LIB) | grep -q "@rpath/$(SHARED_SONAME)" \
	    && echo "ok: install_name = @rpath/$(SHARED_SONAME)" \
	    || { echo "FAIL: install_name != @rpath/$(SHARED_SONAME)"; exit 1; }
else
	@readelf -d $(SHARED_LIB) | grep -q "$(SHARED_SONAME)" \
	    && echo "ok: soname = $(SHARED_SONAME)" \
	    || { echo "FAIL: soname != $(SHARED_SONAME)"; exit 1; }
endif

.PHONY: all lib dist package clean distclean test test-sanitize coverage analyze \
        install uninstall version-check verify-soname
