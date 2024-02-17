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
        static const uint64_t base = 1ll << 32;;
        std::vector<uint32_t> digits;

        uint32_t get_last_digit() const {

            uint32_t result = digits[0] % 10;
            uint64_t current_remains = base % 10;

            for (std::size_t i = 1; i < digits.size(); i++) {
                uint64_t сurrent = (digits[i] % 10) * current_remains;
                сurrent %= 10;
                result += сurrent;
                result %= 10;
                current_remains *= base % 10;
                current_remains %= 10;
            }
            return result;
        }

        bignum &cut_last_digit() {

            std::vector<uint32_t> temp = digits;
            uint64_t rem = 0;

            for (std::size_t i = digits.size(); i >= 1; --i) {
                uint64_t current_digit = digits[i - 1];
                uint32_t current = (current_digit + (rem * base)) / 10;
                temp[i - 1] = current;
                rem = (current_digit + (rem * base)) % 10;
            }

            digits = temp;
            remove_empty();

            return *this;
        }

        bignum &remove_empty() {
            std::size_t new_size = digits.size();
            while (new_size > 0 && digits[new_size - 1] == 0) {
                new_size--;
            }
            digits.resize(new_size);
            return *this;
        }

    public:
        bignum() { //конструктор по умолчанию инициализирует нулем;
            digits.push_back(0);
        }

        bignum(uint32_t value) {
            digits.push_back(value);
        }

        explicit bignum(const std::string &str) {
            bignum tens(1);
            for (int i = str.size() - 1; i >= 0; i--) {

                *this += bignum(str[i] - '0') * tens;
                tens *= 10;
                std::cout << tens << std::endl;
            }
        }


        std::string to_string() const {
            std::string result;
            mp::bignum copy = *this;
            while (copy.digits.size() != 0) {
                uint32_t last_digit = copy.get_last_digit();
                result += std::to_string(last_digit);
                copy = copy.cut_last_digit();
            }
            if (result.empty()) {
                result = '0';
            }
            std::reverse(result.begin(), result.end());
            return result;

        }

        explicit operator uint32_t() const {
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
            return digits.back() != 0;
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
