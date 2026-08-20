# b64.c Design

## Purpose

`b64.c` provides RFC 4648 base64 encoding and decoding as a composable C library and CLI.

## Architecture

The library owns encoding and decoding logic. The CLI adapts stdin/stdout for human use.

```
b64.c (CLI)
  |
  +-- libb64.c (library)
        |
        +-- kc_b64_encode()
        +-- kc_b64_decode()
```

## Public Contract

- `kc_b64_encode(data, size)` returns a malloc'd base64 string, or NULL on allocation failure.
- `kc_b64_decode(str, out_size)` returns malloc'd binary data, or NULL on failure. The caller owns the returned memory.
- Input must have length divisible by 4 (returns NULL otherwise).
- NULL arguments cause NULL return.

## Ownership

- Encoder output is caller-owned, freed with `free()`.
- Decoder output is caller-owned, freed with `free()`.
- Runner output is caller-owned, freed with `free()`.
- Runner error messages are caller-owned, freed with `free()`.

## Limits

- No fixed maximum input size. Both encode and decode use checked dynamic allocation.
- Memory usage is proportional to input size.

## Composition

- CLI composes through stdin/stdout (binary in, base64 out for encode; base64 in, binary out for decode).
- Library composes through function calls.

## Non-Goals

- No streaming/chunked API (caller manages chunks).
- No URL-safe base64 variant.
- No padding options (always uses standard padding).
- No line-wrapping of output.
