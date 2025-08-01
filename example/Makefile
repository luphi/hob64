# The compiler to use. "gcc" works for most *nix systems and Windows with MinGW
CC := gcc
# Compilation flags, intended for gcc in this case. Among these, -I.. and -ansi are important. The former adds the
# folder one level up as an include directory and the latter indicates the ANSI C standard (a.k.a. C89).
CFLAGS := -I.. -Wall -ansi

# If building on Windows (MinGW)
ifeq ($(OS),Windows_NT)
	# Use this .exe file name for the output executable
	EXEC := hob64-example.exe
# If not building on Windows (Linux, macOS, *BSD, BeOS/Haiku, etc.)
else
	# Use this .bin file name for the output executable
	# (Extensions aren't typical on other systems but this is convenient for identifying binaries in our .gitignore)
	EXEC := hob64-example.bin
endif

# Tell make that these targets are not files
.PHONY: release debug remake clean

# Target for building a release executable that excludes debug symbols and uses a high optimization level
release: hob64-example.c
	$(CC) $(CFLAGS) -O3 hob64-example.c -o $(EXEC)

# Target for building a debug executable that includes debug symbols
debug: hob64-example.c
	$(CC) $(CFLAGS) -g hob64-example.c -o $(EXEC)

remake: clean release

# Target for removing files built by this Makefile - just the executable whose name is in the EXEC variable
clean:
	rm -f $(EXEC)