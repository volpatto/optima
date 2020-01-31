// // Optima is a C++ library for solving linear and non-linear constrained optimization problems
// //
// // Copyright (C) 2014-2018 Allan Leal
// //
// // This program is free software: you can redistribute it and/or modify
// // it under the terms of the GNU General Public License as published by
// // the Free Software Foundation, either version 3 of the License, or
// // (at your option) any later version.
// //
// // This program is distributed in the hope that it will be useful,
// // but WITHOUT ANY WARRANTY; without even the implied warranty of
// // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// // GNU General Public License for more details.
// //
// // You should have received a copy of the GNU General Public License
// // along with this program. If not, see <http://www.gnu.org/licenses/>.

// #pragma once

// // Optima includes
// #include <Optima/Index.hpp>
// #include <Optima/Matrix.hpp>

// namespace Optima {

// // Forward declarations
// class Constraints;

// /// A type that describes the Lagrange multipliers in a canonical optimization problem.
// class LagrangeMultipliers
// {
// public:
//     /// Construct a default LagrangeMultipliers instance.
//     LagrangeMultipliers();

//     /// Construct a LagrangeMultipliers instance with given constraints.
//     LagrangeMultipliers(const Constraints& constraints);


//     /// Return the Lagrange multipliers of the canonical optimization problem.
//     auto canonical() const -> VectorConstRef;

//     /// Return the Lagrange multipliers of the canonical optimization problem.
//     auto canonical() -> VectorRef;


//     /// Return the Lagrange multipliers with respect to the linear equality constraints of the original optimization problem.
//     auto wrtLinearEqualityConstraints() const -> VectorConstRef;

//     /// Return the Lagrange multipliers with respect to the linear equality constraints of the original optimization problem.
//     auto wrtLinearEqualityConstraints() -> VectorRef;


//     /// Return the Lagrange multipliers with respect to the non-linear equality constraints of the original optimization problem.
//     auto wrtNonLinearEqualityConstraints() const -> VectorConstRef;

//     /// Return the Lagrange multipliers with respect to the non-linear equality constraints of the original optimization problem.
//     auto wrtNonLinearEqualityConstraints() -> VectorRef;


//     /// Return the Lagrange multipliers with respect to the linear inequality constraints of the original optimization problem.
//     auto wrtLinearInequalityConstraints() const -> VectorConstRef;

//     /// Return the Lagrange multipliers with respect to the linear inequality constraints of the original optimization problem.
//     auto wrtLinearInequalityConstraints() -> VectorRef;


//     /// Return the Lagrange multipliers with respect to the non-linear inequality constraints of the original optimization problem.
//     auto wrtNonLinearInequalityConstraints() const -> VectorConstRef;

//     /// Return the Lagrange multipliers with respect to the non-linear inequality constraints of the original optimization problem.
//     auto wrtNonLinearInequalityConstraints() -> VectorRef;


// private:
//     /// The number of linear equality constraints in the original optimization problem.
//     Index mle = 0;

//     /// The number of non-linear equality constraints in the original optimization problem.
//     Index mne = 0;

//     /// The number of linear inequality constraints in the original optimization problem.
//     Index mli = 0;

//     /// The number of non-linear inequality constraints in the original optimization problem.
//     Index mni = 0;

//     /// The vector containing the Lagrange multipliers of the canonical optimization problem.
//     Vector data;
// };

// } // namespace Optima
