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
#include <cstring>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace cclambda {

void cc2_hbar_extra(void) {
  dpdbuf4 W1, W2, W;

  /* CC2 W(ME,jb) + W(Me,Jb) is constructed in CCEOM for EOM_CC2 */

  if(params.wfn == "CC2") {
    if(params.ref == 0) { /** RHF **/
      /* 2 W(ME,jb) + W(Me,Jb) */
      dpd_->buf4_init(&W1, PSIF_CC2_HET1, 0, 10, 10, 10, 10, 0, "CC2 WMbeJ (Me,Jb)");
      dpd_->buf4_copy(&W1, PSIF_CC2_HET1, "CC2 2 W(ME,jb) + W(Me,Jb)");
      dpd_->buf4_close(&W1);
      dpd_->buf4_init(&W1, PSIF_CC2_HET1, 0, 10, 10, 10, 10, 0, "CC2 2 W(ME,jb) + W(Me,Jb)");
      dpd_->buf4_init(&W2, PSIF_CC2_HET1, 0, 10, 10, 10, 10, 0, "CC2 WMbEj (ME,jb)");
      dpd_->buf4_axpy(&W2, &W1, 2);
      dpd_->buf4_close(&W2);
      dpd_->buf4_close(&W1);
    }
  }
}

}} // namespace psi::cclambda
