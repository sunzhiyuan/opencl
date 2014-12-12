/*----------------------------------------------------------------------------*/  
/**
 *	This confidential and proprietary software may be used only as
 *	authorised by a licensing agreement from ARM Limited
 *	(C) COPYRIGHT 2011-2012 ARM Limited
 *	ALL RIGHTS RESERVED
 *
 *	The entire notice above must be reproduced on all authorised
 *	copies and copies may only be made to the extent permitted
 *	by a licensing agreement from ARM Limited.
 *
 *	@brief	Functions for loading/storing TGA files and the file types
 *			accessible through STB.
 */ 
/*----------------------------------------------------------------------------*/ 

#include "astc_codec_internals.h"
#include "astc_main.h" //This file needs access to the TQonvertImage type. 

#include "softfloat.h"
#include "astc_stdint.h"
#include <stdio.h>

#define STBI_HEADER_FILE_ONLY
//#include "stb_image.c"
#ifdef LEAK_DETECTOR_WIN32
#include "leak_detector.h"
#endif
// Here, we just need to load the image from the data passed from TextureConverter 
//  instead of the data from a file. 
astc_codec_image * load_image_with_stb( int padding, int *result, void* options)
{
	int xsize, ysize;
	int components, y_flip;
	int x, y;
	_ASTCOptions *opt;
	astc_codec_image *astc_img;


	//Not sure why we would do this. 
	y_flip = 0;

	opt	= (_ASTCOptions *)options;
	astc_img = NULL;

	xsize				= opt->astc_pSrc->nWidth;
	ysize				= opt->astc_pSrc->nHeight;
	
	//Although we may not know the bit fidelity, we know we are 
	// getting a four channel RGBA input
	components			= 4; 

	// Yay, we can easily use the 16-bit fidelity option. 
	if (opt->BitFidelity == ASTC_16_BIT)
	{
		uint16_t *imageptr = (uint16_t *)opt->astc_pData;

		if (imageptr != NULL)
		{
			astc_img = allocate_image(16, xsize, ysize, 1, padding);
			for (y = 0; y < ysize; y++)
			{
				int y_dst = y + padding;
				int y_src = y_flip ? (ysize - y - 1) : y;
				uint16_t *src = imageptr + 8 * xsize * y_src;

				for (x = 0; x < xsize; x++)
				{
					int x_dst = x + padding;
					astc_img->imagedata16[0][y_dst][4 * x_dst] = src[4*x];//float_to_sf16(src[4 * x], SF_NEARESTEVEN);
					astc_img->imagedata16[0][y_dst][4 * x_dst + 1] = src[4*x+1];//float_to_sf16(src[4 * x + 1], SF_NEARESTEVEN);
					astc_img->imagedata16[0][y_dst][4 * x_dst + 2] = src[4*x+2];//float_to_sf16(src[4 * x + 2], SF_NEARESTEVEN);
					astc_img->imagedata16[0][y_dst][4 * x_dst + 3] = src[4*x+3];//float_to_sf16(src[4 * x + 3], SF_NEARESTEVEN);
				}

				fill_image_padding_area(astc_img);
				*result = components + 0x80;
				return astc_img;
			}
		}
	}
	else//opt->bit_fidelity == ASTC_8_BIT
	{

		uint8_t *imageptr = opt->astc_pData;

		if (imageptr != NULL)
		{
			astc_img = allocate_image(8, xsize, ysize, 1, padding);
			for (y = 0; y < ysize; y++)
			{
				int y_dst = y + padding;
				int y_src = y_flip ? (ysize - y - 1) : y;
				uint8_t *src = imageptr + 4 * xsize * y_src;

				for (x = 0; x < xsize; x++)
				{
					int x_dst = x + padding;
					astc_img->imagedata8[0][y_dst][4 * x_dst    ] = src[4 * x    ];
					astc_img->imagedata8[0][y_dst][4 * x_dst + 1] = src[4 * x + 1];
					astc_img->imagedata8[0][y_dst][4 * x_dst + 2] = src[4 * x + 2];
					astc_img->imagedata8[0][y_dst][4 * x_dst + 3] = src[4 * x + 3];
				}
			}

			fill_image_padding_area(astc_img);
			*result = components;
			return astc_img;	
		}
	}

	// if we haven't returned, it's because we failed to load the file.
	//printf("Failed to load image %s\nReason: %s\n", filename, stbi_failure_reason());

	*result = -1;
	return NULL;
}







/* 
given a TGA filename, read in a TGA file and create test-vectors from it.
*/

struct tga_header
{
	uint8_t identsize;
	uint8_t colormaptype;
	uint8_t imagetype;
	uint8_t dummied[5];
	uint16_t xstart;
	uint16_t ystart;
	uint16_t xsize;
	uint16_t ysize;
	uint8_t bitsperpixel;
	uint8_t descriptor;
};


enum tga_descriptor
{
	TGA_DESCRIPTOR_XFLIP = 0x10,
	TGA_DESCRIPTOR_YFLIP = 0x20,
};

enum tga_type
{
	TGA_COLORMAP_NONE = 0x00,
	/* other color map type codes are reserved */

	TGA_IMAGETYPE_NONE = 0,		/* no image data */
	TGA_IMAGETYPE_PSEUDOCOLOR = 1,	/* color-mapped image */
	TGA_IMAGETYPE_TRUECOLOR = 2,	/* true-color image */
	TGA_IMAGETYPE_GREYSCALE = 3,	/* true-color single channel */
	TGA_IMAGETYPE_RLE_PSEUDOCOLOR = 9,	/* RLE color-mapped image */
	TGA_IMAGETYPE_RLE_TRUECOLOR = 10,	/* RLE true color */
	TGA_IMAGETYPE_RLE_GREYSCALE = 11,	/* RLE true grey */

	HTGA_IMAGETYPE_TRUECOLOR = 0x82,
	HTGA_IMAGETYPE_GREYSCALE = 0x83,
};

enum tga_errors
{
	TGA_ERROR_OPEN = -1,		/* error opening file */
	TGA_ERROR_READ = -2,		/* error reading file */
	TGA_ERROR_COLORMAP = -3,	/* file has a colormap (not supported) */
	TGA_ERROR_RLE = -4,			/* file is run-length encoded (not supported) */
	TGA_ERROR_FORMAT = -5,		/* file has an unsupported pixel format */
	TGA_ERROR_LAYOUT = -6		/* file layout unsupported (right-to-left flipped) */
};


/* 
	return: if positive number, then the number is #components in the image
	1=Grayscale 2=Grayscale+Alpha 3=RGB 4=RGB+Alpha
	add 0x80 if the file was in fact a HTGA file with HDR content.

	if negative number, then what went wrong
	-1=failed to open file
	-2=failed to read data
	-3=failed to load image because it has a colormap
	-4=failed to load image because it is RLE-encoded
	-5=failed to load image because it has an unsupported pixel type
	-6=failed to load image because it is flipped in the x dimension
*/

astc_codec_image *load_tga_image( int padding, int *result, void* options)
{
	int x, y,i;
	//int y_flip;
    //
	//FILE *f = fopen(tga_filename, "rb");
	//if (!f)
	//{
	//	*result = TGA_ERROR_OPEN;
	//	return NULL;
	//}
    _ASTCOptions *opt = (_ASTCOptions *)options;
//    astc_options *opt = (astc_options *)options;
	tga_header hdr;
	//size_t bytes_read = fread(&hdr, 1, 18, f);
	//if (bytes_read != 18)
	//{
	//	fclose(f);
	//	*result = TGA_ERROR_READ;
	//	return NULL;
	//}
	//if (hdr.colormaptype != 0)
	//{
	//	fclose(f);
	//	*result = TGA_ERROR_COLORMAP;
	//	return NULL;
	//}
    //
	// do a quick test for RLE-pictures so that we reject them
	//if (hdr.imagetype == TGA_IMAGETYPE_RLE_TRUECOLOR || hdr.imagetype == TGA_IMAGETYPE_RLE_PSEUDOCOLOR || hdr.imagetype == TGA_IMAGETYPE_RLE_GREYSCALE)
	//{
	//	fclose(f);
	//	printf("TGA image %s is RLE-encoded; only uncompressed TGAs are supported.\n", tga_filename);
	//	*result = TGA_ERROR_RLE;
	//	return NULL;
	//}
    //
	//// Check for x flip (rare, unsupported) and y flip (supported)
	//if (hdr.descriptor & TGA_DESCRIPTOR_XFLIP)
	//{
	//	fclose(f);
	//	*result = TGA_ERROR_LAYOUT;
	//	return NULL;
	//}
	//if (hdr.descriptor & TGA_DESCRIPTOR_YFLIP)
	//	y_flip = 1;
	//else
	//	y_flip = 0;
    //
    //
	// support 4 formats (non-RLE only):
	// 8-bit grayscale
	// 8-bit grayscale + 8-bit alpha
	// RGB 8:8:8
	// RGBA 8:8:8:8
    //
	//if (!(hdr.imagetype == TGA_IMAGETYPE_TRUECOLOR && hdr.bitsperpixel == 32)
	//	&& !(hdr.imagetype == TGA_IMAGETYPE_TRUECOLOR && hdr.bitsperpixel == 24)
	//	&& !(hdr.imagetype == TGA_IMAGETYPE_GREYSCALE && hdr.bitsperpixel == 16)
	//	&& !(hdr.imagetype == TGA_IMAGETYPE_GREYSCALE && hdr.bitsperpixel == 8)
	//	&& !(hdr.imagetype == HTGA_IMAGETYPE_TRUECOLOR && hdr.bitsperpixel == 64)
	//	&& !(hdr.imagetype == HTGA_IMAGETYPE_TRUECOLOR && hdr.bitsperpixel == 48)
	//	&& !(hdr.imagetype == HTGA_IMAGETYPE_GREYSCALE && hdr.bitsperpixel == 32) && !(hdr.imagetype == HTGA_IMAGETYPE_GREYSCALE && hdr.bitsperpixel == 16))
	//{
	//	fclose(f);
	//	*result = TGA_ERROR_FORMAT;
	//	return NULL;
	//}
    //
    //if (hdr.identsize != 0)		// skip ID field if it present.
    //	fseek(f, hdr.identsize, SEEK_CUR);
    //

    //Bytes per pixel is our channel fidelity * number of channels (4)
    int bytesperpixel = opt->BitFidelity * 4;
	//int bytesperpixel = hdr.bitsperpixel / 8;

    int bitness = opt->BitFidelity == ASTC_8_BIT ? 8 : 16; 
	//int bitness = (hdr.imagetype >= 0x80) ? 16 : 8;

    hdr.ysize = opt->astc_pSrc->nHeight;
    hdr.xsize = opt->astc_pSrc->nWidth;
	// OK, it seems we have a legit TGA or HTGA file of a format we understand.
	// Now, let's read it.

	size_t bytestoread = 0;
	uint8_t **row_pointers8 = NULL;
	uint16_t **row_pointers16 = NULL;
	if (bitness == 8)
	{
		row_pointers8 = (uint8_t**)malloc(sizeof(uint8_t*) * hdr.ysize);
		row_pointers8[0] = (uint8_t*)malloc(sizeof(uint8_t) * hdr.xsize * hdr.ysize * bytesperpixel);
		for (i = 1; i < hdr.ysize; i++)
			row_pointers8[i] = row_pointers8[0] + hdr.xsize * bytesperpixel * i;
		bytestoread = hdr.xsize * hdr.ysize * bytesperpixel;
		
        memcpy(row_pointers8[0], opt->astc_pData, bytestoread);
        //bytes_read = fread(row_pointers8[0], 1, bytestoread, f);
	}
	else if (bitness == 16)
	{
        row_pointers16 = (uint16_t**)malloc(sizeof(uint16_t*) * hdr.ysize);//new uint16_t *[hdr.ysize];
		row_pointers16[0] = (uint16_t*)malloc(sizeof(uint16_t) * hdr.xsize * hdr.ysize * (bytesperpixel / 2));
		for (i = 1; i < hdr.ysize; i++)
			row_pointers16[i] = row_pointers16[0] + hdr.xsize * (bytesperpixel / 2) * i;
		bytestoread = hdr.xsize * hdr.ysize * bytesperpixel;
		
        memcpy(row_pointers16[0], opt->astc_pData, bytestoread);
        //bytes_read = fread(row_pointers16[0], 1, bytestoread, f);
	}

    //fclose(f);
	//if (bytes_read != bytestoread)
	//{
	//	if (row_pointers8)
	//	{
	//		delete[]row_pointers8[0];
	//		delete[]row_pointers8;
	//	}
	//	if (row_pointers16)
	//	{
	//		delete[]row_pointers16[0];
	//		delete[]row_pointers16;
	//	}
	//	*result = -2;
	//	return NULL;
	//}



	// OK, at this point, we can expand the image data to RGBA.
	int ysize = hdr.ysize;
	int xsize = hdr.xsize;

	astc_codec_image *astc_img = allocate_image(bitness, xsize, ysize, 1, padding);

	int retval;

	if (bitness == 8)
	{
		for (y = 0; y < ysize; y++)
		{
			int y_dst = y + padding;
			int y_src = y;

            switch (bytesperpixel)
            {
            case 4:			// four-component, treated as RGBA
                for (x = 0; x < xsize; x++)
                {
                    int x_dst = x + padding;
                    astc_img->imagedata8[0][y_dst][4 * x_dst    ]   = row_pointers8[y_src][4 * x    ];	// tga uses BGR, we use RGB
                    astc_img->imagedata8[0][y_dst][4 * x_dst + 1]   = row_pointers8[y_src][4 * x + 1];
                    astc_img->imagedata8[0][y_dst][4 * x_dst + 2]   = row_pointers8[y_src][4 * x + 2];
                    astc_img->imagedata8[0][y_dst][4 * x_dst + 3]   = row_pointers8[y_src][4 * x + 3];
                }
                break;
            default:
                {
                    //This cannot happen. 
                    return NULL;
                }
            }

        }
		free(row_pointers8[0]);
		free(row_pointers8);

		retval = bytesperpixel;
	}
	else						// if( bitness == 16 )
	{
		for (y = 0; y < ysize; y++)
		{
			int y_dst = y + padding;
            int y_src = y;
            unsigned int temp;
            volatile float f;
            switch (bytesperpixel)
            {
            case 8:			// three-component, treated as RGB
                for (x = 0; x < xsize; x++)
                {
                    int x_dst = x + padding;
                    
                    temp = row_pointers16[y_src][4 * x    ];
                    //temp = (unsigned int)t | ((unsigned int)t << 16);
                    f = float(temp);
                    astc_img->imagedata16[0][y_dst][4 * x_dst    ] = float_to_sf16(f,SF_NEARESTEVEN );
                    
                    temp = row_pointers16[y_src][4 * x    + 1];
                    //temp = (unsigned int)t | ((unsigned int)t << 16);
                    f = float(temp);
                    astc_img->imagedata16[0][y_dst][4 * x_dst   + 1] = float_to_sf16(f,SF_NEARESTEVEN );
                    
                    
                    temp = row_pointers16[y_src][4 * x    + 2];
                    //temp = (unsigned int)t | ((unsigned int)t << 16);
                    f = float(temp);
                    astc_img->imagedata16[0][y_dst][4 * x_dst   + 2] = float_to_sf16(f,SF_NEARESTEVEN );
                    
                    
                    temp = row_pointers16[y_src][4 * x    + 3];
                    //temp = (unsigned int)t | ((unsigned int)t << 16);
                    f = float(temp);
                    astc_img->imagedata16[0][y_dst][4 * x_dst   + 3] = float_to_sf16(f,SF_NEARESTEVEN );


                    astc_img->imagedata16[0][y_dst][4 * x_dst    ] = row_pointers16[y_src][4 * x    ];	// tga uses BGR, we use RGB
                    astc_img->imagedata16[0][y_dst][4 * x_dst + 1] = row_pointers16[y_src][4 * x + 1];
                    astc_img->imagedata16[0][y_dst][4 * x_dst + 2] = row_pointers16[y_src][4 * x + 2];
                    astc_img->imagedata16[0][y_dst][4 * x_dst + 3] = row_pointers16[y_src][4 * x + 3];
                }
                break;
            default:
                return NULL;

            }
        }
		free(row_pointers16[0]);
		free(row_pointers16);
	
		retval = (bytesperpixel / 2) + 0x80;
	}

	fill_image_padding_area(astc_img);
	*result = retval;
	return astc_img;
}




/* 
   returns -1 if any problems arose when writing the file, else the number of color channels it chose to write.
 */
int store_tga_image(const astc_codec_image * img, int bitness, void* options)
{
	int x, y;
	int i;

	int xsize = img->xsize;
	int ysize = img->ysize;
    _ASTCOptions *op = (_ASTCOptions *)options;
//    astc_options *op = (astc_options *)options;
	
	// first scan through the image data
	// to determine how many color channels the image has.

	//We actually are spitting out four channels no matter what. 
	int image_channels = 4; //int image_channels = determine_image_channels(img);

	// construct a header
	tga_header hdr;
	hdr.identsize = 0;
	hdr.colormaptype = 0;
	hdr.imagetype = image_channels >= 3 ? 2 : 3;
	if (bitness == 16)
		hdr.imagetype |= 0x80;

	for (i = 0; i < 5; i++)
		hdr.dummied[i] = 0;
	hdr.xstart = 0;
	hdr.ystart = 0;
	hdr.xsize = xsize;
	hdr.ysize = ysize;
	hdr.bitsperpixel = image_channels * bitness;
	hdr.descriptor = 0;

	int bytesperpixel = (image_channels * bitness) / 8;

	// construct image data to write

	uint8_t **row_pointers8 = NULL;
	uint16_t **row_pointers16 = NULL;
	if (bitness == 8)//This should be correct so no need to use options
	{
		row_pointers8 = (uint8_t**)malloc(hdr.ysize*sizeof(uint8_t*));
		row_pointers8[0] = (uint8_t*)malloc(hdr.xsize * hdr.ysize * bytesperpixel*sizeof(uint8_t));
		for (i = 1; i < hdr.ysize; i++)
			row_pointers8[i] = row_pointers8[0] + hdr.xsize * bytesperpixel * i;

		for (y = 0; y < ysize; y++)
		{
			switch (bytesperpixel)
			{
			case 1:			// single-component, treated as Luminance
				for (x = 0; x < xsize; x++)
				{
					row_pointers8[y][x] = img->imagedata8[0][y][4 * x];
				}
				break;
			case 2:			// two-component, treated as Luminance-Alpha
				for (x = 0; x < xsize; x++)
				{
					row_pointers8[y][2 * x]     = img->imagedata8[0][y][4 * x];
					row_pointers8[y][2 * x + 1] = img->imagedata8[0][y][4 * x + 3];
				}
				break;
			case 3:			// three-component, treated as RGB
				for (x = 0; x < xsize; x++)
				{
					row_pointers8[y][3 * x + 2] = img->imagedata8[0][y][4 * x];
					row_pointers8[y][3 * x + 1] = img->imagedata8[0][y][4 * x + 1];
					row_pointers8[y][3 * x] = img->imagedata8[0][y][4 * x + 2];
				}
				break;
			case 4:			// three-component, treated as RGB
				for (x = 0; x < xsize; x++)
				{
					row_pointers8[y][4 * x	  ] = img->imagedata8[0][y][4 * x    ];
					row_pointers8[y][4 * x + 1] = img->imagedata8[0][y][4 * x + 1];
					row_pointers8[y][4 * x + 2] = img->imagedata8[0][y][4 * x + 2];
					row_pointers8[y][4 * x + 3] = img->imagedata8[0][y][4 * x + 3];
				}
				break;
			}
		}
	}
	else// if bitness == 16
	{
		row_pointers16 = (uint16_t **)malloc(hdr.ysize*sizeof(uint16_t *));
		row_pointers16[0] = (uint16_t *)malloc(hdr.xsize * hdr.ysize * bytesperpixel*sizeof(uint16_t));
		for (i = 1; i < hdr.ysize; i++)
			row_pointers16[i] = row_pointers16[0] + hdr.xsize * bytesperpixel * i;

		for (y = 0; y < ysize; y++)
		{
			switch (bytesperpixel)
			{
			case 1:			// single-component, treated as Luminance
				for (x = 0; x < xsize; x++)
				{
					row_pointers16[y][x] = img->imagedata16[0][y][4 * x];
				}
				break;
			case 2:			// two-component, treated as Luminance-Alpha
				for (x = 0; x < xsize; x++)
				{
					row_pointers16[y][2 * x] = img->imagedata16[0][y][4 * x];
					row_pointers16[y][2 * x + 1] = img->imagedata16[0][y][4 * x + 3];
				}
				break;
			case 3:			// three-component, treated as RGB
				for (x = 0; x < xsize; x++)
				{
					row_pointers16[y][3 * x + 2] = img->imagedata16[0][y][4 * x];
					row_pointers16[y][3 * x + 1] = img->imagedata16[0][y][4 * x + 1];
					row_pointers16[y][3 * x] = img->imagedata16[0][y][4 * x + 2];
				}
				break;
			case 8:			// three-component, treated as RGB
				for (x = 0; x < xsize; x++)
				{

                    
					row_pointers16[y][4 * x	   ] = img->imagedata16[0][y][4 * x    ];
					row_pointers16[y][4 * x + 1] = img->imagedata16[0][y][4 * x + 1];
					row_pointers16[y][4 * x + 2] = img->imagedata16[0][y][4 * x + 2];
					row_pointers16[y][4 * x + 3] = img->imagedata16[0][y][4 * x + 3];
				}
				break;
			}
		}
	}
	
   


	int retval = image_channels;
	
	if (row_pointers8)
	{
		memcpy(op->astc_pData,row_pointers8[0], bytesperpixel * xsize * ysize);

		free(row_pointers8[0]);
		free(row_pointers8);
	}
	if (row_pointers16)
	{
        memcpy(op->astc_pData,img->imagedata16[0][0], bytesperpixel * xsize * ysize);
		free(row_pointers16[0]);
		free(row_pointers16);
	}

	return retval;
}
