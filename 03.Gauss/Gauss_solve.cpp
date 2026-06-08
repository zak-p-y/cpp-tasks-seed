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

    const int n = ab.rows();
    GaussVector x(n);

    for (int pivot_col = 0; pivot_col < n; ++pivot_col)
    {
        int pivot_row = pivot_col;
        double pivot_abs = std::abs(ab(pivot_row, pivot_col));
        for (int row = pivot_col + 1; row < n; ++row)
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

        ab.swap_rows(pivot_col, pivot_row);

        for (int row = pivot_col + 1; row < n; ++row)
        {
            const double pivot = ab(pivot_col, pivot_col);
            const double factor = ab(row, pivot_col) / pivot;
            if (std::abs(factor) <= kPivotEps)
            {
                ab(row, pivot_col) = 0.0;
                continue;
            }

            ab.row(row) -= factor * ab.row(pivot_col);
            ab(row, pivot_col) = 0.0;
        }
    }

    for (int row = n - 1; row >= 0; --row)
    {
        double rhs = ab(row, n);
        for (int col = row + 1; col < n; ++col)
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
