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

#pragma once

// C++ includes
#include <memory>

// Optima includes
#include <Optima/Index.hpp>
#include <Optima/LinearSolverOptions.hpp>
#include <Optima/Matrix.hpp>

namespace Optima {

// Forward declarations
class CanonicalVector;
class CanonicalVectorRef;
class MasterMatrix;
class MasterVector;

/// Used to solve the system of linear equations involving master matrices and master vectors.
class LinearSolver
{
public:
    /// Construct a LinearSolver instance.
    LinearSolver(Index nx, Index np, Index ny, Index nz);

    /// Construct a copy of a LinearSolver instance.
    LinearSolver(const LinearSolver& other);

    /// Destroy this LinearSolver instance.
    virtual ~LinearSolver();

    /// Assign a LinearSolver instance to this.
    auto operator=(LinearSolver other) -> LinearSolver&;

    /// Set the options for the linear solver.
    auto setOptions(const LinearSolverOptions& options) -> void;

    /// Return the current options of this linear solver.
    auto options() const -> const LinearSolverOptions&;

    /// Decompose the canonical form of the given master matrix.
    auto decompose(const MasterMatrix& M) -> void;

    /// Solve the linear problem.
    /// Using this method presumes method @ref decompose has already been
    /// called. This will allow you to reuse the decomposition of the master
    /// matrix for multiple solve computations if needed.
    /// @param M The master matrix in the linear problem.
    /// @param a The right-hand side master vector in the linear problem.
    /// @param[out] u The solution master vector in the linear problem.
    auto solve(const MasterMatrix& M, const MasterVector& a, MasterVector& u) -> void;

    /// Solve the linear problem.
    /// Using this method presumes method @ref decompose has already been
    /// called. This will allow you to reuse the decomposition of the master
    /// matrix for multiple solve computations if needed.
    /// @param M The master matrix in the linear problem.
    /// @param a The right-hand side vector in the linear problem already in its canonical form.
    /// @param[out] u The solution master vector in the linear problem.
    auto solve(const MasterMatrix& M, CanonicalVector a, MasterVector& u) -> void;

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Optima