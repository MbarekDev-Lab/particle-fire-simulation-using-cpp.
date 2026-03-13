# Bitwise Operations — AND, OR & Shifting

> Based on the **C++ Tutorial for Complete Beginners** by John Purcell
> (Lectures: _Bitwise AND_ & _Bitwise OR_)

---

## Table of Contents

1. [How Colors Are Stored](#1-how-colors-are-stored)
2. [The Bit Layout](#2-the-bit-layout)
3. [Bitwise AND — Extracting Colors](#3-bitwise-and--extracting-colors)
4. [Bitwise OR — Combining Colors](#4-bitwise-or--combining-colors)
5. [Bit Shifting](#5-bit-shifting)
6. [Quick Reference](#6-quick-reference)
7. [How This Project Uses It](#7-how-this-project-uses-it)
8. [The `uint32_t` Trap (Undefined Behavior)](#8-the-uint32_t-trap-undefined-behavior)

---

## 1. How Colors Are Stored

A screen pixel has three color channels — **Red**, **Green**, and **Blue**.
Instead of using three separate variables, we pack all three into **one integer**:

```
0x123456
```

Each pair of hex digits is one channel:

| Part  | Hex    | Decimal |
| ----- | ------ | ------- |
| Red   | `0x12` | 18      |
| Green | `0x34` | 52      |
| Blue  | `0x56` | 86      |

---

## 2. The Bit Layout

Computers store numbers in binary. Each color channel uses **8 bits** (1 byte):


```
0x123456 in binary:

┌──────────────────┬──────────────────┬──────────────────┐
│       Red        │      Green       │       Blue       │
│    bits 23-16    │    bits 15-8     │     bits 7-0     │
├──────────────────┼──────────────────┼──────────────────┤
│    0001 0010     │    0011 0100     │    0101 0110     │
│      0x12        │      0x34        │      0x56        │
└──────────────────┴──────────────────┴──────────────────┘
```

---

## 3. Bitwise AND — Extracting Colors

**AND (`&`)** compares two bits. The result is `1` only when **both** bits are `1`:

```
Bit A │ Bit B │ Result
──────┼───────┼───────
  0   │   0   │   0
  0   │   1   │   0
  1   │   0   │   0
  1   │   1   │   1
```

Think of AND as a **filter** — it keeps the bits you want and zeros out everything else.

### Extracting Red

```cpp
unsigned char red = (color & 0xFF0000) >> 16;
```

**Step 1 — Mask** with `& 0xFF0000`:

```
color:  00010010  00110100  01010110
mask:   11111111  00000000  00000000
────────────────────────────────────
result: 00010010  00000000  00000000
```

The mask `0xFF0000` has `1`s only in the red byte — everything else becomes `0`.

**Step 2 — Shift** with `>> 16`:

```
before: 00010010  00000000  00000000
after:  00000000  00000000  00010010   →  0x12 ✓
```

### Extracting Green

```cpp
unsigned char green = (color & 0x00FF00) >> 8;
```

```
color:  00010010  00110100  01010110
mask:   00000000  11111111  00000000
────────────────────────────────────
result: 00000000  00110100  00000000
>> 8:   00000000  00000000  00110100   →  0x34 ✓
```

### Extracting Blue

```cpp
unsigned char blue = color & 0x0000FF;
```

```
color:  00010010  00110100  01010110
mask:   00000000  00000000  11111111
────────────────────────────────────
result: 00000000  00000000  01010110   →  0x56 ✓
```

No shift needed — blue is already in the lowest byte.

---

## 4. Bitwise OR — Combining Colors

**OR (`|`)** compares two bits. The result is `1` if **either** bit is `1`:

```
Bit A │ Bit B │ Result
──────┼───────┼───────
  0   │   0   │   0
  0   │   1   │   1
  1   │   0   │   1
  1   │   1   │   1
```

Think of OR as **glue** — it merges bits together without disturbing each other.

### Rebuilding a Color

Given separate values:

```cpp
unsigned char red   = 0x12;
unsigned char green = 0x34;
unsigned char blue  = 0x56;
```

**Step 1 — Shift** each channel into position:

```
red   << 16  →  00010010  00000000  00000000   (0x120000)
green <<  8  →  00000000  00110100  00000000   (0x003400)
blue         →  00000000  00000000  01010110   (0x000056)
```

**Step 2 — OR** them together:

```
  00010010  00000000  00000000
| 00000000  00110100  00000000
| 00000000  00000000  01010110
──────────────────────────────
  00010010  00110100  01010110   =  0x123456 ✓
```

In code:

```cpp
int color = (red << 16) | (green << 8) | blue;
```

---

## 5. Bit Shifting

Shifting moves bits left or right by a number of positions.

| Operator | Direction | Effect                  |
| -------- | --------- | ----------------------- |
| `<< n`   | Left      | Multiplies by 2ⁿ        |
| `>> n`   | Right     | Divides by 2ⁿ (integer) |

Each color channel is 8 bits wide, so:

| Channel | Shift to pack | Shift to extract |
| ------- | ------------- | ---------------- |
| Red     | `<< 16`       | `>> 16`          |
| Green   | `<< 8`        | `>> 8`           |
| Blue    | none          | none             |

---

## 6. Quick Reference

```
AND (&)  →  Extract / mask bits     →  "take pieces out"
OR  (|)  →  Combine / merge bits    →  "glue pieces together"
<<       →  Shift left (pack)
>>       →  Shift right (unpack)
```

**The full round-trip:**

```
Pack:    color = (R << 16) | (G << 8) | B
Unpack:  R = (color & 0xFF0000) >> 16
         G = (color & 0x00FF00) >> 8
         B = (color & 0x0000FF)
```

---

## 7. How This Project Uses It

### Writing a pixel (`setPixel` in `src/Screen.cpp`)

This project uses **RGBA8888** format — 4 channels (with alpha for transparency):

```
┌────────┬────────┬────────┬────────┐
│  Red   │ Green  │  Blue  │ Alpha  │
│ 31-24  │ 23-16  │ 15-8   │  7-0   │
└────────┴────────┴────────┴────────┘
```

The code packs all 4 channels with OR:

```cpp
m_buffer1[y * m_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
//                            ^^^^^^^^    ^^^^^^^^    ^^^^^^^^    ^^^^
//                            Red to      Green to   Blue to     Alpha
//                            bits 31-24  bits 23-16 bits 15-8   = 0xFF (opaque)
```

### The bit-packing exercise (`examples/main_test.cpp`)

The practice file demonstrates both methods of packing ARGB:

```cpp
// Method 1: Bitwise OR (single expression)
uint32_t color = (static_cast<uint32_t>(alpha) << 24)
               | (static_cast<uint32_t>(red)   << 16)
               | (static_cast<uint32_t>(green) <<  8)
               |  static_cast<uint32_t>(blue);

// Method 2: Shift-and-add (step by step)
uint32_t color2 = 0;
color2 += alpha;     // put alpha in lowest byte
color2 <<= 8;        // shift left → make room for red
color2 += red;       // add red
color2 <<= 8;        // shift left → make room for green
color2 += green;     // add green
color2 <<= 8;        // shift left → make room for blue
color2 += blue;      // add blue
```

Both produce `0xFF123456`.

### Reading back a pixel

If you ever need to extract the red channel from a pixel in the buffer:

```cpp
uint32_t pixel = m_buffer1[y * m_width + x];

uint8_t red   = (pixel & 0xFF000000) >> 24;
uint8_t green = (pixel & 0x00FF0000) >> 16;
uint8_t blue  = (pixel & 0x0000FF00) >> 8;
uint8_t alpha = (pixel & 0x000000FF);
```

---

## 8. The `uint32_t` Trap (Undefined Behavior)

The course example uses:

```cpp
int color = (red << 16) | (green << 8) | blue;
```

This works when values are small (like `red = 0x12`). But with `alpha = 0xFF`:

```cpp
0xFF << 24 = 0xFF000000
```

This sets **bit 31**, which is the **sign bit** of a 32-bit `int`.
Shifting into the sign bit is **undefined behavior** in C++ — the compiler can do anything.

The fix: use an **unsigned** type:

```cpp
uint32_t color = (static_cast<uint32_t>(alpha) << 24)  // safe — unsigned
               | (static_cast<uint32_t>(red)   << 16)
               | ...
```

**Rule of thumb:** always use `uint32_t` (not `int`) when packing pixel colors.

---

## Where These Operations Are Used

| Domain            | Example                                |
| ----------------- | -------------------------------------- |
| Graphics / Games  | Packing/unpacking pixel colors (RGBA)  |
| Networking        | Building/parsing protocol headers      |
| Operating Systems | Permission flags, hardware registers   |
| Data Compression  | Encoding variable-length bit sequences |
| Cryptography      | Bit manipulation in hash functions     |
