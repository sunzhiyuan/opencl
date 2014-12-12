/*======================================================================
*  FILE:
*      reference_transform.cpp
*  DESCRIPTION:
*      Reference code for wavelet transform application
*
*            Copyright (c) 2012 QUALCOMM Incorporated.
*                      All Rights Reserved.
*              QUALCOMM Confidential and Proprietary
*
*======================================================================*/

/**
* \addtogroup wavelettransform
* \{
* \file wavelettransform\reference_transform.cpp
* \brief Reference code for wavelet transform
*/
#include "common.h"

static const int filter_half_length = 2;
static const int filter_length = 5;
static const int haar_filter_length = 2;
static const int image_q_factor = 0;
static const int parameter_q_factor = 15;
static const int image_roundoff = 0;
static const int parameter_roundoff = (1 << (parameter_q_factor - 1));

int max( int a,
        int b )
{
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int min( int a,
        int b )
{
    if (a > b) {
        return b;
    } else {
        return a;
    }
}

void fdwt1d_haar( int      *input,
                 int      *output,
                 const int length )
{
    /*
    float l[2] = {
    sqrt(0.5), sqrt(0.5)
    };

    float h[5] = {
    sqrt(0.5), -sqrt(0.5)
    };
    */
    // Q15
    const int l[] = {
        23170, 23170
    };

    // Q15
    const int h[] = {
        23170, -23170
    };
    int sum;
    int buffer_index = 0;
    int i, j;

    input[length] = input[length - 1];

    for (i = 0; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < haar_filter_length; j++)
        {
            sum += input[i + j] * l[j];
        }
        output[buffer_index++] = (sum + (1 << 14)) >> 15;
    }

    for (i = 1; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < haar_filter_length; j++)
        {
            sum += input[i - 1 + j] * h[j];
        }
        output[buffer_index++] = (sum + (1 << 14)) >> 15;
    }
}

void idwt1d_haar( int      *input,
                 int      *output,
                 const int length )
{
    /*synthesis polyphase filter */
    const int l[] = {
        23170, 23170
    };

    const int h[] = {
        23170, -23170
    };

    /* synthesis high and low pass filter */
    /*
    float h[2] = {
    sqrt(0.5), sqrt(0.5)
    };

    float l[5] = {
    sqrt(0.5), -sqrt(0.5)
    };
    */
    int buffer_index = 0;
    int i, j;
    int sum;

    for (i = 0; i < length; i += 2)
    {
        input[i] = output[buffer_index++];
    }
    for (i = 1; i < length; i += 2)
    {
        input[i] = output[buffer_index++];
    }

    input[length] = 0;

    for (i = 0; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < haar_filter_length; j++)
        {
            sum += input[i + j] * l[j];
        }
        output[i] = (sum + (1 << 14)) >> 15;
    }
    for (i = 1; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < haar_filter_length; j++)
        {
            sum += input[i - 1 + j] * h[j];
        }
        output[i] = (sum + (1 << 14)) >> 15;
    }
}

void fdwt2d_haar( int      *channel,
                 const int image_width,
                 const int image_height,
                 const int stride )
{
    int *input;
    int *output;
    int i, j;
    if (image_width > image_height)
    {
        input = (int *) malloc (sizeof (int) * (image_width + 1));
        output = (int *) malloc (sizeof (int) * image_width);
    }
    else
    {
        input = (int *) malloc (sizeof (int) * (image_height + 1));
        output = (int *) malloc (sizeof (int) * image_height);
    }
    for (i = 0; i < image_height; i++)
    {
        for (j = 0; j < image_width; j++)
        {
            input[j] = channel[i * stride + j];
        }
        fdwt1d_haar (input, output, image_width);
        for (j = 0; j < image_width; j++)
        {
            channel[i * stride + j] = output[j];
        }
    }
    for (j = 0; j < image_width; j++)
    {
        for (i = 0; i < image_height; i++)
        {
            input[i] = channel[i * stride + j];
        }
        fdwt1d_haar (input, output, image_height);
        for (i = 0; i < image_height; i++)
        {
            channel[i * stride + j] = output[i];
        }
    }
    free (input);
    free (output);

}

void idwt2d_haar ( int      *channel,
                  const int image_width,
                  const int image_height,
                  const int stride )
{
    int *input;
    int *output;
    int i, j;

    if (image_width > image_height)
    {
        input = (int *) malloc (sizeof (int) * (image_width + 1));
        output = (int *) malloc (sizeof (int) * image_width);
    }
    else
    {
        input = (int *) malloc (sizeof (int) * (image_height + 1));
        output = (int *) malloc (sizeof (int) * image_height);
    }

    for (j = 0; j < image_width; j++)
    {
        for (i = 0; i < image_height; i++)
        {
            output[i] = channel[i * stride + j];
        }
        idwt1d_haar (input, output, image_height);
        for (i = 0; i < image_height; i++)
        {
            channel[i * stride + j] = output[i];
        }
    }

    for (i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            output[j] = channel[i * stride + j];
        }
        idwt1d_haar (input, output, image_width);
        for (int j = 0; j < image_width; j++)
        {
            channel[i * stride + j] = output[j];
        }
    }
    free (input);
    free (output);


}

int clamp ( int input )
{
    if (input > 255)
        return 255;
    if (input < 0)
        return 0;
    else
        return input;
}

void fdwt1d ( int      *input,
             int      *output,
             const int length )
{
    /*
    float l[5] = {
    -1./8, 1./4, 3./4, 1./4, -1./8
    };

    float h[5] = {
    0          , -1./2, 1., -1./2, 0
    };
    */
    // Q14
    const int l[] = {
        -2896, 5793, 17378, 5793, -2896
    };

    // Q15
    const int h[] = {
        0, -11585, 23170, -11585, 0
    };
    int sum;
    int buffer_index = 0;
    int i, j;

    input[0] = input[4];
    input[1] = input[3];
    input[length + 2] = input[length + 0];
    input[length + 3] = input[length - 1];

    for (i = 0; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < filter_length; j++)
        {
            sum += input[i + j] * l[j];
        }
        output[buffer_index++] = (sum + (1 << 13)) >> 14;
    }

    for (i = 1; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < filter_length; j++)
        {
            sum += input[i + j] * h[j];
        }
        output[buffer_index++] = (sum + (1 << 14)) >> 15;
    }
}

void fdwt2d ( int      *channel,
             const int image_width,
             const int image_height,
             const int stride )
{
    int *input;
    int *output;
    int i, j;
    if (image_width > image_height)
    {
        input =
            (int *) malloc (sizeof (int) *
            (image_width + 2 * filter_half_length));
        output = (int *) malloc (sizeof (int) * image_width);
    }
    else
    {
        input =
            (int *) malloc (sizeof (int) *
            (image_height + 2 * filter_half_length));
        output = (int *) malloc (sizeof (int) * image_height);
    }
    for (i = 0; i < image_height; i++)
    {
        for (j = 0; j < image_width; j++)
        {
            input[j + filter_half_length] = channel[i * stride + j];
        }
        fdwt1d (input, output, image_width);
        for (j = 0; j < image_width; j++)
        {
            channel[i * stride + j] = output[j];
        }
    }
    for (j = 0; j < image_width; j++)
    {
        for (i = 0; i < image_height; i++)
        {
            input[i + filter_half_length] = channel[i * stride + j];
        }
        fdwt1d (input, output, image_height);
        for (i = 0; i < image_height; i++)
        {
            channel[i * stride + j] = output[i];
        }
    }
    free (input);
    free (output);

}


void idwt1d ( int *input,
             int *output,
             const int length )
{
    /*synthesis polyphase filter */
    const int l[] = {
        0, -11585, 23170, -11585, 0
    };

    const int h[] = {
        -2896, 5793, 17378, 5793, -2896
    };

    /* synthesis high and low pass filter */
    /*
    float h[5] = {
    -1./8, -1./4, 3./4, -1./4, -1./8
    };

    float l[5] = {
    0          , 1./2, 1., 1./2, 0
    };
    */
    int buffer_index = 0;
    int i, j;
    int sum;

    for (i = 0; i < length; i += 2)
    {
        input[i + filter_half_length] = output[buffer_index++];
    }
    for (i = 1; i < length; i += 2)
    {
        input[i + filter_half_length] = output[buffer_index++];
    }
    input[0] = input[4];
    input[1] = input[3];

    input[length + 2] = input[length + 0];
    input[length + 3] = input[length - 1];

    for (i = 0; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < filter_length; j++)
        {
            sum += input[i + j] * l[j];
        }
        output[i] = (sum + (1 << 14)) >> 15;

    }
    for (i = 1; i < length; i += 2)
    {
        sum = 0;
        for (j = 0; j < filter_length; j++)
        {
            sum += input[i + j] * h[j];
        }
        output[i] = (sum + (1 << 13)) >> 14;
    }
}

void idwt2d( int      *channel,
            const int image_width,
            const int image_height,
            const int stride )
{
    int *input;
    int *output;
    int i, j;

    if (image_width > image_height)
    {
        input =
            (int *) malloc (sizeof (int) *
            (image_width + 2 * filter_half_length));
        output = (int *) malloc (sizeof (int) * image_width);
    }
    else
    {
        input =
            (int *) malloc (sizeof (int) *
            (image_height + 2 * filter_half_length));
        output = (int *) malloc (sizeof (int) * image_height);
    }

    for (j = 0; j < image_width; j++)
    {
        for (i = 0; i < image_height; i++)
        {
            output[i] = channel[i * stride + j];
        }
        idwt1d (input, output, image_height);
        for (i = 0; i < image_height; i++)
        {
            channel[i * stride + j] = output[i];
        }
    }

    for (i = 0; i < image_height; i++)
    {
        for (int j = 0; j < image_width; j++)
        {
            output[j] = channel[i * stride + j];
        }
        idwt1d (input, output, image_width);
        for (int j = 0; j < image_width; j++)
        {
            channel[i * stride + j] = output[j];
        }
    }
    free (input);
    free (output);
}

int reference_transform(unsigned short level,
                        int            input_width,
                        int            input_height,
                        int           *y_image,
                        int           *cb_image,
                        int           *cr_image)

{
    int haar_level = 1;

    int level_height[6];
    int level_width[6];

    level_height[0] = input_height;
    level_width[0] = input_width;

    for (int i = 0; i < level; i++)
    {
        level_width[i + 1] = (level_width[i] + 1) >> 1;
        level_height[i + 1] = (level_height[i] + 1) >> 1;
    }

    for (int i = 0; i < haar_level; i++)
    {
        fdwt2d_haar (y_image, level_width[i], level_height[i], input_width);

        int cbcr_width  = level_width[i];

        int cbcr_height = level_height[i];

        int cbcr_stride = input_width;

        fdwt2d_haar (cb_image, cbcr_width, cbcr_height, cbcr_stride);

        fdwt2d_haar (cr_image, cbcr_width, cbcr_height, cbcr_stride);
    }

    for (int i = haar_level; i < level; i++)
    {
        fdwt2d (y_image, level_width[i], level_height[i], input_width);

        int cbcr_width = level_width[i];

        int cbcr_height = level_height[i];

        int cbcr_stride = input_width;

        fdwt2d (cb_image, cbcr_width, cbcr_height, cbcr_stride);

        fdwt2d (cr_image,  cbcr_width, cbcr_height, cbcr_stride);
    }

    for (int i = level - 1; i >= haar_level; i--)
    {
        idwt2d (y_image, level_width[i], level_height[i], input_width);

        idwt2d (cb_image, level_width[i], level_height[i], input_width);
        idwt2d (cr_image, level_width[i], level_height[i], input_width);
    }

    for (int i = haar_level - 1; i >= 0; i--)
    {
        idwt2d_haar (y_image, level_width[i], level_height[i], input_width);
        idwt2d_haar (cb_image, level_width[i], level_height[i], input_width);
        idwt2d_haar (cr_image, level_width[i], level_height[i], input_width);
    }

    return 0;
}
/** \} */
