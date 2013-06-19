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
    \ingroup CCDENSITY
    \brief Enter brief description of file here
*/
#include <cstdio>
#include <strings.h>
#include <string.h>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#include "Frozen.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace ccdensity {

    void Gijka(void)
    {
      int h, nirreps, i, j, k, a, I, J, K, A, Isym, Jsym, Ksym, Asym, row, col;
      double value;
      dpdfile2 L1, T1, g;
      dpdbuf4 G, V, T, L, Z, Z1, Z2;
      double factor=0.0;

      nirreps = moinfo.nirreps;

      if(params.ref == 0) { /** RHF **/
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");
    /* - tau(Ij,Ea) l(K,E) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauIjAb");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "LIA");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, 0.0);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(Ij,Ea) t(K,E) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(Ij,Km) t(m,a) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 0, 0, 0, 0, 0, "VMnIj");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(Ia,Kf) T(j,f) --> Z(Ka,Ij) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(Ia,Kj)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "VIaJb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 10, 0, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z);
    /* V(ja,KF) T(I,F) --> Z(Ka,jI) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(ja,KI)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "ViaJB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP2, rqps, 10, 0, "Z(Ka,jI)");
    dpd_->buf4_close(&Z);
    /* Z(Ka,Ij) - Z(Ka,jI) --> G(Ij,Ka) */
    dpd_->buf4_init(&Z2, PSIF_CC_TMP2, 0, 10, 0, 10, 0, 0, "Z(Ka,jI)");
    dpd_->buf4_sort(&Z2, PSIF_CC_TMP0, pqsr, 10, 0, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z2);
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 10, 0, 10, 0, 0, "Z(Ka,Ij)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(Ka,Ij)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 10, "Z(Ij,Ka)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(Ij,Ka)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - g(I,K) T(j,a) --> G(Ij,Ka) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "GMI");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);
      }
      else if(params.ref == 1) { /** ROHF **/

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 10, 2, 10, 0, "GIJKA");
    /* - tau(IJ,EA) l(K,E) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 2, 5, 2, 7, 0, "tauIJAB");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "LIA");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, 0.0);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* - L(IJ,EA) t(K,E) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 2, 5, 2, 7, 0, "LIJAB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(IJ,KM) t(M,A) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 2, 0, 2, 2, 0, "VMNIJ");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(IA,KF) T(J,F) --> Z(KA,IJ) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(IA,KJ)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "VIAJB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 10, 0, "Z(KA,IJ)");
    dpd_->buf4_close(&Z);
    /* Z(KA,IJ) - Z(KA,JI) --> G(IJ,KA) */
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 10, 0, 10, 0, 0, "Z(KA,IJ)");
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, pqsr, 10, 0, "Z(KA,JI)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(KA,JI)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 10, "Z(IJ,KA)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 2, 10, 0, "GIJKA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(IJ,KA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - ( g(I,K) T(J,A) - g(J,K) T(I,A) ) --> G(IJ,KA) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "GMI");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 10, 2, 10, 0, "GIJKA");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          J = g.params->rowidx[j];  I = T1.params->rowidx[i];
          Jsym = g.params->psym[j]; Isym = T1.params->psym[i];

          if((Jsym==Ksym) && (Isym==Asym))
        value -= g.matrix[Jsym][J][K] * T1.matrix[Isym][I][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 10, 2, 10, 0, "Gijka");
    /* - tau(ij,ea) l(k,e) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 2, 5, 2, 7, 0, "tauijab");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "Lia");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, 0.0);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(ij,ea) t(k,e) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 2, 5, 2, 7, 0, "Lijab");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(ij,km) t(m,a) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 2, 0, 2, 2, 0, "Vmnij");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(ia,kf) T(j,f) --> Z(ka,ij) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(ia,kj)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "Viajb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 10, 0, "Z(ka,ij)");
    dpd_->buf4_close(&Z);
    /* Z(ka,ij) - Z(ka,ji) --> G(ij,ka) */
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 10, 0, 10, 0, 0, "Z(ka,ij)");
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, pqsr, 10, 0, "Z(ka,ji)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(ka,ji)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 10, "Z(ij,ka)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 2, 10, 0, "Gijka");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(ij,ka)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - ( g(i,k) T(j,a) - g(j,k) T(i,a) ) --> G(ij,ka) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "Gmi");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 10, 2, 10, 0, "Gijka");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          J = g.params->rowidx[j];  I = T1.params->rowidx[i];
          Jsym = g.params->psym[j]; Isym = T1.params->psym[i];

          if((Jsym==Ksym) && (Isym==Asym))
        value -= g.matrix[Jsym][J][K] * T1.matrix[Isym][I][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");
    /* - tau(Ij,Ea) l(K,E) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauIjAb");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "LIA");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, 0.0);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(Ij,Ea) t(K,E) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(Ij,Km) t(m,a) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 0, 0, 0, 0, 0, "VMnIj");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(Ia,Kf) T(j,f) --> Z(Ka,Ij) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(Ia,Kj)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "VIaJb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 10, 0, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z);
    /* V(ja,KF) T(I,F) --> Z(Ka,jI) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(ja,KI)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "ViaJB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP2, rqps, 10, 0, "Z(Ka,jI)");
    dpd_->buf4_close(&Z);
    /* Z(Ka,Ij) - Z(Ka,jI) --> G(Ij,Ka) */
    dpd_->buf4_init(&Z2, PSIF_CC_TMP2, 0, 10, 0, 10, 0, 0, "Z(Ka,jI)");
    dpd_->buf4_sort(&Z2, PSIF_CC_TMP0, pqsr, 10, 0, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z2);
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 10, 0, 10, 0, 0, "Z(Ka,Ij)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(Ka,Ij)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 10, "Z(Ij,Ka)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(Ij,Ka)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - g(I,K) T(j,a) --> G(Ij,Ka) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "GMI");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GIjKa");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GiJkA");
    /* - tau(iJ,eA) l(k,e) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tauiJaB");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "Lia");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, 0.0);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(iJ,eA) t(k,e) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 0, 5, 0, 5, 0, "LiJaB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    dpd_->buf4_close(&G);
    /* V(iJ,kM) t(M,A) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 11, 0, 11, 0, "Z(Ji,Ak)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 0, 0, 0, 0, 0, "VMnIj");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &V, &Z, 0, 2, 1, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, qprs, 0, 11, "Z(iJ,Ak)");
    dpd_->buf4_close(&Z);
    dpd_->buf4_init(&Z, PSIF_CC_TMP1, 0, 0, 11, 0, 11, 0, "Z(iJ,Ak)");
    dpd_->buf4_sort(&Z, PSIF_CC_TMP0, pqsr, 0, 10, "Z(iJ,kA)");
    dpd_->buf4_close(&Z);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GiJkA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(iJ,kA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);
    /* V(iA,kF) T(J,F) --> Z(kA,iJ) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(iA,kJ)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "ViAjB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 10, 0, "Z(kA,iJ)");
    dpd_->buf4_close(&Z);
    /* V(iA,kf) T(i,f) --> Z(kA,Ji) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(JA,ki)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 10, 10, 10, 10, 0, "VIAjb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP2, rqps, 10, 0, "Z(kA,Ji)");
    dpd_->buf4_close(&Z);
    /* Z(kA,iJ) - Z(kA,Ji) --> G(iJ,kA) */
    dpd_->buf4_init(&Z2, PSIF_CC_TMP2, 0, 10, 0, 10, 0, 0, "Z(kA,Ji)");
    dpd_->buf4_sort(&Z2, PSIF_CC_TMP0, pqsr, 10, 0, "Z(kA,iJ)");
    dpd_->buf4_close(&Z2);
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 10, 0, 10, 0, 0, "Z(kA,iJ)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 10, 0, 10, 0, 0, "Z(kA,iJ)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 10, "Z(iJ,kA)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GiJkA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 10, 0, 10, 0, "Z(iJ,kA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - g(i,k) T(J,A) --> G(iJ,kA) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "Gmi");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 10, 0, 10, 0, "GiJkA");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);
      }
      else if(params.ref == 2) { /** UHF **/

    if(params.wfn == "CCSD_T" && params.dertype==1) {
      /* For CCSD(T) gradients, some density contributions are
         calculated in cctriples */
      factor = 1.0;
    }

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 20, 2, 20, 0, "GIJKA");
    /* - tau(IJ,EA) l(K,E) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 2, 5, 2, 7, 0, "tauIJAB");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "LIA");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, factor);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* - L(IJ,EA) t(K,E) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 2, 5, 2, 7, 0, "LIJAB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(IJ,KM) t(M,A) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 2, 0, 2, 2, 0, "VMNIJ");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(IA,KF) T(J,F) --> Z(KA,IJ) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 20, 0, 20, 0, 0, "Z(IA,KJ)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 20, 20, 20, 20, 0, "VIAJB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 20, 0, "Z(KA,IJ)");
    dpd_->buf4_close(&Z);
    /* Z(KA,IJ) - Z(KA,JI) --> G(IJ,KA) */
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 20, 0, 20, 0, 0, "Z(KA,IJ)");
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, pqsr, 20, 0, "Z(KA,JI)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 20, 0, 20, 0, 0, "Z(KA,JI)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 0, 20, "Z(IJ,KA)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 0, 20, 2, 20, 0, "GIJKA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 0, 20, 0, 20, 0, "Z(IJ,KA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - ( g(I,K) T(J,A) - g(J,K) T(I,A) ) --> G(IJ,KA) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "GMI");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 2, 20, 2, 20, 0, "GIJKA");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          J = g.params->rowidx[j];  I = T1.params->rowidx[i];
          Jsym = g.params->psym[j]; Isym = T1.params->psym[i];

          if((Jsym==Ksym) && (Isym==Asym))
        value -= g.matrix[Jsym][J][K] * T1.matrix[Isym][I][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 12, 30, 12, 30, 0, "Gijka");
    /* - tau(ij,ea) l(k,e) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 12, 15, 12, 17, 0, "tauijab");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 2, 3, "Lia");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, factor);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(ij,ea) t(k,e) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 12, 15, 12, 17, 0, "Lijab");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(ij,km) t(m,a) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 12, 10, 12, 12, 0, "Vmnij");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(ia,kf) T(j,f) --> Z(ka,ij) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 30, 10, 30, 10, 0, "Z(ia,kj)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 30, 30, 30, 30, 0, "Viajb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 30, 10, "Z(ka,ij)");
    dpd_->buf4_close(&Z);
    /* Z(ka,ij) - Z(ka,ji) --> G(ij,ka) */
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 30, 10, 30, 10, 0, "Z(ka,ij)");
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, pqsr, 30, 10, "Z(ka,ji)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 30, 10, 30, 10, 0, "Z(ka,ji)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 10, 30, "Z(ij,ka)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 10, 30, 12, 30, 0, "Gijka");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 10, 30, 10, 30, 0, "Z(ij,ka)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - ( g(i,k) T(j,a) - g(j,k) T(i,a) ) --> G(ij,ka) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 2, 2, "Gmi");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 12, 30, 12, 30, 0, "Gijka");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          J = g.params->rowidx[j];  I = T1.params->rowidx[i];
          Jsym = g.params->psym[j]; Isym = T1.params->psym[i];

          if((Jsym==Ksym) && (Isym==Asym))
        value -= g.matrix[Jsym][J][K] * T1.matrix[Isym][I][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 22, 24, 22, 24, 0, "GIjKa");
    /* - tau(Ij,Ea) l(K,E) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 22, 28, 22, 28, 0, "tauIjAb");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 0, 1, "LIA");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, factor);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(Ij,Ea) t(K,E) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 22, 28, 22, 28, 0, "LIjAb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    /* V(Ij,Km) t(m,a) */
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 22, 22, 22, 22, 0, "VMnIj");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract424(&V, &T1, &G, 3, 0, 0, 1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_close(&G);
    /* V(Ia,Kf) T(j,f) --> Z(Ka,Ij) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 24, 22, 24, 22, 0, "Z(Ia,Kj)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 24, 24, 24, 24, 0, "VIaJb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 24, 22, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z);
    /* V(ja,KF) T(I,F) --> Z(Ka,jI) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 30, 0, 30, 0, 0, "Z(ja,KI)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 30, 20, 30, 20, 0, "ViaJB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP2, rqps, 24, 23, "Z(Ka,jI)");
    dpd_->buf4_close(&Z);
    /* Z(Ka,Ij) - Z(Ka,jI) --> G(Ij,Ka) */
    dpd_->buf4_init(&Z2, PSIF_CC_TMP2, 0, 24, 23, 24, 23, 0, "Z(Ka,jI)");
    dpd_->buf4_sort(&Z2, PSIF_CC_TMP0, pqsr, 24, 22, "Z(Ka,Ij)");
    dpd_->buf4_close(&Z2);
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 24, 22, 24, 22, 0, "Z(Ka,Ij)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 24, 22, 24, 22, 0, "Z(Ka,Ij)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 22, 24, "Z(Ij,Ka)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 22, 24, 22, 24, 0, "GIjKa");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 22, 24, 22, 24, 0, "Z(Ij,Ka)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - g(I,K) T(j,a) --> G(Ij,Ka) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 0, 0, "GMI");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 22, 24, 22, 24, 0, "GIjKa");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);


    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 23, 27, 23, 27, 0, "GiJkA");
    /* - tau(iJ,eA) l(k,e) */
    dpd_->buf4_init(&T, PSIF_CC_TAMPS, 0, 23, 29, 23, 29, 0, "tauiJaB");
    dpd_->file2_init(&L1, PSIF_CC_GLG, 0, 2, 3, "Lia");
    dpd_->contract244(&L1, &T, &G, 1, 2, 1, -1.0, factor);
    dpd_->file2_close(&L1);
    dpd_->buf4_close(&T);
    /* -L(iJ,eA) t(k,e) */
    dpd_->buf4_init(&L, PSIF_CC_GLG, 0, 23, 29, 23, 29, 0, "LiJaB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract244(&T1, &L, &G, 1, 2, 1, -1.0, 1.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&L);
    dpd_->buf4_close(&G);
    /* V(iJ,kM) t(M,A) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 22, 26, 22, 26, 0, "Z(Ji,Ak)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 22, 22, 22, 22, 0, "VMnIj");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract244(&T1, &V, &Z, 0, 2, 1, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, qprs, 23, 26, "Z(iJ,Ak)");
    dpd_->buf4_close(&Z);
    dpd_->buf4_init(&Z, PSIF_CC_TMP1, 0, 23, 26, 23, 26, 0, "Z(iJ,Ak)");
    dpd_->buf4_sort(&Z, PSIF_CC_TMP0, pqsr, 23, 27, "Z(iJ,kA)");
    dpd_->buf4_close(&Z);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 23, 27, 23, 27, 0, "GiJkA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 23, 27, 23, 27, 0, "Z(iJ,kA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);
    /* V(iA,kF) T(J,F) --> Z(kA,iJ) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 27, 23, 27, 23, 0, "Z(iA,kJ)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 27, 27, 27, 27, 0, "ViAjB");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP1, rqps, 27, 23, "Z(kA,iJ)");
    dpd_->buf4_close(&Z);
    /* V(iA,kf) T(i,f) --> Z(kA,Ji) */
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 20, 10, 20, 10, 0, "Z(JA,ki)");
    dpd_->buf4_init(&V, PSIF_CC_MISC, 0, 20, 30, 20, 30, 0, "VIAjb");
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 2, 3, "tia");
    dpd_->contract424(&V, &T1, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&T1);
    dpd_->buf4_close(&V);
    dpd_->buf4_sort(&Z, PSIF_CC_TMP2, rqps, 27, 22, "Z(kA,Ji)");
    dpd_->buf4_close(&Z);
    /* Z(kA,iJ) - Z(kA,Ji) --> G(iJ,kA) */
    dpd_->buf4_init(&Z2, PSIF_CC_TMP2, 0, 27, 22, 27, 22, 0, "Z(kA,Ji)");
    dpd_->buf4_sort(&Z2, PSIF_CC_TMP0, pqsr, 27, 23, "Z(kA,iJ)");
    dpd_->buf4_close(&Z2);
    dpd_->buf4_init(&Z1, PSIF_CC_TMP1, 0, 27, 23, 27, 23, 0, "Z(kA,iJ)");
    dpd_->buf4_init(&Z2, PSIF_CC_TMP0, 0, 27, 23, 27, 23, 0, "Z(kA,iJ)");
    dpd_->buf4_axpy(&Z2, &Z1, -1.0);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_sort(&Z1, PSIF_CC_TMP0, rspq, 23, 27, "Z(iJ,kA)");
    dpd_->buf4_close(&Z1);
    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 23, 27, 23, 27, 0, "GiJkA");
    dpd_->buf4_init(&Z, PSIF_CC_TMP0, 0, 23, 27, 23, 27, 0, "Z(iJ,kA)");
    dpd_->buf4_axpy(&Z, &G, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&G);

    /* - g(i,k) T(J,A) --> G(iJ,kA) */
    dpd_->file2_init(&g, PSIF_CC_GLG, 0, 2, 2, "Gmi");
    dpd_->file2_mat_init(&g);
    dpd_->file2_mat_rd(&g);
    dpd_->file2_init(&T1, PSIF_CC_OEI, 0, 0, 1, "tIA");
    dpd_->file2_mat_init(&T1);
    dpd_->file2_mat_rd(&T1);

    dpd_->buf4_init(&G, PSIF_CC_GAMMA, 0, 23, 27, 23, 27, 0, "GiJkA");

    for(h=0; h < nirreps; h++) {
      dpd_->buf4_mat_irrep_init(&G, h);
      dpd_->buf4_mat_irrep_rd(&G, h);

      for(row=0; row < G.params->rowtot[h]; row++) {
        i = G.params->roworb[h][row][0];
        j = G.params->roworb[h][row][1];
        for(col=0; col < G.params->coltot[h]; col++) {
          k = G.params->colorb[h][col][0];
          a = G.params->colorb[h][col][1];

          value = 0.0;

          I = g.params->rowidx[i];  J = T1.params->rowidx[j];
          Isym = g.params->psym[i]; Jsym = T1.params->psym[j];
          K = g.params->colidx[k];  A = T1.params->colidx[a];
          Ksym = g.params->qsym[k];  Asym = T1.params->qsym[a];

          if((Isym==Ksym) && (Jsym==Asym))
        value += g.matrix[Isym][I][K] * T1.matrix[Jsym][J][A];

          G.matrix[h][row][col] -= value;
        }
      }

      dpd_->buf4_mat_irrep_wrt(&G, h);
      dpd_->buf4_mat_irrep_close(&G, h);
    }

    dpd_->buf4_scm(&G, 0.5);
    dpd_->buf4_close(&G);

    dpd_->file2_mat_close(&g);
    dpd_->file2_close(&g);
    dpd_->file2_mat_close(&T1);
    dpd_->file2_close(&T1);
      }

    }


  }} // namespace psi::ccdensity
