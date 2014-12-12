#ifndef ASTC_TOPLEVEL_H
#define ASTC_TOPLEVEL_H


#include <stdio.h>
#include <stdlib.h>
#include "astc_codec_internals.h"
#include "TextureConverter.h"

#define MAGIC_FILE_CONSTANT 0x5CA1AB13

int set_decode_mode(int,int,int*,astc_decode_mode*);

void set_suppress_progress_counter(int);

void set_diagnostics_tile(int);

int get_diagnostics_tile(void);

void set_progress_counter_divider(int);

void print_block_mode_histogram_func(char *,int,int);

void set_start_coding_time(double);

void set_end_coding_time(double);

void set_end_time(double);

void set_start_time(double);

double get_start_coding_time(void);

double get_end_coding_time(void);

double get_end_time(void);

double get_start_time(void);

double get_time(void);

unsigned get_number_of_cpus(void);

void astc_codec_internal_error(const char *,int);

astc_codec_image *load_astc_file(	int,
									astc_decode_mode,
									swizzlepattern,
									void *);

void *encode_astc_image_threadfunc(void *);

void encode_astc_image(	const astc_codec_image *,
						astc_codec_image *,
						int,
						int,
						int,
						const error_weighting_params *,
						astc_decode_mode,
						swizzlepattern,
						swizzlepattern,
						uint8_t *,
						int,
						int);

void store_astc_file(	const astc_codec_image *, 
						int,
						int,
						int,
						const error_weighting_params *,
						astc_decode_mode,
						swizzlepattern,
						int,
						_ASTCOptions* options = NULL);

astc_codec_image *pack_and_unpack_astc_image(	const astc_codec_image *,
												int,
												int,
												int,
												const error_weighting_params *,
												astc_decode_mode,
												swizzlepattern,
												swizzlepattern,
												int,
												int);

void find_closest_blockdim_3d(float,int *,int *,int *,int);

void compare_two_files(int,int,int);

void test_inappropriate_extended_precision(void);

// Debug routine to dump the entire image if requested.
void dump_image(astc_codec_image *);

#endif