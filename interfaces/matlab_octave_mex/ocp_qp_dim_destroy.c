// system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// hpipm
#include "hpipm_d_ocp_qp_dim.h"
// mex
#include "mex.h"



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
	{

//	mexPrintf("\nin ocp_qp_dim_destroy\n");

	long long *ptr;

	/* RHS */

	// dim_mem
	ptr = (long long *) mxGetData( prhs[0] );
	free( (void *) ptr[0] );

	return;

	}

