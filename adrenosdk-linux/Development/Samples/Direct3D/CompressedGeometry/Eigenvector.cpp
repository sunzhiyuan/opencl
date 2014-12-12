//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include "FrmMath.h"


void tri_diag(double a[], double d[], double e[], double z[], int n,double tol)
{
	int i,j,k,l;
	double f;
	double g;
	double h;
	double hh;

	for( i = 1; i <= n; i++ )
	{
		for( j = 1; j <= i; j++ )
		{
			z[(i-1)*n+j] = a[(i-1)*n+j];
		};
	};

	for( i = n; i >= 2; i--)
	{
		l = i-2;
		f = z[(i-1)*n + i-1];

		g = 0.;
		for( k = 1; k <= l; k++ )
		{
			g += z[(i-1)*n+k]*z[(i-1)*n+k];
		};
		h = g + f*f;

		if( g <= tol )
		{
			e[i] = f;
			h = 0.;
			goto skip;
		};

		l++;
		if( f >= 0. )
		{
			g = e[i] = -sqrt(h);
		}
		else
		{
			g = e[i] = sqrt(h);
		};
		h = h - f*g;
		z[(i-1)*n+i-1] = f-g;
		f = 0.;
		for( j = 1; j <= l; j++ )
		{
			z[(j-1)*n+i] = z[(i-1)*n+j]/h;
			g = 0.;
			for( k = 1; k <= j; k++)
			{
				g += z[(j-1)*n+k]*z[(i-1)*n+k];	
			};
			for( k = j+1; k <= l; k++ )
			{
				g += z[(k-1)*n+j]*z[(i-1)*n+k];					
			};
			e[j] = g/h;
			f += g*z[(j-1)*n+i];
		};
		hh = f/(h+h);
		for( j = 1; j <= l; j++ )
		{
			f = z[(i-1)*n+j];
			g = e[j] - hh*f;
			e[j] = g;
			for( k = 1; k <= j; k++ )
			{
				z[(j-1)*n+k] = z[(j-1)*n+k] - f*e[k] - g*z[(i-1)*n+k];
			};
		};
skip:		d[i] = h;
	};

	d[1] = e[1] = 0.;
	for( i = 1; i <=n; i++ )
	{
		l = i - 1;
		if( d[i] != 0. )
		{
			for( j = 1; j <= l; j++ )
			{
				g = 0.;
				for( k = 1; k <= l; k++ )
				{
					g += z[(i-1)*n+k]*z[(k-1)*n+j];
				};
				for( k = 1; k <= l; k++ )
				{
					z[(k-1)*n+j] = z[(k-1)*n+j] - g*z[(k-1)*n+i];
				};
			};
		};
		d[i] = z[(i-1)*n+i];
		z[(i-1)*n+i] = 1.;
		for( j = 1; j <= l; j++ )
		{
			z[(i-1)*n+j] = z[(j-1)*n+i] = 0.;
		};
	};
};


int calc_eigenstructure(double d[],double e[],double z[], int n, double macheps)
{
	int i,j,k,l,m;
	double b,c,f,g,h,p,r,s;

	for( i = 2; i <=n; i++ )
	{
		e[i-1] = e[i];
	};
	e[n] = b = f = 0.;
	for( l = 1; l <= n; l++ )
	{
		j = 0;
		h = macheps*( abs(d[l]) + abs(e[l]) );
		if( b < h ) b = h;
		for( m = l; m <= n; m++ )
		{
			if( abs(e[m]) <= b ) goto cont;
		};
cont:		if( m == l ) goto root;
nextit:		if( j == 30 ) return(-1);
		j++;
		p = (d[l+1] - d[l])/(2.*e[l]);
		r = sqrt(p*p+1);
		if( p < 0. )
		{
			h = d[l] - e[l]/(p-r);
		}
		else
		{
			h = d[l] - e[l]/(p+r);
		};
		for( i = l; i <= n; i++ )
		{
			d[i] = d[i] - h;
		};
		f += h;
		p = d[m];
		c = 1.;
		s = 0.;
		for( i = m-1; i >= l; i-- )
		{
			g = c*e[i];
			h = c*p;
			if( abs(p) >= abs(e[i]) )
			{
				c = e[i]/p;
				r = sqrt(c*c+1);
				e[i+1] = s*p*r;
				s = c/r;
				c = 1./r;
			}
			else
			{
				c = p/e[i];
				r = sqrt(c*c+1);
				e[i+1] = s*e[i]*r;
				s = 1./r;
				c = c/r;
			};
			p = c*d[i] - s*g;
			d[i+1] = h + s*(c*g + s*d[i]);
			for( k = 1; k <= n; k++ )
			{
				h = z[(k-1)*n+i+1];
				z[(k-1)*n+i+1] = s*z[(k-1)*n+i] + c*h;
				z[(k-1)*n+i] = c*z[(k-1)*n+i] - s*h;
			};
		};
		e[l] = s*p;
		d[l] = c*p;
		if( abs(e[l]) > b ) goto nextit;
root:		d[l] = d[l] + f;
	};
	
	/* order the eigenvectors  */
	for( i = 1; i <= n; i++ )
	{
		k = i;
		p = d[i];
		for( j = i+1; j <= n; j++ )
		{
			if( d[j] < p ) 
			{
				k = j;
				p = d[j];
			};
		};
		if( k != i )
		{
			d[k] = d[i];
			d[i] = p;
			for( j = 1; j <= n; j++ )
			{
				p = z[(j-1)*n+i];
				z[(j-1)*n+i] = z[(j-1)*n+k];
				z[(j-1)*n+k] = p;
			};
		};
	};
	return(0);
};

