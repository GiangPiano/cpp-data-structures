#pragma once
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <stdexcept>

/**
 * @brief A generic Disjoint Set Union (DSU), also known as Union-Find.
 *
 * Supports:
 *   - find(x): returns the representative/root of x's set
 *   - unite(a, b): merges the sets containing a and b
 *   - connected(a, b): checks whether a and b are in the same set
 *   - size(x): returns the size of the set containing x
 *
 * Uses:
 *   - Path compression
 *   - Union by size
 *
 * @tparam T The type of the elements stored in the DSU.
 *           Must be hashable if using the generic unordered_map version.
 *
 * Complexity:
 *   - find:       almost O(1), inverse Ackermann
 *   - unite:      almost O(1)
 *   - connected:  almost O(1)
 *   - Space:      O(n)
 *
 * Example:
 * @code
 *   DSU<int> dsu;
 *
 *   dsu.add(1);
 *   dsu.add(2);
 *   dsu.add(3);
 *
 *   dsu.unite(1, 2);
 *
 *   dsu.connected(1, 2); // true
 *   dsu.connected(1, 3); // false
 *   dsu.size(1);         // 2
 * @endcode
 */
template <typename T>
class DSU {
private:
    std::unordered_map<T, T> parent_;
    std::unordered_map<T, size_t> size_;
    size_t components_;

public:
    /**
     * @brief Constructs an empty DSU.
     */
    DSU() : components_(0) {}

    /**
     * @brief Constructs a DSU from a vector of elements.
     *
     * Each element starts in its own separate set.
     */
    DSU(const std::vector<T>& elems) : components_(0) {
        for (const T& elem : elems) add(elem);
    }

    /**
     * @brief Adds a new element as its own set.
     *
     * If the element already exists, nothing happens.
     */
    void add(const T& x) {
        if (parent_.count(x)) return;

        parent_[x] = x;
        size_[x] = 1;
        components_++;
    }

    /**
     * @brief Returns true if the DSU already contains x.
     */
    bool contains(const T& x) const {
        return parent_.count(x) > 0;
    }

    /**
     * @brief Finds the representative/root of x's set.
     *
     * Uses path compression.
     *
     * @throws std::out_of_range if x does not exist in the DSU.
     */
    T find(const T& x) {
        if (!contains(x)) throw std::out_of_range("DSU::find called on element not in DSU");

        if (parent_[x] == x) return x;

        parent_[x] = find(parent_[x]);
        return parent_[x];
    }

    /**
     * @brief Merges the sets containing a and b.
     *
     * @return true if a merge happened,
     *         false if a and b were already in the same set.
     *
     * @throws std::out_of_range if either element does not exist.
     */
    bool unite(const T& a, const T& b) {
        T root_a = find(a);
        T root_b = find(b);

        if (root_a == root_b) return false;

        if (size_[root_a] < size_[root_b]) std::swap(root_a, root_b);

        parent_[root_b] = root_a;
        size_[root_a] += size_[root_b];
        components_--;

        return true;
    }

    /**
     * @brief Returns true if a and b are in the same set.
     *
     * @throws std::out_of_range if either element does not exist.
     */
    bool is_connected(const T& a, const T& b) { return find(a) == find(b); }

    /**
     * @brief Returns the size of the set containing x.
     *
     * @throws std::out_of_range if x does not exist.
     */
    size_t component_size_of(const T& x) {
        T root = find(x);
        return size_[root];
    }

    /**
     * @brief Returns the number of disjoint sets/components.
     */
    size_t component_count() const { return components_; }

    /**
     * @brief Returns the number of elements stored in the DSU.
     */
    size_t size() const { return parent_.size(); }

    /**
     * @brief Returns true if the DSU contains no elements.
     */
    bool empty() const { return parent_.empty(); }
};
