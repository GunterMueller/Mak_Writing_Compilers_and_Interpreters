/****************************************************************/
/*                                                              */
/*      FILE:       ppprint.h                                   */
/*                                                              */
/*      Copyright (c) 1991 by Ronald Mak                        */
/*      For instructional purposes only.  No warranties.        */
/*                                                              */
/****************************************************************/

#define indent_size 4

#define advance_left_margin() left_margin += indent_size

#define retreat_left_margin() if ((left_margin -= indent_size) < 0) \
				  left_margin = 0;

#define set_left_margin(m)    {m = left_margin; \
			       left_margin = strlen(pprint_buffer);}

#define reset_left_margin(m)  left_margin = m

