#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace util {

    template <typename T>
    bool is_in(const std::vector<T>& vec, const T& element) {
        return std::find(vec.begin(), vec.end(), element) != vec.end();
    }

    template <typename T>
    T& greatest(std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::logic_error("Error in util::greatest: The vector is empty.");
        }
        return *std::max_element(vec.begin(), vec.end());
    }

    template <typename T>
    const T& greatest(const std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::logic_error("Error in utill::greatest: The vector is empty.");
        }
        return *std::max_element(vec.begin(), vec.end());
    }

    template <typename T>
    T& smallest(std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::logic_error("Error in utill::smallest: The vector is empty.");
        }
        return *std::min_element(vec.begin(), vec.end());
    }

    template <typename T>
    const T& smallest(const std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::logic_error("Error in utill::smallest: The vector is empty.");
        }
        return *std::min_element(vec.begin(), vec.end());
    }

    template <typename T>
    void remove(std::vector<T>& vec, const T& element) {
        auto it = std::find(vec.begin(), vec.end(), element);
        if (it != vec.end()) {
            vec.erase(it);
        }
    }

    template <typename T>
    void remove_by_ref(std::vector<T>& vec, const T& element_ref) {
        if (vec.empty()) return;

        auto index = &element_ref - &vec[0];

        if (index >= 0 && static_cast<size_t>(index) < vec.size()) {
            vec.erase(vec.begin() + index);
        }
    }

    template <typename T1, typename T2>
    std::pair<T1, T2>& smallest_first(std::vector<std::pair<T1, T2>>& vec) {
        if (vec.empty()) {
            throw std::runtime_error("Error in utill::smallest_first: The vector is empty.");
        }

        auto it = std::min_element(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        return *it;
    }

    template <typename T>
    typename std::vector<T>::iterator search(std::vector<T>& vec, const T& element) {
        return std::find(vec.begin(), vec.end(), element);
    }
}

#endif
