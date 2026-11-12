# buf — fixed-size buffer pool

`buf` is a pool of same-sized buffers with a packed lock bitmap, allocate and
free by number or pointer, and optional owner/client signaling. Define a pool
with the `proto_buf` macro, initialize it, then push it as the current context
before allocate or free calls.

It builds with the [base](https://github.com/smythereen/base) unit make system, which handles C, C++, and Rust, including large mixed-language projects.

## Public interface (`buf.h`)

- `proto_buf(name, buffers, type, filler)` — define a pool type with a
  `buf_head_t` header, a lock bit array, optional fill bytes, and `buffers`
  elements of `type`.
- `Buffer_Initialize(start, maximum_number_of_buffers, size_of_one_buffer, fill_bytes)` — the implementation stores the second argument as `count` and the third as `size`, which is the reverse of the names in the `buf.h` prototype.
- `Buffer_Push_Context` / `Buffer_Pop_Context` — set or restore the current pool
- `Buffer_Pointer` / `Buffer_Number` — convert between a slot number and its address
- `Buffer_Number_Allocate` / `Buffer_Pointer_Allocate` and the matching `*_Free` calls
- `Buffer_Next_Number` / `Buffer_Next_Pointer` — wait up to a microsecond timeout
- `Buffer_Set_Owner` / `Buffer_Set_Client`
- `Buffer_Number_Signal_Owner` / `Buffer_Pointer_Signal_Owner` and the client variants
- `Buffer_Test_For_Data` / `Buffer_Set_FD_Read_Function`

`buflocal.h` is an implementation header, not an aggregate application API.

## Dependencies and build

Use this directory as `buf/` inside a [base](https://github.com/smythereen/base)
project, beside the root `rules.mak` and `targets.mak`:

```sh
export UNITS_CATALOG_URL=https://unit-catalog.dejavu-archive.workers.dev
./repo/repo.sh add buf
HOME=$PWD make -C buf header library
```

`UNIT.deps` requires `sys`, `mio`, and `now`. The makefile links only `-lsys`.
Sources include `sys` (`common.h`), `mio` (`mem_io.h` in `buf_l.c`), and `now`
(`now.h` in `buf_sig.c`). `atomic` enters through `sys`. Headers export to
`cmn/`, the archive to `lib/libbuf.a`. There is no test program in this unit.

## C and C++

This unit is usable from **C and C++**. Its public header already supplies
`extern "C"` guards. Compile the implementation `.c` files with GCC and link
that C library into your C++ application with G++; do not reinterpret the
legacy C source as C++.

```c
#include <buf.h>

proto_buf(pool, 8, u32, 0);

int main(void) {
    static pool_t pool;
    Buffer_Initialize(&pool, 8, sizeof(u32), 0); /* already pushes context */
    void *slot = Buffer_Pointer_Allocate();
    if (slot == 0)
        return 1;
    return Buffer_Pointer_Free(slot) != 0;
}
```

From a project root with headers in `cmn/` and built libraries in `lib/`:

```sh
gcc -std=gnu11 -Icmn smoke.c -Llib \
  -Wl,--start-group -lbuf -lsys -Wl,--end-group -o buf-smoke
```

A caller that uses the mio descriptor table in `buf_l.c` or the signaling
routines in `buf_sig.c` also needs the `mio` and `now` libraries.

## Verification

The Guardian tree already built this unit with plain `make` in the established
base layout. This publication is a source snapshot, not a fresh clean rebuild
in a separate tree. C++ linkage guards were inspected.

## Source and license

Published from the current Guardian unit directory as a clean snapshot, not
its private Git history. C/header/makefile bytes are preserved; build outputs
and backups are excluded. This guide and `UNIT.deps` are documentation
additions. No release tag is created by this publication. See `LICENSE` and
`LICENSES/` for the existing Apache-2.0 OR GPL-2.0-or-later licensing.
