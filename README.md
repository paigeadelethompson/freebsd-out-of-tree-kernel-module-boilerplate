# FreeBSD Out-of-Tree Kernel Module Boilerplate

A boilerplate for building FreeBSD kernel modules out-of-tree, following
the conventions used by in-tree modules under /usr/src/sys/modules.

## Project Structure

```
.
├── Makefile          # Build file using bsd.kmod.mk
├── example.c         # Sample kernel module source
├── example.h         # Sample header (shared declarations)
├── .gitignore        # Git ignore rules
└── README.md         # This file
```

## Prerequisites

- FreeBSD source tree at `/usr/src` (or set `SYSDIR` to another location)
- Build tools: `make`, `clang`/`gcc`, `ld`

## Building

```sh
# Build the module (.ko file)
make

# Install to /boot/kernel (as root)
make install

# Load the module
make load

# Unload the module
make unload

# Reload (unload then load)
make reload

# Clean build artifacts
make clean
```

## Variables You Can Override

| Variable | Description | Default |
|----------|-------------|---------|
| `KMOD` | Kernel module name | `example` |
| `SRCS` | Source files to compile | `example.c` |
| `SYSDIR` | Path to kernel `sys/` directory | `/usr/src/sys` |
| `SRCTOP` | Path to kernel source root | `${SYSDIR}/..` |
| `KERNBUILDDIR` | Path to kernel build dir (for `opt_*.h`) | Not set |
| `DESTDIR` | Install target root directory | `/` |
| `KMODDIR` | Install directory for `.ko` | `/boot/kernel` |
| `EXPORT_SYMS` | `YES`, a filename, or `NO` | `NO` |

Example:
```sh
make KMOD=mydrv SRCS="mydrv.c helper.c" SYSDIR=/usr/src/sys
```

## How It Works

1. `SYSDIR` points at your kernel source `sys/` directory.
2. `kern.opts.mk` is included so `KERN_OPTS` and `opt_*.h` handling work
   the same as in-tree builds.
3. `KMOD`, `SRCS`, and `EXPORT_SYMS` are set.
4. `<bsd.kmod.mk>` (from `/usr/share/mk`) pulls in
   `/usr/src/sys/conf/kmod.mk` and provides all compiler/linker flags,
   the `.ko` link step, and `install`/`load`/`unload`/`reload` targets.

For details, see `/usr/share/mk/bsd.kmod.mk` and
`/usr/src/sys/conf/kmod.mk`.

## Writing Your Module

Replace `example.c` with your driver. A minimal module:

```c
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int
example_modevent(module_t mod __unused, int type, void *data __unused)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		uprintf("example module loaded\n");
		break;
	case MOD_UNLOAD:
		uprintf("example module unloaded\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

static moduledata_t example_mod = {
	"example",
	example_modevent,
	NULL
};

DECLARE_MODULE(example, example_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
MODULE_VERSION(example, 1);
```

The first argument to `DECLARE_MODULE` must match your `KMOD` name.
Use `__unused` on unused parameters — the build uses `-Werror`.
