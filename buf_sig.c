/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 ******************************************************************************/

#include	"buflocal.h"
#include	<signal.h>
#include	<now.h>

#define	BUF_MAX_FD	100

static	u64		Buf_Next;
static	pid_t	*	Buf_Sender;
static	uchar		writer;
static	uchar		reader;
static	s32		data[ 256 ];
static	pid_t		client[ 256 ];

void	Buffer_SigHand( int sig, siginfo_t * info, void * what )
{
	if( info == NULL ) return;
	data[ ++writer ] = (u32)info->si_int;
	client[ writer ] = (pid_t)info->si_pid;
	if( writer == reader ) reader++;

#ifdef	JUNK
#define	TRC( v ) Trace( 0,\
			"%10s:%08X adr:%08X size:%u offset: %u",\
			#v,\
			info->v,\
			&info->v,\
			sizeof( info->v ),\
			(u32)&info->v - (u32)info )
	TRC(	si_signo	);
	TRC(	si_errno	);
	TRC(	si_code		);
	TRC(	si_pid		);
	TRC(	si_uid		);
	TRC(	si_status	);
	TRC(	si_utime	);
	TRC(	si_stime	);
//	TRC(	si_value	);
	TRC(	si_int		);
//	TRC(	si_ptr		);
//	TRC(	si_addr		);
//	TRC(	si_band		);
//	TRC(	si_fd		);
#endif
	return;
}

static	struct	sigaction	buf_sig_old;

static	struct	sigaction	buf_sig_new	= {
			(__sighandler_t)Buffer_SigHand,
			0,
			SA_SIGINFO | SA_RESTART
};

int	Buffer_Set_Owner( void )
{
	buf_head_t *	h;

	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}

	buf_sig_new.sa_flags = ( SA_SIGINFO | SA_RESTART );
	if( sigaction( BUF_OWNER_SIGNAL, &buf_sig_new, &buf_sig_old ) < 0 ) {
		Trace(	0,
			"Cannot Install Buffer OwnerSignal Handler - %s",
			strerror( errno )
		);
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;
	h->owner = getpid( );
#ifdef	JUNK
	Trace(	0,
		"Buffer_Set_Owner( ) pid=%d",
		h->owner
	);
#endif
	return( 1 );
}

int	Buffer_Set_Client( void )
{
	buf_head_t *	h;
	int		i;
	pid_t		me;

	buf_sig_new.sa_flags = ( SA_SIGINFO | SA_RESTART );
	if( sigaction( BUF_CLIENT_SIGNAL, &buf_sig_new, &buf_sig_old ) < 0 ) {
		Trace(	0,
			"Cannot Install Buffer Client Signal Handler - %s",
			strerror( errno )
		);
		return( -1 );
	}
#ifdef	JUNK
	h = (buf_head_t *)*Buffer_Context;
	me = getpid( );
	Trace(	0,
		"Buffer_Set_Client( ) pid=%d",
		me
	);
	for( i = 0; i < BUF_MAX_PIDS; i++ ) {
		if( h->client[ i ] == me ) return( i );
	}
	for( i = 0; i < BUF_MAX_PIDS; i++ ) {
		if( h->client[ i ] != NULL ) continue;
		h->client[ i ] = me;
		return( i );
	}
	return( -1 );
#endif
	return( 1 );
}

static	fd_set	buf_read_fds;

static	void (*buf_fd_read_funcs[ BUF_MAX_FD ])( int fd );

void	Buffer_Set_FD_Read_Function( int FD, void (*func)( int fd ) )
{
	buf_fd_read_funcs[ FD ] = func;
	if( func != NULL ) {
		FD_SET( FD, &buf_read_fds );
	} else {
		FD_CLR( FD, &buf_read_fds );
	}
}

s32	Buffer_Test_For_Data( int FD, s32 micro_timeout )
{
	int	i;
	struct	timeval	to;
	s32	timeout;

	fd_set	fds;
	memcpy( &fds, &buf_read_fds, sizeof( fds ) );

	if( FD >= 0 ) FD_SET( FD, &fds );

	if( Buf_Next == 0 ) Buf_Next = Micro_Now( ) + micro_timeout;

	while( reader == writer ) {

		timeout = (s32)( (s64)( (s64)Buf_Next - (s64)Micro_Now( ) ) );
		if( timeout <= 0 ) timeout = 1;
		to.tv_sec  = ( timeout / 1000000 );
		to.tv_usec = ( timeout % 1000000 );
//		Trace( 0, "timeout: %u microseconds", timeout );

		switch(
			select(
				(size_t)( BUF_MAX_FD + 1 ),
				(fd_set *)&fds,
				0,
				0,
				( timeout >= 0 ? &to : 0 )
			)
		) {
		case  0:	errno = ETIME;
				Buf_Next = Micro_Now( ) + micro_timeout;
				return( -2 );

		case -1:	switch( errno ) {
				case ERESTART:
				case EAGAIN:
				case EINTR:	break;
				default:	return( -1 );
				}
		default:	for( i = 0; i < BUF_MAX_FD ; i++ ) {
					if( !FD_ISSET( i, &fds ) ) continue;
					if( buf_fd_read_funcs[ i ] != NULL ) (buf_fd_read_funcs[ i ])( i );
				}
				if( FD >= 0 && FD_ISSET( FD, &fds ) ) {
					errno = EEXIST;
					Buf_Sender = NULL;
					return( -3 );
				}
		}
	}
	Buf_Next = Micro_Now( ) + micro_timeout;
	Buf_Sender = &client[ ++reader ];
	return( data[ reader ] );
}

s32	Buffer_Next_Number( s32 micro_timeout )
{
	struct	timeval	to;
	s32		timeout;

	if( reader == writer ) {
		if( micro_timeout <= 0 ) return( -2 );	// Instant Timeout
		if( Buf_Next == 0 ) Buf_Next = Micro_Now( ) + micro_timeout;

		while( reader == writer ) {

			timeout = (s32)( (s64)( (s64)Buf_Next - (s64)Micro_Now( ) ) );
			if( timeout <= 0 ) timeout = 1;
			to.tv_sec  = ( timeout / 1000000 );
			to.tv_usec = ( timeout % 1000000 );
//			Trace( 0, "timeout: %u microseconds", timeout );

			switch( select( 0, 0, 0, 0, ( timeout >= 0 ? &to : 0 ) ) ) {
			case  0:
				errno = ETIME;
				Buf_Next = Micro_Now( ) + micro_timeout;
				return( -2 );

			case -1:
				switch( errno ) {
				case ERESTART:
				case EAGAIN:
				case EINTR:	break;
				default:	return( -1 );
				}
			}
		}
		Buf_Next = Micro_Now( ) + micro_timeout;
	}
	Buf_Sender = &client[ ++reader ];
	return( data[ reader ] );
}

void *	Buffer_Next_Pointer( s32 micro_timeout )
{
	return( Buffer_Pointer( Buffer_Next_Number( micro_timeout ) ) );
}

int	Buffer_Number_Signal_Owner( s32 info )
{
	buf_head_t *	h;

	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;
	if( h->owner == 0 ) {
		Trace( 0, "Buffer_Number_Signal_Owner: Buffer_Initialize by owner has NOT been Called!" );
		return( -1 );
	}
	if( sigqueue( h->owner, BUF_OWNER_SIGNAL, (union sigval)( (int)info ) ) < 0 ) {
		Trace(	0,
			"Buffer_Number_Signal_Owner(num:%u): sigqueue Failed - %s",
			info,
			strerror( errno )
		);
		return( -1 );
	}
	return( 1 );
}

int	Buffer_Pointer_Signal_Owner( void * ptr )
{
	return( Buffer_Number_Signal_Owner( Buffer_Number( ptr ) ) );
}

int	Buffer_Number_Signal_Client( pid_t client, s32 info )
{
	buf_head_t *	h;
	int		i;

	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;
#ifdef	JUNK
	if( h->owner != getpid( ) ) {
		Trace( 0, "Buffer_Number_Signal_Client: Non-Owner Cannot Signal Client!" );
		return( -1 );
	}
#endif
	if( client <= 1 ) {
		Trace( 0, "Buffer_Number_Signal_Client: Client %d Not Found!", client );
		return( -1 );
	}
	if( sigqueue( client, BUF_CLIENT_SIGNAL, (union sigval)( (int)info ) ) >= 0 ) {
		return( 1 );
	}
	Trace(	0,
		"Buffer_Number_Signal_Client(client:%u,num:%u): sigqueue Failed - %s",
		client,
		info,
		strerror( errno )
	);
	return( -1 );
}

int	Buffer_Pointer_Signal_Client( pid_t client, void * ptr )
{
	return( Buffer_Number_Signal_Client( client, Buffer_Number( ptr ) ) );
}
