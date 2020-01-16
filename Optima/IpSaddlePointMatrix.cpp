// Optima is a C++ library for solving linear and non-linear constrained optimization problems
//
// Copyright (C) 2014-2018 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "IpSaddlePointMatrix.hpp"

// Optima includes
#include <Optima/Utils.hpp>

namespace Optima {

IpSaddlePointMatrix::IpSaddlePointMatrix(
    MatrixConstRef H,
    MatrixConstRef Au,
    MatrixConstRef Al,
    VectorConstRef Z,
    VectorConstRef W,
    VectorConstRef L,
    VectorConstRef U,
    IndicesConstRef jf)
: H(H), Au(Au), Al(Al), Z(Z), W(W), L(L), U(U), jf(jf)
{}

IpSaddlePointMatrix::operator Matrix() const
{
    const auto mu = Au.rows();
    const auto ml = Al.rows();
    const auto m = mu + ml;
    const auto n = Au.cols();
    const auto t = 3*n + m;

    Matrix res = zeros(t, t);

    // Block: H
    switch(matrixStructure(H))
    {
    case MatrixStructure::Dense:
        res.topLeftCorner(n, n) = H;
        break;
    case MatrixStructure::Diagonal:
        res.topLeftCorner(n, n) = diag(H.col(0));
        break;
    case MatrixStructure::Zero:
        break;
    }
    // res.topLeftCorner(n, n) <<= H;
    res.topLeftCorner(n, n)(all, jf).fill(0.0);
    res.topLeftCorner(n, n)(jf, all).fill(0.0);
    res.topLeftCorner(n, n).diagonal()(jf).fill(1.0);

    if(mu > 0)
    {
        // Block: tr(Au)
        res.middleCols(n, m).topLeftCorner(n, mu) = tr(Au);
        res.middleCols(n, m).topLeftCorner(n, mu)(jf, all).fill(0.0);

        // Block: Au
        res.middleRows(n, m).topLeftCorner(mu, n) = Au;
    }

    if(ml > 0)
    {
        // Block: tr(Al)
        res.middleCols(n, m).topRightCorner(n, ml) = tr(Al);
        res.middleCols(n, m).topRightCorner(n, ml)(jf, all).fill(0.0);

        // Block: Al
        res.middleRows(n, m).bottomLeftCorner(ml, n) = Al;
    }

    // Block: -I
    res.middleCols(n + m, n).diagonal().fill(-1.0);
    res.middleCols(n + m, n).diagonal()(jf).fill(0.0);

    // Block: -I
    res.rightCols(n).diagonal().fill(-1.0);
    res.rightCols(n).diagonal()(jf).fill(0.0);

    // Block: Z
    res.middleRows(n + m, n).leftCols(n).diagonal() = Z;
    res.middleRows(n + m, n).leftCols(n).diagonal()(jf).fill(0.0);

    // Block: L
    res.middleRows(n + m, n).middleCols(n + m, n).diagonal() = L;
    res.middleRows(n + m, n).middleCols(n + m, n).diagonal()(jf).fill(1.0);

    // Block: W
    res.bottomRows(n).leftCols(n).diagonal() = W;
    res.bottomRows(n).leftCols(n).diagonal()(jf).fill(0.0);

    // Block: U
    res.bottomRows(n).rightCols(n).diagonal() = U;
    res.bottomRows(n).rightCols(n).diagonal()(jf).fill(1.0);

    return res;
}

IpSaddlePointVector::IpSaddlePointVector(
    VectorConstRef a,
    VectorConstRef b,
    VectorConstRef c,
    VectorConstRef d)
: a(a), b(b), c(c), d(d)
{}

IpSaddlePointVector::IpSaddlePointVector(VectorConstRef r, Index n, Index m)
: a(r.head(n)),
  b(r.segment(n, m)),
  c(r.segment(n + m, n)),
  d(r.tail(n))
{}

IpSaddlePointVector::operator Vector() const
{
    const auto n = a.size();
    const auto m = b.size();
    const auto t = 3*n + m;
    Vector res(t);
    res << a, b, c, d;
    return res;
}

IpSaddlePointSolution::IpSaddlePointSolution(
    VectorRef x,
    VectorRef y,
    VectorRef z,
    VectorRef w)
: x(x), y(y), z(z), w(w)
{}

IpSaddlePointSolution::IpSaddlePointSolution(VectorRef s, Index n, Index m)
: x(s.head(n)),
  y(s.segment(n, m)),
  z(s.segment(n + m, n)),
  w(s.tail(n))
{}

auto IpSaddlePointSolution::operator=(VectorConstRef vec) -> IpSaddlePointSolution&
{
    const auto n = x.size();
    const auto m = y.size();
    x.noalias() = vec.head(n);
    y.noalias() = vec.segment(n, m);
    z.noalias() = vec.segment(n + m, n);
    w.noalias() = vec.tail(n);
    return *this;
}

IpSaddlePointSolution::operator Vector() const
{
    const auto n = x.size();
    const auto m = y.size();
    const auto t = 3*n + m;
    Vector res(t);
    res << x, y, z, w;
    return res;
}

auto operator*(IpSaddlePointMatrix lhs, VectorConstRef rhs) -> Vector
{
    Matrix M(lhs);
    Vector res = M * rhs;
    return res;
}

} // namespace Optima
