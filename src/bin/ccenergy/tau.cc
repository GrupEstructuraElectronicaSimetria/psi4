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
    \ingroup CCENERGY
    \brief Enter brief description of file here 
*/
#include <cstdio>
#include <cstdlib>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace ccenergy {

void tau_build(void)
{
  int h, ij, ab, i, j, a, b, I, J, A, B;
  int Isym, Jsym, Asym, Bsym;
  int nirreps;
  dpdbuf4 tauIJAB, tauijab, tauIjAb, tauiJaB, tauIjbA;
  dpdbuf4 tIJAB, tijab, tIjAb;
  dpdfile2 tIA, tia;

  nirreps = moinfo.nirreps;

  if(params.ref == 0) { /** RHF **/

    dpd_->buf4_init(&tIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tIjAb");
    dpd_->buf4_copy(&tIjAb, PSIF_CC_TAMPS, "tauIjAb");
    dpd_->buf4_close(&tIjAb);

    dpd_->file2_init(&tIA, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&tIA);
    dpd_->file2_mat_rd(&tIA);

    dpd_->buf4_init(&tauIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauIjAb");

    for(h=0; h < nirreps; h++) {

      dpd_->buf4_mat_irrep_init(&tauIjAb, h);
      dpd_->buf4_mat_irrep_rd(&tauIjAb, h);

      for(ij=0; ij < tauIjAb.params->rowtot[h]; ij++) {
	i = tauIjAb.params->roworb[h][ij][0];
	j = tauIjAb.params->roworb[h][ij][1];
	I = tIA.params->rowidx[i];
	J = tIA.params->rowidx[j];
	Isym = tIA.params->psym[i];
	Jsym = tIA.params->psym[j];
	for(ab=0; ab < tauIjAb.params->coltot[h]; ab++) {
	  a = tauIjAb.params->colorb[h][ab][0];
	  b = tauIjAb.params->colorb[h][ab][1];
	  A = tIA.params->colidx[a];
	  B = tIA.params->colidx[b];
	  Asym = tIA.params->qsym[a];
	  Bsym = tIA.params->qsym[b];

	  if((Isym==Asym) && (Jsym==Bsym))
	    tauIjAb.matrix[h][ij][ab] +=
	      (tIA.matrix[Isym][I][A] * tIA.matrix[Jsym][J][B]);

	}
      }

      dpd_->buf4_mat_irrep_wrt(&tauIjAb, h);
      dpd_->buf4_mat_irrep_close(&tauIjAb, h);
    }

    /* This will generate the tauIjbA file from tauIjAb */
    dpd_->buf4_sort(&tauIjAb, PSIF_CC_TAMPS, pqsr, 0, 5, "tauIjbA");
    dpd_->buf4_close(&tauIjAb);

    dpd_->file2_mat_close(&tIA);
    dpd_->file2_close(&tIA);
  }
  else if(params.ref == 1) { /** ROHF **/

    dpd_->buf4_init(&tIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tIJAB");
    dpd_->buf4_copy(&tIJAB, PSIF_CC_TAMPS, "tauIJAB");
    dpd_->buf4_close(&tIJAB);

    dpd_->buf4_init(&tijab, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tijab");
    dpd_->buf4_copy(&tijab, PSIF_CC_TAMPS, "tauijab");
    dpd_->buf4_close(&tijab);

    dpd_->buf4_init(&tIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tIjAb");
    dpd_->buf4_copy(&tIjAb, PSIF_CC_TAMPS, "tauIjAb");
    dpd_->buf4_close(&tIjAb);

    dpd_->file2_init(&tIA, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&tIA);
    dpd_->file2_mat_rd(&tIA);
    dpd_->file2_init(&tia, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->file2_mat_init(&tia);
    dpd_->file2_mat_rd(&tia);

    dpd_->buf4_init(&tauIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tauIJAB");

    for(h=0; h < nirreps; h++) {

      dpd_->buf4_mat_irrep_init(&tauIJAB, h);
      dpd_->buf4_mat_irrep_rd(&tauIJAB, h);

      for(ij=0; ij < tauIJAB.params->rowtot[h]; ij++) {
	i = tauIJAB.params->roworb[h][ij][0];
	j = tauIJAB.params->roworb[h][ij][1];
	I = tIA.params->rowidx[i];
	J = tIA.params->rowidx[j];
	Isym = tIA.params->psym[i];
	Jsym = tIA.params->psym[j];
	for(ab=0; ab < tauIJAB.params->coltot[h]; ab++) {
	  a = tauIJAB.params->colorb[h][ab][0];
	  b = tauIJAB.params->colorb[h][ab][1];
	  A = tIA.params->colidx[a];
	  B = tIA.params->colidx[b];
	  Asym = tIA.params->qsym[a];
	  Bsym = tIA.params->qsym[b];

	  if((Isym==Asym) && (Jsym==Bsym))
	    tauIJAB.matrix[h][ij][ab] +=
	      (tIA.matrix[Isym][I][A] * tIA.matrix[Jsym][J][B]);
	  if((Isym==Bsym) && (Jsym==Asym))
	    tauIJAB.matrix[h][ij][ab] -=
	      (tIA.matrix[Isym][I][B] * tIA.matrix[Jsym][J][A]);

	}
      }

      dpd_->buf4_mat_irrep_wrt(&tauIJAB, h);
      dpd_->buf4_mat_irrep_close(&tauIJAB, h);
    }

    dpd_->buf4_close(&tauIJAB);

    dpd_->buf4_init(&tauijab, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tauijab");

    for(h=0; h < nirreps; h++) {

      dpd_->buf4_mat_irrep_init(&tauijab, h);
      dpd_->buf4_mat_irrep_rd(&tauijab, h);

      for(ij=0; ij < tauijab.params->rowtot[h]; ij++) {
	i = tauijab.params->roworb[h][ij][0];
	j = tauijab.params->roworb[h][ij][1];
	I = tia.params->rowidx[i];
	J = tia.params->rowidx[j];
	Isym = tia.params->psym[i];
	Jsym = tia.params->psym[j];
	for(ab=0; ab < tauijab.params->coltot[h]; ab++) {
	  a = tauijab.params->colorb[h][ab][0];
	  b = tauijab.params->colorb[h][ab][1];
	  A = tia.params->colidx[a];
	  B = tia.params->colidx[b];
	  Asym = tia.params->qsym[a];
	  Bsym = tia.params->qsym[b];

	  if((Isym==Asym) && (Jsym==Bsym))
	    tauijab.matrix[h][ij][ab] +=
	      (tia.matrix[Isym][I][A] * tia.matrix[Jsym][J][B]);
	  if((Isym==Bsym) && (Jsym==Asym))
	    tauijab.matrix[h][ij][ab] -=
	      (tia.matrix[Isym][I][B] * tia.matrix[Jsym][J][A]);

	}
      }

      dpd_->buf4_mat_irrep_wrt(&tauijab, h);
      dpd_->buf4_mat_irrep_close(&tauijab, h);
    }

    dpd_->buf4_close(&tauijab);

    dpd_->buf4_init(&tauIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauIjAb");

    for(h=0; h < nirreps; h++) {

      dpd_->buf4_mat_irrep_init(&tauIjAb, h);
      dpd_->buf4_mat_irrep_rd(&tauIjAb, h);

      for(ij=0; ij < tauIjAb.params->rowtot[h]; ij++) {
	i = tauIjAb.params->roworb[h][ij][0];
	j = tauIjAb.params->roworb[h][ij][1];
	I = tIA.params->rowidx[i];
	J = tia.params->rowidx[j];
	Isym = tIA.params->psym[i];
	Jsym = tia.params->psym[j];
	for(ab=0; ab < tauIjAb.params->coltot[h]; ab++) {
	  a = tauIjAb.params->colorb[h][ab][0];
	  b = tauIjAb.params->colorb[h][ab][1];
	  A = tIA.params->colidx[a];
	  B = tia.params->colidx[b];
	  Asym = tIA.params->qsym[a];
	  Bsym = tia.params->qsym[b];

	  if((Isym==Asym) && (Jsym==Bsym))
	    tauIjAb.matrix[h][ij][ab] +=
	      (tIA.matrix[Isym][I][A] * tia.matrix[Jsym][J][B]);

	}
      }

      dpd_->buf4_mat_irrep_wrt(&tauIjAb, h);
      dpd_->buf4_mat_irrep_close(&tauIjAb, h);
    }

    /* This will generate the tauBA and tauIjbA files from tauIjAb */
    dpd_->buf4_sort(&tauIjAb, PSIF_CC_TAMPS, pqsr, 0, 5, "tauIjbA");
    dpd_->buf4_close(&tauIjAb);
    dpd_->buf4_init(&tauIjbA, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauIjbA");
    dpd_->buf4_sort(&tauIjbA, PSIF_CC_TAMPS, qprs,  0, 5, "tauiJaB");
    dpd_->buf4_close(&tauIjbA);

    dpd_->file2_mat_close(&tIA);
    dpd_->file2_close(&tIA);
    dpd_->file2_mat_close(&tia);
    dpd_->file2_close(&tia);

  }
  else if(params.ref == 2) { /*** UHF ***/

    dpd_->buf4_init(&tIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tIJAB");
    dpd_->buf4_copy(&tIJAB, PSIF_CC_TAMPS, "tauIJAB");
    dpd_->buf4_close(&tIJAB);

    dpd_->buf4_init(&tijab, PSIF_CC_TAMPS, 0, 12, 17, 12, 17, 0, "tijab");
    dpd_->buf4_copy(&tijab, PSIF_CC_TAMPS, "tauijab");
    dpd_->buf4_close(&tijab);

    dpd_->buf4_init(&tIjAb, PSIF_CC_TAMPS, 0, 22, 28, 22, 28, 0, "tIjAb");
    dpd_->buf4_copy(&tIjAb, PSIF_CC_TAMPS, "tauIjAb");
    dpd_->buf4_close(&tIjAb);

    dpd_->file2_init(&tIA, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&tIA);
    dpd_->file2_mat_rd(&tIA);
    dpd_->file2_init(&tia, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->file2_mat_init(&tia);
    dpd_->file2_mat_rd(&tia);

    dpd_->buf4_init(&tauIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "tauIJAB");
    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&tauIJAB, h);
      dpd_->buf4_mat_irrep_rd(&tauIJAB, h);
      for(ij=0; ij < tauIJAB.params->rowtot[h]; ij++) {
	i = tauIJAB.params->roworb[h][ij][0];
	j = tauIJAB.params->roworb[h][ij][1];
	I = tIA.params->rowidx[i];
	J = tIA.params->rowidx[j];
	Isym = tIA.params->psym[i];
	Jsym = tIA.params->psym[j];
	for(ab=0; ab < tauIJAB.params->coltot[h]; ab++) {
	  a = tauIJAB.params->colorb[h][ab][0];
	  b = tauIJAB.params->colorb[h][ab][1];
	  A = tIA.params->colidx[a];
	  B = tIA.params->colidx[b];
	  Asym = tIA.params->qsym[a];
	  Bsym = tIA.params->qsym[b];
	  if((Isym==Asym) && (Jsym==Bsym))
	    tauIJAB.matrix[h][ij][ab] +=
	      (tIA.matrix[Isym][I][A] * tIA.matrix[Jsym][J][B]);
	  if((Isym==Bsym) && (Jsym==Asym))
	    tauIJAB.matrix[h][ij][ab] -=
	      (tIA.matrix[Isym][I][B] * tIA.matrix[Jsym][J][A]);
	}
      }
      dpd_->buf4_mat_irrep_wrt(&tauIJAB, h);
      dpd_->buf4_mat_irrep_close(&tauIJAB, h);
    }
    dpd_->buf4_close(&tauIJAB);

    dpd_->buf4_init(&tauijab, PSIF_CC_TAMPS, 0, 12, 17, 12, 17, 0, "tauijab");
    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&tauijab, h);
      dpd_->buf4_mat_irrep_rd(&tauijab, h);
      for(ij=0; ij < tauijab.params->rowtot[h]; ij++) {
	i = tauijab.params->roworb[h][ij][0];
	j = tauijab.params->roworb[h][ij][1];
	I = tia.params->rowidx[i];
	J = tia.params->rowidx[j];
	Isym = tia.params->psym[i];
	Jsym = tia.params->psym[j];
	for(ab=0; ab < tauijab.params->coltot[h]; ab++) {
	  a = tauijab.params->colorb[h][ab][0];
	  b = tauijab.params->colorb[h][ab][1];
	  A = tia.params->colidx[a];
	  B = tia.params->colidx[b];
	  Asym = tia.params->qsym[a];
	  Bsym = tia.params->qsym[b];
	  if((Isym==Asym) && (Jsym==Bsym))
	    tauijab.matrix[h][ij][ab] +=
	      (tia.matrix[Isym][I][A] * tia.matrix[Jsym][J][B]);
	  if((Isym==Bsym) && (Jsym==Asym))
	    tauijab.matrix[h][ij][ab] -=
	      (tia.matrix[Isym][I][B] * tia.matrix[Jsym][J][A]);
	}
      }
      dpd_->buf4_mat_irrep_wrt(&tauijab, h);
      dpd_->buf4_mat_irrep_close(&tauijab, h);
    }
    dpd_->buf4_close(&tauijab);

    dpd_->buf4_init(&tauIjAb, PSIF_CC_TAMPS, 0, 22, 28, 22, 28, 0, "tauIjAb");
    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&tauIjAb, h);
      dpd_->buf4_mat_irrep_rd(&tauIjAb, h);
      for(ij=0; ij < tauIjAb.params->rowtot[h]; ij++) {
	i = tauIjAb.params->roworb[h][ij][0];
	j = tauIjAb.params->roworb[h][ij][1];
	I = tIA.params->rowidx[i];
	J = tia.params->rowidx[j];
	Isym = tIA.params->psym[i];
	Jsym = tia.params->psym[j];
	for(ab=0; ab < tauIjAb.params->coltot[h]; ab++) {
	  a = tauIjAb.params->colorb[h][ab][0];
	  b = tauIjAb.params->colorb[h][ab][1];
	  A = tIA.params->colidx[a];
	  B = tia.params->colidx[b];
	  Asym = tIA.params->qsym[a];
	  Bsym = tia.params->qsym[b];
	  if((Isym==Asym) && (Jsym==Bsym))
	    tauIjAb.matrix[h][ij][ab] +=
	      (tIA.matrix[Isym][I][A] * tia.matrix[Jsym][J][B]);
	}
      }
      dpd_->buf4_mat_irrep_wrt(&tauIjAb, h);
      dpd_->buf4_mat_irrep_close(&tauIjAb, h);
    }
    dpd_->buf4_close(&tauIjAb);

    dpd_->file2_mat_close(&tIA);
    dpd_->file2_close(&tIA);
    dpd_->file2_mat_close(&tia);
    dpd_->file2_close(&tia);

    /* Resort IjAb to IjbA (used in Z.c) and iJaB */
    dpd_->buf4_init(&tauIjAb, PSIF_CC_TAMPS, 0, 22, 28, 22, 28, 0, "tauIjAb");
    dpd_->buf4_sort(&tauIjAb, PSIF_CC_TAMPS, pqsr, 22, 29, "tauIjbA");
    dpd_->buf4_close(&tauIjAb);
    dpd_->buf4_init(&tauIjbA, PSIF_CC_TAMPS, 0, 22, 29, 22, 29, 0, "tauIjbA");
    dpd_->buf4_sort(&tauIjbA, PSIF_CC_TAMPS, qprs,  23, 29, "tauiJaB");
    dpd_->buf4_close(&tauIjbA);

  } /*** UHF ***/
}
}} // namespace psi::ccenergy
