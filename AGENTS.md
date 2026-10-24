# AGENTS.md

## Project Overview

This is a boilerplate for building FreeBSD kernel modules out-of-tree (outside `/usr/src`). It follows the conventions used by in-tree modules under `/usr/src/sys/modules`.

## Project Structure

```
.
├── Makefile          # Build file using <bsd.kmod.mk>
├── example.c         # Sample kernel module source
├── example.h         # Sample header (shared declarations)
├── .gitignore        # Git ignore rules
├── README.md         # User documentation
└── AGENTS.md         # This file (agent guidance)
```

## Build System

The Makefile follows the standard FreeBSD kmod build pattern:

1. Sets `SYSDIR` to the kernel source `sys/` directory (default: `/usr/src/sys`)
2. Includes `${SYSDIR}/conf/kern.opts.mk` for `KERN_OPTS` and `opt_*.h` handling
3. Sets `KMOD`, `SRCS`, and optional variables like `EXPORT_SYMS`
4. Includes `<bsd.kmod.mk>` from `/usr/share/mk`, which pulls in `/usr/src/sys/conf/kmod.mk`

### Key Make Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `KMOD` | `example` | Kernel module name |
| `SRCS` | `example.c` | Source files to compile |
| `SYSDIR` | `/usr/src/sys` | Kernel source `sys/` directory |
| `SRCTOP` | `${SYSDIR}/..` | Kernel source root |
| `KERNBUILDDIR` | unset | Kernel build dir for `opt_*.h` headers |
| `DESTDIR` | `/` | Install target root |
| `KMODDIR` | `/boot/kernel` | Install directory for `.ko` |
| `EXPORT_SYMS` | `NO` | `YES`, a filename, or `NO` |

### Make Targets

- `make` — Build the `.ko` file
- `make install` — Install to `${DESTDIR}${KMODDIR}/` (as root)
- `make load` — `kldload` the module
- `make unload` — `kldunload` the module
- `make reload` — Unload then load
- `make clean` — Remove build artifacts

## Code Conventions

- **Style**: FreeBSD Kernel Normal Form (KNF) — tabs for indentation, K&R brace style
- **Unused parameters**: Mark with `__unused` to avoid `-Werror` failures (the build compiles with `-Werror`)
- **Module declaration**: Use `DECLARE_MODULE(name, mod, subsys, order)` — the first arg must match `KMOD`
- **License header**: BSD 2-Clause with SPDX identifier

## Important Files Reference

- `/usr/share/mk/bsd.kmod.mk` — Top-level include (10 lines), pulls in `${SYSDIR}/conf/kmod.mk`
- `/usr/src/sys/conf/kmod.mk` — Actual build rules (compiler flags, linker flags, targets)
- `/usr/src/sys/conf/kern.opts.mk` — Kernel build options (`KERN_OPTS`, `MK_*` variables)
- `/usr/share/mk/bsd.sysdir.mk` — Auto-detects `SYSDIR` from standard locations

## In-Tree Module Examples to Reference

- `/usr/src/sys/modules/ktest/ktest_example/Makefile` — Simple single-file module
- `/usr/src/sys/modules/if_tuntap/Makefile` — Module with `opt_*.h` headers
- `/usr/src/sys/modules/linuxkpi/Makefile` — Large module with `EXPORT_SYMS=YES`

## When Adding New Source Files

1. Add the `.c` file to `SRCS` in the Makefile
2. If it needs `opt_*.h` headers, add them to `SRCS` too
3. If the module should export symbols, set `EXPORT_SYMS=YES` or provide a symbol list file
4. For architecture-specific sources, use `.if ${MACHINE_CPUARCH} == ...` conditionals in the Makefile
