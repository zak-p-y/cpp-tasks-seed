#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "Gauss_solve.h"

namespace
{
GaussMatrix make_matrix(std::initializer_list<std::initializer_list<double>> rows)
{
    const int row_count = static_cast<int>(rows.size());
    const int col_count = static_cast<int>(rows.begin()->size());
    GaussMatrix matrix(row_count, col_count);
    int row = 0;
    for (const auto &src_row : rows)
    {
        if (static_cast<int>(src_row.size()) != col_count)
        {
            throw std::runtime_error("ragged matrix initializer");
        }
        int col = 0;
        for (double value : src_row)
        {
            matrix(row, col) = value;
            ++col;
        }
        ++row;
    }
    return matrix;
}

std::string make_temp_file(const std::string &content)
{
    char path[] = "/tmp/gaussXXXXXX";
    const int fd = mkstemp(path);
    if (fd < 0)
    {
        throw std::runtime_error("mkstemp failed");
    }
    close(fd);

    std::ofstream out(path);
    out << content;
    out.close();
    return std::string(path);
}

} // namespace

TEST(GaussUtil, LoadCsvSkipsHeader)
{
    const std::string path = make_temp_file(
                                 "A,B,C\n"
                                 "1,2,3\n"
                                 "4,5,6\n");

    const GaussMatrix matrix = load_csv_to_matrix(path.c_str());
    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.cols(), 3);
    EXPECT_DOUBLE_EQ(matrix(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(matrix(1, 2), 6.0);

    std::remove(path.c_str());
}

TEST(GaussUtil, PrintMatrixAsCsv)
{
    const GaussMatrix matrix = make_matrix({{1.25, 2.5}, {3.75, 4.0}});
    std::ostringstream out;
    print_matrix_as_csv(out, matrix, 2);
    EXPECT_EQ(out.str(), "1.25,2.50\n3.75,4.00");
}

TEST(GaussUtil, PrintVectorAsCsv)
{
    GaussVector vector(3);
    vector(0) = 1.0;
    vector(1) = 2.5;
    vector(2) = -3.125;

    std::ostringstream out;
    print_matrix_as_csv(out, vector, 3);
    EXPECT_EQ(out.str(), "1.000\n2.500\n-3.125");
}

TEST(GaussSolve, SmallSolve)
{
    GaussMatrix ab(2, 3);
    ab(0, 0) = 2.0;
    ab(0, 1) = 1.0;
    ab(0, 2) = 5.0;
    ab(1, 0) = 1.0;
    ab(1, 1) = -1.0;
    ab(1, 2) = 1.0;
    const GaussVector x = Gauss_solve(ab);
    EXPECT_NEAR(x(0), 2.0, 1e-9);
    EXPECT_NEAR(x(1), 1.0, 1e-9);
}

TEST(GaussSolve, RequiresPivoting)
{
    GaussMatrix ab(2, 3);
    ab(0, 0) = 0.0;
    ab(0, 1) = 1.0;
    ab(0, 2) = 1.0;
    ab(1, 0) = 2.0;
    ab(1, 1) = 3.0;
    ab(1, 2) = 5.0;
    const GaussVector x = Gauss_solve(ab);
    EXPECT_NEAR(x(0), 1.0, 1e-9);
    EXPECT_NEAR(x(1), 1.0, 1e-9);
}

TEST(GaussSolve, SingularMatrixThrows)
{
    GaussMatrix ab = make_matrix(
    {
        {1.0, 2.0, 3.0},
        {2.0, 4.0, 6.0},
    });

    EXPECT_THROW((void)Gauss_solve(ab), std::runtime_error);
}

TEST(GaussSolve, RandomDominantSystemIsSolved)
{
    constexpr int n = 12;
    std::mt19937_64 rng(20240608ULL);
    std::uniform_real_distribution<double> coeff_dist(-5.0, 5.0);
    std::uniform_int_distribution<int> solution_dist(-4, 4);

    GaussMatrix ab(n, n + 1);
    std::vector<double> expected(static_cast<std::size_t>(n));

    for (int row = 0; row < n; ++row)
    {
        expected[static_cast<std::size_t>(row)] = static_cast<double>(solution_dist(rng));
    }

    for (int row = 0; row < n; ++row)
    {
        double rhs = 0.0;
        double diag_abs_sum = 0.0;
        for (int col = 0; col < n; ++col)
        {
            if (col > row)
            {
                const double value = coeff_dist(rng);
                ab(row, col) = value;
                rhs += value * expected[static_cast<std::size_t>(col)];
                diag_abs_sum += std::abs(value);
            }
            else if (col < row)
            {
                const double value = coeff_dist(rng);
                ab(row, col) = value;
                rhs += value * expected[static_cast<std::size_t>(col)];
                diag_abs_sum += std::abs(value);
            }
        }

        const double diagonal = diag_abs_sum + 10.0 + std::abs(coeff_dist(rng));
        ab(row, row) = diagonal;
        rhs += diagonal * expected[static_cast<std::size_t>(row)];
        ab(row, n) = rhs;
    }

    const GaussVector x = Gauss_solve(ab);
    ASSERT_EQ(x.size(), n);
    for (int i = 0; i < n; ++i)
    {
        EXPECT_NEAR(x(i), expected[static_cast<std::size_t>(i)], 1e-8);
    }
}
