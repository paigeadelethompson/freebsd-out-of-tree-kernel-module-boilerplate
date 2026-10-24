/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include "example.h"

/*
 * Module event handler. Called when the module is loaded, unloaded, or
 * during shutdown.
 */
static int
example_modevent(module_t mod __unused, int type, void *data __unused)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		uprintf("example: module loaded\n");
		break;
	case MOD_UNLOAD:
		uprintf("example: module unloaded\n");
		break;
	case MOD_SHUTDOWN:
		uprintf("example: system shutting down\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

/*
 * Module declaration. The first argument ("example") must match KMOD in
 * the Makefile. The second argument is the modevent handler, and the
 * third is an optional per-module data pointer (NULL here).
 */
static moduledata_t example_mod = {
	"example",
	example_modevent,
	NULL
};

DECLARE_MODULE(example, example_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
MODULE_VERSION(example, 1);
MODULE_DEPEND(example, /* no dependencies */, 1, 1, 1);
