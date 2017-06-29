#ifndef _TYPES_H
#define _TYPES_H


typedef unsigned char   BYTE;
typedef unsigned char   UINT8;
typedef signed short    INT16;
typedef unsigned short  UINT16;
typedef signed int      INT32;
typedef unsigned int    UINT32;
typedef signed long     INT64;
typedef unsigned long   UINT64;
typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
typedef enum _BIT { CLEAR = 0, SET } BIT;

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;


/* this is a special 64bit data type that is 8-byte aligned */
#define aligned_u64 unsigned long __attribute__((aligned(8)))

/**
 * The type used for indexing onto a disc or disc partition.
 *
 * Linux always considers sectors to be 512 bytes long independently
 * of the devices real block size.
 */
typedef unsigned long sector_t;

/*
 * The type of the inode's block count.
 * The type of an index into the pagecache.  Use a #define so asm/types.h
 * can override it.
 */
#ifndef pgoff_t
#define pgoff_t unsigned long
#endif

#endif /* _LINUX_TYPES_H */
