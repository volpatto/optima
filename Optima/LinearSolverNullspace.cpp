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

#include "LinearSolverNullspace.hpp"

// C++ includes
#include <cassert>

// Optima includes
#include <Optima/Exception.hpp>
#include <Optima/MasterMatrix.hpp>
#include <Optima/MasterVector.hpp>
#include <Optima/LU.hpp>

namespace Optima {

struct LinearSolverNullspace::Impl
{
    Vector ax;        ///< The workspace for the right-hand side vectors ax
    Vector ap;        ///< The workspace for the right-hand side vectors ap
    Vector aw;        ///< The workspace for the right-hand side vectors aw
    Matrix Hxx;       ///< The workspace for the auxiliary matrices Hss.
    Matrix Hxp;       ///< The workspace for the auxiliary matrices Hsp.
    Matrix Vpx;       ///< The workspace for the auxiliary matrices Vps.
    Matrix Vpp;       ///< The workspace for the auxiliary matrices Vpp.
    Matrix Mw;        ///< The workspace for the matrix M in the decompose and solve methods.
    Vector rw;        ///< The workspace for the vector r in the decompose and solve methods.
    LU lu;            ///< The LU decomposition solver.

    Impl(Index nx, Index np, Index ny, Index nz)
    {
        const auto nw = ny + nz;
        ax.resize(nx);
        ap.resize(np);
        aw.resize(nw);
        Hxx.resize(nx, nx);
        Hxp.resize(nx, np);
        Vpx.resize(np, nx);
        Vpp.resize(np, np);
        Mw.resize(nx + np + nw, nx + np + nw);
        rw.resize(nx + np + nw);
    }

    auto decompose(CanonicalMatrix J) -> void
    {
        const auto dims = J.dims;

        const auto ns  = dims.ns;
        const auto nbs = dims.nbs;
        const auto nbe = dims.nbe;
        const auto nbi = dims.nbi;
        const auto nns = dims.nns;
        const auto nne = dims.nne;
        const auto nni = dims.nni;
        const auto np  = dims.np;
        const auto nw  = dims.nw;

        auto Hss = Hxx.topLeftCorner(ns, ns);
        auto Hsp = Hxp.topRows(ns);
        auto Vps = Vpx.leftCols(ns);

        Hss = J.Hss;
        Hsp = J.Hsp;
        Vps = J.Vps;
        Vpp = J.Vpp;

        auto Hbsbs = Hss.topRows(nbs).leftCols(nbs);
        auto Hbsns = Hss.topRows(nbs).rightCols(nns);
        auto Hnsbs = Hss.bottomRows(nns).leftCols(nbs);
        auto Hnsns = Hss.bottomRows(nns).rightCols(nns);

        auto Hbebe = Hbsbs.topRows(nbe).leftCols(nbe);
        auto Hbebi = Hbsbs.topRows(nbe).rightCols(nbi);
        auto Hbibe = Hbsbs.bottomRows(nbi).leftCols(nbe);
        auto Hbibi = Hbsbs.bottomRows(nbi).rightCols(nbi);

        auto Hbens = Hbsns.topRows(nbe);
        auto Hbins = Hbsns.bottomRows(nbi);

        auto Hnsbe = Hnsbs.leftCols(nbe);
        auto Hnsbi = Hnsbs.rightCols(nbi);

        auto Hbsp = Hsp.topRows(nbs);
        auto Hnsp = Hsp.bottomRows(nns);

        auto Hbep = Hbsp.topRows(nbe);
        auto Hbip = Hbsp.bottomRows(nbi);

        auto Vpbs = Vps.leftCols(nbs);
        auto Vpns = Vps.rightCols(nns);

        auto Vpbe = Vpbs.leftCols(nbe);
        auto Vpbi = Vpbs.rightCols(nbi);

        const auto Sbsns = J.Sbsns;
        const auto Sbsp  = J.Sbsp;

        const auto Sbens = Sbsns.topRows(nbe);
        const auto Sbins = Sbsns.bottomRows(nbi);

        const auto Sbep = Sbsp.topRows(nbe);
        const auto Sbip = Sbsp.bottomRows(nbi);

        const auto Ibebe = identity(nbe, nbe);
        const auto Opbe  = zeros(np, nbe);
        const auto Obebe = zeros(nbe, nbe);

        const auto t = nbe + nns + np + nbe;

        auto M = Mw.topLeftCorner(t, t);

        auto M1 = M.topRows(nbe);
        auto M2 = M.middleRows(nbe, nns);
        auto M3 = M.middleRows(nbe + nns, np);
        auto M4 = M.bottomRows(nbe);

        Hbins.noalias() -= Hbibi * Sbins;
        Hbens.noalias() -= Hbebi * Sbins;
        Hnsns.noalias() -= Hnsbi * Sbins;
        Vpns.noalias()  -= Vpbi * Sbins;

        Hbip.noalias() -= Hbibi * Sbip;
        Hbep.noalias() -= Hbebi * Sbip;
        Hnsp.noalias() -= Hnsbi * Sbip;
        Vpp.noalias()  -= Vpbi * Sbip;

        Hnsbe.noalias() -= tr(Sbins) * Hbibe;
        Hnsns.noalias() -= tr(Sbins) * Hbins;
        Hnsp.noalias()  -= tr(Sbins) * Hbip;

        if(nbe) M1 << Hbebe, Hbens, Hbep, Ibebe;
        if(nns) M2 << Hnsbe, Hnsns, Hnsp, tr(Sbens);
        if( np) M3 << Vpbe, Vpns, Vpp, Opbe;
        if(nbe) M4 << Ibebe, Sbens, Sbep, Obebe;

        if(t) lu.decompose(M);
    }

    auto solve(CanonicalMatrix J, CanonicalVector a, CanonicalVectorRef u) -> void
    {
        const auto dims = J.dims;

        const auto ns  = dims.ns;
        const auto nbs = dims.nbs;
        const auto nbe = dims.nbe;
        const auto nbi = dims.nbi;
        const auto nns = dims.nns;
        const auto nne = dims.nne;
        const auto nni = dims.nni;
        const auto np  = dims.np;
        const auto nw  = dims.nw;

        const auto Hss = Hxx.topLeftCorner(ns, ns);
        const auto Hsp = Hxp.topRows(ns);
        const auto Vps = Vpx.leftCols(ns);

        const auto Hbsbs = Hss.topRows(nbs).leftCols(nbs);
        const auto Hbsns = Hss.topRows(nbs).rightCols(nns);
        const auto Hnsbs = Hss.bottomRows(nns).leftCols(nbs);

        const auto Hbebi = Hbsbs.topRows(nbe).rightCols(nbi);
        const auto Hbibe = Hbsbs.bottomRows(nbi).leftCols(nbe);
        const auto Hbibi = Hbsbs.bottomRows(nbi).rightCols(nbi);

        const auto Hbins = Hbsns.bottomRows(nbi);
        const auto Hnsbi = Hnsbs.rightCols(nbi);

        const auto Hbsp = Hsp.topRows(nbs);
        const auto Hbip = Hbsp.bottomRows(nbi);

        const auto Vpbs = Vps.leftCols(nbs);
        const auto Vpbi = Vpbs.rightCols(nbi);

        const auto Sbsns = J.Sbsns;
        const auto Sbsp  = J.Sbsp;

        const auto Sbens = Sbsns.topRows(nbe);
        const auto Sbins = Sbsns.bottomRows(nbi);

        const auto Sbep = Sbsp.topRows(nbe);
        const auto Sbip = Sbsp.bottomRows(nbi);

        auto as  = ax.head(ns);
        auto abs = as.head(nbs);
        auto ans = as.tail(nns);
        auto abe = abs.head(nbe);
        auto abi = abs.tail(nbi);

        auto awbs = aw.head(nbs);
        auto awbe = awbs.head(nbe);
        auto awbi = awbs.tail(nbi);

        as = a.xs;
        ap = a.p;
        awbs = a.wbs;

        abi.noalias() -= Hbibi * awbi;
        abe.noalias() -= Hbebi * awbi;
        ans.noalias() -= Hnsbi * awbi;
        ap.noalias()  -= Vpbi * awbi;

        ans -= tr(Sbins) * abi;

        const auto t = nbe + nns + np + nbe;

        auto r = rw.head(t);

        auto dxbe = r.head(nbe);
        auto dxns = r.segment(nbe, nns);
        auto dp   = r.segment(nbe + nns, np);
        auto dwbe = r.tail(nbe);

        if(t) r << abe, ans, ap, awbe;

        if(t) lu.solve(r);

        auto dxbi = awbi;
        auto dwbi = abi;

        dxbi.noalias() = awbi - Sbins*dxns - Sbip*dp;
        dwbi.noalias() = abi - Hbibe*dxbe - Hbins*dxns - Hbip*dp;

        u.xs << dxbe, dxbi, dxns;
        u.p = dp;
        u.wbs << dwbe, dwbi;
    }
};

LinearSolverNullspace::LinearSolverNullspace(Index nx, Index np, Index ny, Index nz)
: pimpl(new Impl(nx, np, ny, nz))
{}

LinearSolverNullspace::LinearSolverNullspace(const LinearSolverNullspace& other)
: pimpl(new Impl(*other.pimpl))
{}

LinearSolverNullspace::~LinearSolverNullspace()
{}

auto LinearSolverNullspace::operator=(LinearSolverNullspace other) -> LinearSolverNullspace&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto LinearSolverNullspace::decompose(CanonicalMatrix M) -> void
{
    pimpl->decompose(M);
}

auto LinearSolverNullspace::solve(CanonicalMatrix J, CanonicalVector a, CanonicalVectorRef u) -> void
{
    pimpl->solve(J, a, u);
}

} // namespace Optima