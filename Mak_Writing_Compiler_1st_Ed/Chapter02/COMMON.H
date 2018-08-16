/****************************************************************/
/*                                                              */
/*      C O M M O N   R O U T I N E S   (Header)                */
/*                                                              */
/*      FILE:       common.h                                    */
/*                                                              */
/*      MODULE:     common                                      */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#ifndef common_h
#define common_h

#define FORM_FEED_CHAR          '\f'

#define MAX_FILE_NAME_LENGTH    32
#define MAX_SOURCE_LINE_LENGTH  256
#define MAX_PRINT_LINE_LENGTH   80
#define MAX_LINES_PER_PAGE	50
#define DATE_STRING_LENGTH      26
#define MAX_TOKEN_STRING_LENGTH MAX_SOURCE_LINE_LENGTH
#define MAX_CODE_BUFFER_SIZE    4096
#define MAX_NESTING_LEVEL       16

typedef enum {
    FALSE, TRUE,
} BOOLEAN;

		/****************************************/
		/*                                      */
		/*      Macros for memory allocation    */
		/*                                      */
		/****************************************/

#define alloc_struct(type)          (type *) malloc(sizeof(type))
#define alloc_array(type, count)    (type *) malloc(count*sizeof(type))
#define alloc_bytes(length)         (char *) malloc(length)

#endif
