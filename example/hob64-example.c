#include <stddef.h> /* NULL, size_t */
#include <stdio.h>  /* printf() */
#include <stdlib.h> /* EXIT_SUCCESS, free() */
#include <string.h> /* strlen() */

#define HOB64_IMPLEMENTATION
#include "hob64.h"

int main(void) {
    const char *unencoded_data1, *unencoded_data2;
    char *encoded1, *encoded2;
    unsigned char *decoded1, *decoded2;
    size_t encoded_length1, encoded_length2, decoded_length1, decoded_length2, i;

    /* Initialize variables by assigning everything to zero or equivalent */
    unencoded_data1 = unencoded_data2 = NULL;
    encoded1 = encoded2 = NULL;
    decoded1 = decoded2 = NULL;
    encoded_length1 = encoded_length2 = decoded_length1 = decoded_length2 = i = 0;

    /* Define some strings to encode and then decode */
    unencoded_data1 = "Many hands make light work.";
    unencoded_data2 = "hob64 is a Header-Only Base64 library.";

    /* Encode the input strings. The returned string is an equivalent Base64-encoded string and the third, output */
    /* parameter will be assigned with the length of that string. */
    encoded1 = hob64_encode((const unsigned char*)unencoded_data1, strlen(unencoded_data1), &encoded_length1);
    encoded2 = hob64_encode((const unsigned char*)unencoded_data2, strlen(unencoded_data2), &encoded_length2);

    /* Display the inputs and outputs of the encode */
    printf("\n");
    printf("Encoded \"%s\" (%lu) as\n        \"%s\" (%lu)\n", unencoded_data1, (unsigned long)strlen(unencoded_data1),
        encoded1, (unsigned long)strlen(encoded1));
    printf("Encoded \"%s\" (%lu) as\n        \"%s\" (%lu)\n", unencoded_data2, (unsigned long)strlen(unencoded_data2),
        encoded2, (unsigned long)strlen(encoded2));

    /* Decode the Base64 strings. The returned arrays will be bit-exact copies of the input strings with the third, */
    /* output parameter being the length of those arrays, in bytes. */
    decoded1 = hob64_decode(encoded1, encoded_length1, &decoded_length1);
    decoded2 = hob64_decode(encoded2, encoded_length2, &decoded_length2);

    /* Display the inputs and outputs of the decode, keeping in mind that the decoded data is not null terminated */
    printf("\n");
    printf("Decoded \"%s\" (%lu) as\n        \"", encoded1, (unsigned long)strlen(encoded1));
    for (i = 0; i < decoded_length1; i++)
        printf("%c", (char)decoded1[i]);
    printf("\" (%lu)\n", (unsigned long)decoded_length1);
    printf("Decoded \"%s\" (%lu) as\n        \"", encoded2, (unsigned long)strlen(encoded2));
    for (i = 0; i < decoded_length2; i++)
        printf("%c", (char)decoded2[i]);
    printf("\" (%lu)\n", (unsigned long)decoded_length2);
    printf("\n");

    /* The encode and decode functions allocated memory to hold their results. Free that memory. */
    free(encoded1);
    free(encoded2);
    free(decoded1);
    free(decoded2);

    return EXIT_SUCCESS;
}
