# hob64

*hob64* is a Header-Only Base64 library written in portable ANSI C.


## Features

- Portable ANSI C (C89), tested with GCC (Windows and Linux) and MSVC
- Encodes and decodes Base64 strings
- Checks to-be-decoded strings for validity

## Usage

Define the implementation before including hob64.
``` c
#define HOB64_IMPLEMENTATION
#include "hob64.h"
```
As usual with header-only libraries, the implementation's definition can be limited to just a single file. This will depend on your specific build configuration.

Encode binary data to Base64 by providing the data and its length, getting a Base64-encoded string and its length (via an output parameter) as a result.
``` c
const char* unencodedData = "hob64 is a Header-Only Base64 library.";
size_t encodedLength;
char* encoded = hob64_encode((const unsigned char*)unencodedData, strlen(unencodedData), &encodedLength);
free(encoded);
```
Remember to free the data when you're done with it.

Decode Base64-encoded strings by providing the string and its length, getting an array of bytes and its length (via an output parameter) as a result.
``` c
const unsigned char* encodedString = "aG9iNjQgaXMgYSBIZWFkZXItT25seSBCYXNlNjQgbGlicmFyeS4=";
size_t decodedLength;
unsigned char* decoded = hob64_decode(encodedData, strlen(encodedString), &decodedLength);
free(decoded);
```