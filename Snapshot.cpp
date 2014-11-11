#include "Scope.h"

#ifdef WIN32
	#pragma comment(lib, "png.lib")
	#pragma comment(lib, "zlib.lib")
	#include <png.h>
#else
	#include <png.h>
#endif

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ObjectFactory.h"

static int snapShotCtr = 0;

void ScopeApp::TakeSnapshot()
{
	char	outfilename[64];
	++snapShotCtr;
#ifdef WIN32
	sprintf(outfilename,"snap_%03d_x_%03d_%d.png", width, height, snapShotCtr);
#else // MAC ONLY
	sprintf(outfilename,"%s/Pictures/snap_%03d_x_%03d_%d.png", getenv("HOME"), width, height, snapShotCtr);
#endif
	unsigned char *buffer = (unsigned char *) malloc(width*height*3);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	png_structp png_ptr;	// was static
	png_infop info_ptr;

	png_text text_ptr[3];

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
						NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		 return;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		return;
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		// If we get here, we had a problem reading the file
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	FILE *outfile = fopen(outfilename, "w");
	png_set_write_fn(png_ptr,outfile, 
									(png_rw_ptr) NULL, 
									(png_flush_ptr) NULL);

	int	pngformat;

	pngformat = PNG_COLOR_TYPE_RGB;

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, pngformat,
	      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  text_ptr[0].key = (char *) "Title";
  text_ptr[0].text = (char *) outfilename; // get this from settings
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[1].key = (char *) "Author";
  text_ptr[1].text = (char *) "Krazydad";  // get this from settings
  text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[2].key = (char *) "Description";
  text_ptr[2].text = (char *) "Kaleidoscope Snapshot";
  text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;

  png_set_text(png_ptr, info_ptr, text_ptr, 3);

  // Write the file header information.  REQUIRED
  png_write_info(png_ptr, info_ptr);

	unsigned char **imgRows = (unsigned char **) malloc(sizeof(unsigned char *) * height);
	for (int j = 0; j < height; ++j)
		imgRows[j] = &buffer[j*width*3];
  png_write_image(png_ptr, imgRows);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(outfile);
	free(imgRows);
	free(buffer);
}