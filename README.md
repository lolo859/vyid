# VYID

VYID is a novel family of Unique Identifier, designed to respond to a various of use cases for UIDs. Before diving in, please note that VYIDv1 is not designed or intended for cryptographic use and should not be employed in security-critical applications where cryptographic randomness or collision resistance is paramount. VYIDv2 offers strong cryptographic security while maintaining excellent performance for its feature set. VYIDv1 stands out as one of the fastest, if not the fastest, identifiers in its category, given its rich feature set. VYIDv2, while still very fast for its capabilities, introduces cryptographically secure entropy and hashing, payload integration, and more advanced features.

---

## Features

VYIDv1 features a powerful combination of elements which make it a compelling alternative to UUIDv4 in many scenarios :
- Very large entropy : the core of VYIDv1's randomness originates from three uint64_t values. Combined, these offer a vast 192-bit state space, ensuring extreme uniqueness. (with strong random seed). The pseudo-random number generator (PRNG) used for this entropy, `Xoshiro256**`, is known for its high quality randomness and speed
- Cyclic corruption detection : VYID includes a cyclic corruption detection mechanism. Its design ensures that the integrity of an ID can be verified by re-deriving internal hashes and components from the ID string itself, allowing for detection of even single-character alterations. The XOR of the core random values also serves as the seed for our hash algorithm, `XXH3_64bits_withSeed`.
- Human-Readable & Compact : the use of a specialized string format (see [structure](#structure) for more info) enables easier human readability. And the use of a very fast `base85` algorithm allows for compactness (only 61 characters) without sacrificing performances
- Extremely fast generation and verification : the use of algorithms such as `XXH3_64bits_withSeed` and `Xoshiro256**`, combined with extreme code optimization, allow for astounding 39 nanoseconds of generation time and 13 nanoseconds verification time (on high-end CPU with outputting as char in automatic seed, see [benchmarkings][#benchmarkings] for more info).
- Bare metal full compatibility : the set of function inside the library allows for outputting as `char*` or `std::string` based on your need. You can also specify your own seed or let the library generate one for you. The `static` implementation of the `Xoshiro256**` algorithm allows for extremely fast VYID bulk generation (up to 21.1 millions per second on high-end CPU) and even more for VYID bulk verification.

VYIDv2 is an upgraded version which features payload integration, cryptographic security and many more, making it highly competitive in the domain :
- Extremely large entropy : the core of VYIDv2's randomness originates from eight uint64_t values. Combined, these offer a whopping 512-bit state space without payload or 256 bits with full payload, ensuring extreme uniqueness. (with strong random seed). The algorithm used for this entropy, `chacha20` is also great for randomness while maintaining speed
- Payload integration : the structure of VYIDv2 allows for the integration of a payload, up to 32 bytes of useful information, which can be extracted afterward. This is a very rare feature in the domain of UIDs. Please note that while the payload is stored inside the VYID, it is not encrypted or hidden beyond its insertion at pseudo-randomized positions within the main block.
- Robust Integrity Verification : VYIDv2 includes a corruption detection mechanism for the mainblock of the VYID. The use of `blake3` allows for maximum security while maintaining speed.
- Cryptographicaly secure : all external algorithms used in VYIDv2 such as `chacha20` and `blake3` ensure that the generation of subsequent VYIDs is unpredictable and the hash is irreversible, making it robust against cryptographic attacks.
- Compactness : the use of a specialized string format (see [structure](#structure) for more info) allows for easier human reading. And the use of a specialized `base512` allows for extreme compactness while staying very fast for the complexity of it, mainly due to the use of a correspondance table for encoding and a TRIE structure for decoding 
- Extremely fast generation and verification : the use of algorithms such as `chacha20` and `blake3` allow, with a combination of extreme optimisation into the code and a very fast base512 encoding system, allows for a very competitive 0.769 microsecond of generation time, 0.203 microsecond of payload extraction time and 0.278 microsecond verification time (on high-end CPU, see [benchmarkings][#benchmarkings] for more info).
- Bare metal full compatibility : You can specify your own seed or let the library generate one for you. The `static` implementation of the `blake3` algorithm allows for very fast VYIDv2 bulk generation (up to 1.3 million per second with full payload or 1.6 million without payload on high-end CPU) and even more for VYIDv2 bulk payload extraction and verification.

---

## Structure

VYIDv1 uses a new, human-readable structure, making it easy to identify and parse with tools like regular expressions:

```
[nnnnn-xxxxxaaaaaaaaaa-yyyyybbbbbbbbbb-zzzzzcccccccccc-kkkkk]
```

Each letter represents a **Base85-encoded character**. Note that `[`, `]`, and `-` are literal separators within the format, not Base85 characters.

Let's break down each component:

* **`nnnnn` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **first half of the hash of each encoded group**.
* **`xxxxx` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **first *encoded* group**. It's calculated as `(aaaaaaaaaa_decoded XOR (XXH3_64bits_withSeed(bbbbbbbbbb_decoded || cccccccccc_decoded, 16, common_seed) >> 32))`.
* **`aaaaaaaaaa` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **first core random value**.
* **`yyyyy` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **second *encoded* group**. It's calculated as `(bbbbbbbbbb_decoded XOR (XXH3_64bits_withSeed(aaaaaaaaaa_decoded || cccccccccc_decoded, 16, common_seed) >> 32))`.
* **`bbbbbbbbbb` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **second core random value**.
* **`zzzzz` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **third *encoded* group**. It's calculated as `(cccccccccc_decoded XOR (XXH3_64bits_withSeed(aaaaaaaaaa_decoded || bbbbbbbbbb_decoded, 16, common_seed) >> 32))`.
* **`cccccccccc` (10 chars):** This is the **Base85-encoded `uint64_t`** representing the **third core random value**.
* **`kkkkk` (5 chars):** This is the **Base85-encoded `uint32_t`** representing the **second half of the hash of each encoded group**.

The **common hash seed** used for all `XXH3_64bits_withSeed` calculations is derived from the XOR sum of the three core random values: `aaaaaaaaaa_decoded XOR bbbbbbbbbb_decoded XOR cccccccccc_decoded`.

Here, `||` signifies **byte concatenation** of the raw `uint64_t` or `uint32_t` values, as if they were treated as `char` arrays for the hashing function. The `>> 32` denotes truncating a `uint64_t` hash output to a `uint32_t`.

VYIDv2 use a similar format but include new features and is less human readable due to it using exotic UTF8 character, all characters are UTF-8, and can range from 1 to 4 bytes in length, though typically 1 or 2 bytes for the Base512 alphabet :

```
[ssssssss-aaaaaaaa-mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm-iiiiiiii]
```

Due to our special implementation of `base512`, each VYIDv2 has the same number of UTF8 character, but not the same amount of bytes. The minimum safe value is 182 bytes. Note that `[`, `]`, and `-` are literal separators within the format, not Base512 characters.

Here is the description of each compoments :
* **`ssssssss` (8 chars):** the session ID, made from a random `uint64_t` that doesn't change between VYIDv2 generation session. It is thread-local and remains constant for all VYIDv2 generations within a given thread's session. It can't be 0.
* **`aaaaaaaa` (8 chars):** the amount of VYIDv2 generated in a session, made from a `uint64_t` which increase by one for each VYIDv2 generated in the session. It can't be 0.
* **`mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm` (64 chars):** the mainblock, it contain the entropy which is overwritted by the payload if given.
* **`iiiiiiii` (8 chars):** the integrity checksum, made from a `uint64_t` which is the result of the `blake3` hash of the mainblock

---

## Benchmarkings

In this section, we will explore the benchmarks result of each method of generation and verification for VYIDv1. The result are shown on Ryzen 5 7600x which the high-end CPU evoked in the feature section

The benchmarking script was compiled on Linux 6.15.3-300.fc42.x86_64 on Fedora 42, with the `-Ofast`, `-flto` and `-march=native` compilation option. The total time showed is the time of 10 millions iterations. You can use the `bench.sh` script to test the perfomance of each test on your CPU.
Note: -flto was not used for the generation/verification benchmarks presented here.

### VYIDv1

```
---------------- VYID Benchmark with 10000000 iterations ----------------
v1 : Generation, output as string, automatic random seed
Total time: 0.436273 seconds
Average time per call: 0.0436273 µs
VYIDs per second: 22921434
---------------- VYID Benchmark with 10000000 iterations ----------------
v1 : Generation, output as char, automatic random seed
Total time: 0.38986 seconds
Average time per call: 0.038986 µs
VYIDs per second: 25650259
---------------- VYID Benchmark with 10000000 iterations ----------------
v1 : Generation, output as string, constant seed provided
Total time: 0.452834 seconds
Average time per call: 0.0452834 µs
VYIDs per second: 22083132
---------------- VYID Benchmark with 10000000 iterations ----------------
v1 : Generation, output as char, constant seed provided
Total time: 0.389505 seconds
Average time per call: 0.0389505 µs
VYIDs per second: 25673607
---------------- VYID Benchmark with 10000000 iterations ----------------
v1 : Generating VYID as char each time + verification
Total time: 0.5223 seconds
Average time per call: 0.05223 µs
VYIDs per second: 19146075
```

### VYIDv2

```
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Generation with full payload (32 bytes), automatic seed
Total time: 7.60858 seconds
Average time per call: 0.760858 µs
VYIDs per second: 1314306
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Generation with no payload, automatic seed
Total time: 6.22153 seconds
Average time per call: 0.622153 µs
VYIDs per second: 1607321
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Generation with full payload (32 bytes), constant provided seed
Total time: 7.73054 seconds
Average time per call: 0.773054 µs
VYIDs per second: 1293570
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Generation with no payload, constant provided seed
Total time: 6.23934 seconds
Average time per call: 0.623934 µs
VYIDs per second: 1602733
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Extracting payload, automatic seed, on the same VYIDv2
Total time: 2.0369 seconds
Average time per call: 0.20369 µs
VYIDs per second: 4909419
---------------- VYID Benchmark with 10000000 iterations ----------------
v2 : Verifying VYID, automatic seed, on the same VYIDv2
Total time: 2.7817 seconds
Average time per call: 0.27817 µs
VYIDs per second: 3594919
```

## Integration

## Integration

Usage of the VYID library is extremely simple :
1. Place all header files (`vyid.h`, `base85.h`, `xxhash.h`, `XoshiroCpp.hpp`, `base512.h`, `blake3_impl.h`, `blake3.h`, `chacha20.h`, `trie.h`, `utf8.h` in a directory accessible to your compiler's include path.
2. Compile `xxhash.c` into an object file (e.g., `xxhash.o`).
3. Link `xxhash.o` and `libblake.a` (which contains optimized `Blake3` implementations) with your final executable.

## Bare-metal compatibility

VYIDv1 was designed with bare-metal compatibility in mind. For environments without an operating system or access to system-level random number generation features (like `/dev/urandom` on Linux), you can define the `VYID_NO_OS_DEPENDENCIES` macro before including `vyid.h`. In this mode, the library disables all `std::string`-related functionality. The automatic random seed generators are also not available. It then becomes your responsibility to provide a high-quality (truly random) seed when using functions like `vyid::generate_vyid_char_seed()`. Without an appropriate random seed, the generated VYIDv1s will not be truly unique.

VYIDv2 is fully compatible with bare-metal and doesn't need any features non-available in bare-metal. However, to provide a good seed to `ChaCha20`, if you're on Linux, you can pass `nullptr` for the `seed` argument of the `initialize_csprng` function (this will get a seed from `/dev/urandom`). If you don't have access to this source of secure entropy, you will need to provide your own secure seed. In both cases, the use of the `initialize_csprng` function remains mandatory.

## Licences

VYID is distributed under the MIT License, offering you significant freedom to use, modify, and distribute it. However, please note that VYID utilizes other libraries, all of which are compliant with the MIT license. The Base85, Base512, and Trie implementations are all custom and fall under the same license as VYID.

## Warning

VYIDv1 includes an internal integrity check that can detect partial or accidental modifications (bitflips, character changes), but it does not provide cryptographic guarantees and cannot prevent deliberate forgery by an informed attacker.