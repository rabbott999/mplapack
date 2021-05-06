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

void Cchktp(bool *dotype, INTEGER const nn, INTEGER *nval, INTEGER const nns, INTEGER *nsval, REAL const thresh, bool const tsterr, INTEGER const /* nmax */, COMPLEX *ap, COMPLEX *ainvp, COMPLEX *b, COMPLEX *x, COMPLEX *xact, COMPLEX *work, REAL *rwork, INTEGER const nout) {
    common_write write(cmn);
    //
    INTEGER iseedy[] = {1988, 1989, 1990, 1991};
    const INTEGER ntran = 3;
    str_arr_ref<1> transs(sve.transs, [ntran]);
    str_arr_ref<1> uplos(sve.uplos, [2]);
    if (is_called_first_time) {
        {
            static const INTEGER values[] = {1988, 1989, 1990, 1991};
            data_of_type<int>(FEM_VALUES_AND_SIZE), iseedy;
        }
        {
            static const char *values[] = {"U", "L"};
            data_of_type_str(FEM_VALUES_AND_SIZE), uplos;
        }
        {
            static const char *values[] = {"N", "T", "C"};
            data_of_type_str(FEM_VALUES_AND_SIZE), transs;
        }
    }
    char path[3];
    INTEGER nrun = 0;
    INTEGER nfail = 0;
    INTEGER nerrs = 0;
    INTEGER i = 0;
    INTEGER iseed[4];
    INTEGER in = 0;
    INTEGER n = 0;
    INTEGER lda = 0;
    INTEGER lap = 0;
    char xtype;
    INTEGER imat = 0;
    const INTEGER ntype1 = 10;
    INTEGER iuplo = 0;
    char uplo;
    char diag;
    INTEGER info = 0;
    INTEGER idiag = 0;
    REAL anorm = 0.0;
    REAL ainvnm = 0.0;
    const REAL zero = 0.0;
    const REAL one = 1.0;
    REAL rcondi = 0.0;
    REAL rcondo = 0.0;
    const INTEGER ntests = 9;
    REAL result[ntests];
    INTEGER irhs = 0;
    INTEGER nrhs = 0;
    INTEGER itran = 0;
    char trans;
    char norm;
    REAL rcondc = 0.0;
    INTEGER k = 0;
    REAL rcond = 0.0;
    const INTEGER ntypes = 18;
    REAL scale = 0.0;
    static const char *format_9996 = "(1x,a,'( ''',a1,''', ''',a1,''', ''',a1,''', ''',a1,''',',i5,"
                                     "', ... ), type ',i2,', test(',i2,')=',g12.5)";
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
    //     .. Scalars in Common ..
    //     ..
    //     .. Common blocks ..
    //     ..
    //     .. Intrinsic Functions ..
    //     ..
    //     .. Data statements ..
    //     ..
    //     .. Executable Statements ..
    //
    //     Initialize constants and the random number seed.
    //
    path[(1 - 1)] = "Zomplex precision";
    path[(2 - 1) + (3 - 1) * ldpath] = "TP";
    nrun = 0;
    nfail = 0;
    nerrs = 0;
    for (i = 1; i <= 4; i = i + 1) {
        iseed[i - 1] = iseedy[i - 1];
    }
    //
    //     Test the error exits
    //
    if (tsterr) {
        Cerrtr(path, nout);
    }
    cmn.infot = 0;
    //
    for (in = 1; in <= nn; in = in + 1) {
        //
        //        Do for each value of N in NVAL
        //
        n = nval[in - 1];
        lda = max((INTEGER)1, n);
        lap = lda * (lda + 1) / 2;
        xtype = "N";
        //
        for (imat = 1; imat <= ntype1; imat = imat + 1) {
            //
            //           Do the tests only if DOTYPE( IMAT ) is true.
            //
            if (!dotype[imat - 1]) {
                goto statement_70;
            }
            //
            for (iuplo = 1; iuplo <= 2; iuplo = iuplo + 1) {
                //
                //              Do first for UPLO = 'U', then for UPLO = 'L'
                //
                uplo = uplos[iuplo - 1];
                //
                //              Call Clattp to generate a triangular test matrix.
                //
                Clattp(imat, uplo, "No transpose", diag, iseed, n, ap, x, work, rwork, info);
                //
                //              Set IDIAG = 1 for non-unit matrices, 2 for unit.
                //
                if (Mlsame(diag, "N")) {
                    idiag = 1;
                } else {
                    idiag = 2;
                }
                //
                //+    TEST 1
                //              Form the inverse of A.
                //
                if (n > 0) {
                    Ccopy(lap, ap, 1, ainvp, 1);
                }
                Ctptri(uplo, diag, n, ainvp, info);
                //
                //              Check error code from Ctptri.
                //
                if (info != 0) {
                    Alaerh(path, "Ctptri", info, 0, uplo + diag, n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                }
                //
                //              Compute the infinity-norm condition number of A.
                //
                anorm = Clantp("I", uplo, diag, n, ap, rwork);
                ainvnm = Clantp("I", uplo, diag, n, ainvp, rwork);
                if (anorm <= zero || ainvnm <= zero) {
                    rcondi = one;
                } else {
                    rcondi = (one / anorm) / ainvnm;
                }
                //
                //              Compute the residual for the triangular matrix times its
                //              inverse.  Also compute the 1-norm condition number of A.
                //
                Ctpt01(uplo, diag, n, ap, ainvp, rcondo, rwork, result[1 - 1]);
                //
                //              Print the test ratio if it is .GE. THRESH.
                //
                if (result[1 - 1] >= thresh) {
                    if (nfail == 0 && nerrs == 0) {
                        Alahd(nout, path);
                    }
                    write(nout, "(' UPLO=''',a1,''', DIAG=''',a1,''', N=',i5,', type ',i2,"
                                "', test(',i2,')= ',g12.5)"),
                        uplo, diag, n, imat, 1, result(1);
                    nfail++;
                }
                nrun++;
                //
                for (irhs = 1; irhs <= nns; irhs = irhs + 1) {
                    nrhs = nsval[irhs - 1];
                    xtype = "N";
                    //
                    for (itran = 1; itran <= ntran; itran = itran + 1) {
                        //
                        //                 Do for op(A) = A, A**T, or A**H.
                        //
                        trans = transs[itran - 1];
                        if (itran == 1) {
                            norm = "O";
                            rcondc = rcondo;
                        } else {
                            norm = 'I';
                            rcondc = rcondi;
                        }
                        //
                        //+    TEST 2
                        //                 Solve and compute residual for op(A)*x = b.
                        //
                        Clarhs(path, xtype, uplo, trans, n, n, 0, idiag, nrhs, ap, lap, xact, lda, b, lda, iseed, info);
                        xtype = "C";
                        Clacpy("Full", n, nrhs, b, lda, x, lda);
                        //
                        Ctptrs(uplo, trans, diag, n, nrhs, ap, x, lda, info);
                        //
                        //                 Check error code from Ctptrs.
                        //
                        if (info != 0) {
                            Alaerh(path, "Ctptrs", info, 0, uplo + trans + diag, n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                        }
                        //
                        Ctpt02(uplo, trans, diag, n, nrhs, ap, x, lda, b, lda, work, rwork, result[2 - 1]);
                        //
                        //+    TEST 3
                        //                 Check solution from generated exact solution.
                        //
                        Cget04(n, nrhs, x, lda, xact, lda, rcondc, result[3 - 1]);
                        //
                        //+    TESTS 4, 5, and 6
                        //                 Use iterative refinement to improve the solution and
                        //                 compute error bounds.
                        //
                        Ctprfs(uplo, trans, diag, n, nrhs, ap, b, lda, x, lda, rwork, &rwork[(nrhs + 1) - 1], work, &rwork[(2 * nrhs + 1) - 1], info);
                        //
                        //                 Check error code from Ctprfs.
                        //
                        if (info != 0) {
                            Alaerh(path, "Ctprfs", info, 0, uplo + trans + diag, n, n, -1, -1, nrhs, imat, nfail, nerrs, nout);
                        }
                        //
                        Cget04(n, nrhs, x, lda, xact, lda, rcondc, result[4 - 1]);
                        Ctpt05(uplo, trans, diag, n, nrhs, ap, b, lda, x, lda, xact, lda, rwork, &rwork[(nrhs + 1) - 1], result[5 - 1]);
                        //
                        //                    Print information about the tests that did not pass
                        //                    the threshold.
                        //
                        for (k = 2; k <= 6; k = k + 1) {
                            if (result[k - 1] >= thresh) {
                                if (nfail == 0 && nerrs == 0) {
                                    Alahd(nout, path);
                                }
                                write(nout, "(' UPLO=''',a1,''', TRANS=''',a1,''', DIAG=''',a1,''', N=',"
                                            "i5,''', NRHS=',i5,', type ',i2,', test(',i2,')= ',g12.5)"),
                                    uplo, trans, diag, n, nrhs, imat, k, result(k);
                                nfail++;
                            }
                        }
                        nrun += 5;
                    }
                }
                //
                //+    TEST 7
                //                 Get an estimate of RCOND = 1/CNDNUM.
                //
                for (itran = 1; itran <= 2; itran = itran + 1) {
                    if (itran == 1) {
                        norm = "O";
                        rcondc = rcondo;
                    } else {
                        norm = 'I';
                        rcondc = rcondi;
                    }
                    Ctpcon(norm, uplo, diag, n, ap, rcond, work, rwork, info);
                    //
                    //                 Check error code from Ctpcon.
                    //
                    if (info != 0) {
                        Alaerh(path, "Ctpcon", info, 0, norm + uplo + diag, n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                    }
                    //
                    Ctpt06(rcond, rcondc, uplo, diag, n, ap, rwork, result[7 - 1]);
                    //
                    //                 Print the test ratio if it is .GE. THRESH.
                    //
                    if (result[7 - 1] >= thresh) {
                        if (nfail == 0 && nerrs == 0) {
                            Alahd(nout, path);
                        }
                        write(nout, "(1x,a,'( ''',a1,''', ''',a1,''', ''',a1,''',',i5,"
                                    "', ... ), type ',i2,', test(',i2,')=',g12.5)"),
                            "Ctpcon", norm, uplo, diag, n, imat, 7, result(7);
                        nfail++;
                    }
                    nrun++;
                }
            }
        statement_70:;
        }
        //
        //        Use pathological test matrices to test Clatps.
        //
        for (imat = ntype1 + 1; imat <= ntypes; imat = imat + 1) {
            //
            //           Do the tests only if DOTYPE( IMAT ) is true.
            //
            if (!dotype[imat - 1]) {
                goto statement_100;
            }
            //
            for (iuplo = 1; iuplo <= 2; iuplo = iuplo + 1) {
                //
                //              Do first for UPLO = 'U', then for UPLO = 'L'
                //
                uplo = uplos[iuplo - 1];
                for (itran = 1; itran <= ntran; itran = itran + 1) {
                    //
                    //                 Do for op(A) = A, A**T, or A**H.
                    //
                    trans = transs[itran - 1];
                    //
                    //                 Call Clattp to generate a triangular test matrix.
                    //
                    Clattp(imat, uplo, trans, diag, iseed, n, ap, x, work, rwork, info);
                    //
                    //+    TEST 8
                    //                 Solve the system op(A)*x = b.
                    //
                    Ccopy(n, x, 1, b, 1);
                    Clatps(uplo, trans, diag, "N", n, ap, b, scale, rwork, info);
                    //
                    //                 Check error code from Clatps.
                    //
                    if (info != 0) {
                        Alaerh(path, "Clatps", info, 0, uplo + trans + diag + const char *("N"), n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                    }
                    //
                    Ctpt03(uplo, trans, diag, n, 1, ap, scale, rwork, one, b, lda, x, lda, work, result[8 - 1]);
                    //
                    //+    TEST 9
                    //                 Solve op(A)*x = b again with NORMIN = 'Y'.
                    //
                    Ccopy(n, x, 1, &b[(n + 1) - 1], 1);
                    Clatps(uplo, trans, diag, "Y", n, ap, &b[(n + 1) - 1], scale, rwork, info);
                    //
                    //                 Check error code from Clatps.
                    //
                    if (info != 0) {
                        Alaerh(path, "Clatps", info, 0, uplo + trans + diag + const char *("Y"), n, n, -1, -1, -1, imat, nfail, nerrs, nout);
                    }
                    //
                    Ctpt03(uplo, trans, diag, n, 1, ap, scale, rwork, one, &b[(n + 1) - 1], lda, x, lda, work, result[9 - 1]);
                    //
                    //                 Print information about the tests that did not pass
                    //                 the threshold.
                    //
                    if (result[8 - 1] >= thresh) {
                        if (nfail == 0 && nerrs == 0) {
                            Alahd(nout, path);
                        }
                        write(nout, format_9996), "Clatps", uplo, trans, diag, "N", n, imat, 8, result(8);
                        nfail++;
                    }
                    if (result[9 - 1] >= thresh) {
                        if (nfail == 0 && nerrs == 0) {
                            Alahd(nout, path);
                        }
                        write(nout, format_9996), "Clatps", uplo, trans, diag, "Y", n, imat, 9, result(9);
                        nfail++;
                    }
                    nrun += 2;
                }
            }
        statement_100:;
        }
    }
    //
    //     Print a summary of the results.
    //
    Alasum(path, nout, nfail, nrun, nerrs);
    //
    //     End of Cchktp
    //
}
