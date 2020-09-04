#pragma once

#include <align/common.h>

namespace align::gatoh {

enum class traceback : uint8_t {
  none = 0,
  d_both,
  d_up,
  d_left,
  q_shift,
  p_shift,
  p_up,
  q_left
};

template <typename T>
struct options {
  template <typename F>
  options(F f, int start, int extend)
      : match_score(f)
      , start_score(start)
      , extend_score(extend)
  {
  }

  std::function<int(T const&, T const&)> match_score;
  int start_score;
  int extend_score;
};

template <typename T, typename Iterator>
struct alignment {
  using aligned_element_t = std::variant<T, gap>;
  using traceback_element = std::pair<int, traceback>;

  static const int negative_inf = std::numeric_limits<int>::min();

  alignment(
      options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
      Iterator b_end);

  auto trace_back() const;

  void dump_scores() const;

private:
  options<T> opts_;

  Iterator a_begin_;
  Iterator a_end_;
  Iterator b_begin_;
  Iterator b_end_;

  int rows_;
  int cols_;

  traceback& D(int row, int col) { return D_[row * cols_ + col]; }
  traceback const& D(int row, int col) const { return D_[row * cols_ + col]; }

  traceback& P(int row, int col) { return P_[row * cols_ + col]; }
  traceback const& P(int row, int col) const { return P_[row * cols_ + col]; }

  traceback& Q(int row, int col) { return Q_[row * cols_ + col]; }
  traceback const& Q(int row, int col) const { return Q_[row * cols_ + col]; }

  std::unique_ptr<traceback_element[]> D_;
  std::unique_ptr<traceback_element[]> P_;
  std::unique_ptr<traceback_element[]> Q_;
};

template <typename T, typename Iterator>
alignment(options<T>, Iterator, Iterator, Iterator, Iterator)
    -> alignment<T, Iterator>;

template <typename T, typename Iterator>
alignment<T, Iterator>::alignment(
    options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
    Iterator b_end)
    : opts_(opts)
    , a_begin_(a_begin)
    , a_end_(a_end)
    , b_begin_(b_begin)
    , b_end_(b_end)
    , rows_(std::distance(a_begin, a_end) + 1)
    , cols_(std::distance(b_begin, b_end) + 1)
    , D_(std::make_unique<traceback_element[]>(rows_ * cols_))
    , P_(std::make_unique<traceback_element[]>(rows_ * cols_))
    , Q_(std::make_unique<traceback_element[]>(rows_ * cols_))
{
}

template <typename T, typename Iterator>
auto alignment<T, Iterator>::trace_back() const
{
}

template <typename T, typename Iterator>
void alignment<T, Iterator>::dump_scores() const
{
}

} // namespace align::gatoh
