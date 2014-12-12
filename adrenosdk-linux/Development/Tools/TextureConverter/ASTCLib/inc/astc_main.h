#ifndef ASTC_MAIN_H
#define ASTC_MAIN_H


#include "astc_toplevel.h"
#include "TextureConverter.h"

// Define this to be 1 to allow "illegal" block sizes
// We need this in Qonvert.cpp which is why it is defined here. 
#define DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES		0
#define ASTC_MAIN_FAILURE					-1
#define ASTC_INVALID_INPUT					-2
#define ASTC_ERROR_CODE                     -3



/**
 * Another place holder for the compresssion submodes. May be repetitive 
 */
enum ASTC_COMMANDS{				
	ASTC_COMPRESS = 1,			//Same as -c command line option	
	ASTC_DECOMPRESS,			//Same as -d command line option
	ASTC_BOTH,					//Same as -t command line option (not implemented)
	ASTC_LDR_SRGB_COMPRESS,		//Same as -cs command line option
	ASTC_LDR_SRGB_DECOMPRESS,	//Same as -ds command line option
	ASTC_LDR_SRGB_BOTH,			//Same as -ts command line option (not implemented)
	ASTC_LDR_LINEAR_COMPRESS,	//Same as -cl command line option
	ASTC_LDR_LINEAR_DECOMPRESS, //Same as -dl command line option
	ASTC_LDR_LINEAR_BOTH,		//Same as -tl command line option (not implemented)
	ASTC_COMPARE,				//Same as -compare command line option
};

/**
 * We need knowledge of this so we can compute the size of the ASTC output file.
 */
struct astc_header
{
	uint8_t magic[4];
	uint8_t blockdim_x;
	uint8_t blockdim_y;
	uint8_t blockdim_z;
	uint8_t xsize[3];			// x-size = xsize[0] + xsize[1] + xsize[2]
	uint8_t ysize[3];			// x-size, y-size and z-size are given in texels;
	uint8_t zsize[3];			// block count is inferred
};

/** astc_toplevel_main
 *
 *  @brief Entrance function for the ASTC library. 
 *
 *  @param astc_options* Pointer to an object containing all the ASTC
 *						options that we need to take into account. 
 *							This pointer also contains pointers to the 
 *							TQonvertImage structures that are coming
 *							from Qonvert. 
 * @return 0 if SUCCESS  
 */
int astc_toplevel_main(void *, int);

/** find_closest_blockdim_2d
 *
 *  @brief Given a bitrate, find the closest 2d block dimensions. 
 *
 *  @param float	input bitrate
 *  @param int *	pointer to an int to store the block x dim
 *  @param int *	pointer to an int to store the block y dim
 *  @param int		Flag whether or not to consider illegal block dims
 *
 *  @return void
 */
/*Needed in Qonvert in order to calculate the destination size*/
void find_closest_blockdim_2d(float,int *,int *,int);

#endif