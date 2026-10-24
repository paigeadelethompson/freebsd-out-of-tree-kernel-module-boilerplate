# AGENTS.md

## Project Overview

This is a boilerplate for building FreeBSD kernel modules out-of-tree (outside `/usr/src`). It follows the conventions used by in-tree modules under `/usr/src/sys/modules`.

## Project Structure

```
.
├── Makefile          # Build file using <bsd.kmod.mk>
├── example.c         # Sample kernel module source
├── example.h         # Sample header (shared declarations)
├── .clang-format     # clang-format config (FreeBSD KNF style)
├── .gitignore        # Git ignore rules
├── LICENSE           # BSD 2-Clause license
├── README.md         # User documentation
├── AGENTS.md         # This file (agent guidance)
└── tests/            # Test kernel modules
    ├── Makefile      # Test directory makefile (bsd.subdir.mk)
    └── module/
        ├── Makefile  # Test makefile (bsd.kmod.mk)
        └── test_example.c  # Test kernel module source
```

## Build System

The Makefile follows the standard FreeBSD kmod build pattern:

1. Sets `SYSDIR` to the kernel source `sys/` directory (default: `/usr/src/sys`)
2. Includes `${SYSDIR}/conf/kern.opts.mk` for `KERN_OPTS` and `opt_*.h` handling
3. Sets `KMOD`, `SRCS`, and optional variables like `EXPORT_SYMS`
4. Includes `<bsd.kmod.mk>` from `/usr/share/mk`, which pulls in `/usr/src/sys/conf/kmod.mk`

### The Include Chain

```
bsd.kmod.mk  (in /usr/share/mk)
  └── bsd.sysdir.mk    — auto-detects SYSDIR (checks /sys, /usr/src/sys, relative paths)
  └── ${SYSDIR}/conf/kmod.mk   — the real build rules from your kernel source tree
```

The actual compiler flags, linker flags, and build targets all live in `/usr/src/sys/conf/kmod.mk`, NOT in `/usr/share/mk`. The `/usr/share/mk/bsd.kmod.mk` is just a 10-line dispatcher.

### Key Make Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `KMOD` | `example` | Kernel module name (must match DECLARE_MODULE first arg) |
| `SRCS` | `example.c` | Source files to compile (add `.h` headers too) |
| `SYSDIR` | `/usr/src/sys` | Kernel source `sys/` directory |
| `SRCTOP` | `${SYSDIR}/..` | Kernel source root |
| `KERNBUILDDIR` | unset | Kernel build dir for `opt_*.h` headers (if building against pre-built kernel) |
| `DESTDIR` | `/` | Install target root |
| `KMODDIR` | `/boot/kernel` | Install directory for `.ko` |
| `EXPORT_SYMS` | `NO` | `YES` (export all), a filename (export listed symbols), or `NO` |
| `FIRMWS` | unset | Firmware images in format `filename:shortname:version` |

### Make Targets

- `make` — Build the `.ko` file
- `make install` — Install to `${DESTDIR}${KMODDIR}/` (as root), runs `kldxref`
- `make load` — `kldload` the module
- `make unload` — `kldunload` the module (if loaded)
- `make reload` — Unload then load
- `make clean` — Remove build artifacts (`.o`, `.ko`, `export_syms`, arch symlinks, `opt_*.h`)

## How to Build Different Types of Kernel Modules

### 1. Simple Single-File Module

Pattern from `/usr/src/sys/modules/ktest/ktest_example/Makefile`:

```make
KMOD=	ktest_example
SRCS=	ktest_example.c
.include <bsd.kmod.mk>
```

The `example.c` in this boilerplate follows this pattern. It has a `modevent` function handling `MOD_LOAD`, `MOD_UNLOAD`, `MOD_SHUTDOWN`, and uses `DECLARE_MODULE` to register.

### 2. Module with `opt_*.h` Headers

Pattern from `/usr/src/sys/modules/if_tuntap/Makefile`:

```make
KMOD=	if_tuntap
SRCS=	if_tuntap.c opt_inet.h opt_inet6.h
.include <bsd.kmod.mk>
```

Add `opt_*.h` files to `SRCS`. When `KERNBUILDDIR` is set, they're symlinked from the kernel build dir; when not set, `config.mk` generates them (e.g., `opt_inet.h` defines `#define INET 1`). The build system auto-generates common ones:
- `opt_global.h` — defines SMP, MAC, VIMAGE, and options from DEFAULTS
- `opt_inet.h` — `#define INET 1`, `#define TCP_OFFLOAD 1`
- `opt_inet6.h` — `#define INET6 1`
- `opt_bpf.h`, `opt_printf.h`, `opt_scsi.h`, `opt_wlan.h`, etc.

### 3. Module Exporting Symbols

Pattern from `/usr/src/sys/modules/linuxkpi/Makefile`:

```make
EXPORT_SYMS=	YES
```

Set `EXPORT_SYMS=YES` to export all symbols from the module. Other modules can then reference them via `MODULE_DEPEND`. Alternatively, set `EXPORT_SYMS` to a filename containing a list of symbols to export.

### 4. Architecture-Specific Sources

Use `.if` conditionals in the Makefile:

```make
.if ${MACHINE_CPUARCH} == "aarch64" || ${MACHINE_CPUARCH} == "amd64" || \
    ${MACHINE_CPUARCH} == "i386"
SRCS+=	opt_acpi.h acpi_if.h linux_acpi.c
.endif
```

The build system also auto-creates architecture symlinks (`machine`, `x86`, `amd64`, `i386`) pointing to the appropriate kernel include directories.

### 5. Module with Firmware

Set `FIRMWS` with format `filename:shortname:version`. The build generates a firmware stub `.c` file and compiles firmware blobs from `${SYSDIR}/kern/firmw.S`.

### 6. Module Using Generated Interface Headers (`_if.m`)

The build system handles `_if.m` files via `makeobjops.awk`:
- `.m.c` → generates C source from interface definition
- `.m.h` → generates header from interface definition

Also handles: `vnode_if.c/h`, `usbdevs.h`, `bhnd_nvram_map.h`, `acpi_quirks.h`, `assym.inc`, `offset.inc`.

### 7. Module Tied to a Specific Kernel Config

Set `MODULE_TIED` to build against a specific kernel configuration. This adds `offset.inc` generation and ties the module to the kernel's option set.

## Important Files Reference

| File | Purpose |
|------|---------|
| `/usr/share/mk/bsd.kmod.mk` | Top-level include (10 lines): includes `bsd.sysdir.mk` then `${SYSDIR}/conf/kmod.mk` |
| `/usr/share/mk/bsd.sysdir.mk` | Auto-detects `SYSDIR` from standard locations; errors if kernel tree not found |
| `/usr/src/sys/conf/kmod.mk` | **The real build rules**: compiler flags (`-D_KERNEL -DKLD_MODULE -nostdinc -Werror`), linker flags, `.ko` link step, `install`/`load`/`unload`/`reload` targets, `EXPORT_SYMS` handling, arch symlinks |
| `/usr/src/sys/conf/kern.opts.mk` | Kernel build options: `KERN_OPTS`, `MK_*` variables, default yes/no options |
| `/usr/src/sys/conf/kmod.opts.mk` | Includes `kern.opts.mk`, `bsd.compiler.mk`, `config.mk` — include this before `bsd.kmod.mk` for out-of-tree builds |
| `/usr/src/sys/conf/config.mk` | Generates `opt_*.h` files, defines per-arch `KERN_OPTS` (e.g., `DEV_PCI` for i386/amd64/powerpc) |

## In-Tree Module Examples to Reference

- `/usr/src/sys/modules/ktest/ktest_example/Makefile` — Simple single-file module
- `/usr/src/sys/modules/if_tuntap/Makefile` — Module with `opt_inet.h opt_inet6.h` in SRCS
- `/usr/src/sys/modules/linuxkpi/Makefile` — Large module with `EXPORT_SYMS=YES`, arch-specific sources, many source files

## When Adding New Source Files

1. Add the `.c` file to `SRCS` in the Makefile
2. If it needs `opt_*.h` headers, add them to `SRCS` too (they'll be auto-generated or symlinked)
3. If the module should export symbols, set `EXPORT_SYMS=YES` or provide a symbol list file
4. For architecture-specific sources, use `.if ${MACHINE_CPUARCH} == ...` conditionals
5. If the module needs firmware, set `FIRMWS` with `filename:shortname:version` format
6. Use `__unused` on unused function parameters — the build compiles with `-Werror`
7. The first argument to `DECLARE_MODULE` must match your `KMOD` name

## Code Conventions

- **Style**: FreeBSD Kernel Normal Form (KNF) — tabs for indentation, K&R brace style
- **Unused parameters**: Mark with `__unused` to avoid `-Werror` failures
- **Module declaration**: Use `DECLARE_MODULE(name, mod, subsys, order)` — the first arg must match `KMOD`
- **License header**: BSD 2-Clause with SPDX identifier
