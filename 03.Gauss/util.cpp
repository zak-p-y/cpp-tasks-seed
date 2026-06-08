#include "util.h"

#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
std::string trim(const std::string &s)
{
    std::size_t begin = 0;
    while (begin < s.size() && std::isspace(static_cast<unsigned char>(s[begin])) != 0)
    {
        ++begin;
    }

    std::size_t end = s.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(s[end - 1])) != 0)
    {
        --end;
    }

    return s.substr(begin, end - begin);
}

bool parse_double_cell(const std::string &cell, double &value)
{
    const std::string trimmed = trim(cell);
    if (trimmed.empty())
    {
        return false;
    }

    std::size_t parsed = 0;
    try
    {
        value = std::stod(trimmed, &parsed);
    }
    catch (const std::exception &)
    {
        return false;
    }

    return parsed == trimmed.size();
}

std::vector<std::string> split_csv_line(const std::string &line)
{
    std::vector<std::string> cells;
    std::string current;
    std::stringstream ss(line);
    while (std::getline(ss, current, ','))
    {
        cells.push_back(current);
    }

    if (!line.empty() && line.back() == ',')
    {
        cells.emplace_back();
    }

    return cells;
}

template<typename VectorLike>
void print_one_column_csv(std::ostream &out, const VectorLike &matrix_like, int prec)
{
    out << std::fixed << std::setprecision(prec);
    for (int row = 0; row < matrix_like.rows(); ++row)
    {
        out << matrix_like(row, 0);
        if (row + 1 < matrix_like.rows())
        {
            out << '\n';
        }
    }
}
} // namespace

GaussMatrix load_csv_to_matrix(const char *filename)
{
    std::ifstream in(filename);
    if (!in)
    {
        throw std::runtime_error(std::string("cannot open csv file: ") + filename);
    }

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> cells = split_csv_line(line);
        std::vector<double> parsed_row;
        parsed_row.reserve(cells.size());

        bool numeric_row = true;
        for (const std::string &cell : cells)
        {
            double value = 0.0;
            if (!parse_double_cell(cell, value))
            {
                numeric_row = false;
                break;
            }
            parsed_row.push_back(value);
        }

        if (!numeric_row)
        {
            if (!rows.empty())
            {
                throw std::runtime_error("csv contains non-numeric data after numeric rows");
            }
            continue;
        }

        if (!rows.empty() && parsed_row.size() != rows.front().size())
        {
            throw std::runtime_error("csv is not rectangular");
        }
        rows.push_back(std::move(parsed_row));
    }

    if (rows.empty())
    {
        throw std::runtime_error("csv file does not contain numeric rows");
    }

    if (rows.front().size() < 2)
    {
        throw std::runtime_error("csv matrix must contain at least two columns");
    }

    GaussMatrix matrix(static_cast<int>(rows.size()), static_cast<int>(rows.front().size()));
    for (int row = 0; row < matrix.rows(); ++row)
    {
        for (int col = 0; col < matrix.cols(); ++col)
        {
            matrix(row, col) = rows[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
        }
    }
    return matrix;
}

void print_matrix_as_csv(std::ostream &out, const GaussMatrix &matrix, int prec)
{
    out << std::fixed << std::setprecision(prec);
    for (int row = 0; row < matrix.rows(); ++row)
    {
        for (int col = 0; col < matrix.cols(); ++col)
        {
            if (col > 0)
            {
                out << ',';
            }
            out << matrix(row, col);
        }
        if (row + 1 < matrix.rows())
        {
            out << '\n';
        }
    }
}

void print_matrix_as_csv(std::ostream &out, const GaussVector &vector, int prec)
{
    print_one_column_csv(out, vector, prec);
}
