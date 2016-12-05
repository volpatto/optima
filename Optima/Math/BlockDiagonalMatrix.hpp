// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2017 Allan Leal
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
// on with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// Optima includes
#include <Optima/Math/Matrix.hpp>

namespace Optima {

// Forward declarations
class BlockDiagonalMatrix;

} // namespace Optima

namespace Eigen {
namespace internal {

template<>
struct traits<Optima::BlockDiagonalMatrix>
{
    typedef Eigen::Dense StorageKind;
    typedef Eigen::MatrixXpr XprKind;
    typedef Optima::Matrix::Scalar Scalar;
    typedef Optima::Matrix::Index Index;
    typedef Optima::Matrix::PlainObject PlainObject;
    enum {
        Flags = Eigen::ColMajor,
        RowsAtCompileTime = Optima::Matrix::RowsAtCompileTime,
        ColsAtCompileTime = Optima::Matrix::ColsAtCompileTime,
        MaxRowsAtCompileTime = Optima::Matrix::MaxRowsAtCompileTime,
        MaxColsAtCompileTime = Optima::Matrix::MaxColsAtCompileTime,
        CoeffReadCost = Optima::Matrix::CoeffReadCost
    };
};

} // namespace internal
} // namespace Eigen

namespace Optima {

/// Used to represent a block diagonal matrix.
class BlockDiagonalMatrix : public Eigen::MatrixBase<BlockDiagonalMatrix>
{
public:
    EIGEN_DENSE_PUBLIC_INTERFACE(BlockDiagonalMatrix)

    /// Construct a default BlockDiagonalMatrix instance.
    BlockDiagonalMatrix() {}

    /// Construct a default BlockDiagonalMatrix instance.
    /// @param m The number of rows in the block diagonal matrix.
    /// @param n The number of columns in the block diagonal matrix.
    BlockDiagonalMatrix(Index numblocks)
    : m_blocks(numblocks) {}

    /// Destroy this BlockDiagonalMatrix instance.
    virtual ~BlockDiagonalMatrix() {}

    /// Return a reference to a block matrix on the diagonal.
    /// @param i The index of the block matrix.
    auto block(Index i) -> Matrix& { return m_blocks[i]; }

    /// Return a const reference to a block matrix on the diagonal.
    /// @param i The index of the block matrix.
    auto block(Index i) const -> const Matrix& { return m_blocks[i]; }

    /// Return a reference to the block matrices on the diagonal.
    auto blocks() -> std::vector<Matrix>& { return m_blocks; }

    /// Return a const reference to the block matrices on the diagonal.
    auto blocks() const -> const std::vector<Matrix>& { return m_blocks; }

    /// Return the number of rows of the block diagonal matrix.
    auto rows() const -> Index
    {
        Index sum = 0;
        for(const Matrix& block : blocks())
            sum += block.rows();
        return sum;
    }

    /// Return the number of columns of the block diagonal matrix.
    auto cols() const -> Index
    {
        Index sum = 0;
        for(const Matrix& block : blocks())
            sum += block.cols();
        return sum;
    }

    // Delete this resize overload method.
    auto resize(Index rows, Index cols) -> void = delete;

    /// Resize the block diagonal matrix.
    /// @param numblocks The number of blocks on the block diagonal matrix.
    auto resize(Index numblocks) -> void
    {
        m_blocks.resize(numblocks);
    }

    /// Return an entry of the block diagonal matrix.
    auto coeff(Index i, Index j) const -> Scalar
    {
        eigen_assert(i < rows() && j < cols());
        const Index nblocks = blocks().size();
        Index irow = 0, icol = 0;
        for(Index iblock = 0; iblock < nblocks; ++iblock)
        {
            const Index nrows = block(iblock).rows();
            const Index ncols = block(iblock).cols();
            if(i < irow + nrows)
            {
                if(icol <= j && j < icol + ncols)
                    return block(iblock)(i - irow, j - icol);
                else return 0.0;
            }
            irow += nrows;
            icol += ncols;
        }
        return 0.0;
    }

    /// Return an entry of the block diagonal matrix.
    auto operator()(Index i, Index j) const -> Scalar { return coeff(i, j); }

    /// Convert this BlockDiagonalMatrix instance to a Matrix instance.
    operator PlainObject() const
    {
        const Index m = rows();
        const Index n = cols();
        const Index nblocks = blocks().size();
        PlainObject res = zeros(m, n);
        Index irow = 0, icol = 0;
        for(Index i = 0; i < nblocks; ++i)
        {
            const Index nrows = block(i).rows();
            const Index ncols = block(i).cols();
            res.block(irow, icol, nrows, ncols) = block(i);
            irow += nrows;
            icol += ncols;
        }
        return res;
    }

private:
    /// The block matrices on the block diagonal matrix.
    std::vector<Matrix> m_blocks;
};

} // namespace Optima
