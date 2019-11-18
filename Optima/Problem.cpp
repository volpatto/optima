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

#include "Problem.hpp"

namespace Optima {

Problem::Problem()
{}

Problem::Problem(const ObjectiveFunction& objective, const Constraints& constraints)
: m_objective(objective), m_constraints(constraints)
{}

// auto Problem::xlower(double val) -> void
// {
//    m_xlower.fill(val);
//    m_nlower = m_n;
// }

// auto Problem::xlower(VectorConstRef values) -> void
// {
//    m_xlower.head(m_n) = values;
//    m_nlower = m_n;
// }

// auto Problem::xlower(IndicesConstRef indices, VectorConstRef values) -> void
// {
//    m_xlower(indices) = values;
//    m_nlower = indices.size();
//    m_lowerpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_lowerpartition.head(m_nlower).swap(m_lowerpartition(indices));
// }

// auto Problem::xlower() const -> VectorConstRef
// {
//    return m_xlower.head(m_nlower);
// }

// auto Problem::xupper(double val) -> void
// {
//    m_xupper.fill(val);
//    m_nupper = m_n;
// }

// auto Problem::xupper(VectorConstRef values) -> void
// {
//    m_xupper.head(m_n) = values;
//    m_nupper = m_n;
// }

// auto Problem::xupper(IndicesConstRef indices, VectorConstRef values) -> void
// {
//    m_xupper(indices) = values;
//    m_nupper = indices.size();
//    m_upperpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_upperpartition.head(m_nlower).swap(m_upperpartition(indices));
// }

// auto Problem::xupper() const -> VectorConstRef
// {
//    return m_xupper.head(m_nupper);
// }

// auto Problem::xfixed(double val) -> void
// {
//    m_xfixed.fill(val);
//    m_nfixed = m_n;
// }

// auto Problem::xfixed(VectorConstRef values) -> void
// {
//    m_xfixed.head(m_n) = values;
//    m_nfixed = m_n;
// }

// auto Problem::xfixed(IndicesConstRef indices, VectorConstRef values) -> void
// {
//    m_xfixed(indices) = values;
//    m_nfixed = indices.size();
//    m_fixedpartition.setLinSpaced(m_n, 0, m_n - 1);
//    m_fixedpartition.head(m_nlower).swap(m_fixedpartition(indices));
// }

// auto Problem::xfixed() const -> VectorConstRef
// {
//    return m_xfixed.head(m_nfixed);
// }

} // namespace Optima
