#include "Gauss_solve.h"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace
{
constexpr double kPivotEps = 1e-12;
}

GaussVector Gauss_solve(GaussMatrix &ab)
{
    if (ab.cols() != ab.rows() + 1)
    {
        throw std::invalid_argument("augmented matrix must have n rows and n+1 columns");
    }

    const Eigen::Index n = ab.rows();
    GaussVector x(n);

    for (Eigen::Index pivot_col = 0; pivot_col < n; ++pivot_col)
    {
        Eigen::Index pivot_row = pivot_col;
        double pivot_abs = std::abs(ab(pivot_row, pivot_col));
        for (Eigen::Index row = pivot_col + 1; row < n; ++row)
        {
            const double candidate_abs = std::abs(ab(row, pivot_col));
            if (candidate_abs > pivot_abs)
            {
                pivot_abs = candidate_abs;
                pivot_row = row;
            }
        }

        if (pivot_abs <= kPivotEps)
        {
            throw std::runtime_error("singular matrix");
        }

        if (pivot_row != pivot_col)
        {
            for (Eigen::Index col = 0; col < ab.cols(); ++col)
            {
                std::swap(ab(pivot_col, col), ab(pivot_row, col));
            }
        }

        for (Eigen::Index row = pivot_col + 1; row < n; ++row)
        {
            const double pivot = ab(pivot_col, pivot_col);
            const double factor = ab(row, pivot_col) / pivot;
            if (std::abs(factor) <= kPivotEps)
            {
                ab(row, pivot_col) = 0.0;
                continue;
            }

            for (Eigen::Index col = pivot_col + 1; col <= n; ++col)
            {
                ab(row, col) -= factor * ab(pivot_col, col);
            }
            ab(row, pivot_col) = 0.0;
        }
    }

    for (Eigen::Index row = n; row-- > 0;)
    {
        double rhs = ab(row, n);
        for (Eigen::Index col = row + 1; col < n; ++col)
        {
            rhs -= ab(row, col) * x(col);
        }

        const double diag = ab(row, row);
        if (std::abs(diag) <= kPivotEps)
        {
            throw std::runtime_error("singular matrix");
        }

        x(row) = rhs / diag;
    }

    return x;
}
