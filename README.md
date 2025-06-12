# VYID

VYID is a novel Unique Identifier, designed for efficient integrity verification. Before diving in, please note that VYID is not designed or intended for cryptographic use and should not be employed in security-critical applications where cryptographic randomness or collision resistance is paramount. VYID stands out as one of the fastest, if not the fastest, identifiers in its category, given its rich feature set.

---

## Features

VYID features a amazing conbination of elements which make it a powerful alternative to UUIDv4 in many scenarios :
- Very large entropy : the core of VYID's randomness originates from three uint64_t values. Combined, these offer a vast 192-bit state space, ensuring extreme uniqueness. (with strong random seed). The algorithm used for this entropy, `Xoshiro256**` is also great for randomness
- Cyclic corruption detection : VYID includes a cyclic corruption detection mechanism. Its design ensures that the integrity of an ID can be verified by re-deriving internal hashes and components from the ID string itself, allowing for detection of even single-character alterations. The XOR of the core random values also serves as the seed for our hash algorithm, `XXH3_64bits_withSeed`.
- Human-Readable & Compact : the use of a specialized string format (see [structure](#structure) for more info) allow for easier human reading. And the use of a very fast `base85` algorithm allow for compactness (only 61 characters) without sacrificing performances
- Extremely fast generation and verification : the use of algorithms such as `XXH3_64bits_withSeed` and `Xoshiro256**` allow, with a combination of extreme optimisation into the code allow for an astounding 61 nanoseconds of generation time and 15 nanoseconds verification time for verification (on high-end CPU, see [benchmarkings][#benchmarkings] for more info).
- Bare metal full compatibility : the set of function inside the library allow for an outputting as `char*` or `std::string` depending on your need. You can also specify your own seed or let the library generate one for you. The `static` implementation of the `Xoshiro256**` algorithm allow for extremely fast VYID bulk generation (up to 16.4 millions per second on high-end CPU) and even more for VYID bulk verification.

---

## Structure

VYID uses a new, human-readable structure, making it easy to identify and parse with tools like regular expressions:

```
[nnnnn-xxxxxaaaaaaaaaa-yyyyybbbbbbbbbb-zzzzzcccccccccc-kkkkk]
```

Each letter represents a **Base85-encoded character**. Note that `[`, `]`, and `-` are literal separators within the format, not Base85 characters.

Let's break down each component:

* **`nnnnn` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **truncated hash of `kkkkk`**. Specifically, it's `(XXH3_64bits_withSeed(kkkkk_decoded_as_char_array, sizeof_of_kkkkk_decoded, common_seed) >> 32)`.
* **`xxxxx` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **first *encoded* group**. It's calculated as `(aaaaaaaaaa_decoded XOR (XXH3_64bits_withSeed(bbbbbbbbbb_decoded || cccccccccc_decoded, 16, common_seed) >> 32))`.
* **`aaaaaaaaaa` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **first core random value**.
* **`yyyyy` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **second *encoded* group**. It's calculated as `(bbbbbbbbbb_decoded XOR (XXH3_64bits_withSeed(aaaaaaaaaa_decoded || cccccccccc_decoded, 16, common_seed) >> 32))`.
* **`bbbbbbbbbb` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **second core random value**.
* **`zzzzz` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **third *encoded* group**. It's calculated as `(cccccccccc_decoded XOR (XXH3_64bits_withSeed(aaaaaaaaaa_decoded || bbbbbbbbbb_decoded, 16, common_seed) >> 32))`.
* **`cccccccccc` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **third core random value**.
* **`kkkkk` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **truncated hash of the three *encoded* groups (`xxxxx`, `yyyyy`, `zzzzz`)**. It's calculated as `(XXH3_64bits_withSeed(xxxxx_decoded || yyyyy_decoded || zzzzz_decoded, 12, common_seed) >> 32)`.

The **common hash seed** used for all `XXH3_64bits_withSeed` calculations is derived from the XOR sum of the three core random values: `aaaaaaaaaa_decoded XOR bbbbbbbbbb_decoded XOR cccccccccc_decoded`.

Here, `||` signifies **byte concatenation** of the raw `uint64_t` or `uint32_t` values, as if they were treated as `char` arrays for the hashing function. The `>> 32` denotes truncating a `uint64_t` hash output to a `uint32_t`.

---

## Benchmarkings

In this section, we will explore the benchmarks result of each method of generation and verification. The result are showed on two CPUs :
- Ryzen 5 7600x : the high-end CPU evoked in the feature section
- Athlon Silver 3050U 2.30 GHz
For both CPU, the benchmarking script was compiled on Linux 6.14.9-300.fc42.x86_64 on Fedora 42, with the `-Ofast` and `-march=native` compilation option. The total time showed is the time of 10 millions iterations.

### Ryzen 5 7600x

|Task                                                    |Total time (10M iterations)|Average time per call| Calls/second  (Millions) |
|:-------------------------------------------------------|:-------------------------:|:-------------------:|:------------------------:|
|**Generation, output as string, automatic random seed** |0.850294 second            |0.0850294 µs         |11.76                     |
|**Generation, output as char, automatic random seed**   |0.61315 second             |0.061315 µs          |16.31                     |
|**Generation, output as string, constant seed provided**|0.854977 second            |0.085083 µs          |11.75                     |
|**Generation, output as char, constant seed provided**  |0.609742 second            |0.0609742 µs         |16.40                     |
|**Generating VYID as char each time + verification**    |0.798953 second            |0.0798953 µs         |12.52                     |
|**Verification on the same VYID**                       |0.15412 second             |0.015412 µs          |64.88                     |

For both task which include verification, the VYID has each time been generated with automatic random seed.

### Athlon Silver 3050U 2.30 GHz

|Task                                                    |Total time (10M iterations)|Average time per call| Calls/second  (Millions) |
|:-------------------------------------------------------|:-------------------------:|:-------------------:|:------------------------:|
|**Generation, output as string, automatic random seed** |2.21133 second             |0.221133 µs          |4.52                      |
|**Generation, output as char, automatic random seed**   |1.67205 second             |0.167205 µs          |5.98                      |
|**Generation, output as string, constant seed provided**|2.25088 second             |0.225088 µs          |4.44                      |
|**Generation, output as char, constant seed provided**  |1.68907 second             |0.168907 µs          |5.92                      |
|**Generating VYID as char each time + verification**    |2.0865 second              |0.20865 µs           |4.79                      |
|**Verification on the same VYID**                       |0.482152 second            |0.0482152 µs         |20.74                     |

For both task which include verification, the VYID has each time been generated with automatic random seed.

## Integration

Usage of the VYID library is extremely simple : put the `vyid.h` header in your project directory as well as `base85.h`, `xxhash.c`, `xxhash.h` and `XoshiroCpp.hpp` and link `xxhash.c` to your executable.

## Bare-metal compatibility

VYID was designed with bare-metal compatibility in mind. For environments without an operating system or access to system-level random number generation features (like `/dev/urandom` on Linux), you can define the `VYID_NO_OS_DEPENDENCIES` macro before including vyid.h. In this mode, the library doesn't include `<string>` and disable all related function. The automatic random seed generator are also not available. It then becomes your responsibility to provide a high-quality (truly random) seed when using the `vyid::generate_vyid_char_seed()` function. Without an appropriate random seed, the generated VYIDs will not be truly unique.

## Licences

VYID is distributed under the MIT License, offering you significant freedom to use, modify, and distribute it. However, please note that VYID utilizes the XXH3 library (part of XXHash), which is also licensed under the MIT License. Ensure you understand and comply with the terms of both licenses when integrating VYID into your projects.

## Warning

VYID includes an internal integrity check that can detect partial or accidental modifications (bitflips, character changes), but it does not provide cryptographic guarantees and cannot prevent deliberate forgery by an informed attacker.
