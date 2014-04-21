#ifndef __RANGE_H__
#define __RANGE_H__

// This implements an encapsulated iterator range, which itself can be used as an
// iterator, e.g.
//
//   std::vector<T> v = ...;
//   std::cout << "length of v = " << v.length() << '\n';
//   for (auto it = range(v.begin(), v.end())); it.is_not_at_end(); ++it) {
//     T &t = *it;
//     ...
//   }
//
// The global `range` function can be used to construct a Range<...> without the
// need to specify the underlying iterator type (the type is inferred).
//
// One reason why this class exists is so that iterator ranges can be used in
// container classes without weakly typing them as std::pair<...> and manually
// doing the iterator dereferencing and incrementing.
template <typename Iterator_, typename T_ = typename Iterator_::value_type> // default param value from std::iterator
class Range {
public:

  typedef Iterator_ Iterator;
  typedef T_ T;

  Range (Iterator const &start, Iterator const &end) : m_start(start), m_end(end) { }
  Range (Range const &r) : m_start(r.m_start), m_end(r.m_end) { }

  Range &operator = (Range const &r) { m_start = r.m_start; m_end = r.m_end; return *this; }

  bool operator == (Range const &r) { return m_start == r.m_start && m_end == r.m_end; }
  bool operator != (Range const &r) { return m_start != r.m_start || m_end != r.m_end; }

  size_t length () const { return m_end - m_start; } // only defined for random-access iterators
  bool is_at_end () const { return m_start == m_end; }
  bool is_not_at_end () const { return m_start != m_end; }
  bool is_empty () const { return m_start == m_end; }
  bool is_nonempty () const { return m_start != m_end; }
  Range &operator ++ () { ++m_start; return *this; }
  T_ &operator * () { return m_start.operator*(); }
  T_ *operator -> () { return m_start.operator->(); }
  Iterator_ start () const { return m_start; }
  Iterator_ end () const { return m_end; }

private:

  Range () = delete; // disallowed

  Iterator m_start;
  Iterator m_end;
};

// specifically for constructing Ranges from stl::iterator types
template <typename Iterator_>
Range<Iterator_,typename Iterator_::value_type> range (Iterator_ const &start, Iterator_ const &end) {
  return Range<Iterator_,typename Iterator_::value_type>(start, end);
}

#endif
