/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Buffer Format Data
 ******************************************************************************/

#include	"buflocal.h"
#include	<mem_io.h>

#define	STRUCT	buf_head_t
	Mitem		buf_head_l[ ] =	{
	Ulong(	count,		"Number Of Buffers"			),
	Ulong(	size,		"Single Buffer Size"			),
	Ulong(	lock_bytes,	"Number Of Lock Bytes"			),
	Ulong(	fill_bytes,	"Number Of Filler Bytes"		),
	Ulong(	offset,		"Offset From Head To Buffers In Bytes"	),
	Ulong(	used,		"Number Of Used Buffers"		),
	Ulong(	next,		"Next Buffer Number To Process"		),
	Ulong(	owner,		"PID Of Shared Memory Segment Owner"	),
	{ END_OF_LIST }
};
#undef	STRUCT

