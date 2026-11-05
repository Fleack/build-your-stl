#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <sstream>
#include <type_traits>
#include <vector>

#include <bys/equal.hpp>
#include <bys/test/test.hpp>

namespace
{
using StreamIterator = std::istream_iterator<int>;

struct CountedElement
{
    int value;
    int* comparisons;

    bool operator==(int other) const
    {
        ++*comparisons;
        return value == other;
    }
};

template <typename LeftRange, typename RightRange>
void check_iterator_pair()
{
    LeftRange const left{1, 2, 3};
    LeftRange const empty_left;
    RightRange const right{1, 2, 3};
    RightRange const different_first{9, 2, 3};
    RightRange const different_middle{1, 9, 3};
    RightRange const different_last{1, 2, 9};
    RightRange const shorter{1, 2};
    RightRange const longer{1, 2, 3, 4};
    RightRange const empty_right;

    CHECK(bys::equal(left.begin(), left.end(), right.begin(), right.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), different_first.begin(), different_first.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), different_middle.begin(), different_middle.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), different_last.begin(), different_last.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), shorter.begin(), shorter.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), longer.begin(), longer.end()));
    CHECK(bys::equal(empty_left.begin(), empty_left.end(), empty_right.begin(), empty_right.end()));
    CHECK_FALSE(bys::equal(empty_left.begin(), empty_left.end(), right.begin(), right.end()));
    CHECK_FALSE(bys::equal(left.begin(), left.end(), empty_right.begin(), empty_right.end()));
    CHECK(bys::equal(std::next(left.begin()), left.end(), std::next(right.begin()), right.end()));
    CHECK(bys::equal(left.begin(), std::next(left.begin(), 2), longer.begin(), std::next(longer.begin(), 2)));
}

template <typename LeftRange>
void check_iterator_pairs()
{
    check_iterator_pair<LeftRange, std::forward_list<int>>();
    check_iterator_pair<LeftRange, std::list<int>>();
    check_iterator_pair<LeftRange, std::deque<int>>();
    check_iterator_pair<LeftRange, std::vector<int>>();
}

template <typename Range>
void check_stream_iterator_pair()
{
    auto const check = [](char const* data, Range const& items, bool expected) {
        std::istringstream left(data);
        std::istringstream right(data);
        CHECK_EQ(bys::equal(StreamIterator{left}, StreamIterator{}, items.begin(), items.end()), expected);
        CHECK_EQ(bys::equal(items.begin(), items.end(), StreamIterator{right}, StreamIterator{}), expected);
    };

    Range const items{4, 8, 15};
    Range const empty;
    check("4 8 15", items, true);
    check("9 8 15", items, false);
    check("4 9 15", items, false);
    check("4 8 9", items, false);
    check("4 8", items, false);
    check("4 8 15 16", items, false);
    check("", items, false);
    check("4 8 15", empty, false);
    check("", empty, true);
}
} // namespace

STATIC_CHECK([] {
    int const left[]{1, 2, 3};
    int const right[]{1, 2, 3};
    STATIC_CHECK(std::is_same_v<decltype(bys::equal(left, left + 3, right, right + 3)), bool>,
        "bys::equal must return bool");
    return bys::equal(left, left + 3, right, right + 3);
}(),
    "bys::equal must work in a constant expression");

TEST_CASE("equal.empty")
{
    int items[]{1};
    CHECK(bys::equal(items, items, items, items));
    CHECK_FALSE(bys::equal(items, items, items, items + 1));
    CHECK_FALSE(bys::equal(items, items + 1, items, items));
}

TEST_CASE("equal.respects_bounds")
{
    int const left[]{0, 1, 2, 3, 4};
    int const right[]{9, 1, 2, 3, 8};
    CHECK(bys::equal(left + 1, left + 4, right + 1, right + 4));
}

TEST_CASE("equal.different_elements")
{
    int const left[]{1, 2, 3};
    int const right[]{1, 3, 2};
    CHECK_FALSE(bys::equal(left, left + 3, right, right + 3));
}

TEST_CASE("equal.forward_iterator_pairs")
{
    check_iterator_pairs<std::forward_list<int>>();
}

TEST_CASE("equal.bidirectional_iterator_pairs")
{
    check_iterator_pairs<std::list<int>>();
}

TEST_CASE("equal.random_access_iterator_pairs")
{
    check_iterator_pairs<std::deque<int>>();
}

TEST_CASE("equal.contiguous_iterator_pairs")
{
    check_iterator_pairs<std::vector<int>>();
}

TEST_CASE("equal.input_with_container_iterators")
{
    check_stream_iterator_pair<std::forward_list<int>>();
    check_stream_iterator_pair<std::list<int>>();
    check_stream_iterator_pair<std::deque<int>>();
    check_stream_iterator_pair<std::vector<int>>();
}

TEST_CASE("equal.single_pass")
{
    std::istringstream left("4 8 15");
    std::istringstream right("4 8 15");
    CHECK(bys::equal(StreamIterator{left}, StreamIterator{}, StreamIterator{right}, StreamIterator{}));
}

TEST_CASE("equal.single_pass_different_lengths")
{
    int comparisons = 0;
    CountedElement const items[]{{4, &comparisons}, {8, &comparisons}};
    std::istringstream longer("4 8 15");
    std::istringstream shorter("4");

    CHECK_FALSE(bys::equal(items, items + 2, StreamIterator{longer}, StreamIterator{}));
    CHECK(comparisons <= 2);

    comparisons = 0;
    CHECK_FALSE(bys::equal(items, items + 2, StreamIterator{shorter}, StreamIterator{}));
    CHECK(comparisons <= 1);
}

TEST_CASE("equal.comparison_limit")
{
    int comparisons = 0;
    CountedElement const left[]{{1, &comparisons}, {2, &comparisons}, {3, &comparisons}};
    int const right[]{1, 2, 3};

    CHECK(bys::equal(left, left + 3, right, right + 3));
    CHECK(comparisons <= 3);
}

TEST_CASE("equal.random_access_different_lengths")
{
    int comparisons = 0;
    CountedElement const left[]{{1, &comparisons}, {2, &comparisons}, {3, &comparisons}};
    int const right[]{1, 2};

    CHECK_FALSE(bys::equal(left, left + 3, right, right + 2));
    CHECK_EQ(comparisons, 0);
}
