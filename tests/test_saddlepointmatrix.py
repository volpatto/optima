# Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
#
# Copyright (C) 2014-2017 Allan Leal
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.


from optima import *
from numpy import *
from pytest import approx


def headRows(n):
    return slice(n), slice(None)


def middleRows(start, n):
    return slice(start, start + n), slice(None)


def tailRows(n):
    return slice(-n), slice(None)


def headCols(n):
    return slice(None), slice(n)


def middleCols(start, n):
    return slice(None), slice(start, start + n)


def tailCols(n):
    return slice(None), slice(-n)


def block(startrow, startcol, nrows, ncols):
    return slice(startrow, startrow + nrows), slice(startcol, startcol + ncols)


def test_saddle_point_matrix():
    H = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    A = [[1, 2, 3], [3, 4, 5]]
    G = [[1, 2], [3, 4]]
    n = 3
    nx = 3
    nf = 0

    mat1 = SaddlePointMatrix(H, A, G, nx, nf)

    M = [[1, 2, 3, 1, 3],
         [4, 5, 6, 2, 4],
         [7, 8, 9, 3, 5],
         [1, 2, 3, 1, 2],
         [3, 4, 5, 3, 4]]

    # Check conversion to a Matrix instance
    assert mat1.matrix() == approx(M)

    # Testing conversion when some variables are fixed
    nx = 2
    nf = 1

    mat2 = SaddlePointMatrix(H, A, G, nx, nf)

    M[middleRows(nx, nf)] = 0.0
    M[middleCols(nx, nf)][topRows(n)] = 0.0
    M[block(nx, nx, nf, nf)] = eye(nf)

    # Check conversion to a Matrix instance
    assert mat2.matrix() == approx(M)


def test_saddle_point_vector():
    n = 5
    m = 3
    t = n + m

    r = arange(1, t + 1)

    vec = SaddlePointVector(r, n, m)

    assert vec.vector() == approx(r)
