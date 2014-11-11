
#include "Scope.h"


#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XMD_H	1
#undef FAR

extern "C" {
// #include <jinclude.h>
#include <jpeglib.h>
#include <jerror.h>		/* get library error codes too */
}

typedef struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
} my_error_mgr, *my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);
  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


MFILE *mopen(const unsigned char *buf,unsigned int size) 
{
	MFILE *m=new MFILE; 
	m->size=size; 
	m->buf=(const char*)buf; 
	m->pos=0; 
	return m;
}

void  mclose(MFILE *m) 
{
	delete m;
}

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  MFILE * infile;		/* source stream */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;
#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

METHODDEF(void)
null_memfunc (j_decompress_ptr cinfo)
{
  // my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
}

// This isn't called...
METHODDEF(boolean)
fill_input_membuffer (j_decompress_ptr cinfo)
{
	// Shouldn't be called
  return TRUE;
}

METHODDEF(void)
skip_input_memdata (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  if (num_bytes > 0) {
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}


GLOBAL(void)
jpeg_memory_src (j_decompress_ptr cinfo, MFILE * infile)
{
  my_src_ptr src;

  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = null_memfunc;
  src->pub.fill_input_buffer = fill_input_membuffer;
  src->pub.skip_input_data = skip_input_memdata;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = null_memfunc;
  src->infile = infile;
  // src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  // src->pub.next_input_byte = NULL; /* until buffer loaded */
  src->pub.bytes_in_buffer = infile->size; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = (unsigned char *) infile->buf; /* until buffer loaded */
}


bool FinishLoadingTexture(TextureImage *texture);

bool LoadJPEGTexture(TextureImage *texture, const unsigned char *maskData, unsigned int maskSize, int glMode)
{
  struct my_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  MFILE	*ifile = NULL;
  // Initialize the JPEG decompression object with default error handling.
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
		if (ifile != NULL)
			mclose(ifile);
    return false;
  }
  
	jpeg_create_decompress(&cinfo);

  /* Now safe to enable signal catcher. */

	 /* Specify data source for decompression */
    // EVENTUALLY needs to be changed for efficiency
	ifile = mopen(maskData, maskSize);
	if (ifile == 0)
			return false;
	jpeg_memory_src(&cinfo, ifile);

//	if ((ifile = fopen(maskname, "rb")) == NULL)
//		 return false;
// jpeg_stdio_src(&cinfo, ifile);

	/* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

	/* Adjust decompression parameters here */
  // file_index = parse_switches(&cinfo, argc, argv, 0, TRUE);

  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);

	int width = cinfo.output_width;
	int height = cinfo.output_height;
	int rowBytes = width * cinfo.output_components;

	texture->width = width;
	texture->height = height;
	texture->bpp = 8 * cinfo.output_components;
	texture->imageData = (unsigned char *) malloc(texture->height*rowBytes);
	memset(texture->imageData,0,texture->height*rowBytes);

  JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, rowBytes, 1);

	int y = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
		// NOTE: first time I tried multiple scan lines, using manually
		// allocated scanline ptrs (ala PNG), it only accepted 1 scanline
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
		unsigned char *sp = (unsigned char *) buffer[0];
		unsigned char *dp = texture->imageData + (y*rowBytes);
		memcpy(dp, sp, rowBytes);
		dp += rowBytes;
		sp += rowBytes;
		y++;
	}
	
	(void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

	mclose(ifile);

	// Finish loading texture
	return FinishLoadingTexture(texture);

	return true;
}

bool FinishLoadingTexture(TextureImage *texture)
{
	GLuint		type=GL_ALPHA;

	// Build A Texture From The Data
	glGenTextures(1, &texture->texID);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture->texID);			// Bind Our Texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texture->bpp == 24)
		type = GL_RGB;
	else
		type = GL_ALPHA;

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);
//		glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);
	free(texture->imageData);

	return true;											// Texture Building Went Ok, Return True
}

