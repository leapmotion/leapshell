#ifndef __RANGE_H__
#define __RANGE_H__

/// @brief A range [start, end) of iterators.
/// @details This implements an encapsulated iterator range, which itself can be used as an
/// iterator, e.g.
///
/// @code{.cpp}
///   std::vector<T> v = ...;
///   std::cout << "length of v = " << v.length() << '\n';
///   for (auto it = range(v.begin(), v.end())); it.is_not_at_end(); ++it) {
///     T &t = *it;
///     ...
///   }
/// @endcode
///
/// The global `range` function can be used to construct a Range<...> without the
/// need to specify the underlying iterator type (the type is inferred).
///
/// One reason why this class exists is so that iterator ranges can be used in
/// container classes without weakly typing them as std::pair<...> and manually
/// doing the iterator dereferencing and incrementing.
template <typename Iterator_, typename T_ = typename Iterator_::value_type> // default param value from std::iterator
class Range {
public:

  typedef Iterator_ Iterator;
  typedef T_ T;

  /// @brief Construct a range [start, end).
  /// @details The "start" element is included in the range, but the "end" element isn't.
  Range (Iterator const &start, Iterator const &end) : m_start(start), m_end(end) { }
  /// @brief Copy constructor.  Does the obvious thing.
  Range (Range const &r) : m_start(r.m_start), m_end(r.m_end) { }

  /// @brief Assignment operator.  Does the obvious thing.
  Range &operator = (Range const &r) { m_start = r.m_start; m_end = r.m_end; return *this; }

  /// @brief Equality operator.  Does the obvious thing.
  bool operator == (Range const &r) const { return m_start == r.m_start && m_end == r.m_end; }
  /// @brief Inequality operator.  Does the obvious thing.
  bool operator != (Range const &r) const { return m_start != r.m_start || m_end != r.m_end; }

  /// @brief Returns the number of elements in the range.
  /// @details This method is only well-defined if the iterator type is a random-access iterator.
  size_t length () const { return m_end - m_start; }
  /// @brief A synonym for @see is_empty which is useful when using a Range as an iterator.
  bool is_at_end () const { return m_start == m_end; }
  /// @brief A synonym for @see is_nonempty which is useful when using a Range as an iterator.
  bool is_not_at_end () const { return m_start != m_end; }
  /// @brief Returns true if and only if the range is empty (more specifically, if start == end).
  bool is_empty () const { return m_start == m_end; }
  /// @brief Returns true if and only if the range is nonempty (more specifically, if start != end).
  bool is_nonempty () const { return m_start != m_end; }
  /// @brief Increment operator for using a Range as an iterator.
  /// @details Increments the "start" element, potentially changing the return value of @see is_empty.
  Range &operator ++ () { ++m_start; return *this; }
  /// @brief Dereference the "start" element, for use of a Range as an iterator.
  T_ &operator * () const { return m_start.operator*(); }
  /// @brief Dereference the "start" element with pointer semantics, for use of a Range as an iterator.
  T_ *operator -> () const { return m_start.operator->(); }
  /// @brief Returns the iterator for the "start" element.
  Iterator_ start () const { return m_start; }
  /// @brief Returns the iterator for the "end" element.
  /// @details Because the "end" element is not included in the defined range, this may be the
  /// end iterator for a container, e.g. an std::vector.
  Iterator_ end () const { return m_end; }

private:

  /// @brief Disallow ill-defined Ranges.
  Range() { }

  Iterator m_start;
  Iterator m_end;
};

/// @brief Convenience function for constructing a Range from std::iterator types.
/// @details The element type of the Range is defined to be the value_type typedef in Iterator_.
template <typename Iterator_>
Range<Iterator_,typename Iterator_::value_type> range (Iterator_ const &start, Iterator_ const &end) {
  return Range<Iterator_,typename Iterator_::value_type>(start, end);
}

#endif
