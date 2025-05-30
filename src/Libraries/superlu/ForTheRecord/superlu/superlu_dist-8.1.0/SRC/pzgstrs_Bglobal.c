/*! \file
Copyright (c) 2003, The Regents of the University of California, through
Lawrence Berkeley National Laboratory (subject to receipt of any required
approvals from U.S. Dept. of Energy)

All rights reserved.

The source code is distributed under BSD license, see the file License.txt
at the top-level directory.
*/

/*! @file
 * \brief Solves a system of distributed linear equations A*X = B with a general N-by-N matrix A using the LU factorization
 *
 * <pre>
 * -- Distributed SuperLU routine (version 2.3) --
 * Lawrence Berkeley National Lab, Univ. of California Berkeley.
 * October 15, 2008
 *
 * Modified:
 *     Feburary 7, 2001    use MPI_Isend/MPI_Irecv
 *     October 2, 2001     use MPI_Isend/MPI_Irecv with MPI_Test
 *     October 15, 2008  use fewer MPI_Reduce
 * </pre>
 */

#include "superlu_zdefs.h"

#define ISEND_IRECV

/*
 * Function prototypes
 */
#ifdef _CRAY
fortran void CTRSM(_fcd, _fcd, _fcd, _fcd, int*, int*, doublecomplex*,
		   doublecomplex*, int*, doublecomplex*, int*);
fortran void CGEMM(_fcd, _fcd, int*, int*, int*, doublecomplex*, doublecomplex*,
		   int*, doublecomplex*, int*, doublecomplex*, doublecomplex*, int*);
_fcd ftcs1;
_fcd ftcs2;
_fcd ftcs3;
#endif
static void gather_diag_to_all(int_t, int_t, doublecomplex [], Glu_persist_t *,
                               zLocalLU_t *, gridinfo_t *, int_t, int_t [],
                               int_t [], doublecomplex [], int_t, doublecomplex []);

/*! \brief
 *
 * <pre>
 * Purpose
 * =======
 *
 * pzgstrs_Bglobal solves a system of distributed linear equations
 * A*X = B with a general N-by-N matrix A using the LU factorization
 * computed by pzgstrf.
 *
 * Arguments
 * =========
 *
 * options (input) superlu_dist_options_t*
 *         The structure defines the input parameters to control
 *         how the LU decomposition and triangular solve are performed.
 *
 * n      (input) int (global)
 *        The order of the system of linear equations.
 *
 * LUstruct (input) zLUstruct_t*
 *        The distributed data structures storing L and U factors.
 *        The L and U factors are obtained from pzgstrf for
 *        the possibly scaled and permuted matrix A.
 *        See superlu_ddefs.h for the definition of 'zLUstruct_t'.
 *
 * grid   (input) gridinfo_t*
 *        The 2D process mesh. It contains the MPI communicator, the number
 *        of process rows (NPROW), the number of process columns (NPCOL),
 *        and my process rank. It is an input argument to all the
 *        parallel routines.
 *        Grid can be initialized by subroutine SUPERLU_GRIDINIT.
 *        See superlu_ddefs.h for the definition of 'gridinfo_t'.
 *
 * B      (input/output) doublecomplex*
 *        On entry, the right-hand side matrix of the possibly equilibrated
 *        and row permuted system.
 *        On exit, the solution matrix of the possibly equilibrated
 *        and row permuted system if info = 0;
 *
 *        NOTE: Currently, the N-by-NRHS  matrix B must reside on all
 *              processes when calling this routine.
 *
 * ldb    (input) int (global)
 *        Leading dimension of matrix B.
 *
 * nrhs   (input) int (global)
 *        Number of right-hand sides.
 *
 * stat   (output) SuperLUStat_t*
 *        Record the statistics about the triangular solves.
 *        See util.h for the definition of 'SuperLUStat_t'.
 *
 * info   (output) int*
 * 	   = 0: successful exit
 *	   < 0: if info = -i, the i-th argument had an illegal value
 * </pre>
 */

void
pzgstrs_Bglobal(superlu_dist_options_t *options, int_t n,
                zLUstruct_t *LUstruct, gridinfo_t *grid,
                doublecomplex *B, int_t ldb, int nrhs,
                SuperLUStat_t *stat, int *info)
{
    Glu_persist_t *Glu_persist = LUstruct->Glu_persist;
    zLocalLU_t *Llu = LUstruct->Llu;
    doublecomplex alpha = {1.0, 0.0};
    doublecomplex zero = {0.0, 0.0};
    doublecomplex *lsum;  /* Local running sum of the updates to B-components */
    doublecomplex *x;     /* X component at step k. */
    doublecomplex *lusup, *dest;
    doublecomplex *recvbuf, *tempv;
    doublecomplex *rtemp; /* Result of full matrix-vector multiply. */
    int_t  **Ufstnz_br_ptr = Llu->Ufstnz_br_ptr;
    int_t  *Urbs, *Urbs1; /* Number of row blocks in each block column of U. */
    Ucb_indptr_t **Ucb_indptr;/* Vertical linked list pointing to Uindex[] */
    int_t  **Ucb_valptr;      /* Vertical linked list pointing to Unzval[] */
    int_t  kcol, krow, mycol, myrow;
    int_t  i, ii, il, j, jj, k, lb, ljb, lk, lptr, luptr;
    int_t  nb, nlb, nub, nsupers;
    int_t  *xsup, *lsub, *usub;
    int_t  *ilsum;    /* Starting position of each supernode in lsum (LOCAL)*/
    int    Pc, Pr, iam;
    int    knsupc, nsupr;
    int    ldalsum;   /* Number of lsum entries locally owned. */
    int    maxrecvsz, p, pi;
    int_t  **Lrowind_bc_ptr;
    doublecomplex **Lnzval_bc_ptr;
    MPI_Status status;
#if defined (ISEND_IRECV) || defined (BSEND)
    MPI_Request *send_req, recv_req;
#endif

    /*-- Counts used for L-solve --*/
    int  *fmod;         /* Modification count for L-solve. */
    int  **fsendx_plist = Llu->fsendx_plist;
    int  nfrecvx = Llu->nfrecvx; /* Number of X components to be recv'd. */
    int  *frecv;        /* Count of modifications to be recv'd from
			     processes in this row. */
    int  nfrecvmod = 0; /* Count of total modifications to be recv'd. */
    int  nleaf = 0, nroot = 0;

    /*-- Counts used for U-solve --*/
    int  *bmod;         /* Modification count for L-solve. */
    int  **bsendx_plist = Llu->bsendx_plist;
    int  nbrecvx = Llu->nbrecvx; /* Number of X components to be recv'd. */
    int  *brecv;        /* Count of modifications to be recv'd from
			     processes in this row. */
    int_t  nbrecvmod = 0; /* Count of total modifications to be recv'd. */
    double t;
#if ( DEBUGlevel>=2 )
    int_t Ublocks = 0;
#endif

    int *mod_bit = Llu->mod_bit; /* flag contribution from each row block */

    t = SuperLU_timer_();

    /* Test input parameters. */
    *info = 0;
    if ( n < 0 ) *info = -1;
    else if ( nrhs < 0 ) *info = -9;
    if ( *info ) {
	pxerr_dist("PZGSTRS_BGLOBAL", grid, -*info);
	return;
    }

    /*
     * Initialization.
     */
    iam = grid->iam;
    Pc = grid->npcol;
    Pr = grid->nprow;
    myrow = MYROW( iam, grid );
    mycol = MYCOL( iam, grid );
    nsupers = Glu_persist->supno[n-1] + 1;
    xsup = Glu_persist->xsup;
    Lrowind_bc_ptr = Llu->Lrowind_bc_ptr;
    Lnzval_bc_ptr = Llu->Lnzval_bc_ptr;
    nlb = CEILING( nsupers, Pr ); /* Number of local block rows. */
    stat->ops[SOLVE] = 0.0;
    Llu->SolveMsgSent = 0;

#if ( DEBUGlevel>=1 )
    CHECK_MALLOC(iam, "Enter pzgstrs_Bglobal()");
#endif

    /* Save the count to be altered so it can be used by
       subsequent call to PDGSTRS_BGLOBAL. */
    if ( !(fmod = int32Malloc_dist(nlb)) )
	ABORT("Calloc fails for fmod[].");
    for (i = 0; i < nlb; ++i) fmod[i] = Llu->fmod[i];
    if ( !(frecv = int32Malloc_dist(nlb)) )
	ABORT("Malloc fails for frecv[].");
    Llu->frecv = frecv;

#if defined (ISEND_IRECV) || defined (BSEND)
    k = SUPERLU_MAX( Llu->nfsendx, Llu->nbsendx ) + nlb;
    if ( !(send_req = (MPI_Request*) SUPERLU_MALLOC(k*sizeof(MPI_Request))) )
	ABORT("Malloc fails for send_req[].");
#endif

#ifdef _CRAY
    ftcs1 = _cptofcd("L", strlen("L"));
    ftcs2 = _cptofcd("N", strlen("N"));
    ftcs3 = _cptofcd("U", strlen("U"));
#endif


    /* Obtain ilsum[] and ldalsum for process column 0. */
    ilsum = Llu->ilsum;
    ldalsum = Llu->ldalsum;

    /* Allocate working storage. */
    knsupc = sp_ienv_dist(3, options);
    maxrecvsz = knsupc * nrhs + SUPERLU_MAX( XK_H, LSUM_H );
    if ( !(lsum = doublecomplexCalloc_dist(((size_t)ldalsum) * nrhs
        + nlb * LSUM_H)) )
	ABORT("Calloc fails for lsum[].");
    if ( !(x = doublecomplexMalloc_dist(((size_t)ldalsum) * nrhs
        + nlb * XK_H)) )
	ABORT("Malloc fails for x[].");
    if ( !(recvbuf = doublecomplexMalloc_dist(maxrecvsz)) )
	ABORT("Malloc fails for recvbuf[].");
    if ( !(rtemp = doublecomplexCalloc_dist(maxrecvsz)) )
	ABORT("Malloc fails for rtemp[].");


    /*---------------------------------------------------
     * Forward solve Ly = b.
     *---------------------------------------------------*/

    /*
     * Copy B into X on the diagonal processes.
     */
    ii = 0;
    for (k = 0; k < nsupers; ++k) {
	knsupc = SuperSize( k );
	krow = PROW( k, grid );
	if ( myrow == krow ) {
	    lk = LBi( k, grid );   /* Local block number. */
	    il = LSUM_BLK( lk );
	    lsum[il - LSUM_H].r = k;/* Block number prepended in the header. */
	    lsum[il - LSUM_H].i = 0;
	    kcol = PCOL( k, grid );
	    if ( mycol == kcol ) { /* Diagonal process. */
		jj = X_BLK( lk );
		x[jj - XK_H].r = k; /* Block number prepended in the header. */
		x[jj - XK_H].i = 0;
		RHS_ITERATE(j)
		    for (i = 0; i < knsupc; ++i) /* X is stored in blocks. */
			x[i + jj + j*knsupc] = B[i + ii + j*ldb];
	    }
	}
	ii += knsupc;
    }

    /*
     * Compute frecv[] and nfrecvmod counts on the diagonal processes.
     */
    {
	superlu_scope_t *scp = &grid->rscp;

#if 1
	for (k = 0; k < nlb; ++k) mod_bit[k] = 0;
	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid );
		if ( mycol != kcol && fmod[lk] )
		    mod_bit[lk] = 1;  /* contribution from off-diagonal */
	    }
	}

	/* Every process receives the count, but it is only useful on the
	   diagonal processes.  */
#if 0	   
	MPI_Allreduce( mod_bit, frecv, nlb, mpi_int_t, MPI_SUM, scp->comm );
#else	
	MPI_Allreduce( mod_bit, frecv, nlb, MPI_INT, MPI_SUM, scp->comm );
#endif	

	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid );
		if ( mycol == kcol ) { /* Diagonal process. */
		    nfrecvmod += frecv[lk];
		    if ( !frecv[lk] && !fmod[lk] ) ++nleaf;
		}
	    }
	}

#else /* old */

	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid ); /* Root process in this row scope. */
		if ( mycol != kcol && fmod[lk] )
		    i = 1;  /* Contribution from non-diagonal process. */
		else i = 0;
		MPI_Reduce( &i, &frecv[lk], 1, mpi_int_t,
			   MPI_SUM, kcol, scp->comm );
		if ( mycol == kcol ) { /* Diagonal process. */
		    nfrecvmod += frecv[lk];
		    if ( !frecv[lk] && !fmod[lk] ) ++nleaf;
#if ( DEBUGlevel>=2 )
		    printf("(%2d) frecv[%4d]  %2d\n", iam, k, frecv[lk]);
		    assert( frecv[lk] < Pc );
#endif
		}
	    }
	}
#endif
    }

    /* ---------------------------------------------------------
       Solve the leaf nodes first by all the diagonal processes.
       --------------------------------------------------------- */
#if ( DEBUGlevel>=2 )
    printf("(%2d) nleaf %4d\n", iam, nleaf);
#endif
    for (k = 0; k < nsupers && nleaf; ++k) {
	krow = PROW( k, grid );
	kcol = PCOL( k, grid );
	if ( myrow == krow && mycol == kcol ) { /* Diagonal process */
	    knsupc = SuperSize( k );
	    lk = LBi( k, grid );
	    if ( frecv[lk]==0 && fmod[lk]==0 ) {
		fmod[lk] = -1;  /* Do not solve X[k] in the future. */
		ii = X_BLK( lk );
		lk = LBj( k, grid ); /* Local block number, column-wise. */
		lsub = Lrowind_bc_ptr[lk];
		lusup = Lnzval_bc_ptr[lk];
		nsupr = lsub[1];
#ifdef _CRAY
		CTRSM(ftcs1, ftcs1, ftcs2, ftcs3, &knsupc, &nrhs, &alpha,
		      lusup, &nsupr, &x[ii], &knsupc);
#elif defined (USE_VENDOR_BLAS)
		ztrsm_("L", "L", "N", "U", &knsupc, &nrhs, &alpha,
		       lusup, &nsupr, &x[ii], &knsupc, 1, 1, 1, 1);
#else
		ztrsm_("L", "L", "N", "U", &knsupc, &nrhs, &alpha,
		       lusup, &nsupr, &x[ii], &knsupc);
#endif
		stat->ops[SOLVE] += 4 * knsupc * (knsupc - 1) * nrhs
		    + 10 * knsupc * nrhs; /* complex division */
		--nleaf;
#if ( DEBUGlevel>=2 )
		printf("(%2d) Solve X[%2d]\n", iam, k);
#endif

		/*
		 * Send Xk to process column Pc[k].
		 */
		for (p = 0; p < Pr; ++p) {
		    if ( fsendx_plist[lk][p] != EMPTY ) {
			pi = PNUM( p, kcol, grid );
#ifdef ISEND_IRECV
			MPI_Isend( &x[ii - XK_H], knsupc * nrhs + XK_H,
				   SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm,
                                   &send_req[Llu->SolveMsgSent++]);
#else
#ifdef BSEND
			MPI_Bsend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                   SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#else

			MPI_Send( &x[ii - XK_H], knsupc * nrhs + XK_H,
				  SuperLU_MPI_DOUBLE_COMPLEX,
                                  pi, Xk, grid->comm );
#endif
#endif
#if ( DEBUGlevel>=2 )
			printf("(%2d) Sent X[%2.0f] to P %2d\n",
			       iam, x[ii-XK_H], pi);
#endif
		    }
		}
		/*
		 * Perform local block modifications: lsum[i] -= L_i,k * X[k]
		 */
		nb = lsub[0] - 1;
		lptr = BC_HEADER + LB_DESCRIPTOR + knsupc;
		luptr = knsupc; /* Skip diagonal block L(k,k). */

		zlsum_fmod(lsum, x, &x[ii], rtemp, nrhs, knsupc, k,
			   fmod, nb, lptr, luptr, xsup, grid, Llu,
			   send_req,stat);
	    }
	} /* if diagonal process ... */
    } /* for k ... */

    /* -----------------------------------------------------------
       Compute the internal nodes asynchronously by all processes.
       ----------------------------------------------------------- */
#if ( DEBUGlevel>=2 )
    printf("(%2d) nfrecvx %4d,  nfrecvmod %4d,  nleaf %4d\n",
	   iam, nfrecvx, nfrecvmod, nleaf);
#endif

    while ( nfrecvx || nfrecvmod ) { /* While not finished. */

	/* Receive a message. */
#if 1
	MPI_Recv( recvbuf, maxrecvsz, SuperLU_MPI_DOUBLE_COMPLEX,
		 MPI_ANY_SOURCE, MPI_ANY_TAG, grid->comm, &status );
#else
	/* -MPI- FATAL: Remote protocol queue full */
	MPI_Irecv( recvbuf, maxrecvsz, SuperLU_MPI_DOUBLE_COMPLEX,
		  MPI_ANY_SOURCE, MPI_ANY_TAG, grid->comm, &request );
	MPI_Wait( &request, &status );
#endif

	k = (*recvbuf).r;



#if ( DEBUGlevel>=2 )
	printf("(%2d) Recv'd block %d, tag %2d\n", iam, k, status.MPI_TAG);
#endif

	switch ( status.MPI_TAG ) {
	  case Xk:
	      --nfrecvx;
	      lk = LBj( k, grid ); /* Local block number, column-wise. */
	      lsub = Lrowind_bc_ptr[lk];
	      lusup = Lnzval_bc_ptr[lk];
	      if ( lsub ) {
		  nb   = lsub[0];
		  lptr = BC_HEADER;
		  luptr = 0;
		  knsupc = SuperSize( k );

		  /*
		   * Perform local block modifications: lsum[i] -= L_i,k * X[k]
		   */
		  zlsum_fmod(lsum, x, &recvbuf[XK_H], rtemp, nrhs, knsupc, k,
			     fmod, nb, lptr, luptr, xsup, grid, Llu,
			     send_req, stat);
	      } /* if lsub */

	      break;

	  case LSUM: /* Receiver must be a diagonal process */
	      --nfrecvmod;
	      lk = LBi( k, grid ); /* Local block number, row-wise. */
	      ii = X_BLK( lk );
	      knsupc = SuperSize( k );
	      tempv = &recvbuf[LSUM_H];
	      RHS_ITERATE(j)
		  for (i = 0; i < knsupc; ++i)
		      z_add(&x[i + ii + j*knsupc],
			    &x[i + ii + j*knsupc],
			    &tempv[i + j*knsupc]);

	      if ( (--frecv[lk])==0 && fmod[lk]==0 ) {
		  fmod[lk] = -1; /* Do not solve X[k] in the future. */
		  lk = LBj( k, grid ); /* Local block number, column-wise. */
		  lsub = Lrowind_bc_ptr[lk];
		  lusup = Lnzval_bc_ptr[lk];
		  nsupr = lsub[1];
#ifdef _CRAY
		  CTRSM(ftcs1, ftcs1, ftcs2, ftcs3, &knsupc, &nrhs, &alpha,
			lusup, &nsupr, &x[ii], &knsupc);
#elif defined (USE_VENDOR_BLAS)
		  ztrsm_("L", "L", "N", "U", &knsupc, &nrhs, &alpha,
			 lusup, &nsupr, &x[ii], &knsupc, 1, 1, 1, 1);
#else
		  ztrsm_("L", "L", "N", "U", &knsupc, &nrhs, &alpha,
			 lusup, &nsupr, &x[ii], &knsupc);
#endif
		  stat->ops[SOLVE] += 4 * knsupc * (knsupc - 1) * nrhs
		      + 10 * knsupc * nrhs; /* complex division */

#if ( DEBUGlevel>=2 )
		  printf("(%2d) Solve X[%2d]\n", iam, k);
#endif

		  /*
		   * Send Xk to process column Pc[k].
		   */
		  kcol = PCOL( k, grid );
		  for (p = 0; p < Pr; ++p) {
		      if ( fsendx_plist[lk][p] != EMPTY ) {
			  pi = PNUM( p, kcol, grid );
#ifdef ISEND_IRECV
			  MPI_Isend( &x[ii - XK_H], knsupc * nrhs + XK_H,
				     SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm,
				     &send_req[Llu->SolveMsgSent++]);
#else
#ifdef BSEND
			  MPI_Bsend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                     SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#else
			  MPI_Send( &x[ii - XK_H], knsupc * nrhs + XK_H,
				   SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#endif
#endif
#if ( DEBUGlevel>=2 )
			  printf("(%2d) Sent X[%2.0f] to P %2d\n",
				 iam, x[ii-XK_H], pi);
#endif
		      }
                  }
		  /*
		   * Perform local block modifications.
		   */
		  nb = lsub[0] - 1;
		  lptr = BC_HEADER + LB_DESCRIPTOR + knsupc;
		  luptr = knsupc; /* Skip diagonal block L(k,k). */

		  zlsum_fmod(lsum, x, &x[ii], rtemp, nrhs, knsupc, k,
			     fmod, nb, lptr, luptr, xsup, grid, Llu,
			     send_req, stat);
	      } /* if */

	      break;

#if ( DEBUGlevel>=2 )
	    default:
	      printf("(%2d) Recv'd wrong message tag %4d\n", iam, status.MPI_TAG);
	      break;
#endif
	  } /* switch */

    } /* while not finished ... */


#if ( PRNTlevel>=2 )
    t = SuperLU_timer_() - t;
    if ( !iam ) printf(".. L-solve time\t%8.2f\n", t);
    t = SuperLU_timer_();
#endif

#if ( DEBUGlevel>=2 )
    printf("\n(%d) .. After L-solve: y =\n", iam);
    for (i = 0, k = 0; k < nsupers; ++k) {
	krow = PROW( k, grid );
	kcol = PCOL( k, grid );
	if ( myrow == krow && mycol == kcol ) { /* Diagonal process */
	    knsupc = SuperSize( k );
	    lk = LBi( k, grid );
	    ii = X_BLK( lk );
	    for (j = 0; j < knsupc; ++j)
		printf("\t(%d)\t%4d\t%.10f\n", iam, xsup[k]+j, x[ii+j]);
	}
	MPI_Barrier( grid->comm );
    }
#endif

    SUPERLU_FREE(fmod);
    SUPERLU_FREE(frecv);
    SUPERLU_FREE(rtemp);

#ifdef ISEND_IRECV
    for (i = 0; i < Llu->SolveMsgSent; ++i) MPI_Request_free(&send_req[i]);
    Llu->SolveMsgSent = 0;
#endif


    /*---------------------------------------------------
     * Back solve Ux = y.
     *
     * The Y components from the forward solve is already
     * on the diagonal processes.
     *---------------------------------------------------*/

    /* Save the count to be altered so it can be used by
       subsequent call to PDGSTRS_BGLOBAL. */
    if ( !(bmod = int32Malloc_dist(nlb)) )
	ABORT("Calloc fails for bmod[].");
    for (i = 0; i < nlb; ++i) bmod[i] = Llu->bmod[i];
    if ( !(brecv = int32Malloc_dist(nlb)) )
	ABORT("Malloc fails for brecv[].");
    Llu->brecv = brecv;

    /*
     * Compute brecv[] and nbrecvmod counts on the diagonal processes.
     */
    {
	superlu_scope_t *scp = &grid->rscp;

#if 1
	for (k = 0; k < nlb; ++k) mod_bit[k] = 0;
	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid ); /* Root process in this row scope. */
		if ( mycol != kcol && bmod[lk] )
		    mod_bit[lk] = 1;  /* Contribution from off-diagonal */
	    }
	}

	/* Every process receives the count, but it is only useful on the
	   diagonal processes.  */
#if 0	   
	MPI_Allreduce( mod_bit, brecv, nlb, mpi_int_t, MPI_SUM, scp->comm );
#else	
	MPI_Allreduce( mod_bit, brecv, nlb, MPI_INT, MPI_SUM, scp->comm );
#endif

	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid ); /* Root process in this row scope. */
		if ( mycol == kcol ) { /* Diagonal process. */
		    nbrecvmod += brecv[lk];
		    if ( !brecv[lk] && !bmod[lk] ) ++nroot;
#if ( DEBUGlevel>=2 )
		    printf("(%2d) brecv[%4d]  %2d\n", iam, k, brecv[lk]);
		    assert( brecv[lk] < Pc );
#endif
		}
	    }
	}

#else /* old */

	for (k = 0; k < nsupers; ++k) {
	    krow = PROW( k, grid );
	    if ( myrow == krow ) {
		lk = LBi( k, grid );    /* Local block number. */
		kcol = PCOL( k, grid ); /* Root process in this row scope. */
		if ( mycol != kcol && bmod[lk] )
		    i = 1;  /* Contribution from non-diagonal process. */
		else i = 0;
		MPI_Reduce( &i, &brecv[lk], 1, mpi_int_t,
			   MPI_SUM, kcol, scp->comm );
		if ( mycol == kcol ) { /* Diagonal process. */
		    nbrecvmod += brecv[lk];
		    if ( !brecv[lk] && !bmod[lk] ) ++nroot;
#if ( DEBUGlevel>=2 )
		    printf("(%2d) brecv[%4d]  %2d\n", iam, k, brecv[lk]);
		    assert( brecv[lk] < Pc );
#endif
		}
	    }
	}
#endif
    }

    /* Re-initialize lsum to zero. Each block header is already in place. */
    for (k = 0; k < nsupers; ++k) {
	krow = PROW( k, grid );
	if ( myrow == krow ) {
	    knsupc = SuperSize( k );
	    lk = LBi( k, grid );
	    il = LSUM_BLK( lk );
	    dest = &lsum[il];
	    RHS_ITERATE(j)
		for (i = 0; i < knsupc; ++i) dest[i + j*knsupc] = zero;
	}
    }

    /* Set up additional pointers for the index and value arrays of U.
       nub is the number of local block columns. */
    nub = CEILING( nsupers, Pc ); /* Number of local block columns. */
    if ( !(Urbs = (int_t *) intCalloc_dist(2*((size_t)nub))) )
	ABORT("Malloc fails for Urbs[]"); /* Record number of nonzero
					     blocks in a block column. */
    Urbs1 = Urbs + nub;
    if ( !(Ucb_indptr = SUPERLU_MALLOC(nub * sizeof(Ucb_indptr_t *))) )
        ABORT("Malloc fails for Ucb_indptr[]");
    if ( !(Ucb_valptr = SUPERLU_MALLOC(nub * sizeof(int_t *))) )
        ABORT("Malloc fails for Ucb_valptr[]");

    /* Count number of row blocks in a block column.
       One pass of the skeleton graph of U. */
    for (lk = 0; lk < nlb; ++lk) {
	usub = Ufstnz_br_ptr[lk];
	if ( usub ) { /* Not an empty block row. */
	    /* usub[0] -- number of column blocks in this block row. */
#if ( DEBUGlevel>=2 )
	    Ublocks += usub[0];
#endif
	    i = BR_HEADER; /* Pointer in index array. */
	    for (lb = 0; lb < usub[0]; ++lb) { /* For all column blocks. */
		k = usub[i];            /* Global block number */
		++Urbs[LBj(k,grid)];
		i += UB_DESCRIPTOR + SuperSize( k );
	    }
	}
    }

    /* Set up the vertical linked lists for the row blocks.
       One pass of the skeleton graph of U. */
    for (lb = 0; lb < nub; ++lb) {
	if ( Urbs[lb] ) { /* Not an empty block column. */
	    if ( !(Ucb_indptr[lb]
		   = SUPERLU_MALLOC(Urbs[lb] * sizeof(Ucb_indptr_t))) )
		ABORT("Malloc fails for Ucb_indptr[lb][]");
	    if ( !(Ucb_valptr[lb] = (int_t *) intMalloc_dist(Urbs[lb])) )
		ABORT("Malloc fails for Ucb_valptr[lb][]");
	}
    }
    for (lk = 0; lk < nlb; ++lk) { /* For each block row. */
	usub = Ufstnz_br_ptr[lk];
	if ( usub ) { /* Not an empty block row. */
	    i = BR_HEADER; /* Pointer in index array. */
	    j = 0;         /* Pointer in nzval array. */
	    for (lb = 0; lb < usub[0]; ++lb) { /* For all column blocks. */
		k = usub[i];          /* Global block number, column-wise. */
		ljb = LBj( k, grid ); /* Local block number, column-wise. */
		Ucb_indptr[ljb][Urbs1[ljb]].lbnum = lk;
		Ucb_indptr[ljb][Urbs1[ljb]].indpos = i;
		Ucb_valptr[ljb][Urbs1[ljb]] = j;
		++Urbs1[ljb];
		j += usub[i+1];
		i += UB_DESCRIPTOR + SuperSize( k );
	    }
	}
    }

#if ( DEBUGlevel>=2 )
    for (p = 0; p < Pr*Pc; ++p) {
	if (iam == p) {
	    printf("(%2d) .. Ublocks %d\n", iam, Ublocks);
	    for (lb = 0; lb < nub; ++lb) {
		printf("(%2d) Local col %2d: # row blocks %2d\n",
		       iam, lb, Urbs[lb]);
		if ( Urbs[lb] ) {
		    for (i = 0; i < Urbs[lb]; ++i)
			printf("(%2d) .. row blk %2d:\
                               lbnum %d, indpos %d, valpos %d\n",
			       iam, i,
			       Ucb_indptr[lb][i].lbnum,
			       Ucb_indptr[lb][i].indpos,
			       Ucb_valptr[lb][i]);
		}
	    }
	}
	MPI_Barrier( grid->comm );
    }
    for (p = 0; p < Pr*Pc; ++p) {
	if ( iam == p ) {
	    printf("\n(%d) bsendx_plist[][]", iam);
	    for (lb = 0; lb < nub; ++lb) {
		printf("\n(%d) .. local col %2d: ", iam, lb);
		for (i = 0; i < Pr; ++i)
		    printf("%4d", bsendx_plist[lb][i]);
	    }
	    printf("\n");
	}
	MPI_Barrier( grid->comm );
    }
#endif /* DEBUGlevel */


#if ( PRNTlevel>=2 )
    t = SuperLU_timer_() - t;
    if ( !iam) printf(".. Setup U-solve time\t%8.2f\n", t);
    t = SuperLU_timer_();
#endif

    /*
     * Solve the roots first by all the diagonal processes.
     */
#if ( DEBUGlevel>=2 )
    printf("(%2d) nroot %4d\n", iam, nroot);
#endif
    for (k = nsupers-1; k >= 0 && nroot; --k) {
	krow = PROW( k, grid );
	kcol = PCOL( k, grid );
	if ( myrow == krow && mycol == kcol ) { /* Diagonal process. */
	    knsupc = SuperSize( k );
	    lk = LBi( k, grid ); /* Local block number, row-wise. */
	    if ( brecv[lk]==0 && bmod[lk]==0 ) {
		bmod[lk] = -1;       /* Do not solve X[k] in the future. */
		ii = X_BLK( lk );
		lk = LBj( k, grid ); /* Local block number, column-wise */
		lsub = Lrowind_bc_ptr[lk];
		lusup = Lnzval_bc_ptr[lk];
		nsupr = lsub[1];
#ifdef _CRAY
		CTRSM(ftcs1, ftcs3, ftcs2, ftcs2, &knsupc, &nrhs, &alpha,
		      lusup, &nsupr, &x[ii], &knsupc);
#elif defined (USE_VENDOR_BLAS)
		ztrsm_("L", "U", "N", "N", &knsupc, &nrhs, &alpha,
		       lusup, &nsupr, &x[ii], &knsupc, 1, 1, 1, 1);
#else
		ztrsm_("L", "U", "N", "N", &knsupc, &nrhs, &alpha,
		       lusup, &nsupr, &x[ii], &knsupc);
#endif
		stat->ops[SOLVE] += 4 * knsupc * (knsupc + 1) * nrhs
		    + 10 * knsupc * nrhs; /* complex division */
		--nroot;
#if ( DEBUGlevel>=2 )
		printf("(%2d) Solve X[%2d]\n", iam, k);
#endif
		/*
		 * Send Xk to process column Pc[k].
		 */
		for (p = 0; p < Pr; ++p) {
		    if ( bsendx_plist[lk][p] != EMPTY ) {
			pi = PNUM( p, kcol, grid );
#ifdef ISEND_IRECV
			MPI_Isend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                   SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm,
                                   &send_req[Llu->SolveMsgSent++]);
#else
#ifdef BSEND
			MPI_Bsend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                   SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#else
			MPI_Send( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                  SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#endif
#endif
#if ( DEBUGlevel>=2 )
			printf("(%2d) Sent X[%2.0f] to P %2d\n",
			       iam, x[ii-XK_H], pi);
#endif
		    }
		}
		/*
		 * Perform local block modifications: lsum[i] -= U_i,k * X[k]
		 */
		if ( Urbs[lk] )
		    zlsum_bmod(lsum, x, &x[ii], nrhs, k, bmod, Urbs,
			       Ucb_indptr, Ucb_valptr, xsup, grid, Llu,
			       send_req, stat);
	    } /* if root ... */
	} /* if diagonal process ... */
    } /* for k ... */


    /*
     * Compute the internal nodes asynchronously by all processes.
     */
    while ( nbrecvx || nbrecvmod ) { /* While not finished. */

	/* Receive a message. */
	MPI_Recv( recvbuf, maxrecvsz, SuperLU_MPI_DOUBLE_COMPLEX, MPI_ANY_SOURCE,
		 MPI_ANY_TAG, grid->comm, &status );

	k = (*recvbuf).r;

#if ( DEBUGlevel>=2 )
	printf("(%2d) Recv'd block %d, tag %2d\n", iam, k, status.MPI_TAG);
#endif

	switch ( status.MPI_TAG ) {
	    case Xk:
	        --nbrecvx;
		lk = LBj( k, grid ); /* Local block number, column-wise. */
		/*
		 * Perform local block modifications:
		 *         lsum[i] -= U_i,k * X[k]
		 */
		zlsum_bmod(lsum, x, &recvbuf[XK_H], nrhs, k, bmod, Urbs,
			   Ucb_indptr, Ucb_valptr, xsup, grid, Llu,
			   send_req, stat);

	        break;

	    case LSUM: /* Receiver must be a diagonal process */
		--nbrecvmod;
		lk = LBi( k, grid ); /* Local block number, row-wise. */
		ii = X_BLK( lk );
		knsupc = SuperSize( k );
		tempv = &recvbuf[LSUM_H];
		RHS_ITERATE(j)
		    for (i = 0; i < knsupc; ++i)
			z_add(&x[i + ii + j*knsupc],
			      &x[i + ii + j*knsupc],
			      &tempv[i + j*knsupc]);

		if ( (--brecv[lk])==0 && bmod[lk]==0 ) {
		    bmod[lk] = -1; /* Do not solve X[k] in the future. */
		    lk = LBj( k, grid ); /* Local block number, column-wise. */
		    lsub = Lrowind_bc_ptr[lk];
		    lusup = Lnzval_bc_ptr[lk];
		    nsupr = lsub[1];
#ifdef _CRAY
		    CTRSM(ftcs1, ftcs3, ftcs2, ftcs2, &knsupc, &nrhs, &alpha,
			  lusup, &nsupr, &x[ii], &knsupc);
#elif defined (USE_VENDOR_BLAS)
		    ztrsm_("L", "U", "N", "N", &knsupc, &nrhs, &alpha,
			   lusup, &nsupr, &x[ii], &knsupc, 1, 1, 1, 1);
#else
		    ztrsm_("L", "U", "N", "N", &knsupc, &nrhs, &alpha,
			   lusup, &nsupr, &x[ii], &knsupc);
#endif
		    stat->ops[SOLVE] += 4 * knsupc * (knsupc + 1) * nrhs
			+ 10 * knsupc * nrhs; /* complex division */
#if ( DEBUGlevel>=2 )
		    printf("(%2d) Solve X[%2d]\n", iam, k);
#endif
		    /*
		     * Send Xk to process column Pc[k].
		     */
		    kcol = PCOL( k, grid );
		    for (p = 0; p < Pr; ++p) {
			if ( bsendx_plist[lk][p] != EMPTY ) {
			    pi = PNUM( p, kcol, grid );
#ifdef ISEND_IRECV
			    MPI_Isend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                       SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm,
				       &send_req[Llu->SolveMsgSent++] );
#else
#ifdef BSEND
			    MPI_Bsend( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                       SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#else
			    MPI_Send( &x[ii - XK_H], knsupc * nrhs + XK_H,
                                      SuperLU_MPI_DOUBLE_COMPLEX, pi, Xk, grid->comm );
#endif
#endif
#if ( DEBUGlevel>=2 )
			    printf("(%2d) Sent X[%2.0f] to P %2d\n",
				   iam, x[ii - XK_H], pi);
#endif
			}
		    }
		    /*
		     * Perform local block modifications:
		     *         lsum[i] -= U_i,k * X[k]
		     */
		    if ( Urbs[lk] )
			zlsum_bmod(lsum, x, &x[ii], nrhs, k, bmod, Urbs,
				   Ucb_indptr, Ucb_valptr, xsup, grid, Llu,
				   send_req, stat);
		} /* if becomes solvable */

		break;

#if ( DEBUGlevel>=2 )
	      default:
		printf("(%2d) Recv'd wrong message tag %4d\n", iam, status.MPI_TAG);
		break;
#endif

	} /* switch */

    } /* while not finished ... */

#if ( PRNTlevel>=2 )
    t = SuperLU_timer_() - t;
    if ( !iam ) printf(".. U-solve time\t%8.2f\n", t);
#endif


    /* Copy the solution X into B (on all processes). */
    {
	int_t num_diag_procs, *diag_procs, *diag_len;
	doublecomplex *work;

	get_diag_procs(n, Glu_persist, grid, &num_diag_procs,
		       &diag_procs, &diag_len);
	jj = diag_len[0];
	for (j = 1; j < num_diag_procs; ++j) jj = SUPERLU_MAX(jj, diag_len[j]);
	if ( !(work = doublecomplexMalloc_dist(((size_t)jj)*nrhs)) )
	    ABORT("Malloc fails for work[]");
	gather_diag_to_all(n, nrhs, x, Glu_persist, Llu,
			   grid, num_diag_procs, diag_procs, diag_len,
			   B, ldb, work);
	SUPERLU_FREE(diag_procs);
	SUPERLU_FREE(diag_len);
	SUPERLU_FREE(work);
    }

    /* Deallocate storage. */

    SUPERLU_FREE(lsum);
    SUPERLU_FREE(x);
    SUPERLU_FREE(recvbuf);
    for (i = 0; i < nub; ++i)
	if ( Urbs[i] ) {
	    SUPERLU_FREE(Ucb_indptr[i]);
	    SUPERLU_FREE(Ucb_valptr[i]);
	}
    SUPERLU_FREE(Ucb_indptr);
    SUPERLU_FREE(Ucb_valptr);
    SUPERLU_FREE(Urbs);
    SUPERLU_FREE(bmod);
    SUPERLU_FREE(brecv);
#ifdef ISEND_IRECV
    for (i = 0; i < Llu->SolveMsgSent; ++i) MPI_Request_free(&send_req[i]);
    SUPERLU_FREE(send_req);
#endif
#ifdef BSEND
    SUPERLU_FREE(send_req);
#endif

    stat->utime[SOLVE] = SuperLU_timer_() - t;

#if ( DEBUGlevel>=1 )
    CHECK_MALLOC(iam, "Exit pzgstrs_Bglobal()");
#endif

} /* PZGSTRS_BGLOBAL */


/*
 * Gather the components of x vector on the diagonal processes
 * onto all processes, and combine them into the global vector y.
 */
static void
gather_diag_to_all(int_t n, int_t nrhs, doublecomplex x[],
		   Glu_persist_t *Glu_persist, zLocalLU_t *Llu,
		   gridinfo_t *grid, int_t num_diag_procs,
		   int_t diag_procs[], int_t diag_len[],
		   doublecomplex y[], int_t ldy, doublecomplex work[])
{
    int_t i, ii, j, k, lk, lwork, nsupers, p;
    int_t *ilsum, *xsup;
    int iam, knsupc, pkk;
    doublecomplex *x_col, *y_col;

    iam = grid->iam;
    nsupers = Glu_persist->supno[n-1] + 1;
    xsup = Glu_persist->xsup;
    ilsum = Llu->ilsum;

    for (p = 0; p < num_diag_procs; ++p) {
	pkk = diag_procs[p];
	if ( iam == pkk ) {
	    /* Copy x vector into a buffer. */
	    lwork = 0;
	    for (k = p; k < nsupers; k += num_diag_procs) {
		knsupc = SuperSize( k );
		lk = LBi( k, grid );
		ii = X_BLK( lk ); /*ilsum[lk] + (lk+1)*XK_H;*/
		x_col = &x[ii];
		for (j = 0; j < nrhs; ++j) {
		    for (i = 0; i < knsupc; ++i) work[i+lwork] = x_col[i];
		    lwork += knsupc;
		    x_col += knsupc;
		}
	    }
	    MPI_Bcast( work, lwork, SuperLU_MPI_DOUBLE_COMPLEX, pkk, grid->comm );
	} else {
	    MPI_Bcast( work, diag_len[p]*nrhs, SuperLU_MPI_DOUBLE_COMPLEX, pkk, grid->comm );
	}
	/* Scatter work[] into global y vector. */
	lwork = 0;
	for (k = p; k < nsupers; k += num_diag_procs) {
	    knsupc = SuperSize( k );
	    ii = FstBlockC( k );
	    y_col = &y[ii];
	    for (j = 0; j < nrhs; ++j) {
		for (i = 0; i < knsupc; ++i) y_col[i] = work[i+lwork];
		lwork += knsupc;
		y_col += ldy;
	    }
	}
    }
} /* GATHER_DIAG_TO_ALL */

