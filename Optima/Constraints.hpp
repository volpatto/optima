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

// Optima includes
#include <Optima/Index.hpp>
#include <Optima/Matrix.hpp>

namespace Optima {

/// The constraints in an optimization problem.
class Constraints
{
public:
    /// Construct a default Constraints instance.
    Constraints();

    /// Construct a Constraints instance with given number of variables.
    /// @param n The number of variables in \eq{x} in the optimization problem.
    explicit Constraints(Index n);

    /// Set the equality constraint matrix \eq{A_e}.
    auto setEqualityConstraintMatrix(MatrixConstRef Ae) -> void;

    /// Set the inequality constraint matrix \eq{A_i}.
    auto setInequalityConstraintMatrix(MatrixConstRef Ai) -> void;

    /// Set the indices of the variables in \eq{x} with lower bounds.
    auto setVariablesWithLowerBounds(IndicesConstRef indices) -> void;

    /// Set all variables in \eq{x} with lower bounds.
    auto allVariablesHaveLowerBounds() -> void;

    /// Set the indices of the variables in \eq{x} with upper bounds.
    auto setVariablesWithUpperBounds(IndicesConstRef indices) -> void;

    /// Set all variables in \eq{x} with upper bounds.
    auto allVariablesHaveUpperBounds() -> void;

    /// Set the indices of the variables in \eq{x} with fixed values.
    auto setVariablesWithFixedValues(IndicesConstRef indices) -> void;

    /// Return the number of variables.
    auto numVariables() const -> Index;

    /// Return the number of linear equality constraints.
    auto numEqualityConstraints() const -> Index;

    /// Return the number of linear inequality constraints.
    auto numInequalityConstraints() const -> Index;

    /// Return the equality constraint matrix \eq{A_e}.
    auto equalityConstraintMatrix() const -> MatrixConstRef;

    /// Return the inequality constraint matrix \eq{A_i}.
    auto inequalityConstraintMatrix() const -> MatrixConstRef;

    /// Return the indices of the variables with lower bounds.
    auto variablesWithLowerBounds() const -> IndicesConstRef;

    /// Return the indices of the variables with upper bounds.
    auto variablesWithUpperBounds() const -> IndicesConstRef;

    /// Return the indices of the variables with fixed values.
    auto variablesWithFixedValues() const -> IndicesConstRef;

    /// Return the indices of the variables without lower bounds.
    auto variablesWithoutLowerBounds() const -> IndicesConstRef;

    /// Return the indices of the variables without upper bounds.
    auto variablesWithoutUpperBounds() const -> IndicesConstRef;

    /// Return the indices of the variables without fixed values.
    auto variablesWithoutFixedValues() const -> IndicesConstRef;

    /// Return the indices of the variables partitioned in [without, with] lower bounds.
    auto orderingLowerBounds() const -> IndicesConstRef;

    /// Return the indices of the variables partitioned in [without, with] upper bounds.
    auto orderingUpperBounds() const -> IndicesConstRef;

    /// Return the indices of the variables partitioned in [without, with] fixed values.
    auto orderingFixedValues() const -> IndicesConstRef;

private:
    /// The number of variables in the optimization problem.
    Index n;

    /// The coefficient matrix of the linear equality constraint equations \eq{A_{e}x=b_{e}}.
    Matrix Ae;

    /// The coefficient matrix of the linear inequality constraint equations \eq{A_{i}x\geq b_{i}}.
    Matrix Ai;

    /// The number of variables with lower bounds.
    Index nlower;

    /// The number of variables with upper bounds.
    Index nupper;

    /// The number of variables with fixed values.
    Index nfixed;

    /// The indices of the variables partitioned in [with, without] lower bounds.
    Indices lowerpartition;

    /// The indices of the variables partitioned in [with, without] upper bounds.
    Indices upperpartition;

    /// The indices of the variables partitioned in [with, without] fixed values.
    Indices fixedpartition;
};

} // namespace Optima
