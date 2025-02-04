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

#include <time.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

void Cchkaa(void) {
    common cmn;
    common_write write(cmn);
    const INTEGER nmax = 132;
    const INTEGER maxin = 12;
    const INTEGER matmax = 30;
    const INTEGER maxrhs = 16;
    const INTEGER nin = 5;
    const INTEGER nout = 6;
    const INTEGER kdmax = nmax + (nmax + 1) / 4;

    char c1[2];
    char c2[3];
    char path[4];
    char aline[72];
    char buf[1024];

    REAL eps = 0.0;
    time_t s1;
    time_t s2;
    REAL threq = 2.0;
    REAL thresh = 0.0;

    INTEGER i = 0;
    INTEGER ic = 0;
    INTEGER j = 0;
    INTEGER k = 0;
    INTEGER la = 0;
    INTEGER lafac = 0;
    INTEGER lda;
    INTEGER ldaorg = (kdmax + 1) * nmax;
    INTEGER nb = 0;
    INTEGER nm = 0;
    INTEGER nmats = 0;
    INTEGER nn = 0;
    INTEGER nnb = 0;
    INTEGER nnb2 = 0;
    INTEGER nns = 0;
    INTEGER nrhs = 0;
    INTEGER ntypes = 0;
    INTEGER nrank = 0;
    INTEGER mplapack_vers_major = 0;
    INTEGER mplapack_vers_minor = 0;
    INTEGER mplapack_vers_patch = 0;
    INTEGER lapack_vers_major = 0;
    INTEGER lapack_vers_minor = 0;
    INTEGER lapack_vers_patch = 0;

    bool dotype[matmax];
    INTEGER mval[maxin];
    INTEGER nbval[maxin];
    INTEGER nval[maxin];
    INTEGER nbval2[maxin];
    INTEGER nsval[maxin];
    INTEGER nxval[maxin];
    INTEGER rankval[maxin];
    INTEGER piv[nmax];
    REAL s[2 * nmax];
    COMPLEX e[nmax];

    INTEGER iwork[25 * nmax];
    INTEGER ldb = (nmax * maxrhs);

    REAL *rwork = new REAL[150 * nmax + 2 * maxrhs];
    COMPLEX *a = new COMPLEX[((kdmax + 1) * nmax) * 7];
    COMPLEX *b = new COMPLEX[(nmax * maxrhs) * 4];
    COMPLEX *work = new COMPLEX[nmax * (nmax + maxrhs + 10)];

    char tstchk_str[1];
    char tstdrv_str[1];
    char tsterr_str[1];
    bool fatal = false;
    bool tstchk = false;
    bool tstdrv = false;
    bool tsterr = false;

    static const char *format_9988 = "(/,1x,a3,' driver routines were not tested')";
    static const char *format_9989 = "(/,1x,a3,' routines were not tested')";
    static const char *format_9990 = "(/,1x,a3,':  Unrecognized path name')";
    static const char *format_9991 = "(' Relative machine ',a,' is taken to be',d16.6)";
    static const char *format_9993 = "(4x,a4,':  ',10i6,/,11x,10i6)";
    static const char *format_9995 = "(' Invalid input value: ',a4,'=',i6,'; must be <=',i6)";
    static const char *format_9996 = "(' Invalid input value: ',a4,'=',i6,'; must be >=',i6)";
    //
    std::string str;
    s1 = time(NULL);
    lda = nmax;
    fatal = false;
    //
    //     Read a dummy line.
    //
    getline(cin, str);
    //
    //     Report values of parameters.
    //
    iMlaver(mplapack_vers_major, mplapack_vers_minor, mplapack_vers_patch, lapack_vers_major, lapack_vers_minor, lapack_vers_patch);
    write(nout, "(' Tests of the Multiple precision version of LAPACK MPLAPACK VERSION ',i1,'.',i1,'.',i1,/, "
                "' Based on the original LAPACK VERSION ',i1,'.',i1,'.',i1,/,/, 'The following parameter values will be used:')"),
        mplapack_vers_major, mplapack_vers_minor, mplapack_vers_patch, lapack_vers_major, lapack_vers_minor, lapack_vers_patch;
    //
    //     Read the values of M
    //
    getline(cin, str);
    stringstream ss(str);
    ss >> nm;
    if (nm < 1) {
        write(nout, format_9996), " NM ", nm, 1;
        nm = 0;
        fatal = true;
    } else if (nm > maxin) {
        write(nout, format_9995), " NM ", nm, maxin;
        nm = 0;
        fatal = true;
    }
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nm; i = i + 1) {
        ss >> mval[i - 1];
    }
    for (i = 1; i <= nm; i = i + 1) {
        if (mval[i - 1] < 0) {
            write(nout, format_9996), " M  ", mval[i - 1], 0;
            fatal = true;
        } else if (mval[i - 1] > nmax) {
            write(nout, format_9995), " M  ", mval[i - 1], nmax;
            fatal = true;
        }
    }
    if (nm > 0) {
        printf("     %4s  :", "M");
        for (i = 1; i <= nm; i = i + 1) {
            printf("%6ld", mval[i - 1]);
        }
        printf("\n");
    }
    //
    //     Read the values of N
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> nn;
    if (nn < 1) {
        write(nout, format_9996), " NN ", nn, 1;
        nn = 0;
        fatal = true;
    } else if (nn > maxin) {
        write(nout, format_9995), " NN ", nn, maxin;
        nn = 0;
        fatal = true;
    }
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nn; i = i + 1) {
        ss >> nval[i - 1];
    }
    for (i = 1; i <= nn; i = i + 1) {
        if (nval[i - 1] < 0) {
            write(nout, format_9996), " N  ", nval[i - 1], 0;
            fatal = true;
        } else if (nval[i - 1] > nmax) {
            write(nout, format_9995), " N  ", nval[i - 1], nmax;
            fatal = true;
        }
    }
    if (nn > 0) {
        printf("     %4s  :", "N");
        for (i = 1; i <= nn; i = i + 1) {
            printf("%6ld", nval[i - 1]);
        }
        printf("\n");
    }
    //
    //     Read the values of NRHS
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> nns;
    if (nns < 1) {
        write(nout, format_9996), " NNS", nns, 1;
        nns = 0;
        fatal = true;
    } else if (nns > maxin) {
        write(nout, format_9995), " NNS", nns, maxin;
        nns = 0;
        fatal = true;
    }
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nns; i = i + 1)
        ss >> nsval[i - 1];
    for (i = 1; i <= nns; i = i + 1) {
        if (nsval[i - 1] < 0) {
            write(nout, format_9996), "NRHS", nsval[i - 1], 0;
            fatal = true;
        } else if (nsval[i - 1] > maxrhs) {
            write(nout, format_9995), "NRHS", nsval[i - 1], maxrhs;
            fatal = true;
        }
    }
    if (nns > 0) {
        printf("     %4s  :", "NRHS");
        for (i = 1; i <= nns; i = i + 1) {
            printf("%6ld", nsval[i - 1]);
        }
    }
    printf("\n");
    //
    //     Read the values of NB
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> nnb;
    if (nnb < 1) {
        write(nout, format_9996), "NNB ", nnb, 1;
        nnb = 0;
        fatal = true;
    } else if (nnb > maxin) {
        write(nout, format_9995), "NNB ", nnb, maxin;
        nnb = 0;
        fatal = true;
    }
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nnb; i = i + 1) {
        ss >> nbval[i - 1];
    }
    for (i = 1; i <= nnb; i = i + 1) {
        if (nbval[i - 1] < 0) {
            write(nout, format_9996), " NB ", nbval[i - 1], 0;
            fatal = true;
        }
    }
    if (nnb > 0) {
        printf("     %4s  :", "NB");
        for (i = 1; i <= nnb; i = i + 1) {
            printf("%6ld", nbval[i - 1]);
        }
    }
    printf("\n");
    //
    //     Set NBVAL2 to be the set of unique values of NB
    //
    nnb2 = 0;
    for (i = 1; i <= nnb; i = i + 1) {
        nb = nbval[i - 1];
        for (j = 1; j <= nnb2; j = j + 1) {
            if (nb == nbval2[j - 1]) {
                goto statement_60;
            }
        }
        nnb2++;
        nbval2[nnb2 - 1] = nb;
    statement_60:;
    }
    //
    //     Read the values of NX
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nnb; i = i + 1) {
        ss >> nxval[i - 1];
    }
    for (i = 1; i <= nnb; i = i + 1) {
        if (nxval[i - 1] < 0) {
            write(nout, format_9996), " NX ", nxval[i - 1], 0;
            fatal = true;
        }
    }
    if (nnb > 0) {
        printf("     %4s  :", "NX");
        for (i = 1; i <= nnb; i = i + 1)
            printf("%6ld", nxval[i - 1]);
    }
    printf("\n");
    //
    //     Read the values of RANKVAL
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> nrank;
    if (nn < 1) {
        write(nout, format_9996), " NRANK ", nrank, 1;
        nrank = 0;
        fatal = true;
    } else if (nn > maxin) {
        write(nout, format_9995), " NRANK ", nrank, maxin;
        nrank = 0;
        fatal = true;
    }
    ss.str("");
    getline(cin, str);
    ss.str(str);
    for (i = 1; i <= nrank; i = i + 1) {
        ss >> rankval[i - 1];
    }
    for (i = 1; i <= nrank; i = i + 1) {
        if (rankval[i - 1] < 0) {
            write(nout, format_9996), " RANK  ", rankval[i - 1], 0;
            fatal = true;
        } else if (rankval[i - 1] > 100) {
            write(nout, format_9995), " RANK  ", rankval[i - 1], 100;
            fatal = true;
        }
    }
    if (nrank > 0) {
        printf("     %4s  :", "RANK");
        for (i = 1; i <= nrank; i = i + 1)
            printf("%6ld", rankval[i - 1]);
    }
    printf("\n");
    //
    //     Read the threshold value for the test ratios.
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    double dtmp;
    ss >> dtmp;
    thresh = dtmp;
    write(nout, "(/,' Routines pass computational tests if test ratio is ','less than',"
                "f8.2,/)"),
        cast2double(thresh);
    //
    //     Read the flag that indicates whether to test the LAPACK routines.
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> tstchk_str;
    if (Mlsame(tstchk_str, "T"))
        tstchk = true;
    else
        tstchk = false;
    //
    //     Read the flag that indicates whether to test the driver routines.
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> tstdrv_str;
    if (Mlsame(tstdrv_str, "T"))
        tstdrv = true;
    else
        tstdrv = false;
    //
    //     Read the flag that indicates whether to test the error exits.
    //
    ss.str("");
    getline(cin, str);
    ss.str(str);
    ss >> tsterr_str;
    if (Mlsame(tsterr_str, "T"))
        tsterr = true;
    else
        tsterr = false;
    //
    if (fatal) {
        write(nout, "(/,' Execution not attempted due to input errors')");
        exit(0);
    }
    //
    //     Calculate and print the machine dependent constants.
    //
    eps = Rlamch("Underflow threshold");
    sprintnum_short(buf, eps);
    cout << " Relative machine underflow is taken to be : " << buf << endl;
    eps = Rlamch("Overflow threshold");
    sprintnum_short(buf, eps);
    cout << " Relative machine overflow  is taken to be : " << buf << endl;
    eps = Rlamch("Epsilon");
    sprintnum_short(buf, eps);
    cout << " Relative machine precision is taken to be : " << buf << endl;
    //
    //     Read a test path and the number of matrix types to use.
    //
    for (int i = 0; i < 100; i++) {
        iparms[i] = (INTEGER)0;
    }
    nmats = matmax;
    while (getline(cin, str)) {
        istringstream iss(str);
        vector<string> result;
        for (string s; iss >> s;)
            result.push_back(s);
        INTEGER n = result.size();
        if (n >= 1) {
            if (result[0].length() == 3) {
                path[0] = result[0][0];
                path[1] = result[0][1];
                path[2] = result[0][2];
                path[3] = '\0';
                c1[0] = result[0][0];
                c2[0] = result[0][1];
                c2[1] = result[0][2];
            } else {
                printf("wrong three letters\n");
                exit(0);
            }
            if (n >= 2) {
                nmats = stoi(result[1]);
            }
        }
        nrhs = nsval[1 - 1];
        //
        //     Check first character for correct precision.
        //
        if (!Mlsame(c1, "C") && !Mlsame(c1, "Z")) {
            write(nout, format_9990), path;
            //
        } else if (nmats <= 0) {
            //
            //        Check for a positive number of tests requested.
            //
            write(nout, format_9989), path;
            //
        } else if (Mlsamen(2, c2, "GE")) {
            //
            //        GE:  general matrices
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkge(dotype, nm, mval, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1)], &a[(2 - 1) * ldaorg], &a[(3 - 1) * ldaorg], &b[(1 - 1) * ldb], &b[(2 - 1) * ldb], &b[(3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvge(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], s, work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "GB")) {
            //
            //        GB:  general banded matrices
            //
            la = (2 * kdmax + 1) * nmax;
            lafac = (3 * kdmax + 1) * nmax;
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkgb(dotype, nm, mval, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], la, &a[(1 - 1) + (3 - 1) * ldaorg], lafac, &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvgb(dotype, nn, nval, nrhs, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], la, &a[(1 - 1) + (3 - 1) * ldaorg], lafac, &a[(1 - 1) + (6 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], s, work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "GT")) {
            //
            //        GT:  general tridiagonal matrices
            //
            ntypes = 12;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkgt(dotype, nn, nval, nns, nsval, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvgt(dotype, nn, nval, nrhs, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "PO")) {
            //
            //        PO:  positive definite matrices
            //
            ntypes = 9;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkpo(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvpo(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], s, work, rwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "PS")) {
            //
            //        PS:  positive semi-definite matrices
            //
            ntypes = 9;
            //
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkps(dotype, nn, nval, nnb2, nbval2, nrank, rankval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], piv, work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "PP")) {
            //
            //        PP:  positive definite packed matrices
            //
            ntypes = 9;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkpp(dotype, nn, nval, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvpp(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], s, work, rwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "PB")) {
            //
            //        PB:  positive definite banded matrices
            //
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkpb(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvpb(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], s, work, rwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "PT")) {
            //
            //        PT:  positive definite tridiagonal matrices
            //
            ntypes = 12;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkpt(dotype, nn, nval, nns, nsval, thresh, tsterr, &a[(1 - 1) + (1 - 1) * lda], s, &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvpt(dotype, nn, nval, nrhs, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], s, &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "HE")) {
            //
            //        HE:  Hermitian indefinite matrices
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhe(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhe(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "HR")) {
            //
            //        HR:  Hermitian indefinite matrices,
            //             with bounded Bunch-Kaufman (rook) pivoting algorithm,
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhe_rook(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhe_rook(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "HK")) {
            //
            //        HK:  Hermitian indefinite matrices,
            //             with bounded Bunch-Kaufman (rook) pivoting algorithm,
            //             different matrix storage format than HR path version.
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhe_rk(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], e, &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhe_rk(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], e, &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "HA")) {
            //
            //        HA:  Hermitian matrices,
            //             Aasen Algorithm
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhe_aa(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhe_aa(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "H2")) {
            //
            //        H2:  Hermitian matrices,
            //             with partial (Aasen's) pivoting algorithm
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhe_aa_2stage(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhe_aa_2stage(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "HP")) {
            //
            //        HP:  Hermitian indefinite packed matrices
            //
            ntypes = 10;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkhp(dotype, nn, nval, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvhp(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "SY")) {
            //
            //        SY:  symmetric indefinite matrices,
            //             with partial (Bunch-Kaufman) pivoting algorithm
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksy(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsy(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "SR")) {
            //
            //        SR:  symmetric indefinite matrices,
            //             with bounded Bunch-Kaufman (rook) pivoting algorithm
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksy_rook(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsy_rook(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "SK")) {
            //
            //        SK:  symmetric indefinite matrices,
            //             with bounded Bunch-Kaufman (rook) pivoting algorithm,
            //             different matrix storage format than SR path version.
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksy_rk(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], e, &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsy_rk(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], e, &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "SA")) {
            //
            //        SA:  symmetric indefinite matrices with Aasen's algorithm,
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksy_aa(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsy_aa(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "S2")) {
            //
            //        S2:  symmetric indefinite matrices with Aasen's algorithm
            //             2 stage
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksy_aa_2stage(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsy_aa_2stage(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "SP")) {
            //
            //        SP:  symmetric indefinite packed matrices,
            //             with partial (Bunch-Kaufman) pivoting algorithm
            //
            ntypes = 11;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchksp(dotype, nn, nval, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
            if (tstdrv) {
                Cdrvsp(dotype, nn, nval, nrhs, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9988), path;
            }
            //
        } else if (Mlsamen(2, c2, "TR")) {
            //
            //        TR:  triangular matrices
            //
            ntypes = 18;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchktr(dotype, nn, nval, nnb2, nbval2, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TP")) {
            //
            //        TP:  triangular packed matrices
            //
            ntypes = 18;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchktp(dotype, nn, nval, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TB")) {
            //
            //        TB:  triangular banded matrices
            //
            ntypes = 17;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchktb(dotype, nn, nval, nns, nsval, thresh, tsterr, lda, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "QR")) {
            //
            //        QR:  QR factorization
            //
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkqr(dotype, nm, mval, nn, nval, nnb, nbval, nxval, nrhs, thresh, tsterr, nmax, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &a[(1 - 1) + (4 - 1) * ldaorg], &a[(1 - 1) + (5 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "LQ")) {
            //
            //        LQ:  LQ factorization
            //
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchklq(dotype, nm, mval, nn, nval, nnb, nbval, nxval, nrhs, thresh, tsterr, nmax, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &a[(1 - 1) + (4 - 1) * ldaorg], &a[(1 - 1) + (5 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "QL")) {
            //
            //        QL:  QL factorization
            //
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkql(dotype, nm, mval, nn, nval, nnb, nbval, nxval, nrhs, thresh, tsterr, nmax, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &a[(1 - 1) + (4 - 1) * ldaorg], &a[(1 - 1) + (5 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "RQ")) {
            //
            //        RQ:  RQ factorization
            //
            ntypes = 8;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkrq(dotype, nm, mval, nn, nval, nnb, nbval, nxval, nrhs, thresh, tsterr, nmax, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &a[(1 - 1) + (4 - 1) * ldaorg], &a[(1 - 1) + (5 - 1) * ldaorg], &b[(1 - 1) + (1 - 1) * ldb], &b[(1 - 1) + (2 - 1) * ldb], &b[(1 - 1) + (3 - 1) * ldb], &b[(1 - 1) + (4 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "EQ")) {
            //
            //        EQ:  Equilibration routines for general and positive definite
            //             matrices (THREQ should be between 2 and 10)
            //
            if (tstchk) {
                Cchkeq(threq, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TZ")) {
            //
            //        TZ:  Trapezoidal matrix
            //
            ntypes = 3;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchktz(dotype, nm, mval, nn, nval, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &s[1 - 1], &b[(1 - 1) + (1 - 1) * ldb], work, rwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "QP")) {
            //
            //        QP:  QR factorization with pivoting
            //
            ntypes = 6;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstchk) {
                Cchkq3(dotype, nm, mval, nn, nval, nnb, nbval, nxval, thresh, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &s[1 - 1], &b[(1 - 1) + (1 - 1) * ldb], work, rwork, iwork, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "LS")) {
            //
            //        LS:  Least squares drivers
            //
            ntypes = 6;
            Alareq(path, nmats, dotype, ntypes, nin, nout);
            //
            if (tstdrv) {
                Cdrvls(dotype, nm, mval, nn, nval, nns, nsval, nnb, nbval, nxval, thresh, tsterr, &a[(1 - 1) + (1 - 1) * ldaorg], &a[(1 - 1) + (2 - 1) * ldaorg], &a[(1 - 1) + (3 - 1) * ldaorg], &a[(1 - 1) + (4 - 1) * ldaorg], &a[(1 - 1) + (5 - 1) * ldaorg], &s[1 - 1], &s[(nmax + 1) - 1], nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "QT")) {
            //
            //        QT:  QRT routines for general matrices
            //
            if (tstchk) {
                Cchkqrt(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "QX")) {
            //
            //        QX:  QRT routines for triangular-pentagonal matrices
            //
            if (tstchk) {
                Cchkqrtp(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TQ")) {
            //
            //        TQ:  LQT routines for general matrices
            //
            if (tstchk) {
                Cchklqt(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "XQ")) {
            //
            //        XQ:  LQT routines for triangular-pentagonal matrices
            //
            if (tstchk) {
                Cchklqtp(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TS")) {
            //
            //        TS:  QR routines for tall-skinny matrices
            //
            if (tstchk) {
                Cchktsqr(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TQ")) {
            //
            //        TQ:  LQT routines for general matrices
            //
            if (tstchk) {
                Cchklqt(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "XQ")) {
            //
            //        XQ:  LQT routines for triangular-pentagonal matrices
            //
            if (tstchk) {
                Cchklqtp(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "TS")) {
            //
            //        TS:  QR routines for tall-skinny matrices
            //
            if (tstchk) {
                Cchktsqr(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else if (Mlsamen(2, c2, "HH")) {
            //
            //        HH:  Householder reconstruction for tall-skinny matrices
            //
            if (tstchk) {
                Cchkunhr_col(thresh, tsterr, nm, mval, nn, nval, nnb, nbval, nout);
            } else {
                write(nout, format_9989), path;
            }
            //
        } else {
            //
            write(nout, format_9990), path;
        }
        //
        //     Go back to get another input line.
        //
    }
    delete[] b;
    delete[] a;
    delete[] rwork;
    delete[] work;
    s2 = time(NULL);
    write(nout, "(/,' End of tests')");
    write(nout, "(' Total time used = ',f12.2,' seconds',/)"), int(s2 - s1);
    //
    //     End of Cchkaa
    //
}
int main(int argc, char const *argv[]) { Cchkaa(); }
