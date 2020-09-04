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

  int gap_score(int length) const
  {
    return opts_.start_score + opts_.extend_score * length;
  }

  void dump_scores(std::unique_ptr<traceback_element[]> const&) const;

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
  using elt = alignment<T, Iterator>::aligned_element_t;

  auto a_align = std::vector<elt> {};
  auto b_align = std::vector<elt> {};

  return std::pair {a_align, b_align};
}

template <typename T, typename Iterator>
void alignment<T, Iterator>::dump_scores() const
{
  std::cout << "D\n";
  dump_scores(D_);

  std::cout << "P\n";
  dump_scores(P_);

  std::cout << "Q\n";
  dump_scores(Q_);
}

template <typename T, typename Iterator>
void alignment<T, Iterator>::dump_scores(
    std::unique_ptr<typename alignment<T, Iterator>::traceback_element[]> const&
        data) const
{
  auto score_at = [&data, this](auto row, auto col) {
    return data[row * cols_ + col].first;
  };

  auto compare_tb
      = [](auto const& a, auto const& b) { return a.first < b.first; };

  constexpr bool use_chars = std::is_same_v<char, T>;

  auto begin = data.get();
  auto end = data.get() + (rows_ * cols_);

  auto max = std::max_element(begin, end, compare_tb)->first;
  auto min = std::min_element(begin, end, compare_tb)->first;

  auto elt_width
      = std::max({std::to_string(max).size(), std::to_string(min).size()}) + 1;

  std::cout << std::right;

  if constexpr (use_chars) {
    std::cout << "  " << std::setw(elt_width) << ' ' << ' ';

    for (auto col = 1; col < cols_; ++col) {
      std::cout << std::setw(elt_width) << *(b_begin_ + (col - 1)) << ' ';
    }

    std::cout << '\n';
  }

  for (auto row = 0; row < rows_; ++row) {
    if constexpr (use_chars) {
      std::cout << (row > 0 ? *(a_begin_ + (row - 1)) : ' ') << ' ';
    }

    for (auto col = 0; col < cols_; ++col) {
      std::cout << std::setw(elt_width) << score_at(row, col) << ' ';
    }

    std::cout << '\n';
  }

  std::cout << "\n\n";
}

} // namespace align::gatoh
