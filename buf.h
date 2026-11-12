/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	(BUF) Buffer Routines
 *
 *	This file defines common structures, variables, and routines
 *	needed for interface to BUF.
 ******************************************************************************/
#ifndef	BUF_H
#define	BUF_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<common.h>


/*
 *	Common Data Definitions
 */

typedef	struct	buf_head_s	{
	volatile	s32	count;
	volatile	s32	size;
	volatile	s32	lock_bytes;
	volatile	s32	fill_bytes;
	volatile	s32	offset;
	volatile	s32	used;
	volatile	s32	next;
	volatile	pid_t	owner;
}	buf_head_t;

#define	proto_buf( name, buffers, type, filler )	\
typedef	struct	name##_s	{			\
	buf_head_t	head;				\
	volatile	uchar	lock[((buffers+7)>>3)];	\
	volatile	uchar	fill[ filler ];		\
	volatile	type	name[ buffers ];	\
}	name##_t

#define	proto_buf_mem( type, name, list )	\
	Struct( buf_head_t,	head,	buf_head_l	),\
	Xhuge(		lock,	"Buffer Lock Bit Array"	),\
	Uchar(		fill,	"Fill Bytes"		),\
	Struct_(4,	type,	name,	list		)

/*
 *	Common Data
 */

/*
 *	Common	Functions
 */

extern	int	Buffer_Push_Context(
	void *	start_of_buffer_struct
);

extern	int	Buffer_Pop_Context( void );

extern	void	Buffer_Initialize(
	void *	start_of_buffer_struct,
	u32 	size_of_one_buffer,
	u32	maximum_number_of_buffers,
	u32	fill_bytes
);

extern	void *	Buffer_Pointer(	s32	buffer_number	);
extern	s32	Buffer_Number(	void *	buffer_pointer	);

extern	s32	Buffer_Number_Allocate( void	);
extern	void *	Buffer_Pointer_Allocate( void	);

extern	s32	Buffer_Number_Free(	s32	buffer_number	);
extern	s32	Buffer_Pointer_Free(	void *	buffer_pointer	);

extern	s32	Buffer_Next_Number(	s32	micro_timeout	);
extern	void *	Buffer_Next_Pointer(	s32	micro_timeout	);

extern	int	Buffer_Set_Owner(	void	);
extern	int	Buffer_Set_Client(	void	);

extern	int	Buffer_Number_Signal_Owner(	s32	buffer_number	);
extern	int	Buffer_Pointer_Signal_Owner(	void *	buffer_pointer	);

extern	int	Buffer_Number_Signal_Client(	pid_t	client,	s32	buffer_number	);
extern	int	Buffer_Pointer_Signal_Client(	pid_t	client,	void *	buffer_pointer	);

extern	s32	Buffer_Test_For_Data(	int	FD,	s32	micro_timeout	);
extern	void	Buffer_Set_FD_Read_Function(	int	FD,	void	(*func)( int fd )	);

#if	defined( __cplusplus )
}
#endif

#endif
