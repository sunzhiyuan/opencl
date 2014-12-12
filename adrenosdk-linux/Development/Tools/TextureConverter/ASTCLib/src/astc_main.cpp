/**
  *
  *
  *
  *
  *
  **/
#include "astc_main.h"

#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#ifdef LEAK_DETECTOR_WIN32
#include "leak_detector.h"
#endif
/** astc_toplevel_main 
 * 
 *  This is the entry point from TextureConverter to the ASTC library. 
 *   Much of the functionality is adapted to work with TextureConverter. I 
 *   wouldn't expect any of this to work from the command line as the  
 *   author of this library intended. 
 * 
 *  @param options - A pointer to an astc_options structure for this instance
 *					of ASTC compression. This is needed in place of command
 *					line arguments. It also passes other essential information
 *					from TextureConverter to ASTC Lib. 
 *
 *	@return - Returns 0 on success and a negative error code on failure. 
 */
int astc_toplevel_main(void *ASTCoptions, int CompressOrDecompress)
{
    _ASTCOptions *options = (_ASTCOptions *)ASTCoptions;

	int opmode,xdim,ydim,zdim;		// 0=compress, 1=decompress, 2=do both, 4=compare
	error_weighting_params ewp;
	astc_decode_mode decode_mode;
	char input_filename[120];		//Please don't buffer overflow attack me.
	char output_filename[120];
	
	int input_components;
	int input_image_is_hdr;
	int load_result;
	int *load_results;
	astc_codec_image *input_image;
	astc_codec_image *output_image;
	astc_codec_image **input_images;
	
	ewp.rgb_power					= 1.0f;
	ewp.alpha_power					= 1.0f;
	ewp.rgb_base_weight				= 1.0f;
	ewp.alpha_base_weight			= 1.0f;
	ewp.rgb_mean_weight				= 0.0f;
	ewp.rgb_stdev_weight			= 0.0f;
	ewp.alpha_mean_weight			= 0.0f;
	ewp.alpha_stdev_weight			= 0.0f;

	ewp.rgb_mean_and_stdev_mixing	= 0.0f;
	ewp.mean_stdev_radius			= 0;
	ewp.enable_rgb_scale_with_alpha = 0;
	ewp.alpha_radius				= 0;

	ewp.block_artifact_suppression	= 0.0f;
	ewp.rgba_weights[0]				= 1.0f;
	ewp.rgba_weights[1]				= 1.0f;
	ewp.rgba_weights[2]				= 1.0f;
	ewp.rgba_weights[3]				= 1.0f;
	ewp.ra_normal_angular_scale		= 0;

	swizzlepattern swz_encode		= { 0, 1, 2, 3 };
	swizzlepattern swz_decode		= { 0, 1, 2, 3 };

	int array_size					= 1;
	int silentmode					= 0;
	int timemode					= 0;
	int psnrmode					= 0;
	
	int thread_count				= 0;
	int thread_count_autodetected	= 0;

	int preset_has_been_set			= 0;

	int plimit_autoset				= -1;
	int plimit_user_specified		= -1;
	int plimit_set_by_user			= 0;

	float dblimit_autoset_2d		= 0.0;
	float dblimit_autoset_3d		= 0.0;
	float dblimit_user_specified	= 0.0;
	int dblimit_set_by_user			= 0;

	float oplimit_autoset			= 0.0;
	float oplimit_user_specified	= 0.0;
	int oplimit_set_by_user			= 0;

	float mincorrel_autoset			= 0.0;
	float mincorrel_user_specified	= 0.0;
	int mincorrel_set_by_user		= 0;

	float bmc_user_specified		= 0.0;
	float bmc_autoset				= 0.0;
	int bmc_set_by_user				= 0;

	int maxiters_user_specified		= 0;
	int maxiters_autoset			= 0;
	int maxiters_set_by_user		= 0;

	int pcdiv						= 1;

	int xdim_2d = 0;
	int ydim_2d = 0;
	int xdim_3d = 0;
	int ydim_3d = 0;
	int zdim_3d = 0;

	int target_bitrate_set			= 0;
	float target_bitrate			= 0;



	float log10_texels_2d			= 0.0f;
	float log10_texels_3d			= 0.0f;

	int low_fstop					= -10;
	int high_fstop					= 10;

	
	//Initialization routines
	test_inappropriate_extended_precision();
	prepare_angular_tables();
	build_quantization_mode_table();
	set_start_time(get_time());

	#ifdef DEBUG_CAPTURE_NAN
		feenableexcept(FE_DIVBYZERO | FE_INVALID);
	#endif
	
	//Setting up the opmode and the decode_mode.
    if(set_decode_mode(options->CompressionMode, CompressOrDecompress, &opmode, &decode_mode) == ASTC_INVALID_INPUT)
		return ASTC_MAIN_FAILURE;


    //We don't want to do this because we are calling through QCompress. 
//	if(options->save_output_files)
//	{
//		if(options->outputfile)
//			strcpy(output_filename,options->outputfile);
//		else
//		{
//			char testnum[10];
//			itoa(options->testnum, testnum, 10);
//			strcpy(output_filename, "C:\\Users\\gpitts\\astc_test\\redlines\\redlines_test_");
//			strcat(output_filename, testnum);
//			strcat(output_filename, ".astc");
//		}
//	}


	//reading the input for block size/bitrate
	//If we are doing compression (0) or compress+decompress (2)
	if (opmode == 0 || opmode == 2)
	{
		//We need a minimum number of settings in order to compress correctly. 
		if(options->BitRate == 0 && options->xDimension == 0 && options->yDimension == 0)
		{
			return ASTC_INVALID_INPUT;
		}

		//We can either have a bitrate or block dimension to use.
		// The case where we specify a bitrate
		if (options->UseBitRate != 0)
		{
			target_bitrate = options->BitRate;
			target_bitrate_set = 1;
			find_closest_blockdim_2d(target_bitrate, &xdim_2d, &ydim_2d, DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES);
			find_closest_blockdim_3d(target_bitrate, &xdim_3d, &ydim_3d, &zdim_3d, DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES);
		}
		// The case where we specify a block size
		else
		{
			xdim_3d = options->xDimension;
			ydim_3d = options->yDimension;

			//if zdim == 1 then we want 2d, otherwise 3. 
			int dimensions = 2; //options->z_dim == 1 ? 2 : 3;
			
			switch (dimensions)
			{
			case 0:
			case 1:
				// failed to parse the blocksize argument at all.
				printf("Blocksize not specified\n");
				exit(1);
			case 2:
				{
					zdim_3d = 1;

					// Check 2D constraints
					if(!(xdim_3d ==4 || xdim_3d == 5 || xdim_3d == 6 || xdim_3d == 8 || xdim_3d == 10 || xdim_3d == 12) ||
					   !(ydim_3d ==4 || ydim_3d == 5 || ydim_3d == 6 || ydim_3d == 8 || ydim_3d == 10 || ydim_3d == 12) )
					{
						printf("Block dimensions %d x %d unsupported\n", xdim_3d, ydim_3d);
						exit(1);
					}

					int is_legal_2d = (xdim_3d==ydim_3d) || (xdim_3d==ydim_3d+1) || ((xdim_3d==ydim_3d+2) && !(xdim_3d==6 && ydim_3d==4)) ||
									  (xdim_3d==8 && ydim_3d==5) || (xdim_3d==10 && ydim_3d==5) || (xdim_3d==10 && ydim_3d==6);

					if(!DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES && !is_legal_2d)
					{
						printf("Block dimensions %d x %d disallowed\n", xdim_3d, ydim_3d);
						exit(1);
					}
				}
				break;

			default:
				{
					// Check 3D constraints
					if(xdim_3d < 3 || xdim_3d > 6 || ydim_3d < 3 || ydim_3d > 6 || zdim_3d < 3 || zdim_3d > 6)
					{
						printf("Block dimensions %d x %d x %d unsupported\n", xdim_3d, ydim_3d, zdim_3d);
						exit(1);
					}

					int is_legal_3d = ((xdim_3d==ydim_3d)&&(ydim_3d==zdim_3d)) || ((xdim_3d==ydim_3d+1)&&(ydim_3d==zdim_3d)) || ((xdim_3d==ydim_3d)&&(ydim_3d==zdim_3d+1));

					if(!DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES && !is_legal_3d)
					{
						printf("Block dimensions %d x %d x %d disallowed\n", xdim_3d, ydim_3d, zdim_3d);
						exit(1);
					}
				}
				break;
			}

			xdim_2d = xdim_3d;
			ydim_2d = ydim_3d;
		}

		log10_texels_2d = log((float)(xdim_2d * ydim_2d)) / log(10.0f);
		log10_texels_3d = log((float)(xdim_3d * ydim_3d * zdim_3d)) / log(10.0f);
	}

    //Add these options later. 
//	//Taking additional arguments and options. 
//	//We will do this is the same order as the original 
//	//  library, although the mechanism has changed. 	
//	if (options->silent_mode)
//	{
//		silentmode = 1;
//		set_suppress_progress_counter(1);
//	}
//	if (options->time_mode)
//	{
//		timemode = 1;
//	}
//
//	if(options->showpnsr)
//	{
//		psnrmode = 1;
//	}
//
//	if(options->v)
//	{
//		ewp.mean_stdev_radius			= options->v_radius;
//		ewp.rgb_power					= options->v_power;
//		ewp.rgb_base_weight				= options->v_baseweight;
//		ewp.rgb_mean_weight				= options->v_avgscale;
//		ewp.rgb_stdev_weight			= options->v_stdevscale;
//		ewp.rgb_mean_and_stdev_mixing	= options->v_mixing_factor;
//	}
//
//	if(options->va)
//	{
//		ewp.alpha_power					= options->va_power;
//		ewp.alpha_base_weight			= options->va_baseweight;
//		ewp.alpha_mean_weight			= options->va_avgscale;
//		ewp.alpha_stdev_weight			= options->va_stdevscale;
//	}
//
//	if(options->ch)
//	{
//		ewp.rgba_weights[0] = options->ch_red_weight;
//		ewp.rgba_weights[1] = options->ch_green_weight;
//		ewp.rgba_weights[2] = options->ch_blue_weight;
//		ewp.rgba_weights[3] = options->ch_alpha_weight;
//	}
//
//	if(options->a)
//	{
//		ewp.enable_rgb_scale_with_alpha = 1;
//		ewp.alpha_radius = options->a_radius;
//	}
//	
//	if(options->rn)
//	{
//		ewp.ra_normal_angular_scale = 1;	
//	}
//
//	if(options->b)
//	{
//		ewp.block_artifact_suppression = options->b_weighting;
//	}
//
//	if(options->esw)
//	{
//	//Let's add this option later
//		/*
//		argidx += 2;
//		if (argidx > argc)
//		{
//			printf("-esw switch with no argument\n");
//			exit(1);
//		}
//		if (strlen(argv[argidx - 1]) != 4)
//		{
//			printf("Swizzle pattern for the -esw switch must have exactly 4 characters\n");
//			exit(1);
//		}
//		int swizzle_components[4];
//		for (i = 0; i < 4; i++)
//			switch (argv[argidx - 1][i])
//				{
//				case 'r':
//					swizzle_components[i] = 0;
//					break;
//				case 'g':
//					swizzle_components[i] = 1;
//					break;
//				case 'b':
//					swizzle_components[i] = 2;
//					break;
//				case 'a':
//					swizzle_components[i] = 3;
//					break;
//				case '0':
//					swizzle_components[i] = 4;
//					break;
//				case '1':
//					swizzle_components[i] = 5;
//					break;
//				default:
//					printf("Character '%c' is not a valid swizzle-character\n", argv[argidx - 1][i]);
//					exit(1);
//				}
//		swz_encode.r = swizzle_components[0];
//		swz_encode.g = swizzle_components[1];
//		swz_encode.b = swizzle_components[2];
//		swz_encode.a = swizzle_components[3];
//		*/
//	}
//
//	if(options->dsw)
//	{
//		//Let's add this option later
//			/*
//			argidx += 2;
//			if (argidx > argc)
//			{
//				printf("-dsw switch with no argument\n");
//				exit(1);
//			}
//			if (strlen(argv[argidx - 1]) != 4)
//			{
//				printf("Swizzle pattern for the -dsw switch must have exactly 4 characters\n");
//				exit(1);
//			}
//			int swizzle_components[4];
//			for (i = 0; i < 4; i++)
//				switch (argv[argidx - 1][i])
//				{
//				case 'r':
//					swizzle_components[i] = 0;
//					break;
//				case 'g':
//					swizzle_components[i] = 1;
//					break;
//				case 'b':
//					swizzle_components[i] = 2;
//					break;
//				case 'a':
//					swizzle_components[i] = 3;
//					break;
//				case '0':
//					swizzle_components[i] = 4;
//					break;
//				case '1':
//					swizzle_components[i] = 5;
//					break;
//				case 'z':
//					swizzle_components[i] = 6;
//					break;
//				default:
//					printf("Character '%c' is not a valid swizzle-character\n", argv[argidx - 1][i]);
//					exit(1);
//				}
//			swz_decode.r = swizzle_components[0];
//			swz_decode.g = swizzle_components[1];
//			swz_decode.b = swizzle_components[2];
//			swz_decode.a = swizzle_components[3];
//			*/
//	}
	

	/***********************/
	/**Presets Begin Here***/
	/***********************/
    /*========== PSNR Optimization ==========*/
	if(options->ApplyPSNROptimization)
	{
		ewp.rgba_weights[0] = 1.0f;
		ewp.rgba_weights[1] = 0.0f;
		ewp.rgba_weights[2] = 0.0f;
		ewp.rgba_weights[3] = 1.0f;
		ewp.ra_normal_angular_scale = 1;
		swz_encode.r = 0;	// r <- red
		swz_encode.g = 0;	// g <- red
		swz_encode.b = 0;	// b <- red
		swz_encode.a = 1;	// a <- green
		swz_decode.r = 0;	// r <- red
		swz_decode.g = 3;	// g <- alpha
		swz_decode.b = 6;	// b <- reconstruct
		swz_decode.a = 5;	// 1.0
		oplimit_user_specified = 1000.0f;
		oplimit_set_by_user = 1;
		mincorrel_user_specified = 0.99f;
		mincorrel_set_by_user = 1;
	}
	
    /*========== Perceptual Optimization ==========*/
	if(options->ApplyPerceptualOptimization)
	{
		ewp.rgba_weights[0] = 1.0f;
		ewp.rgba_weights[1] = 0.0f;
		ewp.rgba_weights[2] = 0.0f;
		ewp.rgba_weights[3] = 1.0f;
		ewp.ra_normal_angular_scale = 1;
		swz_encode.r = 0;	// r <- red
		swz_encode.g = 0;	// g <- red
		swz_encode.b = 0;	// b <- red
		swz_encode.a = 1;	// a <- green
		swz_decode.r = 0;	// r <- red
		swz_decode.g = 3;	// g <- alpha
		swz_decode.b = 6;	// b <- reconstruct
		swz_decode.a = 5;	// 1.0

		oplimit_user_specified = 1000.0f;
		oplimit_set_by_user = 1;
		mincorrel_user_specified = 0.99f;
		mincorrel_set_by_user = 1;

		dblimit_user_specified = 999;
		dblimit_set_by_user = 1;

		ewp.block_artifact_suppression = 1.8f;
		ewp.mean_stdev_radius = 3;
		ewp.rgb_mean_weight = 0;
		ewp.rgb_stdev_weight = 50;
		ewp.rgb_mean_and_stdev_mixing = 0.0;
		ewp.alpha_mean_weight = 0;
		ewp.alpha_stdev_weight = 50;
	}

	if(options->ApplyMaskOptimization)
	{
		ewp.mean_stdev_radius = 3;
		ewp.rgb_mean_weight = 0.0f;
		ewp.rgb_stdev_weight = 25.0f;
		ewp.rgb_mean_and_stdev_mixing = 0.03f;
		ewp.alpha_mean_weight = 0.0f;
		ewp.alpha_stdev_weight = 25.0f;
	}

	if(options->ApplyAlphaOptimization)
	{
		ewp.enable_rgb_scale_with_alpha = 1;
		ewp.alpha_radius = 1;
	}

	if(options->ApplyHDRWithAlphaChannelOptimization)
	{
		if(decode_mode != DECODE_HDR)
		{
			printf("The option -hdra is only available in HDR mode\n");
			return ASTC_INVALID_INPUT;
		}
		else
		{
			ewp.mean_stdev_radius = 0;
			ewp.rgb_power = 0.75;
			ewp.rgb_base_weight = 0;
			ewp.rgb_mean_weight = 1;
			ewp.alpha_power = 0.75;
			ewp.alpha_base_weight = 0;
			ewp.alpha_mean_weight = 1;
			rgb_force_use_of_hdr = 1;
			alpha_force_use_of_hdr = 1;
			dblimit_user_specified = 999;
			dblimit_set_by_user = 1;
		}
	}

	if(options->ApplyHDROptimization)
	{
		if(decode_mode != DECODE_HDR)
		{
			printf("The option -hdr is only available in HDR mode\n");
			return ASTC_INVALID_INPUT;		
		}
		else
		{
			ewp.mean_stdev_radius = 0;
			ewp.rgb_power = 0.75;
			ewp.rgb_base_weight = 0;
			ewp.rgb_mean_weight = 1;
			ewp.alpha_base_weight = 0.05f;
			rgb_force_use_of_hdr = 1;
			alpha_force_use_of_hdr = 0;
			dblimit_user_specified = 999;
			dblimit_set_by_user = 1;
		}
	}
	
	if(options->ApplyHDRWithAlphaChannelForLogErrorOptimization)
	{
		if(decode_mode != DECODE_HDR)
		{
			printf("The option -hdra_log is only available in HDR mode\n");
			return ASTC_INVALID_INPUT;
		}
		else
		{
			ewp.mean_stdev_radius = 0;
			ewp.rgb_power = 1;
			ewp.rgb_base_weight = 0;
			ewp.rgb_mean_weight = 1;
			ewp.alpha_power = 1;
			ewp.alpha_base_weight = 0;
			ewp.alpha_mean_weight = 1;
			rgb_force_use_of_hdr = 1;
			alpha_force_use_of_hdr = 1;
			dblimit_user_specified = 999;
			dblimit_set_by_user = 1;
		}
	}

	if(options->ApplyHDRForLogErrorOptimization)
	{
		ewp.mean_stdev_radius = 0;
		ewp.rgb_power = 1;
		ewp.rgb_base_weight = 0;
		ewp.rgb_mean_weight = 1;
		ewp.alpha_base_weight = 0.05f;
		rgb_force_use_of_hdr = 1;
		alpha_force_use_of_hdr = 0;
		dblimit_user_specified = 999;
		dblimit_set_by_user = 1;
	}

	/***********************/
	/***Presets End Here****/
	/***********************/
	
    //We also will wait to implement these options. 
//	if(options->forcehdr_rgb)
//	{
//		if(decode_mode != DECODE_HDR)
//		{
//			printf("The option -forcehdr_rgb is only available in HDR mode\n");
//			return ASTC_INVALID_INPUT;
//		}
//		else
//		{
//			rgb_force_use_of_hdr = 1;
//		}
//	}
//
//	if(options->forcehdr_rgba)
//	{
//		if(decode_mode != DECODE_HDR)
//		{
//			printf("The option -forcehdr_rgbs is only available in HDR mode\n");
//			return ASTC_INVALID_INPUT;
//		}
//		else
//		{
//			rgb_force_use_of_hdr = 1;
//			alpha_force_use_of_hdr = 1;
//		}
//	}
//
//	if(options->bmc)
//	{
//		float cutoff = options->bmc_value;
//		if (cutoff > 100 || !(cutoff >= 0))
//			cutoff = 100;
//		bmc_user_specified = cutoff;
//		bmc_set_by_user = 1;
//	}
//
//	if(options->plimit)
//	{
//		plimit_user_specified = options->plimit_number;
//		plimit_set_by_user = 1;
//	}
//	
//	if(options->dblimit)
//	{
//		dblimit_user_specified = options->dblimit_number;
//		dblimit_set_by_user = 1;
//	}
//
//	if(options->oplimit)
//	{
//		oplimit_user_specified = options->oplimit_factor;
//		oplimit_set_by_user = 1;
//	}
//
//	if(options->mincorrel)
//	{
//		mincorrel_user_specified = options->mincorrel_value;
//		mincorrel_set_by_user = 1;
//	}
//
//	if(options->maxiters)
//	{
//		maxiters_user_specified = options->maxiters_value;
//		maxiters_set_by_user = 1;
//	}

	switch(options->CompressionSpeed)
	{
	case ASTC_VERY_FAST:
		{
			plimit_autoset = 2;
			oplimit_autoset = 1.0;
			dblimit_autoset_2d = MAX(70 - 35 * log10_texels_2d, 53 - 19 * log10_texels_2d);
			dblimit_autoset_3d = MAX(70 - 35 * log10_texels_3d, 53 - 19 * log10_texels_3d);
			bmc_autoset = 25;
			mincorrel_autoset = 0.5;
			maxiters_autoset = 1;

			switch (ydim_2d)
			{
			case 4:
				pcdiv = 240;
				break;
			case 5:
				pcdiv = 56;
				break;
			case 6:
				pcdiv = 64;
				break;
			case 8:
				pcdiv = 47;
				break;
			case 10:
				pcdiv = 36;
				break;
			case 12:
				pcdiv = 30;
				break;
			default:
				pcdiv = 30;
				break;
			};
			preset_has_been_set++;
			break;
		}
	case ASTC_FAST:
		{
			plimit_autoset = 4;
			oplimit_autoset = 1.0;
			mincorrel_autoset = 0.5;
			dblimit_autoset_2d = MAX(85 - 35 * log10_texels_2d, 63 - 19 * log10_texels_2d);
			dblimit_autoset_2d = MAX(85 - 35 * log10_texels_3d, 63 - 19 * log10_texels_3d);
			bmc_autoset = 50;
			maxiters_autoset = 1;

			switch (ydim_2d)
			{
			case 4:
				pcdiv = 60;
				break;
			case 5:
				pcdiv = 27;
				break;
			case 6:
				pcdiv = 30;
				break;
			case 8:
				pcdiv = 24;
				break;
			case 10:
				pcdiv = 16;
				break;
			case 12:
				pcdiv = 20;
				break;
			default:
				pcdiv = 20;
				break;
			};
			preset_has_been_set++;
			break;
		}
	case ASTC_MEDIUM:
		{
			plimit_autoset = 25;
			oplimit_autoset = 1.2f;
			mincorrel_autoset = 0.75f;
			dblimit_autoset_2d = MAX(95 - 35 * log10_texels_2d, 70 - 19 * log10_texels_2d);
			dblimit_autoset_3d = MAX(95 - 35 * log10_texels_3d, 70 - 19 * log10_texels_3d);
			bmc_autoset = 75;
			maxiters_autoset = 2;

			switch (ydim_2d)
			{
			case 4:
				pcdiv = 25;
				break;
			case 5:
				pcdiv = 15;
				break;
			case 6:
				pcdiv = 15;
				break;
			case 8:
				pcdiv = 10;
				break;
			case 10:
				pcdiv = 8;
				break;
			case 12:
				pcdiv = 6;
				break;
			default:
				pcdiv = 6;
				break;
			};
			preset_has_been_set++;
			break;
		}
	case ASTC_THOROUGH:
		{
			plimit_autoset = 100;
			oplimit_autoset = 2.5f;
			mincorrel_autoset = 0.95f;
			dblimit_autoset_2d = MAX(105 - 35 * log10_texels_2d, 77 - 19 * log10_texels_2d);
			dblimit_autoset_3d = MAX(105 - 35 * log10_texels_3d, 77 - 19 * log10_texels_3d);
			bmc_autoset = 95;
			maxiters_autoset = 4;

			switch (ydim_2d)
			{
			case 4:
				pcdiv = 12;
				break;
			case 5:
				pcdiv = 7;
				break;
			case 6:
				pcdiv = 7;
				break;
			case 8:
				pcdiv = 5;
				break;
			case 10:
				pcdiv = 4;
				break;
			case 12:
				pcdiv = 3;
				break;
			default:
				pcdiv = 3;
				break;
			};
			preset_has_been_set++;
			break;
		}
	case ASTC_EXHAUSTIVE:
		{
			plimit_autoset = PARTITION_COUNT;
			oplimit_autoset = 1000.0f;
			mincorrel_autoset = 0.99f;
			dblimit_autoset_2d = 999.0f;
			dblimit_autoset_3d = 999.0f;
			bmc_autoset = 100;
			maxiters_autoset = 4;

			preset_has_been_set++;
			switch (ydim_2d)
			{
			case 4:
				pcdiv = 3;
				break;
			case 5:
				pcdiv = 1;
				break;
			case 6:
				pcdiv = 1;
				break;
			case 8:
				pcdiv = 1;
				break;
			case 10:
				pcdiv = 1;
				break;
			case 12:
				pcdiv = 1;
				break;
			default:
				pcdiv = 1;
				break;
			}
			break;
		}
	}

    //And wait to implement this as well. 
//	if(options->j)
//	{
//		thread_count = options->j_numthreads;
//	}
//
//	if(options->srgb)
//	{
//		perform_srgb_transform = 1;
//		dblimit_user_specified = 60;
//		dblimit_set_by_user = 1;
//	}
//
//	if(options->mpsnr)
//	{
//		low_fstop = options->mpsnr_low;
//		high_fstop = options->mpsnr_high;
//		if (high_fstop < low_fstop)
//		{
//			printf("For -mpsnr switch, the <low> argument cannot be greater than the\n" "high argument.\n");
//			return ASTC_INVALID_INPUT;
//		}
//	}
//
//	if(options->bmstat)
//	{
//		print_block_mode_histogram = 1;
//	}
//
//	if(options->pte)
//	{
//		print_tile_errors = 1;
//	}
//
//	if(options->stats)
//	{
//		print_statistics = 1;
//	}
//
//	if(options->uarray)
//	{
//		// Only supports compressing (not decompressing or comparison).
//		if (opmode != 0)
//		{
//			printf("-array switch given when not compressing files - decompression and comparison of arrays not supported.\n");
//			return ASTC_INVALID_INPUT;
//		}
//		array_size = options->uarray_size;
//	}

	float texel_avg_error_limit_2d = 0.0f;
	float texel_avg_error_limit_3d = 0.0f;

	// The rest of the setup
	//  At this point, we have read in all the inputs and done appropriate calculations
	if (opmode == 0 || opmode == 2)
	{
		// if encode, process the parsed commandline values

		if (preset_has_been_set != 1)
		{
			printf("For encoding, need to specify exactly one performace-quality\n"
				   "tradeoff preset option. The available presets are:\n" " -veryfast\n" " -fast\n" " -medium\n" " -thorough\n" " -exhaustive\n");
			exit(1);
		}

		set_progress_counter_divider(pcdiv);

		int partitions_to_test = plimit_set_by_user ? plimit_user_specified : plimit_autoset;
		float dblimit_2d = dblimit_set_by_user ? dblimit_user_specified : dblimit_autoset_2d;
		float dblimit_3d = dblimit_set_by_user ? dblimit_user_specified : dblimit_autoset_3d;
		float oplimit = oplimit_set_by_user ? oplimit_user_specified : oplimit_autoset;
		float mincorrel = mincorrel_set_by_user ? mincorrel_user_specified : mincorrel_autoset;

		int maxiters = maxiters_set_by_user ? maxiters_user_specified : maxiters_autoset;
		ewp.max_refinement_iters = maxiters;

		ewp.block_mode_cutoff = (bmc_set_by_user ? bmc_user_specified : bmc_autoset) / 100.0f;

		if (rgb_force_use_of_hdr == 0)
		{
			texel_avg_error_limit_2d = pow(0.1f, dblimit_2d * 0.1f) * 65535.0f * 65535.0f;
			texel_avg_error_limit_3d = pow(0.1f, dblimit_3d * 0.1f) * 65535.0f * 65535.0f;
		}
		else
		{
			texel_avg_error_limit_2d = 0.0f;
			texel_avg_error_limit_3d = 0.0f;
		}
		ewp.partition_1_to_2_limit = oplimit;
		ewp.lowest_correlation_cutoff = mincorrel;

		if (partitions_to_test < 1)
			partitions_to_test = 1;
		else if (partitions_to_test > PARTITION_COUNT)
			partitions_to_test = PARTITION_COUNT;
		ewp.partition_search_limit = partitions_to_test;

//		int diagnostics;
		// if diagnostics are run, force the thread count to 1.
		if (
			#ifdef DEBUG_PRINT_DIAGNOSTICS
			   //(diagnostics = get_diagnostics_tile()) >= 0 ||
				0 ||
			#endif
			   print_tile_errors > 0 || print_statistics > 0)
		{
			thread_count = 1;
			thread_count_autodetected = 0;
		}

		if (thread_count < 1)
		{
			thread_count = get_number_of_cpus();
			thread_count_autodetected = 1;
		}


		// Specifying the error weight of a color component as 0 is not allowed.
		// If weights are 0, then they are instead set to a small positive value.

		float max_color_component_weight = MAX(MAX(ewp.rgba_weights[0], ewp.rgba_weights[1]),
											   MAX(ewp.rgba_weights[2], ewp.rgba_weights[3]));
		ewp.rgba_weights[0] = MAX(ewp.rgba_weights[0], max_color_component_weight / 1000.0f);
		ewp.rgba_weights[1] = MAX(ewp.rgba_weights[1], max_color_component_weight / 1000.0f);
		ewp.rgba_weights[2] = MAX(ewp.rgba_weights[2], max_color_component_weight / 1000.0f);
		ewp.rgba_weights[3] = MAX(ewp.rgba_weights[3], max_color_component_weight / 1000.0f);


		// print all encoding settings unless specifically told otherwise.
		if (!silentmode)
		{
			printf("Encoding settings:\n\n");
			if (target_bitrate_set)
				printf("Target bitrate provided: %.2f bpp\n", target_bitrate);
			printf("2D Block size: %dx%d (%.2f bpp)\n", xdim_2d, ydim_2d, 128.0 / (xdim_2d * ydim_2d));
			printf("3D Block size: %dx%dx%d (%.2f bpp)\n", xdim_3d, ydim_3d, zdim_3d, 128.0 / (xdim_3d * ydim_3d * zdim_3d));
			printf("Radius for mean-and-stdev calculations: %d texels\n", ewp.mean_stdev_radius);
			printf("RGB power: %g\n", ewp.rgb_power);
			printf("RGB base-weight: %g\n", ewp.rgb_base_weight);
			printf("RGB local-mean weight: %g\n", ewp.rgb_mean_weight);
			printf("RGB local-stdev weight: %g\n", ewp.rgb_stdev_weight);
			printf("RGB mean-and-stdev mixing across color channels: %g\n", ewp.rgb_mean_and_stdev_mixing);
			printf("Alpha power: %g\n", ewp.alpha_power);
			printf("Alpha base-weight: %g\n", ewp.alpha_base_weight);
			printf("Alpha local-mean weight: %g\n", ewp.alpha_mean_weight);
			printf("Alpha local-stdev weight: %g\n", ewp.alpha_stdev_weight);
			printf("RGB weights scale with alpha: ");
			if (ewp.enable_rgb_scale_with_alpha)
				printf("enabled (radius=%d)\n", ewp.alpha_radius);
			else
				printf("disabled\n");
			printf("Color channel relative weighting: R=%g G=%g B=%g A=%g\n", ewp.rgba_weights[0], ewp.rgba_weights[1], ewp.rgba_weights[2], ewp.rgba_weights[3]);
			printf("Block-artifact suppression parameter : %g\n", ewp.block_artifact_suppression);
			printf("Number of distinct partitionings to test: %d (%s)\n", ewp.partition_search_limit, plimit_set_by_user ? "specified by user" : "preset");
			printf("PSNR decibel limit: 2D: %f 3D: %f (%s)\n", dblimit_2d, dblimit_3d, dblimit_set_by_user ? "specified by user" : "preset");
			printf("1->2 partition limit: %f\n", oplimit);
			printf("Dual-plane color-correlation cutoff: %f (%s)\n", mincorrel, mincorrel_set_by_user ? "specified by user" : "preset");
			printf("Block Mode Percentile Cutoff: %f (%s)\n", ewp.block_mode_cutoff * 100.0f, bmc_set_by_user ? "specified by user" : "preset");
			printf("Max refinement iterations: %d (%s)\n", ewp.max_refinement_iters, maxiters_set_by_user ? "specified by user" : "preset");
			printf("Thread count : %d (%s)\n", thread_count, thread_count_autodetected ? "autodetected" : "specified by user");

			printf("\n");
		}

	}

	int bitness;
	int padding = MAX(ewp.mean_stdev_radius, ewp.alpha_radius);

	//Qonvert is going to pick our bit fidelity!
	if(options->BitFidelity == ASTC_8_BIT)
	{
		bitness = 8;
	}
	else if(options->BitFidelity == ASTC_16_BIT)
	{
		bitness = 16;
	}
	else if(options->BitFidelity == ASTC_32_BIT)
	{
		bitness = -1;
		printf("We do not support 32 bitness in ASTC\n");
		return ASTC_MAIN_FAILURE;
	}
	else 
	{
		bitness = -1;
		printf("Unsupported bit fidelity\n");
		return ASTC_MAIN_FAILURE;
	}


	xdim = ydim = zdim = -1;
	
	// Temporary image array (for merging multiple 2D images into one 3D image).
	load_result = 0;
	input_components = 0;
	input_image_is_hdr = 0;
	load_results = NULL;
	input_image = NULL;
	output_image = NULL;
	input_images = NULL;
	
	// load image
	if (opmode == 0 || opmode == 2 || opmode == 3)
	{
		// Allocate arrays for image data and load results.
		load_results = (int*)malloc(array_size*sizeof(int));
		input_images = (astc_codec_image **)malloc(sizeof(astc_codec_image *)*array_size);

		// Iterate over all input images.
		for (int image_index = 0; image_index < array_size; image_index++)
		{
			// 2D input data.
			if (array_size == 1)
			{
                //gpittsgpitts
				input_images[image_index] = astc_codec_load_image( padding, &load_results[image_index], (void *)options);
			}

			// 3D input data - multiple 2D images.
			else
			{
				char new_input_filename[256];

				// Check for extension: <name>.<extension>
				if (NULL == strrchr(input_filename, '.'))
				{
					printf("Unable to determine file type from extension: %s\n", input_filename);
					exit(1);
				}

				// Construct new file name and load: <name>_N.<extension>
				strcpy(new_input_filename, input_filename);
				sprintf(strrchr(new_input_filename, '.'), "_%d%s", image_index, strrchr(input_filename, '.'));
				input_images[image_index] = astc_codec_load_image( padding, &load_results[image_index]);

				// Check image is not 3D.
				if (input_images[image_index]->zsize != 1)
				{
					printf("3D source images not supported with -array option: %s\n", new_input_filename);
					exit(1);
				}

				// BCJ(DEBUG)
				// printf("\n\n Image %d \n", image_index);
				// dump_image( input_images[image_index] );
				// printf("\n\n");
			}

			// Check load result.
			if (load_results[image_index] < 0)
			{
				printf("Failed to load image %s\n", input_filename);
                return ASTC_ERROR_CODE;
			}

			// Check format matches other slices.
			if (load_results[image_index] != load_results[0])
			{
				printf("Mismatching image format - image 0 and %d are a different format\n", image_index);
				return ASTC_ERROR_CODE;
			}
		}

		load_result = load_results[0];

		// Assign input image.
		if (array_size == 1)
		{
			input_image = input_images[0];
		}

		// Merge input image data.
		else
		{
			int i, z, xsize, ysize, zsize, bitness, slice_size;

			xsize = input_images[0]->xsize;
			ysize = input_images[0]->ysize;
			zsize = array_size;
			bitness = (load_result & 0x80) ? 16 : 8;
			slice_size = (xsize + (2 * padding)) * (ysize + (2 * padding));

			// Allocate image memory.
			input_image = allocate_image(bitness, xsize, ysize, zsize, padding);

			// Combine 2D source images into one 3D image (skip padding slices as these don't exist in 2D textures).
			for (z = padding; z < zsize + padding; z++)
			{
				if (bitness == 8)
				{
					memcpy(*input_image->imagedata8[z], *input_images[z - padding]->imagedata8[0], slice_size * 4 * sizeof(uint8_t));
				}
				else
				{
					memcpy(*input_image->imagedata16[z], *input_images[z - padding]->imagedata16[0], slice_size * 4 * sizeof(uint16_t));
				}
			}

			// Clean up temporary images.
			for (i = 0; i < array_size; i++)
			{
				destroy_image(input_images[i]);
			}
			input_images = NULL;

			// Clamp texels outside the actual image area.
			fill_image_padding_area(input_image);

			// BCJ(DEBUG)
			// dump_image( input_image );
		}

		input_components = load_result & 7;
		input_image_is_hdr = (load_result & 0x80) ? 1 : 0;

		if (input_image->zsize > 1)
		{
			xdim = xdim_3d;
			ydim = ydim_3d;
			zdim = zdim_3d;
			ewp.texel_avg_error_limit = texel_avg_error_limit_3d;
		}
		else
		{
			xdim = xdim_2d;
			ydim = ydim_2d;
			zdim = 1;
			ewp.texel_avg_error_limit = texel_avg_error_limit_2d;
		}
		expand_block_artifact_suppression(xdim, ydim, zdim, &ewp);


		if (!silentmode)
		{
			printf(" %dD %s image, %d x %d x %d, %d components\n\n",
				   input_image->zsize > 1 ? 3 : 2, input_image_is_hdr ? "HDR" : "LDR", input_image->xsize, input_image->ysize, input_image->zsize, load_result & 7);
		}

		if (padding > 0 || ewp.rgb_mean_weight != 0.0f || ewp.rgb_stdev_weight != 0.0f || ewp.alpha_mean_weight != 0.0f || ewp.alpha_stdev_weight != 0.0f)
		{
			if (!silentmode)
			{
				printf("Computing texel-neigborhood means and variances ... ");
				fflush(stdout);
			}
			compute_averages_and_variances(input_image, ewp.rgb_power, ewp.alpha_power, ewp.mean_stdev_radius, ewp.alpha_radius, swz_encode);
			if (!silentmode)
			{
				printf("done\n");
				fflush(stdout);
			}
		}
	}

	set_start_coding_time(get_time());
//	start_coding_time = get_time();

	if (opmode == 1)
		output_image = load_astc_file( bitness, decode_mode, swz_decode, (void *)options);


	// process image, if relevant
	if (opmode == 2)
		output_image = pack_and_unpack_astc_image(input_image, xdim, ydim, zdim, &ewp, decode_mode, swz_encode, swz_decode, bitness, thread_count);

	set_end_coding_time(get_time());


	// print PSNR if encoding
	if (opmode == 2)
	{
		if (psnrmode == 1)
		{
			compute_error_metrics(input_image_is_hdr, input_components, input_image, output_image, low_fstop, high_fstop, psnrmode);
		}
	}


	// store image
	if (opmode == 1 || opmode == 2)
	{
		int store_result = -1;
		const char *format_string = "N/A";

		store_result = astc_codec_store_image(output_image,  bitness, (void *)options);

		if (store_result < 0)
		{
			printf("Failed to store image %s\n", output_filename);
			exit(1);
		}
		else
		{
			if (!silentmode)
			{
				printf("Stored %s image %s with %d color channels\n", format_string, output_filename, store_result);
			}
		}
	}
	
	// If we are compressing the iamge. 
	if (opmode == 0)
	{
		store_astc_file(input_image,  xdim, ydim, zdim, &ewp, decode_mode, swz_encode, 1, options);
	}


	if (timemode)
	{
		printf("\nElapsed time: %.2lf seconds, of which coding time: %.2lf seconds\n", get_end_time() - get_start_time(), get_end_coding_time() - get_start_coding_time());
	}

	printf("\n");
	return 0;
	
}




/** init_astc_options
 * 
 * Let's just avoid all issues and initialize 
 * everything to zero in one function. 
 *
 * Of course, there is no need to initialize 
 * lots of the settings because they won't be used 
 * without the options being enabled. 
 *
 * @param options - A pointer to the astc_options 
 *					struct we are using. 
 *	
 * @return - void
 */
//void init_astc_options(astc_options *options){
//	int i;
//	options->performance_quality_tradeoff	=  0;
//	options->x_dim							=  0;
//	options->y_dim							=  0;
//	options->z_dim							=  0;
//	options->silent_mode					=  0;
//	options->time_mode						=  0;
//	options->compression_submode			=  0;
//	options->astc_command					=  0;
//	options->bitrate						=  0.0;
//	options->astc_pSrc						= NULL;
//	options->astc_pDest						= NULL;
//	options->astc_pData						= NULL;
//	options->bit_fidelity					= 0;
//	options->QFormat						= 0;
//
//	options->save_output_files				= 0;
//	options->testnum						= 0;
//	for(i=0;i<80;i++)
//		options->outputfile[i]				= 0;
//	options->isQcompress					= 0;
//
////Built in error weighting presets
////--------------------------------
//	options->normal_psnr					= 0;
//	options->normal_percep					= 0;
//	options->mask							= 0;
//	options->alphablend						= 0;
//	options->hdr							= 0;
//	options->hdra							= 0;
//	options->hdr_log						= 0;
//	options->hdra_log						= 0;
//
////Low Level error weighting options:
////----------------------------------
//	options->v								= 0;
//	options->va								= 0;
//	options->a								= 0;
//	options->ch								= 0;
//	options->rn								= 0;
//	options->b								= 0;
//
////Low Level Performance Quality Tradeoff Options
////----------------------------------------------
//	options->plimit							= 0;
//	options->dblimit						= 0;
//	options->oplimit						= 0;
//	options->mincorrel						= 0;
//	options->bmc							= 0;
//	options->maxiters						= 0;
//	
////Other Options
////-------------
//	options->uarray							= 0;
//	options->forcehdr_rgb					= 0;
//	options->forcehdr_rgba					= 0;
//	options->esw							= 0;
//	options->srgb							= 0;
//	options->j								= 0;
//	options->silentmode						= 0;
//	options->time							= 0;
//	options->showpnsr						= 0;
//	options->mpsnr							= 0;
//
//	options->diag							= 0;
//	options->bmstat							= 0;			
//	options->pte							= 0;
//	options->stats							= 0;
//}
