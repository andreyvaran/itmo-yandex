#ifndef MP_BIGNUM_HPP
#define MP_BIGNUM_HPP

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>

namespace mp {
    class bignum {
    private:
        static const uint64_t base = std::pow(2, 32);
        std::vector<uint32_t> digits;

    public:
        bignum() { //конструктор по умолчанию инициализирует нулем;
            digits.push_back(0);
        }

        bignum(uint32_t value) { // неявное конструирование от целого числа (uint32_t);
            digits.push_back(value);
        }

        explicit bignum(const std::string &str) {
            bignum tens(1);
            for (int i = str.size() - 1; i >= 0; i--) {
                bignum tmp = bignum(str[i] - '0') * tens;

                *this += bignum(str[i] - '0') * tens;
                tens *= 10;
            }
            for (auto elem : digits){
                std::cout << elem << std::endl;
            }
        }


        std::string to_string() const {
            std::string str;
            bignum temp;
            for (int i = digits.size() - 1; i >= 0; i--) {
                str += std::to_string(digits[i]);
            }
            return str;
        }

        explicit operator uint32_t() const {
//            if (digit s.empty()) {
//                return 0;
//            }
//            uint32_t result = 0;
//            uint64_t multiplier = 1;
//            for (size_t i = 0; i < digits.size(); ++i) {
//                result += digits[i] * multiplier;
//                multiplier *= base;
//            }
//            return result;
            return *digits.begin();
        }

        friend bignum operator+(const bignum &a, const bignum &b) {

            bignum result;
            uint32_t remains = 0;
            size_t max_len = std::max(a.digits.size(), b.digits.size());

            for (size_t i = 0; i < max_len; i++) {
                if (i == result.digits.size())
                    result.digits.push_back(0);
                uint64_t sum = static_cast<uint64_t>(result.digits[i]) + remains;
                if (i < a.digits.size()) sum += a.digits[i];
                if (i < b.digits.size()) sum += b.digits[i];
                result.digits[i] = static_cast<uint32_t>(sum % base);
                remains = sum / base;
            }
            if (remains != 0) result.digits.push_back(1);
            return result;
        }


        friend bignum operator*(const bignum &a, const bignum &b) {
            bignum result;
            size_t len_a = a.digits.size();
            size_t len_b = b.digits.size();
            result.digits.resize(len_a + len_b, 0);

            for (size_t i = 0; i < len_a; i++) {
                uint32_t carry = 0;
                for (size_t j = 0; j < len_b || carry; j++) {
                    uint64_t mul = static_cast<uint64_t>(result.digits[i + j]) +
                                   static_cast<uint64_t>(a.digits[i]) * (j < len_b ? b.digits[j] : 0) + carry;
                    result.digits[i + j] = static_cast<uint32_t>(mul % base);
                    carry = static_cast<uint32_t>(mul / base);
                }
            }

            while (result.digits.size() > 1 && result.digits.back() == 0) {
                result.digits.pop_back();
            }

            return result;
        }


        bignum &operator+=(const bignum &other) {
            *this = *this + other;
            return *this;
        }

        bignum &operator*=(const bignum &other) {
            *this = *this * other;
            return *this;
        }

//        explicit bignum(const std::string &str) {
//            bignum temp;
//            uint64_t sum = 0, mult = 1;
//
//            for (int i = str.size() - 1; i >= 0; i--) {
//                sum = (str[i] - '0') * mult;
//                mult *= 10;
//                if (sum > base) {
//                    digits.push_back(static_cast<uint32_t>(sum / base));
//                    sum %= base;
//                }
//            }
//            digits.push_back(static_cast<uint32_t>(sum));
//        }


        bignum(const bignum &other) {
            digits = other.digits;
        }

        bignum &operator=(const bignum &other) {
            if (this != &other) {
                digits = other.digits;
            }
            return *this;
        }


        bool operator!() const {
            return digits.size() == 1 && digits[0] == 0;
        }

        operator bool() const {
            for (const auto &digit: digits) {
                if (digit != 0) {
                    return true;
                }
            }
            return false;
        }

        friend std::ostream &operator<<(std::ostream &os, const bignum &num) {
            os << num.to_string();
            return os;
        }

        friend std::istream &operator>>(std::istream &is, bignum &num) {
            std::string str;
            is >> str;
            num = bignum(str);
            return is;
        }

    };

}

#endif // MP_BIGNUM_HPP
