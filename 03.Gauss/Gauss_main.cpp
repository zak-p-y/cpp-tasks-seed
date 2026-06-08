#include <iostream>
#include <stdexcept>

#include "Gauss_solve.h"

int main(int argc, const char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "usage: " << argv[0] << " <input.csv>\n";
            return 1;
        }

        GaussMatrix ab = load_csv_to_matrix(argv[1]);
        GaussVector solution = Gauss_solve(ab);
        print_matrix_as_csv(std::cout, solution);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "gauss: " << e.what() << '\n';
        return 1;
    }
}
