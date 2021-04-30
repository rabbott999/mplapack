/*
 * Copyright (c) 2008-2021
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <mpblas.h>
#include <mplapack.h>

void Rerrqrt(common &cmn, const char *path, INTEGER const nunit) {
    common_write write(cmn);
    // COMMON infoc
    INTEGER &infot = cmn.infot;
    INTEGER &nout = cmn.nout;
    bool &ok = cmn.ok;
    bool &lerr = cmn.lerr;
    // COMMON srnamc
    str<32> &srnamt = cmn.srnamt;
    //
    //
    //  -- LAPACK test routine --
    //  -- LAPACK is a software package provided by Univ. of Tennessee,    --
    //  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
    //
    //     .. Scalar Arguments ..
    //     ..
    //
    //  =====================================================================
    //
    //     .. Parameters ..
    //     ..
    //     .. Local Scalars ..
    //     ..
    //     .. Local Arrays ..
    //     ..
    //     .. External Subroutines ..
    //     ..
    //     .. Scalars in Common ..
    //     ..
    //     .. Common blocks ..
    //     ..
    //     .. Intrinsic Functions ..
    //     ..
    //     .. Executable Statements ..
    //
    nout = nunit;
    write(nout, star);
    //
    //     Set the variables to innocuous values.
    //
    INTEGER j = 0;
    const INTEGER nmax = 2;
    INTEGER i = 0;
    arr_2d<nmax, nmax, REAL> a(fill0);
    arr_2d<nmax, nmax, REAL> c(fill0);
    arr_2d<nmax, nmax, REAL> t(fill0);
    arr_1d<nmax, REAL> w(fill0);
    for (j = 1; j <= nmax; j = j + 1) {
        for (i = 1; i <= nmax; i = i + 1) {
            a[(i - 1) + (j - 1) * lda] = 1.0 / (i + j).real();
            c[(i - 1) + (j - 1) * ldc] = 1.0 / (i + j).real();
            t[(i - 1) + (j - 1) * ldt] = 1.0 / (i + j).real();
        }
        w[j - 1] = 0.0;
    }
    ok = true;
    //
    //     Error exits for QRT factorization
    //
    //     Rgeqrt
    //
    srnamt = "Rgeqrt";
    infot = 1;
    INTEGER info = 0;
    Rgeqrt(-1, 0, 1, a, 1, t, 1, w, info);
    chkxer("Rgeqrt", infot, nout, lerr, ok);
    infot = 2;
    Rgeqrt(0, -1, 1, a, 1, t, 1, w, info);
    chkxer("Rgeqrt", infot, nout, lerr, ok);
    infot = 3;
    Rgeqrt(0, 0, 0, a, 1, t, 1, w, info);
    chkxer("Rgeqrt", infot, nout, lerr, ok);
    infot = 5;
    Rgeqrt(2, 1, 1, a, 1, t, 1, w, info);
    chkxer("Rgeqrt", infot, nout, lerr, ok);
    infot = 7;
    Rgeqrt(2, 2, 2, a, 2, t, 1, w, info);
    chkxer("Rgeqrt", infot, nout, lerr, ok);
    //
    //     Rgeqrt2
    //
    srnamt = "Rgeqrt2";
    infot = 1;
    Rgeqrt2(-1, 0, a, 1, t, 1, info);
    chkxer("Rgeqrt2", infot, nout, lerr, ok);
    infot = 2;
    Rgeqrt2(0, -1, a, 1, t, 1, info);
    chkxer("Rgeqrt2", infot, nout, lerr, ok);
    infot = 4;
    Rgeqrt2(2, 1, a, 1, t, 1, info);
    chkxer("Rgeqrt2", infot, nout, lerr, ok);
    infot = 6;
    Rgeqrt2(2, 2, a, 2, t, 1, info);
    chkxer("Rgeqrt2", infot, nout, lerr, ok);
    //
    //     Rgeqrt3
    //
    srnamt = "Rgeqrt3";
    infot = 1;
    Rgeqrt3(-1, 0, a, 1, t, 1, info);
    chkxer("Rgeqrt3", infot, nout, lerr, ok);
    infot = 2;
    Rgeqrt3(0, -1, a, 1, t, 1, info);
    chkxer("Rgeqrt3", infot, nout, lerr, ok);
    infot = 4;
    Rgeqrt3(2, 1, a, 1, t, 1, info);
    chkxer("Rgeqrt3", infot, nout, lerr, ok);
    infot = 6;
    Rgeqrt3(2, 2, a, 2, t, 1, info);
    chkxer("Rgeqrt3", infot, nout, lerr, ok);
    //
    //     Rgemqrt
    //
    srnamt = "Rgemqrt";
    infot = 1;
    Rgemqrt("/", "N", 0, 0, 0, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 2;
    Rgemqrt("L", "/", 0, 0, 0, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 3;
    Rgemqrt("L", "N", -1, 0, 0, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 4;
    Rgemqrt("L", "N", 0, -1, 0, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 5;
    Rgemqrt("L", "N", 0, 0, -1, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 5;
    Rgemqrt("R", "N", 0, 0, -1, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 6;
    Rgemqrt("L", "N", 0, 0, 0, 0, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 8;
    Rgemqrt("R", "N", 1, 2, 1, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 8;
    Rgemqrt("L", "N", 2, 1, 1, 1, a, 1, t, 1, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 10;
    Rgemqrt("R", "N", 1, 1, 1, 1, a, 1, t, 0, c, 1, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    infot = 12;
    Rgemqrt("L", "N", 1, 1, 1, 1, a, 1, t, 1, c, 0, w, info);
    chkxer("Rgemqrt", infot, nout, lerr, ok);
    //
    //     Print a summary line.
    //
    Alaesm(path, ok, nout);
    //
    //     End of Rerrqrt
    //
}