// system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// hpipm
#include "hpipm_d_ocp_qp_dim.h"
#include "hpipm_d_ocp_qp.h"
#include "hpipm_d_ocp_qp_utils.h"
// mex
#include "mex.h"



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
	{

//	mexPrintf("\nin ocp_qp_codegen\n");

	long long *l_ptr;

	/* RHS */

	// dim
	l_ptr = mxGetData( prhs[0] );
	struct d_ocp_qp_dim *dim = (struct d_ocp_qp_dim *) *l_ptr;

	// qp
	l_ptr = mxGetData( prhs[1] );
	struct d_ocp_qp *qp = (struct d_ocp_qp *) *l_ptr;

	// file
	char *file_name = mxArrayToString( prhs[2] );

	// mode
	char *mode = mxArrayToString( prhs[3] );

	/* body */
	d_ocp_qp_codegen(file_name, mode, dim, qp);

	return;

	}





