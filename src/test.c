/**
 * test.c - Contract tests for the b64 library
 * Summary: Public API tests for encode and decode.
 *
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#include "libb64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Verifies one boolean condition.
 * @param name Check description.
 * @param cond Non-zero when the check passed.
 * @return 0 on success, 1 on failure.
 */
static int expect_true(const char *name, int cond) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", name);
        return 1;
    }
    return 0;
}

/**
 * Verifies one integer result.
 * @param name Check description.
 * @param expected Expected value.
 * @param actual Actual value.
 * @return 0 on success, 1 on failure.
 */
static int expect_int(const char *name, int expected, int actual) {
    if (expected != actual) {
        fprintf(stderr, "FAIL: %s (expected %d, got %d)\n", name, expected, actual);
        return 1;
    }
    return 0;
}

/**
 * Verifies one string result.
 * @param name Check description.
 * @param expected Expected string.
 * @param actual Actual string.
 * @return 0 on success, 1 on failure.
 */
static int expect_str(const char *name, const char *expected, const char *actual) {
    if (strcmp(expected, actual) != 0) {
        fprintf(stderr, "FAIL: %s (expected \"%s\", got \"%s\")\n", name, expected, actual);
        return 1;
    }
    return 0;
}

/**
 * Tests encoding empty input.
 * @return 0 on success, 1 on failure.
 */
static int case_encode_empty(void) {
    char *encoded = kc_b64_encode("", 0);
    int rc = 0;
    rc += expect_true("encode empty returns non-NULL", encoded != NULL);
    if (encoded) {
        rc += expect_str("encode empty returns \"\"", "", encoded);
        free(encoded);
    }
    return rc;
}

/**
 * Tests encoding "hello" string.
 * @return 0 on success, 1 on failure.
 */
static int case_encode_hello(void) {
    const char *data = "hello";
    char *encoded = kc_b64_encode(data, strlen(data));
    int rc = 0;
    rc += expect_true("encode hello returns non-NULL", encoded != NULL);
    if (encoded) {
        rc += expect_str("encode hello", "aGVsbG8=", encoded);
        free(encoded);
    }
    return rc;
}

/**
 * Tests encoding binary data with round-trip.
 * @return 0 on success, 1 on failure.
 */
static int case_encode_binary(void) {
    const unsigned char data[] = {0, 1, 127, 128, 255};
    char *encoded = kc_b64_encode(data, sizeof(data));
    size_t decoded_len = 0;
    void *decoded;
    int rc = 0;
    rc += expect_true("encode binary returns non-NULL", encoded != NULL);
    if (encoded) {
        decoded = kc_b64_decode(encoded, &decoded_len);
        rc += expect_true("round-trip decode returns non-NULL", decoded != NULL);
        if (decoded) {
            rc += expect_int("round-trip length", (int)sizeof(data), (int)decoded_len);
            rc += expect_true("round-trip data matches", memcmp(data, decoded, sizeof(data)) == 0);
            free(decoded);
        }
        free(encoded);
    }
    return rc;
}

/**
 * Tests decoding empty string.
 * @return 0 on success, 1 on failure.
 */
static int case_decode_empty(void) {
    size_t out_size = 0;
    void *decoded = kc_b64_decode("", &out_size);
    int rc = 0;
    rc += expect_true("decode empty returns non-NULL", decoded != NULL);
    rc += expect_int("decode empty size", 0, (int)out_size);
    free(decoded);
    return rc;
}

/**
 * Tests decoding "aGVsbG8=" to "hello".
 * @return 0 on success, 1 on failure.
 */
static int case_decode_hello(void) {
    size_t out_size = 0;
    void *decoded = kc_b64_decode("aGVsbG8=", &out_size);
    int rc = 0;
    rc += expect_true("decode hello returns non-NULL", decoded != NULL);
    if (decoded) {
        rc += expect_int("decode hello length", 5, (int)out_size);
        rc += expect_true("decode hello matches", memcmp(decoded, "hello", 5) == 0);
        free(decoded);
    }
    return rc;
}

/**
 * Tests encode/decode round-trip with various byte values.
 * @return 0 on success, 1 on failure.
 */
static int case_roundtrip(void) {
    const unsigned char data[] = {0, 1, 127, 128, 255, 'A', 'B'};
    size_t data_len = sizeof(data);
    char *encoded;
    size_t decoded_len = 0;
    void *decoded;
    int rc = 0;

    encoded = kc_b64_encode(data, data_len);
    rc += expect_true("roundtrip encode returns non-NULL", encoded != NULL);

    if (encoded != NULL) {
        decoded = kc_b64_decode(encoded, &decoded_len);
        rc += expect_true("roundtrip decode returns non-NULL", decoded != NULL);
        rc += expect_int("roundtrip length", (int)data_len, (int)decoded_len);
        if (decoded != NULL) {
            rc += expect_true("roundtrip data matches", memcmp(data, decoded, data_len) == 0);
        }
        free(decoded);
    }
    free(encoded);
    return rc;
}

/**
 * Tests decoding invalid base64 string.
 * @return 0 on success, 1 on failure.
 */
static int case_decode_invalid(void) {
    size_t out_size = 0;
    void *decoded = kc_b64_decode("invalid!", &out_size);
    int rc = 0;
    rc += expect_true("decode invalid returns NULL", decoded == NULL);
    return rc;
}

/**
 * Tests decoding string with length not divisible by 4.
 * @return 0 on success, 1 on failure.
 */
static int case_decode_bad_length(void) {
    size_t out_size = 0;
    void *decoded = kc_b64_decode("abc", &out_size);
    int rc = 0;
    rc += expect_true("decode bad length returns NULL", decoded == NULL);
    return rc;
}

/**
 * Tests NULL argument handling.
 * @return 0 on success, 1 on failure.
 */
static int case_null_args(void) {
    int rc = 0;
    rc += expect_true("encode NULL data returns NULL", kc_b64_encode(NULL, 0) == NULL);
    rc += expect_true("decode NULL str returns NULL", kc_b64_decode(NULL, &(size_t){0}) == NULL);
    return rc;
}

/**
 * Tests runner encode subcommand.
 * @return 0 on success, 1 on failure.
 */
static int case_run_encode(void) {
    char *err = NULL;
    char *result = kc_b64_run("{\"cmd\":\"encode\",\"data\":\"hello\"}", &err);
    int rc = 0;
    rc += expect_true("run encode returns non-NULL", result != NULL);
    rc += expect_true("run encode no error", err == NULL);
    if (result) {
        rc += expect_true("run encode contains result", strstr(result, "aGVsbG8=") != NULL);
        free(result);
    }
    free(err);
    return rc;
}

/**
 * Tests runner decode subcommand.
 * @return 0 on success, 1 on failure.
 */
static int case_run_decode(void) {
    char *err = NULL;
    char *result = kc_b64_run("{\"cmd\":\"decode\",\"data\":\"aGVsbG8=\"}", &err);
    int rc = 0;
    rc += expect_true("run decode returns non-NULL", result != NULL);
    rc += expect_true("run decode no error", err == NULL);
    if (result) {
        rc += expect_true("run decode contains data", strstr(result, "hello") != NULL);
        free(result);
    }
    free(err);
    return rc;
}

/**
 * Tests runner with missing cmd.
 * @return 0 on success, 1 on failure.
 */
static int case_run_errors(void) {
    char *err = NULL;
    char *result;
    int rc = 0;

    result = kc_b64_run(NULL, &err);
    rc += expect_true("run NULL payload returns NULL", result == NULL);
    rc += expect_true("run NULL payload sets error", err != NULL);
    free(err);

    err = NULL;
    result = kc_b64_run("{\"data\":\"test\"}", &err);
    rc += expect_true("run missing cmd returns NULL", result == NULL);
    rc += expect_true("run missing cmd sets error", err != NULL);
    free(err);

    err = NULL;
    result = kc_b64_run("{\"cmd\":\"unknown\"}", &err);
    rc += expect_true("run unknown cmd returns NULL", result == NULL);
    rc += expect_true("run unknown cmd sets error", err != NULL);
    free(err);

    return rc;
}

/**
 * Tests version function.
 * @return 0 on success, 1 on failure.
 */
static int case_version(void) {
    int rc = 0;
    rc += expect_true("version returns non-zero", kc_b64_version() != 0);
    return rc;
}

/**
 * Runs one b64 public API test case.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, 1 or 2 on failure.
 */
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "test case: expected one argument, got %d\n", argc - 1);
        return 2;
    }
    if (strcmp(argv[1], "encode-empty") == 0) return case_encode_empty();
    if (strcmp(argv[1], "encode-hello") == 0) return case_encode_hello();
    if (strcmp(argv[1], "encode-binary") == 0) return case_encode_binary();
    if (strcmp(argv[1], "decode-empty") == 0) return case_decode_empty();
    if (strcmp(argv[1], "decode-hello") == 0) return case_decode_hello();
    if (strcmp(argv[1], "roundtrip") == 0) return case_roundtrip();
    if (strcmp(argv[1], "decode-invalid") == 0) return case_decode_invalid();
    if (strcmp(argv[1], "decode-bad-length") == 0) return case_decode_bad_length();
    if (strcmp(argv[1], "null-args") == 0) return case_null_args();
    if (strcmp(argv[1], "run-encode") == 0) return case_run_encode();
    if (strcmp(argv[1], "run-decode") == 0) return case_run_decode();
    if (strcmp(argv[1], "run-errors") == 0) return case_run_errors();
    if (strcmp(argv[1], "version") == 0) return case_version();
    fprintf(stderr, "unknown test case: %s\n", argv[1]);
    return 2;
}
