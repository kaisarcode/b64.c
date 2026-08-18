/**
 * libb64.h - Base64 encode and decode
 * Summary: Public API for the b64 library.
 *
 * Author:  KaisarCode
 * Website: https://kaisarcode.com
 * License: https://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef KC_B64_H
#define KC_B64_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the build version generated at compile time.
 * @return Unix timestamp for the current build.
 */
uint64_t kc_b64_version(void);

/**
 * Base64-encodes binary data into a malloc'd string.
 * @param data Input data.
 * @param size Input size.
 * @return malloc'd base64 string, or NULL on allocation failure.
 */
char *kc_b64_encode(const void *data, size_t size);

/**
 * Base64-decodes a string into malloc'd binary data.
 * @param str Base64 string.
 * @param out_size Receives the decoded size.
 * @return malloc'd data, or NULL on failure.
 */
void *kc_b64_decode(const char *str, size_t *out_size);

/**
 * Executes a CLI subcommand from a JSON payload and returns the result as a
 * JSON string. The CLI builds a payload from its arguments, calls this
 * function, and formats the result back to its usual output.
 * @param payload_json JSON payload with "cmd" and "args".
 * @param out_err Receives a malloc'd error message on failure, or NULL on
 *     success.
 * @return malloc'd JSON result string, or NULL on failure.
 */
char *kc_b64_run(const char *payload_json, char **out_err);

#ifdef __cplusplus
}
#endif

#endif
