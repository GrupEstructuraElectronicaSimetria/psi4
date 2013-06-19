/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */

/*! \file
    \ingroup CCLAMBDA
    \brief Enter brief description of file here 
*/
#include <cstdio>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace cclambda {

/** The RHF/ROHF contractions can be improved here **/

void cc2_fmiL2(int L_irr)
{
  int h, m;
  dpdbuf4 Lijab, LIJAB, LIjAb;
  dpdbuf4 newLijab, newLIJAB, newLIjAb;
  dpdfile2 fij, fIJ, F;
  dpdbuf4 X, X1, X2;
  dpdbuf4 L2, newL2;

  /* RHS -= P(ij)*Limab*Fjm */
  if(params.ref == 0) { /** RHF **/

    dpd_->buf4_init(&X, PSIF_CC_TMP0, L_irr, 0, 5, 0, 5, 0, "X(Ij,Ab)");

    dpd_->buf4_init(&L2, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->file2_init(&F, PSIF_CC_OEI, 0, 0, 0, "fIJ");
    dpd_->contract244(&F, &L2, &X, 1, 0, 0, -1.0, 0);
    dpd_->file2_close(&F);
    dpd_->buf4_close(&L2);

    dpd_->buf4_sort_axpy(&X, PSIF_CC_LAMBDA, qpsr, 0, 5, "New LIjAb", 1);
    dpd_->buf4_init(&newL2, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "New LIjAb");
    dpd_->buf4_axpy(&X, &newL2, 1);
    dpd_->buf4_close(&newL2);

    dpd_->buf4_close(&X);
  }
  else if(params.ref == 1) { /** RHF/ROHF **/

    dpd_->file2_init(&fIJ, PSIF_CC_OEI, 0, 0, 0, "fIJ");
    dpd_->file2_init(&fij, PSIF_CC_OEI, 0, 0, 0, "fij");

    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "LIJAB");
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(0,7) 1");
    dpd_->contract424(&LIJAB, &fIJ, &X1, 1, 1, 1, -1.0, 0.0);
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(0,7) 2");
    dpd_->contract244(&fIJ, &LIJAB, &X2, 1, 0, 0, -1.0, 0.0);
    dpd_->buf4_close(&LIJAB);
    dpd_->buf4_axpy(&X1, &X2, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "New LIJAB");
    dpd_->buf4_axpy(&X2, &newLIJAB, 1.0);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&newLIJAB);

    dpd_->buf4_init(&Lijab, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "Lijab");
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(0,7) 1");
    dpd_->contract424(&Lijab, &fij, &X1, 1, 1, 1, -1.0, 0.0);
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(0,7) 2");
    dpd_->contract244(&fij, &Lijab, &X2, 1, 0, 0, -1.0, 0.0);
    dpd_->buf4_close(&Lijab);
    dpd_->buf4_axpy(&X1, &X2, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_init(&newLijab, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "New Lijab");
    dpd_->buf4_axpy(&X2, &newLijab, 1.0);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&newLijab);

    dpd_->buf4_init(&LIjAb, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->buf4_init(&newLIjAb, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "New LIjAb");
    dpd_->contract424(&LIjAb, &fij, &newLIjAb, 1, 1, 1, -1.0, 1.0);
    dpd_->contract244(&fIJ, &LIjAb, &newLIjAb, 1, 0, 0, -1.0, 1.0);
    dpd_->buf4_close(&LIjAb);
    dpd_->buf4_close(&newLIjAb);

    dpd_->file2_close(&fij);
    dpd_->file2_close(&fIJ);
  }
  else if(params.ref == 2) { /** UHF **/

    dpd_->file2_init(&fIJ, PSIF_CC_OEI, 0, 0, 0, "fIJ");
    dpd_->file2_init(&fij, PSIF_CC_OEI, 0, 2, 2, "fij");
    dpd_->file2_copy(&fIJ, PSIF_CC_OEI, "fIJ diag");
    dpd_->file2_copy(&fij, PSIF_CC_OEI, "fij diag");
    dpd_->file2_close(&fIJ);
    dpd_->file2_close(&fij);

    dpd_->file2_init(&fIJ, PSIF_CC_OEI, 0, 0, 0, "fIJ diag");
    dpd_->file2_init(&fij, PSIF_CC_OEI, 0, 2, 2, "fij diag");

    dpd_->file2_mat_init(&fIJ);
    dpd_->file2_mat_rd(&fIJ);
    dpd_->file2_mat_init(&fij);
    dpd_->file2_mat_rd(&fij);

    for(h=0; h < moinfo.nirreps; h++) {
      for(m=0; m < fIJ.params->rowtot[h]; m++) 
	fIJ.matrix[h][m][m] = 0;

      for(m=0; m < fij.params->rowtot[h]; m++) 
	fij.matrix[h][m][m] = 0;
    }

    dpd_->file2_mat_wrt(&fIJ);
    dpd_->file2_mat_close(&fIJ);
    dpd_->file2_mat_wrt(&fij);
    dpd_->file2_mat_close(&fij);

    dpd_->file2_close(&fIJ);
    dpd_->file2_close(&fij);

    dpd_->file2_init(&fIJ, PSIF_CC_OEI, 0, 0, 0, "fIJ diag");
    dpd_->file2_init(&fij, PSIF_CC_OEI, 0, 2, 2, "fij diag");

    /** X(IJ,AB) = F(I,M) L(MJ,AB) **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(IJ,AB) B");
    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "LIJAB");
    dpd_->contract244(&fIJ, &LIJAB, &X, 1, 0, 0, -1, 0);
    dpd_->buf4_close(&LIJAB);
    /** X(IJ,AB) --> X'(JI,AB) **/
    dpd_->buf4_sort(&X, PSIF_CC_TMP1, qprs, 0, 7, "X'(JI,AB)");
    dpd_->buf4_close(&X);

    /** X(IJ,AB) = X(IJ,AB) - X'(JI,AB) **/
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X(IJ,AB) B");
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 0, 7, 0, 7, 0, "X'(JI,AB)");
    dpd_->buf4_axpy(&X2, &X1, -1.0);
    dpd_->buf4_close(&X2);
    /** L(IJ,AB) <--- X(IJ,AB) **/
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 0, 7, 2, 7, 0, "New LIJAB");
    dpd_->buf4_axpy(&X1, &newLIJAB, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_close(&newLIJAB);


    /** X(ij,ab) = F(i,m) L(mj,ab) **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 10, 17, 10, 17, 0, "X(ij,ab) B");
    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 10, 17, 12, 17, 0, "Lijab");
    dpd_->contract244(&fij, &LIJAB, &X, 1, 0, 0, -1, 0);
    dpd_->buf4_close(&LIJAB);
    /** X(ij,ab) --> X'(ji,ab) **/
    dpd_->buf4_sort(&X, PSIF_CC_TMP1, qprs, 10, 17, "X'(ji,ab)");
    dpd_->buf4_close(&X);

    /** X(ij,ab) = X(ij,ab) - X'(ji,ab) **/
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 10, 17, 10, 17, 0, "X(ij,ab) B");
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 10, 17, 10, 17, 0, "X'(ji,ab)");
    dpd_->buf4_axpy(&X2, &X1, -1.0);
    dpd_->buf4_close(&X2);
    /** L(ij,ab) <--- X(ij,ab) **/
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 10, 17, 12, 17, 0, "New Lijab");
    dpd_->buf4_axpy(&X1, &newLIJAB, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_close(&newLIJAB);

    /** L(Ij,Ab) <-- L(Im,Ab) F(j,m) - F(I,M) L(Mj,Ab) **/
    dpd_->buf4_init(&LIjAb, PSIF_CC_LAMBDA, L_irr, 22, 28, 22, 28, 0, "LIjAb");
    dpd_->buf4_init(&newLIjAb, PSIF_CC_LAMBDA, L_irr, 22, 28, 22, 28, 0, "New LIjAb");
    dpd_->contract424(&LIjAb, &fij, &newLIjAb, 1, 1, 1, -1, 1);
    dpd_->contract244(&fIJ, &LIjAb, &newLIjAb, 1, 0, 0, -1, 1);
    dpd_->buf4_close(&LIjAb);
    dpd_->buf4_close(&newLIjAb);

    dpd_->file2_close(&fij);
    dpd_->file2_close(&fIJ);
  }
}

}} // namespace psi::cclambda
