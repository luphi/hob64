#include <stdio.h> /* printf() */
#include <stdlib.h> /* EXIT_SUCCESS, free() */
#include <string.h> /* strlen() */

#define HOB64_IMPLEMENTATION
#include "hob64.h"

int main(int argc, char** argv) {
    /* Define some strings to encode and then decode */
    const char* unencodedData1 = "Many hands make light work.";
    const char* unencodedData2 = "hob64 is a Header-Only Base64 library.";

    /* Encode the input strings. The returned string is an equivalent Base64-encoded string and the third, output */
    /* parameter will be assigned with the length of that string. */
    size_t encodedLength1, encodedLength2;
    char* encoded1 = hob64_encode((const unsigned char*)unencodedData1, strlen(unencodedData1), &encodedLength1);
    char* encoded2 = hob64_encode((const unsigned char*)unencodedData2, strlen(unencodedData2), &encodedLength2);

    /* Display the inputs and outputs of the encode */
    printf("\n");
    printf("Encoded \"%s\" (%zu) as\n        \"%s\" (%zu)\n", unencodedData1, strlen(unencodedData1), encoded1,
        strlen(encoded1));
    printf("Encoded \"%s\" (%zu) as\n        \"%s\" (%zu)\n", unencodedData2, strlen(unencodedData2), encoded2,
        strlen(encoded2));

    /* Decode the Base64 strings. The returned arrays will be bit-exact copies of the input strings with the third, */
    /* output parameter being the length of those arrays, in bytes. */
    size_t decodedLength1, decodedLength2;
    unsigned char* decoded1 = hob64_decode(encoded1, encodedLength1, &decodedLength1);
    unsigned char* decoded2 = hob64_decode(encoded2, encodedLength2, &decodedLength2);

    /* Display the inputs and outputs of the decode, keeping in mind that the decoded data is not null terminated */
    printf("\n");
    printf("Decoded \"%s\" (%zu) as\n        \"", encoded1, strlen(encoded1));
    for (size_t i = 0; i < decodedLength1; i++)
        printf("%c", (char)decoded1[i]);
    printf("\" (%zu)\n", decodedLength1);
    printf("Decoded \"%s\" (%zu) as\n        \"", encoded2, strlen(encoded2));
    for (size_t i = 0; i < decodedLength2; i++)
        printf("%c", (char)decoded2[i]);
    printf("\" (%zu)\n", decodedLength2);

    /* The encode and decode functions allocated memory to hold their results. Free that memory. */
    free(encoded1);
    free(encoded2);
    free(decoded1);
    free(decoded2);

    return EXIT_SUCCESS;
}
