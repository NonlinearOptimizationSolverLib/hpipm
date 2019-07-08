/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High-Performance Interior Point Method.                                                *
* Copyright (C) 2017-2018 by Gianluca Frison.                                                     *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* This program is free software: you can redistribute it and/or modify                            *
* it under the terms of the GNU General Public License as published by                            *
* the Free Software Foundation, either version 3 of the License, or                               *
* (at your option) any later version                                                              *.
*                                                                                                 *
* This program is distributed in the hope that it will be useful,                                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                   *
* GNU General Public License for more details.                                                    *
*                                                                                                 *
* You should have received a copy of the GNU General Public License                               *
* along with this program.  If not, see <https://www.gnu.org/licenses/>.                          *
*                                                                                                 *
* The authors designate this particular file as subject to the "Classpath" exception              *
* as provided by the authors in the LICENSE file that accompained this code.                      *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/



int OCP_QP_SOL_STRSIZE()
	{
	return sizeof(struct OCP_QP_SOL);
	}



int OCP_QP_SOL_MEMSIZE(struct OCP_QP_DIM *dim)
	{

	// extract dim
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// loop index
	int ii;

	int nvt = 0;
	int net = 0;
	int nct = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nu[ii]+nx[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		}
	nvt += nu[ii]+nx[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];

	int size = 0;

	size += 3*(N+1)*sizeof(struct STRVEC); // ux lam t
	size += 1*N*sizeof(struct STRVEC); // pi

	size += 1*SIZE_STRVEC(nvt); // ux
	size += 1*SIZE_STRVEC(net); // pi
	size += 2*SIZE_STRVEC(nct); // lam t

	size = (size+63)/64*64; // make multiple of typical cache line size
	size += 64; // align to typical cache line size

	return size;

	}



void OCP_QP_SOL_CREATE(struct OCP_QP_DIM *dim, struct OCP_QP_SOL *qp_sol, void *mem)
	{

	// extract dim
	int N = dim->N;
	int *nx = dim->nx;
	int *nu = dim->nu;
	int *nb = dim->nb;
	int *ng = dim->ng;
	int *ns = dim->ns;

	// loop index
	int ii;

	int nvt = 0;
	int net = 0;
	int nct = 0;
	for(ii=0; ii<N; ii++)
		{
		nvt += nu[ii]+nx[ii]+2*ns[ii];
		net += nx[ii+1];
		nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];
		}
	nvt += nu[ii]+nx[ii]+2*ns[ii];
	nct += 2*nb[ii]+2*ng[ii]+2*ns[ii];


	// vector struct stuff
	struct STRVEC *sv_ptr = (struct STRVEC *) mem;

	qp_sol->ux = sv_ptr;
	sv_ptr += N+1;
	qp_sol->pi = sv_ptr;
	sv_ptr += N;
	qp_sol->lam = sv_ptr;
	sv_ptr += N+1;
	qp_sol->t = sv_ptr;
	sv_ptr += N+1;


	// align to typical cache line size
	long long l_ptr = (long long) sv_ptr;
	l_ptr = (l_ptr+63)/64*64;


	// REAL stuff
	char *c_ptr;
	c_ptr = (char *) l_ptr;

	char *tmp_ptr;

	// ux
	tmp_ptr = c_ptr;
	c_ptr += SIZE_STRVEC(nvt);
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(nu[ii]+nx[ii]+2*ns[ii], qp_sol->ux+ii, tmp_ptr);
		tmp_ptr += nu[ii]*sizeof(REAL); // u
		tmp_ptr += nx[ii]*sizeof(REAL); // x
		tmp_ptr += ns[ii]*sizeof(REAL); // s_ls
		tmp_ptr += ns[ii]*sizeof(REAL); // s_us
		VECSE(nu[ii]+nx[ii]+2*ns[ii], 0.0, qp_sol->ux+ii, 0);
		}
	// pi
	tmp_ptr = c_ptr;
	c_ptr += SIZE_STRVEC(net);
	for(ii=0; ii<N; ii++)
		{
		CREATE_STRVEC(nx[ii+1], qp_sol->pi+ii, tmp_ptr);
		tmp_ptr += nx[ii+1]*sizeof(REAL); // pi
		VECSE(nx[ii+1], 0.0, qp_sol->pi+ii, 0);
		}
	// lam
	tmp_ptr = c_ptr;
	c_ptr += SIZE_STRVEC(nct);
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], qp_sol->lam+ii, tmp_ptr);
		tmp_ptr += nb[ii]*sizeof(REAL); // lb
		tmp_ptr += ng[ii]*sizeof(REAL); // lg
		tmp_ptr += nb[ii]*sizeof(REAL); // ub
		tmp_ptr += ng[ii]*sizeof(REAL); // ug
		tmp_ptr += ns[ii]*sizeof(REAL); // ls
		tmp_ptr += ns[ii]*sizeof(REAL); // us
		VECSE(2*nb[ii]+2*ng[ii]+2*ns[ii], 0.0, qp_sol->lam+ii, 0);
		}
	// t
	tmp_ptr = c_ptr;
	c_ptr += SIZE_STRVEC(nct);
	for(ii=0; ii<=N; ii++)
		{
		CREATE_STRVEC(2*nb[ii]+2*ng[ii]+2*ns[ii], qp_sol->t+ii, tmp_ptr);
		tmp_ptr += nb[ii]*sizeof(REAL); // lb
		tmp_ptr += ng[ii]*sizeof(REAL); // lg
		tmp_ptr += nb[ii]*sizeof(REAL); // ub
		tmp_ptr += ng[ii]*sizeof(REAL); // ug
		tmp_ptr += ns[ii]*sizeof(REAL); // ls
		tmp_ptr += ns[ii]*sizeof(REAL); // us
		VECSE(2*nb[ii]+2*ng[ii]+2*ns[ii], 0.0, qp_sol->t+ii, 0);
		}

	qp_sol->dim = dim;

	qp_sol->memsize = OCP_QP_SOL_MEMSIZE(dim);


#if defined(RUNTIME_CHECKS)
	if(c_ptr > ((char *) mem) + qp_sol->memsize)
		{
		printf("\nCreate_ocp_qp_sol: outsize memory bounds!\n\n");
		exit(1);
		}
#endif


	return;

	}



void OCP_QP_SOL_GET_ALL(struct OCP_QP_SOL *qp_sol, REAL **u, REAL **x, REAL **ls, REAL **us, REAL **pi, REAL **lam_lb, REAL **lam_ub, REAL **lam_lg, REAL **lam_ug, REAL **lam_ls, REAL **lam_us)
	{

	int N = qp_sol->dim->N;
	int *nx = qp_sol->dim->nx;
	int *nu = qp_sol->dim->nu;
	int *nb = qp_sol->dim->nb;
	int *ng = qp_sol->dim->ng;
	int *ns = qp_sol->dim->ns;

	int ii;

	for(ii=0; ii<N; ii++)
		{
		CVT_STRVEC2VEC(nx[ii+1], qp_sol->pi+ii, 0, pi[ii]);
		}

	for(ii=0; ii<=N; ii++)
		{
		CVT_STRVEC2VEC(nu[ii], qp_sol->ux+ii, 0, u[ii]);
		CVT_STRVEC2VEC(nx[ii], qp_sol->ux+ii, nu[ii], x[ii]);
		if(nb[ii]>0)
			{
			CVT_STRVEC2VEC(nb[ii], qp_sol->lam+ii, 0, lam_lb[ii]);
			CVT_STRVEC2VEC(nb[ii], qp_sol->lam+ii, nb[ii]+ng[ii], lam_ub[ii]);
			}
		if(ng[ii]>0)
			{
			CVT_STRVEC2VEC(ng[ii], qp_sol->lam+ii, nb[ii], lam_lg[ii]);
			CVT_STRVEC2VEC(ng[ii], qp_sol->lam+ii, 2*nb[ii]+ng[ii], lam_ug[ii]);
			}
		if(ns[ii]>0)
			{
			CVT_STRVEC2VEC(ns[ii], qp_sol->ux+ii, nu[ii]+nx[ii], ls[ii]);
			CVT_STRVEC2VEC(ns[ii], qp_sol->ux+ii, nu[ii]+nx[ii]+ns[ii], us[ii]);
			CVT_STRVEC2VEC(ns[ii], qp_sol->lam+ii, 2*nb[ii]+2*ng[ii], lam_ls[ii]);
			CVT_STRVEC2VEC(ns[ii], qp_sol->lam+ii, 2*nb[ii]+2*ng[ii]+ns[ii], lam_us[ii]);
			}
		}

	return;

	}



void OCP_QP_SOL_SET_ALL(REAL **u, REAL **x, REAL **ls, REAL **us, REAL **pi, REAL **lam_lb, REAL **lam_ub, REAL **lam_lg, REAL **lam_ug, REAL **lam_ls, REAL **lam_us, struct OCP_QP_SOL *qp_sol)
	{

	int N = qp_sol->dim->N;
	int *nx = qp_sol->dim->nx;
	int *nu = qp_sol->dim->nu;
	int *nb = qp_sol->dim->nb;
	int *ng = qp_sol->dim->ng;
	int *ns = qp_sol->dim->ns;

	int ii;

	for(ii=0; ii<N; ii++)
		{
		CVT_VEC2STRVEC(nx[ii+1], pi[ii], qp_sol->pi+ii, 0);
		}

	for(ii=0; ii<=N; ii++)
		{
		CVT_VEC2STRVEC(nu[ii], u[ii], qp_sol->ux+ii, 0);
		CVT_VEC2STRVEC(nx[ii], x[ii], qp_sol->ux+ii, nu[ii]);
		if(nb[ii]>0)
			{
			CVT_VEC2STRVEC(nb[ii], lam_lb[ii], qp_sol->lam+ii, 0);
			CVT_VEC2STRVEC(nb[ii], lam_ub[ii], qp_sol->lam+ii, nb[ii]+ng[ii]);
			}
		if(ng[ii]>0)
			{
			CVT_VEC2STRVEC(ng[ii], lam_lg[ii], qp_sol->lam+ii, nb[ii]);
			CVT_VEC2STRVEC(ng[ii], lam_ug[ii], qp_sol->lam+ii, 2*nb[ii]+ng[ii]);
			}
		if(ns[ii]>0)
			{
			CVT_VEC2STRVEC(ns[ii], ls[ii], qp_sol->ux+ii, nu[ii]+nx[ii]);
			CVT_VEC2STRVEC(ns[ii], us[ii], qp_sol->ux+ii, nu[ii]+nx[ii]+ns[ii]);
			CVT_VEC2STRVEC(ns[ii], lam_ls[ii], qp_sol->lam+ii, 2*nb[ii]+2*ng[ii]);
			CVT_VEC2STRVEC(ns[ii], lam_us[ii], qp_sol->lam+ii, 2*nb[ii]+2*ng[ii]+ns[ii]);
			}
		}

	return;

	}



void OCP_QP_SOL_GET(char *field, int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	if(hpipm_strcmp(field, "u"))
		{ 
		OCP_QP_SOL_GET_U(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "x"))
		{ 
		OCP_QP_SOL_GET_X(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "pi"))
		{ 
		OCP_QP_SOL_GET_PI(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "lam_lb"))
		{ 
		OCP_QP_SOL_GET_LAM_LB(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "lam_ub"))
		{ 
		OCP_QP_SOL_GET_LAM_UB(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "lam_lg"))
		{ 
		OCP_QP_SOL_GET_LAM_LG(stage, qp_sol, vec);
		}
	else if(hpipm_strcmp(field, "lam_ug"))
		{ 
		OCP_QP_SOL_GET_LAM_UG(stage, qp_sol, vec);
		}
	else 
		{
		printf("error [OCP_QP_DIM_GET]: unknown field name '%s'. Exiting.\n", field);
		exit(1);
		}
	return;
	}



void OCP_QP_SOL_GET_U(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nu = qp_sol->dim->nu;
	CVT_STRVEC2VEC(nu[stage], qp_sol->ux+stage, 0, vec);
	}



void OCP_QP_SOL_GET_X(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nx = qp_sol->dim->nx;
	int *nu = qp_sol->dim->nu;
	CVT_STRVEC2VEC(nx[stage], qp_sol->ux+stage, nu[stage], vec);
	}



void OCP_QP_SOL_GET_PI(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nx = qp_sol->dim->nx;
	CVT_STRVEC2VEC(nx[stage+1], qp_sol->pi+stage, 0, vec);
	}



void OCP_QP_SOL_GET_LAM_LB(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nb = qp_sol->dim->nb;
	CVT_STRVEC2VEC(nb[stage], qp_sol->lam+stage, 0, vec);
	}



void OCP_QP_SOL_GET_LAM_UB(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nb = qp_sol->dim->nb;
	int *ng = qp_sol->dim->ng;
	CVT_STRVEC2VEC(nb[stage], qp_sol->lam+stage, nb[stage]+ng[stage], vec);
	}



void OCP_QP_SOL_GET_LAM_LG(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nb = qp_sol->dim->nb;
	int *ng = qp_sol->dim->ng;
	CVT_STRVEC2VEC(ng[stage], qp_sol->lam+stage, nb[stage], vec);
	}



void OCP_QP_SOL_GET_LAM_UG(int stage, struct OCP_QP_SOL *qp_sol, REAL *vec)
	{
	int *nb = qp_sol->dim->nb;
	int *ng = qp_sol->dim->ng;
	CVT_STRVEC2VEC(ng[stage], qp_sol->lam+stage, 2*nb[stage]+ng[stage], vec);
	}



void OCP_QP_SOL_SET(char *field, int stage, REAL *vec, struct OCP_QP_SOL *qp_sol)
	{
	if(hpipm_strcmp(field, "u"))
		{ 
		OCP_QP_SOL_SET_U(stage, vec, qp_sol);
		}
	else if(hpipm_strcmp(field, "x"))
		{ 
		OCP_QP_SOL_SET_X(stage, vec, qp_sol);
		}
	else if(hpipm_strcmp(field, "sl"))
		{ 
		OCP_QP_SOL_SET_SL(stage, vec, qp_sol);
		}
	else if(hpipm_strcmp(field, "su"))
		{ 
		OCP_QP_SOL_SET_SU(stage, vec, qp_sol);
		}
	else 
		{
		printf("error [OCP_QP_DIM_SET]: unknown field name '%s'. Exiting.\n", field);
		exit(1);
		}
	return;
	}



void OCP_QP_SOL_SET_U(int stage, REAL *vec, struct OCP_QP_SOL *qp_sol)
	{
	int *nu = qp_sol->dim->nu;
	CVT_VEC2STRVEC(nu[stage], vec, qp_sol->ux+stage, 0);
	return;
	}



void OCP_QP_SOL_SET_X(int stage, REAL *vec, struct OCP_QP_SOL *qp_sol)
	{
	int *nu = qp_sol->dim->nu;
	int *nx = qp_sol->dim->nx;
	CVT_VEC2STRVEC(nx[stage], vec, qp_sol->ux+stage, nu[stage]);
	return;
	}



void OCP_QP_SOL_SET_SL(int stage, REAL *vec, struct OCP_QP_SOL *qp_sol)
	{
	int *nu = qp_sol->dim->nu;
	int *nx = qp_sol->dim->nx;
	int *ns = qp_sol->dim->ns;
	CVT_VEC2STRVEC(ns[stage], vec, qp_sol->ux+stage, nu[stage]+nx[stage]);
	return;
	}



void OCP_QP_SOL_SET_SU(int stage, REAL *vec, struct OCP_QP_SOL *qp_sol)
	{
	int *nu = qp_sol->dim->nu;
	int *nx = qp_sol->dim->nx;
	int *ns = qp_sol->dim->ns;
	CVT_VEC2STRVEC(ns[stage], vec, qp_sol->ux+stage, nu[stage]+nx[stage]+ns[stage]);
	return;
	}
