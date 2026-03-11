# Two's Complement — How Computers Store Negative Numbers

> Based on **Lesson 18** of the C++ Tutorial for Complete Beginners
> by John Purcell

---

## Table of Contents

1. [The Problem: How Do You Store Negative Numbers?](#1-the-problem-how-do-you-store-negative-numbers)
2. [Two's Complement — The Solution](#2-twos-complement--the-solution)
3. [Creating a Negative Number (Flip + Add 1)](#3-creating-a-negative-number-flip--add-1)
4. [Overflow & Wraparound](#4-overflow--wraparound)
5. [Sign Extension](#5-sign-extension)
6. [Arithmetic Just Works](#6-arithmetic-just-works)
7. [Bitwise Operations on Signed Numbers](#7-bitwise-operations-on-signed-numbers)
8. [Type Conversions — Watch Out](#8-type-conversions--watch-out)
9. [Integer Ranges](#9-integer-ranges)
10. [Practical Examples](#10-practical-examples)
11. [Quick Reference](#11-quick-reference)

---

## 1. The Problem: How Do You Store Negative Numbers?

Binary only has `0` and `1` — there's no minus sign. So how does a computer
know that a number is negative?

**Old approach — Sign and Magnitude:**

Use the leftmost bit (MSB) as a sign flag:

```
+5 = 0 0000101    (MSB = 0 → positive)
-5 = 1 0000101    (MSB = 1 → negative)
```

Problem: this gives you **two zeros** (`+0` and `-0`) and makes arithmetic
circuits more complex.

**Modern approach — Two's Complement:**

A single representation of zero, and addition/subtraction use the same
hardware circuit. Every modern CPU uses this.

---

## 2. Two's Complement — The Solution

In an 8-bit signed integer (`char`):

- The **MSB** (bit 7) is the sign bit: `0` = positive, `1` = negative
- Positive numbers look the same as unsigned
- Negative numbers are encoded with a specific pattern

```
Positive numbers (MSB = 0):
  0   = 00000000
  1   = 00000001
  42  = 00101010
  127 = 01111111   ← maximum

Negative numbers (MSB = 1):
  -1   = 11111111
  -42  = 11010110
  -127 = 10000001
  -128 = 10000000   ← minimum
```

The range of an 8-bit signed integer: **-128 to 127**.

---

## 3. Creating a Negative Number (Flip + Add 1)

To find `-X` from `X` in two's complement:

1. **Flip** all the bits (0→1, 1→0) — this is called the _one's complement_
2. **Add 1** to the result

### Example: Find -5 from 5

```
Step 0 — Start with 5:
  00000101

Step 1 — Flip all bits:
  11111010     (one's complement)

Step 2 — Add 1:
  11111010
+        1
──────────
  11111011     = -5 in two's complement ✓
```

### Verify: converting -5 back to 5

The same process works in reverse — flip and add 1:

```
Start:   11111011   (-5)
Flip:    00000100
Add 1:   00000101   = 5 ✓
```

### In C++

```cpp
char value = 5;                     // 00000101

// Method 1: Direct negation
char negated = -value;              // -5

// Method 2: Manual two's complement
unsigned char flipped = ~value;     // 11111010 (flip bits)
unsigned char result  = flipped + 1; // 11111011 = -5
```

---

## 4. Overflow & Wraparound

When you go past the maximum or minimum value, it **wraps around**:

### Overflow (past maximum)

```
  127  =  01111111
+ 1       00000001
──────────────────
  -128 =  10000000   ← wrapped to minimum!
```

```cpp
char value = 127;
value++;              // value is now -128
```

### Underflow (past minimum)

```
  -128 =  10000000
- 1       00000001
──────────────────
  127  =  01111111   ← wrapped to maximum!
```

```cpp
char value = -128;
value--;              // value is now 127
```

Think of it like a clock: after 12 comes 1 again. The number circle for
an 8-bit signed integer looks like:

```
         0
     -1     1
   -2         2
  ...          ...
 -127          127
     -128
```

---

## 5. Sign Extension

When you assign a small type to a larger type, C++ **copies the sign bit**
into all the new higher-order bits. This preserves the value:

```
char -42  (8-bit):   11010110
int  -42 (32-bit):   11111111 11111111 11111111 11010110
                     ^^^^^^^^ ^^^^^^^^ ^^^^^^^^
                     sign bit copied to all new bits
```

The value stays `-42` — the extra `1`s are just padding to keep the same
negative meaning in a wider type.

```cpp
char small = -42;
int  large = small;   // sign extension happens automatically

// small: 11010110
// large: 11111111111111111111111111010110
// Both represent -42
```

---

## 6. Arithmetic Just Works

The beauty of two's complement: **addition works the same** whether the
numbers are positive, negative, or mixed. The CPU doesn't need separate
circuits.

### Positive + Positive

```
   50  =  00110010
+  30  =  00011110
──────────────────
   80  =  01010000  ✓
```

### Negative + Positive

```
  -20  =  11101100
+  50  =  00110010
──────────────────
   30  =  00011110  ✓  (carry bit discarded)
```

### Negative + Negative

```
  -30  =  11100010
+ -40  =  11011000
──────────────────
  -70  =  10111010  ✓  (carry bit discarded)
```

This is **why** computers use two's complement — the hardware for addition
and subtraction is the same regardless of sign.

---

## 7. Bitwise Operations on Signed Numbers

Bitwise operators work on the raw bits. Two's complement affects how you
**interpret** the result.

### NOT (`~`) — Flip all bits

```cpp
char value = 42;           // 00101010
char flipped = ~value;     // 11010101 = -43
```

Note: `~X` always equals `-(X+1)` in two's complement.

### Left Shift (`<<`) — Multiply by 2

```cpp
char value = 10;           // 00001010 = 10
value = value << 1;        // 00010100 = 20
```

### Right Shift (`>>`) — Divide by 2 (preserves sign)

For signed types, the compiler performs an **arithmetic** right shift —
the sign bit is copied into the vacated positions:

```
Original:   -16  =  11110000
After >> 2:  -4  =  11111100
                     ^^
                     sign bit extended (not zeros)
```

```cpp
char value = -16;          // 11110000
value = value >> 2;        // 11111100 = -4
```

---

## 8. Type Conversions — Watch Out

The same raw bits have **different meanings** depending on the type:

### Signed → Larger Signed (sign extension)

```cpp
char c = -1;               // 11111111
int  i = c;                // 11111111 11111111 11111111 11111111
                           // Still -1 ✓ (sign bit extended)
```

### Unsigned → Larger Type (zero extension)

```cpp
unsigned char uc = 255;    // 11111111
int i = uc;                // 00000000 00000000 00000000 11111111
                           // = 255 ✓ (zeros added, not sign bit)
```

### Same bits, different interpretation

```cpp
char          signed_val   = -1;    // 11111111 → interpreted as -1
unsigned char unsigned_val = -1;    // 11111111 → interpreted as 255
```

The binary is **identical** (`11111111`). The type tells the compiler how
to read it.

---

## 9. Integer Ranges

Two's complement determines the range: $-2^{n-1}$ to $2^{n-1} - 1$

| Type        | Bits | Minimum                    | Maximum                   |
| ----------- | ---- | -------------------------- | ------------------------- |
| `char`      | 8    | -128                       | 127                       |
| `short`     | 16   | -32,768                    | 32,767                    |
| `int`       | 32   | -2,147,483,648             | 2,147,483,647             |
| `long long` | 64   | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807 |

Why is the minimum's magnitude one larger than the maximum?

```
8-bit example:
  Maximum:  01111111  =  127   (7 bits for value)
  Minimum:  10000000  = -128   (the pattern left after 127 positives + zero)
```

There are 256 possible patterns in 8 bits: 1 for zero, 127 positive, 128 negative.

---

## 10. Practical Examples

### Check if a number is negative

```cpp
// Simple way
if (num < 0) { /* negative */ }

// Bit-level way (32-bit int): check if MSB is set
if (num & 0x80000000) { /* negative */ }
```

### Absolute value

```cpp
int absValue = (value < 0) ? -value : value;
```

Under the hood, `-value` does the flip-and-add-1 operation.

### Negate a number

```cpp
short sh = 100;
short negated = -sh;

// 100:   0000000001100100
// -100:  1111111110011100   (flip + add 1)
```

### Detect overflow

```cpp
int a = INT_MAX;          // 2,147,483,647
int b = a + 1;            // -2,147,483,648 (wrapped around!)
```

> **Warning:** Signed integer overflow is **undefined behavior** in C++.
> The compiler is allowed to assume it never happens and may optimize
> your overflow checks away. Use compiler flags like `-ftrapv` to catch
> these bugs during development.

---

## 11. Quick Reference

```
Two's complement encoding:
  Positive:  MSB = 0, rest = value in binary
  Negative:  Flip all bits of |value|, add 1
  Zero:      All bits = 0 (single representation)

Convert X → -X:
  1. Flip all bits  (~X)
  2. Add 1          (~X + 1)

Useful identities:
  ~X     = -(X + 1)
  -X     = ~X + 1
  X + ~X = -1  (all bits set)

Range of n-bit signed integer:
  Minimum: -2^(n-1)
  Maximum:  2^(n-1) - 1

Overflow behavior:
  MAX + 1 → MIN  (wraps around)
  MIN - 1 → MAX  (wraps around)

Type conversion:
  Signed → larger:    sign extension (copies MSB)
  Unsigned → larger:  zero extension (adds zeros)
```

### Connection to the project

In the particle fire simulation, pixels are stored as `uint32_t` — **unsigned**.
This avoids all two's complement sign issues when bit-shifting:

```cpp
// src/Screen.cpp — no sign bit to worry about
m_buffer1[i] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
```

If this were a signed `int`, shifting `0xFF` into bit 31 would set the sign
bit — undefined behavior (see [bitwise-operations.md](bitwise-operations.md#8-the-uint32_t-trap-undefined-behavior)).
Using `uint32_t` sidesteps the problem entirely.
