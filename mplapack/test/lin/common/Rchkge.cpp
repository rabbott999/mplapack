/*
 * Copyright (c) 2021
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

#include <fem.hpp> // Fortran EMulation library of fable module
using namespace fem::major_types;
using fem::common;

#include <mplapack_matgen.h>
#include <mplapack_lin.h>

#include <mplapack_debug.h>

void Rchkge(bool *dotype, INTEGER const nm, INTEGER *mval, INTEGER const nn, INTEGER *nval, INTEGER const nnb, INTEGER *nbval, INTEGER const nns, INTEGER *nsval, REAL const thresh, bool const tsterr, INTEGER const nmax, REAL *a, REAL *afac, REAL *ainv, REAL *b, REAL *x, REAL *xact, REAL *work, REAL *rwork, INTEGER *iwork, INTEGER const nout) {
    common cmn;
    common_write write(cmn);
    // seeds are ignored
    static const char *values[] = {"N", "T", "C"};
    char path[3];
    INTEGER nrun = 0;
    INTEGER nfail = 0;
    INTEGER nerrs = 0;
    INTEGER i = 0;
    INTEGER iseed[4];
    INTEGER im = 0;
    INTEGER m = 0;
    INTEGER lda = 0;
    INTEGER in = 0;
    INTEGER n = 0;
    char xtype;
    const INTEGER ntypes = 11;
    INTEGER nimat = 0;
    INTEGER imat = 0;
    bool zerot = false;
    char type;
    INTEGER kl = 0;
    INTEGER ku = 0;
    REAL anorm = 0.0;
    INTEGER mode = 0;
    REAL cndnum = 0.0;
    char dist;
    INTEGER info = 0;
    INTEGER izero = 0;
    INTEGER ioff = 0;
    const REAL zero = 0.0;
    INTEGER inb = 0;
    INTEGER nb = 0;
    bool trfcon = false;
    const INTEGER ntests = 8;
    REAL result[ntests];
    INTEGER nt = 0;
    INTEGER nrhs = 0;
    INTEGER lwork = 0;
    REAL rcondo = 0.0;
    REAL anormo = 0.0;
    REAL anormi = 0.0;
    REAL ainvnm = 0.0;
    const REAL one = 1.0;
    REAL rcondi = 0.0;
    INTEGER k = 0;
    INTEGER irhs = 0;
    INTEGER itran = 0;
    char trans;
    REAL rcondc = 0.0;
    char norm;
    REAL rcond = 0.0;
    REAL dummy = 0.0;
    char nopacking[] = "No packing";
    INTEGER ntran = 3;
    char transs[ntran] = {'N', 'T', 'C'};
    //
    //  -- LAPACK test routine --
    //  -- LAPACK is a software package provided by Univ. of Tennessee,    --
    //  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
    //
    //     .. Scalar Arguments ..
    //     ..
    //     .. Array Arguments ..
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
    //     .. External Functions ..
    //     ..
    //     .. External Subroutines ..
    //     ..
    //     .. Intrinsic Functions ..
    //     ..
    //     .. Scalars in Common ..
    //     ..
    //     .. Common blocks ..
    //     ..
    //     .. Data statements ..
    //     ..
    //     .. Executable Statements ..
    //
    //     Initialize constants and the random number seed.
    //
    path[0] = 'R'; // Real
    path[1] = 'G';
    path[2] = 'E';
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i = 1; i <= 4; i = i + 1) {
        iseed[i - 1] = 0; // seed is dummy in MPLAPACK
    }
    //
    //     Test the error exits
    //
    if (tsterr) {
        Rerrge(path, nout);
    }
    //
    //     Do for each value of M in MVAL
    //
    for (im = 1; im <= nm; im = im + 1) {
        m = mval[im - 1];
        lda = max((INTEGER)1, m);
        //
        //        Do for each value of N in NVAL
        //
        for (in = 1; in <= nn; in = in + 1) {
            n = nval[in - 1];
            xtype = 'N';
            nimat = ntypes;
            if (m <= 0 || n <= 0) {
                nimat = 1;
            }
            //
            for (imat = 1; imat <= nimat; imat = imat + 1) {
                //
                //              Do the tests only if DOTYPE( IMAT ) is true.
                //
                if (!dotype[imat - 1]) {
                    continue;
                }
                //
                //              Skip types 5, 6, or 7 if the matrix size is too small.
                //
                zerot = imat >= 5 && imat <= 7;
                if (zerot && n < imat - 4) {
                    continue;
                }
                //
                //              Set up parameters with Rlatb4 and generate a test matrix
                //              with DLATMS.
                //
                Rlatb4(path, imat, m, n, &type, kl, ku, anorm, mode, cndnum, &dist);
                //
                Rlatms(m, n, &dist, iseed, &type, rwork, mode, cndnum, anorm, kl, ku, nopacking, a, lda, work, info);
                //
                //              Check error code from Rlatms.
                //
                if (info != 0) {
                    Alaerh(path, "Rlatms", info, 0, " ", m, n, -1, -1, -1, imat, nfail, nerrs, nout);
                    goto statement_100;
                }
                //
                //              For types 5-7, zero one or more columns of the matrix to
                //              test that INFO is returned correctly.
                //
                if (zerot) {
                    if (imat == 5) {
                        izero = 1;
                    } else if (imat == 6) {
                        izero = min(m, n);
                    } else {
                        izero = min(m, n) / 2 + 1;
                    }
                    ioff = (izero - 1) * lda;
                    if (imat < 7) {
                        for (i = 1; i <= m; i = i + 1) {
                            a[(ioff + i) - 1] = zero;
                        }
                    } else {
                        Rlaset("Full", m, n - izero + 1, zero, zero, &a[(ioff + 1) - 1], lda);
                    }
                } else {
                    izero = 0;
                }
                //
                //              These lines, if used in place of the calls in the DO 60
                //              loop, cause the code to bomb on a Sun SPARCstation.
                //
                //               ANORMO = Rlange( 'O', M, N, A, LDA, RWORK )
                //               ANORMI = Rlange( 'I', M, N, A, LDA, RWORK )
                //
                //              Do for each blocksize in NBVAL
                //
                for (inb = 1; inb <= nnb; inb = inb + 1) {
                    nb = nbval[inb - 1];
                    //
                    //                 Compute the LU factorization of the matrix.
                    //
                    Rlacpy("Full", m, n, a, lda, afac, lda);
                    Rgetrf(m, n, afac, lda, iwork, info);
                    //
                    //                 Check error code from Rgetrf.
                    //
                    printf("Rgetrf m:%ld n:%ld lda:%ld info: %ld infoe: %ld \n", m, n, lda, info, izero);
                    if (info != izero) {
                        Alaerh(path, "Rgetrf", info, izero, " ", m, n, -1, -1, nb, imat, nfail, nerrs, nout);
                    }
                    trfcon = false;
                    //
                    //+    TEST 1
                    //                 Reconstruct matrix from factors and compute residual.
                    //
                    Rlacpy("Full", m, n, afac, lda, ainv, lda);
                    Rget01(m, n, a, lda, ainv, lda, iwork, rwork, result[1 - 1]);
                    nt = 1;
                    //
                    //+    TEST 2
                    //                 Form the inverse if the factorization was successful
                    //                 and compute the residual.
                    //
                    if (m == n && info == 0) {
                        Rlacpy("Full", n, n, afac, lda, ainv, lda);
                        nrhs = nsval[1 - 1];
                        lwork = nmax * max(3, nrhs);
                        Rgetri(n, ainv, lda, iwork, work, lwork, info);
                        //
                        //                    Check error code from Rgetri.
                        //
                        if (info != 0) {
                            Alaerh(path, "Rgetri", info, 0, " ", n, n, -1, -1, nb, imat, nfail, nerrs, nout);
                        }
                        //
                        //                    Compute the residual for the matrix times its
                        //                    inverse.  Also compute the 1-norm condition number
                        //                    of A.
                        //
                        Rget03(n, a, lda, ainv, lda, work, lda, rwork, rcondo, result[2 - 1]);
                        anormo = Rlange("O", m, n, a, lda, rwork);
                        //
                        //                    Compute the infinity-norm condition number of A.
                        //
                        anormi = Rlange("I", m, n, a, lda, rwork);
                        ainvnm = Rlange("I", n, n, ainv, lda, rwork);
                        if (anormi <= zero || ainvnm <= zero) {
                            rcondi = one;
                        } else {
                            rcondi = (one / anormi) / ainvnm;
                        }
                        nt = 2;
                    } else {
                        //
                        //                    Do only the condition estimate if INFO > 0.
                        //
                        trfcon = true;
                        anormo = Rlange("O", m, n, a, lda, rwork);
                        anormi = Rlange("I", m, n, a, lda, rwork);
                        rcondo = zero;
                        rcondi = zero;
                    }
                    //
                    //                 Print information about the tests so far that did not
                    //                 pass the threshold.
                    //
                    for (k = 1; k <= nt; k = k + 1) {
                        if (result[k - 1] >= thresh) {
                            if (nfail == 0 && nerrs == 0) {
                                Alahd(nout, path);
                            }
                            write(nout, "(' M = ',i5,', N =',i5,', NB =',i4,', type ',i2,', test(',i2,"
                                        "') =',g12.5) XXXcast2doubleXXX"),
                                m, n, nb, imat, k, cast2double(result[k - 1]);
                            nfail++;
                        }
                    }
                    nrun += nt;
                    //
                    //                 Skip the remaining tests if this is not the first
                    //                 block size or if M .ne. N.  Skip the solve tests if
                    //                 the matrix is singular.
                    //
                    if (inb > 1 || m != n) {
                        goto statement_90;
                    }
                    if (trfcon) {
                        goto statement_70;
                    }
                    //
                    for (irhs = 1; irhs <= nns; irhs = irhs + 1) {
                        nrhs = nsval[irhs - 1];
                        xtype = 'N';
                        //
                        for (itran = 1; itran <= ntran; itran = itran + 1) {
                            trans = transs[itran - 1];
                            if (itran == 1) {
                                rcondc = rcondo;
                            } else {
                                rcondc = rcondi;
                            }
                            //
                            //+    TEST 3
                            //                       Solve and compute residual for A * X = B.
                            //
                            Rlarhs(path, &xtype, " ", &trans, n, n, kl, ku, nrhs, a, lda, xact, lda, b, lda, iseed, info);
                            xtype = 'C';
                            //
                            Rlacpy("Full", n, nrhs, b, lda, x, lda);
                            Rgetrs(&trans, n, nrhs, afac, lda, iwork, x, lda, info);
                            //
                            //                       Check error code from Rgetrs.
                            //
                            if (info != 0) {
                                Alaerh(path, "Rgetrs", info, 0, &trans, n, n, -1, -1, nrhs, imat, nfail, nerrs, nout);
                            }
                            //
                            Rlacpy("Full", n, nrhs, b, lda, work, lda);
                            Rget02(&trans, n, n, nrhs, a, lda, x, lda, work, lda, rwork, result[3 - 1]);
                            //
                            //+    TEST 4
                            //                       Check solution from generated exact solution.
                            //
                            Rget04(n, nrhs, x, lda, xact, lda, rcondc, result[4 - 1]);
                            //
                            //+    TESTS 5, 6, and 7
                            //                       Use iterative refinement to improve the
                            //                       solution.
                            //
                            Rgerfs(&trans, n, nrhs, a, lda, afac, lda, iwork, b, lda, x, lda, rwork, &rwork[(nrhs + 1) - 1], work, &iwork[(n + 1) - 1], info);
                            //
                            //                       Check error code from RgerFS.
                            //
                            if (info != 0) {
                                Alaerh(path, "Rgerfs", info, 0, &trans, n, n, -1, -1, nrhs, imat, nfail, nerrs, nout);
                            }
                            //
                            Rget04(n, nrhs, x, lda, xact, lda, rcondc, result[5 - 1]);
                            Rget07(&trans, n, nrhs, a, lda, b, lda, x, lda, xact, lda, rwork, true, &rwork[(nrhs + 1) - 1], &result[6 - 1]);
                            //
                            //                       Print information about the tests that did not
                            //                       pass the threshold.
                            //
                            for (k = 3; k <= 7; k = k + 1) {
                                if (result[k - 1] >= thresh) {
                                    if (nfail == 0 && nerrs == 0) {
                                        Alahd(nout, path);
                                    }
                                    write(nout, "(' TRANS=''',a1,''', N =',i5,', NRHS=',i3,', type ',i2,"
                                                "', test(',i2,') =',g12.5) xxx cast2double xxx"),
                                        trans, n, nrhs, imat, k, cast2double(result[k - 1]);
                                    nfail++;
                                }
                            }
                            nrun += 5;
                        }
                    }
                //
                //+    TEST 8
                //                    Get an estimate of RCOND = 1/CNDNUM.
                //
                statement_70:
                    for (itran = 1; itran <= 2; itran = itran + 1) {
                        if (itran == 1) {
                            anorm = anormo;
                            rcondc = rcondo;
                            norm = 'O';
                        } else {
                            anorm = anormi;
                            rcondc = rcondi;
                            norm = 'I';
                        }
                        Rgecon(&norm, n, afac, lda, anorm, rcond, work, &iwork[(n + 1) - 1], info);
                        //
                        //                       Check error code from Rgecon.
                        //
                        if (info != 0) {
                            Alaerh(path, "Rgecon", info, 0, &norm, n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                        }
                        //
                        //                       This line is needed on a Sun SPARCstation.
                        //
                        dummy = rcond;
                        //
                        result[8 - 1] = Rget06(rcond, rcondc);
                        //
                        //                    Print information about the tests that did not pass
                        //                    the threshold.
                        //
                        if (result[8 - 1] >= thresh) {
                            if (nfail == 0 && nerrs == 0) {
                                Alahd(nout, path);
                            }
                            write(nout, "(' NORM =''',a1,''', N =',i5,',',10x,' type ',i2,', test(',"
                                        "i2,') =',g12.5) xxx cast2double xxx"),
                                norm, n, imat, 8, cast2double(result[8 - 1]);
                            nfail++;
                        }
                        nrun++;
                    }
                statement_90:;
                }
            statement_100:;
            }
        }
    }
    //
    //     Print a summary of the results.
    //
    Alasum(path, nout, nfail, nrun, nerrs);
    //
    //     End of Rchkge
    //
}