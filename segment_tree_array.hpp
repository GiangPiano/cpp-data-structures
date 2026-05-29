#pragma once
#include <cstddef>
#include <functional>
#include <vector>

/**
 * @brief A generic segment tree backed by a flat array, supporting point updates
 *        and range queries.
 *
 * Stores elements of type T and answers range queries using a binary associative
 * operator. The operator determines what the tree computes — sum, min, max, XOR, etc.
 *
 * Compared to a pointer-based segment tree, this implementation uses a pre-allocated
 * flat array of size 4*n, where children of node i are at 2*i and 2*i+1 (1-indexed).
 * This avoids heap allocations per node and improves cache locality.
 *
 * @tparam T        The element type. Must be default-constructible (T{} is used
 *                  as the identity/empty value).
 * @tparam Operator A callable type with signature `T(T, T)`. Must be:
 *                    - Binary: takes exactly two T arguments
 *                    - Associative: op(a, op(b, c)) == op(op(a, b), c)
 *                  Defaults to std::plus<T> (range sum queries).
 *
 * @note The operator does NOT need to be commutative.
 * @note For correct query results the operator should be associative. Non-associative
 *       operators will compile but produce undefined query behaviour.
 *
 * Complexity:
 *   - Build:  O(n)
 *   - Update: O(log n)
 *   - Query:  O(log n)
 *   - Space:  O(n)
 *
 * Example — range sum (default):
 * @code
 *   std::vector<int> nums = {1, 2, 3, 4, 5};
 *   SegmentTreeArray<int> tree(nums);
 *   tree.query(1, 3);     // returns 9  (2+3+4)
 *   tree.update(2, 10);   // nums[2] = 10
 *   tree.query(1, 3);     // returns 16 (2+10+4)
 * @endcode
 *
 * Example — range max with custom operator:
 * @code
 *   struct MaxOp {
 *       int operator()(int a, int b) const { return std::max(a, b); }
 *   };
 *   SegmentTreeArray<int, MaxOp> tree(nums);
 *   tree.query(0, 4);     // returns max element in [0, 4]
 * @endcode
 */
template <typename T, typename Operator = std::plus<T>>
class SegmentTreeArray {
private:
  std::vector<T> tree;
  size_t size_;
  Operator oper_;

  /**
   * @brief Recursively builds the segment tree from the input elements.
   *
   * @param elems The source elements.
   * @param l     Left bound of the current segment (0-based, inclusive).
   * @param r     Right bound of the current segment (0-based, inclusive).
   * @param cur   Current node index in the flat array (1-indexed).
   */
  void build_tree(const std::vector<T>& elems, int l, int r, int cur) {
    if (l == r) {
      tree[cur] = elems[l];
      return;
    }
    int mid = (l + r) / 2;
    build_tree(elems, l, mid, 2 * cur);
    build_tree(elems, mid + 1, r, 2 * cur + 1);
    tree[cur] = oper_(tree[2 * cur], tree[2 * cur + 1]);
  }

  /**
   * @brief Recursively updates a single element and propagates changes upward.
   *
   * @param l     Left bound of the current segment (0-based, inclusive).
   * @param r     Right bound of the current segment (0-based, inclusive).
   * @param cur   Current node index in the flat array (1-indexed).
   * @param index 0-based index of the element to update.
   * @param val   The new value.
   */
  void update_tree(int l, int r, int cur, int index, T val) {
    if (l == r) {
      tree[cur] = val;
      return;
    }
    int mid = (l + r) / 2;
    if (mid > index) update_tree(l, mid, 2 * cur, index, val);
    else update_tree(mid + 1, r, 2 * cur + 1, index, val);
    tree[cur] = oper_(tree[2 * cur], tree[2 * cur + 1]);
  }

  /**
   * @brief Recursively queries the combined value over [left, right].
   *
   * @param l     Left bound of the current segment (0-based, inclusive).
   * @param r     Right bound of the current segment (0-based, inclusive).
   * @param cur   Current node index in the flat array (1-indexed).
   * @param left  Left bound of the query range (0-based, inclusive).
   * @param right Right bound of the query range (0-based, inclusive).
   * @return      The result of applying Operator across [left, right].
   */
  T query_tree(int l, int r, int cur, int left, int right) {
    if (l == left && r == right) return tree[cur];
    int mid = (l + r) / 2;
    if (mid >= right) return query_tree(l, mid, 2 * cur, left, right);
    if (mid < left) return query_tree(mid + 1, r, 2 * cur + 1, left, right);
    return oper_(query_tree(l, mid, 2 * cur, left, mid),
                 query_tree(mid + 1, r, 2 * cur + 1, mid + 1, right));
  }

public:
  /**
   * @brief Constructs an empty segment tree.
   *
   * Only valid if Operator is default-constructible. Calling update() or
   * query() on an empty tree is undefined behaviour.
   */
  SegmentTreeArray() : size_(0), oper_(Operator{}) {}

  /**
   * @brief Constructs a segment tree from a vector of elements.
   *
   * Allocates a flat array of size 4*n and builds the tree in O(n) time.
   *
   * @param elems The initial elements. If empty, the tree is valid but
   *              update() and query() must not be called.
   * @param oper  The combining operator instance. Only needs to be passed
   *              explicitly for stateful operators (e.g. weighted sum).
   *              Stateless operators (std::plus, MaxOp, etc.) are
   *              default-constructed automatically.
   */
  SegmentTreeArray(const std::vector<T>& elems, Operator oper = Operator{})
      : tree(elems.size() * 4), size_{elems.size()}, oper_{oper} {
    build_tree(elems, 0, size_ - 1, 1);
  }

  /**
   * @brief Updates the element at the given index to a new value.
   *
   * @param index 0-based index of the element to update. Must be < size().
   * @param val   The new value to assign.
   *
   * @note Behaviour is undefined if index >= size() or the tree is empty.
   * @note val is currently typed as int rather than T; implicit conversion
   *       applies for numeric types but this may not work for custom T.
   */
  void update(int index, int val) { update_tree(0, size_ - 1, 1, index, val); }

  /**
   * @brief Queries the combined value over the range [left, right] (inclusive).
   *
   * Returns op(elems[left], op(elems[left+1], ... op(elems[right-1], elems[right]))).
   *
   * @param left  0-based index of the range start. Must be <= right.
   * @param right 0-based index of the range end (inclusive). Must be < size().
   * @return      The result of applying Operator across all elements in [left, right].
   *
   * @note Behaviour is undefined if left > right, right >= size(), or the tree is empty.
   */
  T query(int left, int right) { return query_tree(0, size_ - 1, 1, left, right); }

  /**
   * @brief Returns the number of elements in the tree.
   */
  size_t size() { return size_; }

  /**
   * @brief Returns true if the tree contains no elements.
   */
  bool empty() { return size_ == 0; }
};
