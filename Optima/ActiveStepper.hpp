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
#include <Optima/Matrix.hpp>

namespace Optima {

// Forward declarations
class SaddlePointMatrix;
class SaddlePointVector;
class Options;

/// The problem data needed to calculate a step using ActiveStepper.
struct ActiveStepperProblem
{
    /// The current state of the primal variables of the canonical optimization problem.
    VectorConstRef x;

    /// The current state of the Lagrange multipliers of the canonical optimization problem.
    VectorConstRef y;

    /// The coefficient matrix of the linear equality constraints of the canonical optimization problem.
    MatrixConstRef A;

    /// The right-hand side vector of the linear equality constraints of the canonical optimization problem.
    VectorConstRef b;

    /// The value of the equality constraint function.
    VectorConstRef h;

    /// The Jacobian of the equality constraint function.
    MatrixConstRef J;

    /// The gradient of the objective function.
    VectorConstRef g;

    /// The Hessian of the objective function.
    MatrixConstRef H;

    /// The values of the lower bounds of the variables constrained with lower bounds.
    VectorConstRef xlower;

    /// The values of the upper bounds of the variables constrained with upper bounds.
    VectorConstRef xupper;

    /// The indices of the variables with lower bounds.
    IndicesConstRef ilower;

    /// The indices of the variables with upper bounds.
    IndicesConstRef iupper;

    /// The indices of the variables with fixed values.
    IndicesConstRef ifixed;
};

/// The class that implements the step calculation.
class ActiveStepper
{
public:
    /// Construct a default ActiveStepper instance.
    ActiveStepper();

    /// Construct a copy of an ActiveStepper instance.
    ActiveStepper(const ActiveStepper& other);

    /// Destroy this ActiveStepper instance.
    virtual ~ActiveStepper();

    /// Assign an ActiveStepper instance to this.
    auto operator=(ActiveStepper other) -> ActiveStepper&;

    /// Set the options for the step calculation.
    auto setOptions(const Options& options) -> void;

    /// Decompose the interior-point saddle point matrix used to compute the step vectors.
    auto decompose(const ActiveStepperProblem& problem) -> void;

    /// Solve the interior-point saddle point matrix used to compute the step vectors.
    /// @note Method ActiveStepper::decompose needs to be called first.
    auto solve(const ActiveStepperProblem& problem) -> void;

    /// Return the calculated Newton step vector.
    /// @note Method ActiveStepper::solve needs to be called first.
    auto step() const -> SaddlePointVector;

    /// Return the calculated residual vector for the current optimum state.
    /// @note Method ActiveStepper::solve needs to be called first.
    auto residual() const -> SaddlePointVector;

    /// Return the assembled interior-point saddle point matrix.
    /// @note Method ActiveStepper::decompose needs to be called first.
    auto matrix(const ActiveStepperProblem& problem) -> SaddlePointMatrix;

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Optima