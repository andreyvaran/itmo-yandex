//
// Created by andreyvaran on 26.10.23.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace mp {
    class polynomial {
    public:
        polynomial(const std::string& input) {
            std::istringstream ss(input);
            std::string term;

            while (std::getline(ss, term, '+')) {
                int coef = 0;
                int power = 0;
                std::istringstream term_ss(term);
                term_ss >> coef;
                term_ss.ignore(256, 'x');
                term_ss >> power;

                if (power >= static_cast<int>(coefficients.size())) {
                    coefficients.resize(power + 1, 0);
                }
                coefficients[power] = coef;
            }
        }

        const uint32_t& at(std::size_t index) const {

            return coefficients[index];
        }

        uint32_t& at(std::size_t index) {
            if (index >= coefficients.size()) {
                coefficients.resize(index + 1, 0);
            }
            return coefficients[index];
        }

        template <typename T>
        T operator()(T x) const {
            T result = 0;
            for (int i = coefficients.size() - 1; i >= 0; --i) {
                result = result * x + static_cast<T>(coefficients[i]);
            }
            return result;
        }

    private:
        std::vector<uint32_t> coefficients;
    };
}