# Lecture: Bit Masking, Bit Shifting & Bit Combining

> **Course:** C++ Tutorial for Complete Beginners — Lesson on Bitwise Operations
>
> **Prerequisite:** You should understand binary (base-2) and hexadecimal
> (base-16) notation. If `0xFF` or `10110011` looks unfamiliar, review
> those topics first.

---

## What You Will Learn

By the end of this lecture you will be able to:

1. **Mask** — use `&` to isolate specific bits inside an integer
2. **Shift** — use `<<` and `>>` to move bits into position
3. **Combine** — use `|` to merge separate values into one integer
4. Explain **why** these three operations always appear together
5. Apply them to a real use case: RGB pixel colors

---

## Part 1: Why Pack Data Into a Single Integer?

A pixel on your screen has three color channels:

| Channel | Purpose   | Range   |
| ------- | --------- | ------- |
| Red     | How red   | 0 – 255 |
| Green   | How green | 0 – 255 |
| Blue    | How blue  | 0 – 255 |

Each channel fits in **8 bits** (1 byte). You _could_ store them as three
separate variables:

```cpp
unsigned char red   = 0x12;   // 18
unsigned char green = 0x34;   // 52
unsigned char blue  = 0x56;   // 86
```

But a screen has millions of pixels. Three separate allocations per pixel
wastes memory and slows down the CPU cache. Instead, we **pack** all three
channels into a single 32-bit integer:

```
0x123456
```

This is faster, smaller, and how every graphics system actually works.

The question becomes: **how do you put values in, and how do you get them
back out?** That's where masking, shifting, and combining come in.

---

## Part 2: The Memory Layout

When we write `0x123456`, the 24 bits are arranged like this:

```
Bit position:  23       16 15        8 7         0
              ┌──────────┬──────────┬──────────┐
              │   Red    │  Green   │   Blue   │
              │ 00010010 │ 00110100 │ 01010110 │
              │   0x12   │   0x34   │   0x56   │
              └──────────┴──────────┴──────────┘
```

Each channel occupies a fixed **byte lane**:

| Channel | Bit positions | Hex mask   |
| ------- | ------------- | ---------- |
| Red     | 23 – 16       | `0xFF0000` |
| Green   | 15 – 8        | `0x00FF00` |
| Blue    | 7 – 0         | `0x0000FF` |

Keep this picture in your head. Everything below refers back to it.

---

## Part 3: Bit Masking (`&`) — Extracting Pieces

### The AND Truth Table

The bitwise AND operator compares each pair of bits:

| A   | B   | A & B |
| --- | --- | ----- |
| 0   | 0   | 0     |
| 0   | 1   | 0     |
| 1   | 0   | 0     |
| 1   | 1   | 1     |

A bit survives **only if both** inputs are `1`. Everywhere else becomes `0`.

### The Concept of a Mask

A **mask** is a number where:

- bits you **want to keep** are set to `1`
- bits you **want to erase** are set to `0`

When you AND a value with a mask, you **extract** the bits you care about
and zero out everything else.

### Extracting Red from `0x123456`

```
value:  00010010  00110100  01010110     (0x123456)
mask:   11111111  00000000  00000000     (0xFF0000)
────────────────────────────────────
result: 00010010  00000000  00000000     (0x120000)
```

The green and blue bytes are gone — zeroed out by the mask. Only the red
byte survived.

But the result is `0x120000`, not `0x12`. The red value is still sitting
in bits 23–16. We need to **move** it down to bits 7–0. That brings us to
shifting.

### Extracting Green and Blue

```cpp
// Green: mask with 0x00FF00, then shift right 8
unsigned char green = (color & 0x00FF00) >> 8;

// Blue: mask with 0x0000FF, no shift needed (already in bits 7-0)
unsigned char blue = color & 0x0000FF;
```

### Key Insight

> **Masking answers the question: "Which byte do I want?"**
> It selects the channel. But the value is still in the wrong position.
> Shifting fixes that.

---

## Part 4: Bit Shifting (`<<` and `>>`) — Moving Pieces

### Right Shift (`>>`)

Moves every bit to the right by `n` positions. Bits that fall off the
right edge are discarded. New bits on the left are filled with `0`
(for unsigned types).

```
Before:  10000000
>> 1:    01000000
>> 2:    00100000
```

Each right shift by 1 **divides the value by 2** (integer division).

### Left Shift (`<<`)

Moves every bit to the left. Bits that fall off the left edge are
discarded. New bits on the right are filled with `0`.

```
Before:  00000010
<< 1:    00000100
<< 2:    00001000
```

Each left shift by 1 **multiplies the value by 2**.

### Why Shifting Matters for Colors

After masking red from `0x123456`:

```
result: 00010010  00000000  00000000     (= 0x120000)
```

The value `0x12` is stuck in bits 23–16. To get the actual number `0x12`
(= 18), we shift it down 16 positions:

```
>> 16:  00000000  00000000  00010010     (= 0x12 = 18)
```

Complete extraction:

```cpp
unsigned char red = (color & 0xFF0000) >> 16;
// Step 1 — mask:  keep only bit 23-16      → 0x120000
// Step 2 — shift: move bits 23-16 → 7-0    → 0x12
```

### Shift Distances for Each Channel

| Channel | Mask       | Shift   | Why                       |
| ------- | ---------- | ------- | ------------------------- |
| Red     | `0xFF0000` | `>> 16` | Bits 23–16 → move down 16 |
| Green   | `0x00FF00` | `>> 8`  | Bits 15–8 → move down 8   |
| Blue    | `0x0000FF` | none    | Already in bits 7–0       |

### Key Insight

> **Shifting answers the question: "Where should this byte go?"**
> It moves a value to or from its correct byte lane.

---

## Part 5: Bit Combining (`|`) — Assembling Pieces

### The OR Truth Table

| A   | B   | A \| B |
| --- | --- | ------ |
| 0   | 0   | 0      |
| 0   | 1   | 1      |
| 1   | 0   | 1      |
| 1   | 1   | 1      |

The result is `1` if **either** input is `1`. OR never turns a `1` into a `0`.

### Building a Color from Separate Values

We have three separate values:

```cpp
unsigned char red   = 0x12;
unsigned char green = 0x34;
unsigned char blue  = 0x56;
```

First, shift each value into its correct byte lane:

```
red   << 16:  00010010  00000000  00000000     (0x120000)
green <<  8:  00000000  00110100  00000000     (0x003400)
blue       :  00000000  00000000  01010110     (0x000056)
```

Notice: each value occupies **different** bit positions. There is no
overlap. This is important — OR works perfectly when values don't share
bit positions.

Now OR them together:

```
  00010010  00000000  00000000     (red,   shifted)
| 00000000  00110100  00000000     (green, shifted)
| 00000000  00000000  01010110     (blue,  not shifted)
──────────────────────────────
  00010010  00110100  01010110     = 0x123456 ✓
```

In one line of C++:

```cpp
int color = (red << 16) | (green << 8) | blue;
```

### Key Insight

> **OR answers the question: "How do I glue these bytes together?"**
> It merges non-overlapping values into a single integer.

---

## Part 6: The Complete Round-Trip

These three operations form a **pack/unpack cycle**:

### Packing (writing): Shift + OR

```cpp
// Three separate values → one integer
int color = (red << 16) | (green << 8) | blue;
```

### Unpacking (reading): AND + Shift

```cpp
// One integer → three separate values
unsigned char red   = (color & 0xFF0000) >> 16;
unsigned char green = (color & 0x00FF00) >> 8;
unsigned char blue  = (color & 0x0000FF);
```

### Diagram of the Full Cycle

```
 PACK (combine)                         UNPACK (extract)
 ──────────────                         ────────────────

 R=0x12  ──── << 16 ──→ 0x120000 ─┐
                                   ├── OR ──→ 0x123456 ──┬── & 0xFF0000 → >> 16 → 0x12 (R)
 G=0x34  ──── <<  8 ──→ 0x003400 ─┤                     ├── & 0x00FF00 → >>  8 → 0x34 (G)
                                   │                     └── & 0x0000FF ────────→ 0x56 (B)
 B=0x56  ──── (none) ──→ 0x000056 ─┘
```

---

## Part 7: The Mental Model

Think of a packed integer as a **shelf with labeled compartments**:

```
┌────────────┬────────────┬────────────┐
│    Red     │   Green    │    Blue    │
│ compartment│ compartment│ compartment│
└────────────┴────────────┴────────────┘
```

| Operation | Analogy                                      |
| --------- | -------------------------------------------- |
| `&` Mask  | **Peek** into one compartment (block others) |
| `>>` `<<` | **Slide** an item to the correct compartment |
| `\|` OR   | **Place** items into their compartments      |

Or even simpler:

```
Mask  (&)  →  take pieces out
Shift      →  move pieces left or right
OR    (|)  →  glue pieces together
```

---

## Part 8: Real-World Pixel Format

Modern graphics systems add a 4th channel — **Alpha** (transparency):

```
0xAARRGGBB     or     0xRRGGBBAA
```

In this project, `setPixel` uses **RGBA8888** format:

```
┌────────┬────────┬────────┬────────┐
│  Red   │ Green  │  Blue  │ Alpha  │
│ 31-24  │ 23-16  │ 15-8   │  7-0   │
└────────┴────────┴────────┴────────┘
```

```cpp
// src/Screen.cpp — packing a pixel with OR
m_buffer1[y * m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
```

And if you needed to **read back** a pixel:

```cpp
uint32_t pixel = m_buffer1[y * m_width + x];

uint8_t r = (pixel & 0xFF000000) >> 24;   // mask + shift
uint8_t g = (pixel & 0x00FF0000) >> 16;
uint8_t b = (pixel & 0x0000FF00) >> 8;
uint8_t a = (pixel & 0x000000FF);          // no shift needed
```

Same three operations. Same pattern. Just one extra channel.

---

## Part 9: Where This Is Used Beyond Graphics

| Domain            | What gets packed                           |
| ----------------- | ------------------------------------------ |
| Graphics / Games  | Pixel colors (RGBA), vertex data           |
| Networking        | IP addresses, TCP flags, protocol headers  |
| Operating Systems | File permissions, CPU status registers     |
| Data Compression  | Huffman codes, variable-length encoding    |
| Cryptography      | Hash functions (SHA, MD5 use shifts + XOR) |
| Embedded Systems  | Hardware register control (GPIO pins)      |

The technique is always the same:

1. **Shift** values into non-overlapping positions
2. **OR** them together to store
3. **AND** with a mask to extract
4. **Shift** back to get the original value

---

## Summary

```
┌──────────────────────────────────────────────────────┐
│               The Three Core Operations              │
├──────────────┬───────────────────────────────────────┤
│  Mask  (&)   │  Isolate specific bits from a number  │
│  Shift (<>)  │  Move bits to the correct position    │
│  OR    (|)   │  Merge non-overlapping values         │
├──────────────┴───────────────────────────────────────┤
│                                                      │
│  Pack:    color = (R << 16) | (G << 8)  | B          │
│  Unpack:  R = (color & 0xFF0000) >> 16               │
│           G = (color & 0x00FF00) >> 8                │
│           B = (color & 0x0000FF)                     │
│                                                      │
└──────────────────────────────────────────────────────┘
```

---

## Further Reading

- [bitwise-operations.md](bitwise-operations.md) — Reference guide with the
  `uint32_t` undefined behavior trap
- [twos-complement.md](twos-complement.md) — How signed numbers affect
  bit shifting (arithmetic vs logical shifts)
