#ifndef HEADER_OFL_PPM_H
#define HEADER_OFL_PPM_H
//#include <stdint.h>
#include "kgstdint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ofl.h"
//#include "ofl_macro.h"
// これはメモリアライン処理のため
//#include "ofl_type.h"
namespace nsOfl{
/**
	oflReadPPM
	-	read a PPM raw (type P6) file.  The PPM file has a header that should look something like:
	   P6
	   # comment
	   width height max_value
	   rgbrgbrgb...
	-	where "P6" is the magic cookie which identifies the file type and
		should be the only characters on the first line followed by a
		carriage return.  Any line starting with a # mark will be treated
		as a comment and discarded.   After the magic cookie, three integer
		values are expected: width, height of the image and the maximum
		value for a pixel (max_value must be < 256 for PPM raw files).  The
		data section consists of width*height rgb triplets (one byte each)
		in binary format (i.e., such as that written with fwrite() or
		equivalent).
	-	The rgb data is returned as an array of unsigned chars (packed
		rgb).  The malloc()'d memory should be free()'d by the caller.  If
		an error occurs, an error message is sent to stderr and NULL is
		returned.
	-	filename   - name of the .ppm file.
	-	width      - will contain the width of the image on return.
	-	height     - will contain the height of the image on return.
*/
uint8_t* oflReadPPM(char *filename, int *width, int *height);
} //nsOfl
#endif // HEADER_OFL_PPM_H
