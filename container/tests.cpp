#include "bucket_storage.hpp"
#include "helpers.h"
#include <type_traits>

#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <utility>

TEST(traits, default_constructor)
{
	ASSERT_TRUE(std::is_default_constructible< bs_sizet_t >());
	ASSERT_TRUE(std::is_default_constructible< bs_string_t >());
	ASSERT_TRUE(std::is_default_constructible< bs_nc_t >());
	ASSERT_TRUE(std::is_default_constructible< bs_co_t >());
}

TEST(traits, copy_constructible)
{
	ASSERT_TRUE(std::is_copy_constructible< bs_sizet_t >());
	ASSERT_TRUE(std::is_copy_constructible< bs_string_t >());
	ASSERT_TRUE(std::is_copy_constructible< bs_nc_t >());
	ASSERT_TRUE(std::is_copy_constructible< bs_co_t >());
}

TEST(traits, move_constructible)
{
	ASSERT_TRUE(std::is_move_constructible< bs_sizet_t >());
	ASSERT_TRUE(std::is_move_constructible< bs_string_t >());
	ASSERT_TRUE(std::is_move_constructible< bs_nc_t >());
	ASSERT_TRUE(std::is_move_constructible< bs_co_t >());
}

TEST(traits, destructible)
{
	ASSERT_TRUE(std::is_destructible< bs_sizet_t >());
	ASSERT_TRUE(std::is_destructible< bs_string_t >());
	ASSERT_TRUE(std::is_destructible< bs_nc_t >());
	ASSERT_TRUE(std::is_destructible< bs_co_t >());
}

TEST(traits, copy_constructors)
{
	static_assert(!noexcept(bs_sizet_t(std::declval< const bs_sizet_t & >())));
	static_assert(!noexcept(bs_string_t(std::declval< const bs_string_t & >())));
	static_assert(!noexcept(bs_nc_t(std::declval< const bs_nc_t & >())));
	static_assert(!noexcept(bs_co_t(std::declval< const bs_co_t & >())));
}

TEST(traits, move_constructors)
{
	static_assert(noexcept(bs_sizet_t(std::declval< bs_sizet_t && >())));
	static_assert(noexcept(bs_string_t(std::declval< bs_string_t && >())));
	static_assert(noexcept(bs_nc_t(std::declval< bs_nc_t && >())));
	static_assert(noexcept(bs_co_t(std::declval< bs_co_t && >())));
}

TEST(traits, iterator)
{
	using it = typename std::iterator_traits< bs_sizet_t::iterator >::iterator_category;
	using cit = typename std::iterator_traits< bs_sizet_t::const_iterator >::iterator_category;

	static_assert(std::is_same_v< it, std::bidirectional_iterator_tag >);
	static_assert(std::is_same_v< cit, std::bidirectional_iterator_tag >);
}

TEST(traits, typedefs)
{
	static_assert(std::is_same_v< bs_sizet_t::value_type, size_t >);
	static_assert(std::is_same_v< bs_string_t::value_type, std::string >);
	static_assert(std::is_same_v< bs_nc_t::value_type, NoCopy >);

	static_assert(std::is_same_v< bs_sizet_t::reference, size_t & >);
	static_assert(std::is_same_v< bs_string_t::reference, std::string & >);

	static_assert(std::is_same_v< bs_nc_t::const_reference, const NoCopy & >);
	static_assert(std::is_same_v< bs_co_t::const_reference, const CountedOperationObject & >);

	static_assert(std::is_same_v< bs_sizet_t::difference_type, std::iterator_traits< bs_sizet_t::const_iterator >::difference_type >);

	static_assert(std::is_same_v< bs_sizet_t::difference_type, std::iterator_traits< bs_sizet_t::iterator >::difference_type >);

	static_assert(std::is_same_v< bs_sizet_t::size_type, size_t >);
	static_assert(std::is_same_v< bs_co_t::size_type, size_t >);
}

TEST(traits, methods)
{
	bs_sizet_t b;
	const bs_sizet_t b_const;

	(void)(bs_sizet_t & (bs_sizet_t::*)(const bs_sizet_t &)) & bs_sizet_t::operator=;
	(void)(bs_sizet_t & (bs_sizet_t::*)(bs_sizet_t &&)) & bs_sizet_t::operator=;

	(void)(bs_sizet_t::iterator(bs_sizet_t::*)(const bs_sizet_t::value_type &)) & bs_sizet_t::insert;
	(void)(bs_sizet_t::iterator(bs_sizet_t::*)(bs_sizet_t::value_type &&)) & bs_sizet_t::insert;

	(void)(bs_sizet_t::iterator(bs_sizet_t::*)(bs_sizet_t::const_iterator)) & bs_sizet_t::erase;

	(void)(bool(bs_sizet_t::*)() const noexcept) & bs_sizet_t::empty;
	static_assert(std::is_same_v< decltype(&bs_sizet_t::empty), bool (bs_sizet_t::*)() const noexcept >);

	(void)(bs_sizet_t::size_type(bs_sizet_t::*)() const noexcept) & bs_sizet_t::size;
	static_assert(std::is_same_v< decltype(&bs_sizet_t::size), bs_sizet_t::size_type (bs_sizet_t::*)() const noexcept >);

	(void)(bs_sizet_t::size_type(bs_sizet_t::*)() const noexcept) & bs_sizet_t::capacity;
	static_assert(std::is_same_v< decltype(&bs_sizet_t::capacity), bs_sizet_t::size_type (bs_sizet_t::*)() const noexcept >);

	(void)(void(bs_sizet_t::*)()) & bs_sizet_t::shrink_to_fit;

	(void)(void(bs_sizet_t::*)()) & bs_sizet_t::clear;

	(void)(void(bs_sizet_t::*)(bs_sizet_t &) noexcept) & bs_sizet_t::swap;
	static_assert(noexcept(b.swap(b)));

	static_assert(std::is_same_v< decltype(b.begin()), bs_sizet_t::iterator >);
	static_assert(noexcept(b.begin()));

	static_assert(std::is_same_v< decltype(b_const.begin()), bs_sizet_t::const_iterator >);
	static_assert(noexcept(b_const.begin()));

	static_assert(std::is_same_v< decltype(b.cbegin()), bs_sizet_t::const_iterator >);
	static_assert(noexcept(b.cbegin()));

	static_assert(std::is_same_v< decltype(b.end()), bs_sizet_t::iterator >);
	static_assert(noexcept(b.end()));

	static_assert(std::is_same_v< decltype(b_const.end()), bs_sizet_t::const_iterator >);
	static_assert(noexcept(b_const.end()));

	static_assert(std::is_same_v< decltype(b.cend()), bs_sizet_t::const_iterator >);
	static_assert(noexcept(b.cend()));

	static_assert(
		std::is_invocable_r_v< bs_sizet_t::iterator, decltype(METHOD(get_to_distance)), bs_sizet_t &, bs_sizet_t::iterator, const bs_sizet_t::difference_type >);
}

TEST(base, ctor)
{
	bs_nc_t b = bs_nc_t(2);
	ASSERT_TRUE(b.empty());
	ASSERT_EQ(b.size(), 0);

	bool throw_detected = false;
	try
	{
		b.insert(NoCopy(1));
		NoCopy c(2);
		b.insert(std::move(c));
	} catch (int)
	{
		throw_detected = true;
	}
	ASSERT_FALSE(throw_detected);
	ASSERT_EQ(b.size(), 2);

	try
	{
		NoCopy c(666);
		b.insert(c);
	} catch (int)
	{
		throw_detected = true;
	}
	ASSERT_TRUE(throw_detected);
	ASSERT_EQ(b.size(), 2);

	b.insert(NoCopy(0));
	ASSERT_EQ(b.size(), 3);

	int v[3] = {};
	for (bs_nc_t::iterator it = b.begin(); it != b.end(); ++it)
		if (it->m_value <= 2 && it->m_value >= 0)
			v[it->m_value]++;

	EXPECT_EQ(v[0], 1);
	EXPECT_EQ(v[1], 1);
	EXPECT_EQ(v[2], 1);
}

TEST(base, empty)
{
	bs_co_t b = bs_co_t();
	EXPECT_TRUE(b.empty());
	ASSERT_EQ(b.size(), 0);
	ASSERT_EQ(b.capacity(), 0);
	ASSERT_EQ(b.begin(), b.end());
	ASSERT_EQ(opCount.creationCount, 0);

	CountedOperationObject el = CountedOperationObject(1);
	b.insert(el);
	EXPECT_FALSE(b.empty());
	ASSERT_EQ(b.size(), 1);
	ASSERT_EQ(b.capacity(), 64);
	ASSERT_NE(b.begin(), b.end());
	ASSERT_EQ(opCount.creationCount, 1);
}

TEST(base, insert)
{
	constexpr size_t n = 1000;
	bs_sizet_t b = bs_sizet_t();

	for (size_t i = 0; i < n / 2; ++i)
	{
		b.insert(i);
		ASSERT_EQ(b.size(), i + 1);
		ASSERT_EQ(b.capacity(), (i + 64) & -64);
	}

	for (size_t i = n / 2; i < n; ++i)
	{
		b.insert(std::move(i));
		ASSERT_EQ(b.size(), i + 1);
		ASSERT_EQ(b.capacity(), (i + 64) & -64);
	}

	for (size_t i = 0; i < n; ++i)
	{
		bs_sizet_t::iterator it = std::find(b.begin(), b.end(), i);
		ASSERT_TRUE(*it == i);
	}

	size_t v[n] = {};
	for (size_t i = 0; i < n; ++i)
	{
		bs_sizet_t::iterator e = b.get_to_distance(b.begin(), i);
		ASSERT_TRUE(*e < n);
		v[*e]++;
	}

	for (size_t i = 0; i < n; ++i)
		ASSERT_EQ(v[i], 1);

	for (size_t i = 0; i < n; ++i)
	{
		bs_sizet_t::iterator e = b.get_to_distance(b.end(), -1 - i);
		ASSERT_TRUE(*e < n);
		v[*e]++;
	}

	for (size_t i = 0; i < n; ++i)
		ASSERT_EQ(v[i], 2);

	bs_sizet_t::iterator it = b.end()--;
	do
	{
		--it;
		ASSERT_TRUE(*it < n);
		v[*it]++;
	} while (it != b.begin());

	for (size_t i = 0; i < n; ++i)
		ASSERT_EQ(v[i], 3);
}

TEST(base, erase)
{
	bs_co_t b = prepare();
	size_t n = b.size();
	for (size_t i = 0; i < n; ++i)
	{
		ASSERT_EQ(b.size(), n - i);
		b.erase(b.begin());
	}

	EXPECT_EQ(b.size(), 0);
	ASSERT_TRUE(b.empty());
	ASSERT_EQ(opCount.dtorCount, n);
}

TEST(base, shrink_to_fit)
{
	bs_sizet_t b = bs_sizet_t();

	size_t n_insert = 192;
	size_t n_erase = 90;
	size_t n_end_erase = n_insert - 40;

	for (size_t i = 0; i < n_insert; ++i)
		b.insert(i);
	ASSERT_EQ(b.capacity(), n_insert);

	for (size_t i = 0; i < n_erase; ++i)
	{
		bs_sizet_t::iterator it = std::find(b.begin(), b.end(), i);
		b.erase(it);
	}
	ASSERT_EQ(b.capacity(), n_insert - 64);

	for (size_t i = n_end_erase; i < n_insert; ++i)
	{
		bs_sizet_t::iterator it = std::find(b.begin(), b.end(), i);
		b.erase(it);
	}
	ASSERT_EQ(b.capacity(), n_insert - 64);

	b.shrink_to_fit();
	ASSERT_EQ(b.size(), n_end_erase - n_erase);
	ASSERT_EQ(b.capacity(), 64);

	for (size_t i = n_erase; i < n_end_erase; ++i)
	{
		bs_sizet_t::iterator it = std::find(b.begin(), b.end(), i);
		ASSERT_EQ(*it, i);
	}
}

TEST(base, clear)
{
	bs_co_t b = prepare();
	size_t n = b.size();
	b.clear();

	ASSERT_EQ(opCount, OpCount(0, 0, 0, 0, 0, n));
	EXPECT_TRUE(b.empty());
	ASSERT_EQ(b.size(), 0);
	ASSERT_EQ(b.begin(), b.end());
}

TEST(base, iterating)
{
	bs_co_t b = prepare();
	size_t n = b.size();
	size_t counter = 0;
	size_t sum = 0;
	for (CountedOperationObject &i : b)
	{
		sum += i.number;
		counter++;
	}

	ASSERT_EQ(sum, (n) * (n / 2) - (n % 2 == 0 ? (n / 2) : 0));
	ASSERT_EQ(counter, b.size());
}

TEST(base, swap)
{
	bs_co_t b = prepare();
	size_t n = b.size();

	bs_co_t c = bs_co_t();
	c.insert(CountedOperationObject{ 1 });
	std::swap(b, c);

	ASSERT_EQ(b.size(), 1);
	ASSERT_EQ(c.size(), n);

	bs_co_t e = bs_co_t();
	e.insert(CountedOperationObject{ 1 });
	c.swap(e);

	ASSERT_EQ(c.size(), 1);
	ASSERT_EQ(e.size(), n);
}

TEST(coperators, simple_five_rule_count)
{
	bs_co_t b = prepare();
	size_t n = b.size();

	bs_co_t c = b;
	ASSERT_EQ(opCount, OpCount(0, n, 0, 0, 0, 0));

	opCount.clearCounters();
	bs_co_t d = std::move(b);
	ASSERT_EQ(opCount, OpCount(0, 0, 0, 0, 0, 0));

	bs_co_t def;
	for (size_t i = 0; i < n / 2; i++)
		def.insert(CountedOperationObject(i));

	opCount.clearCounters();
	c = def;
	ASSERT_EQ(opCount, OpCount(0, n / 2, 0, 0, 0, n));

	opCount.clearCounters();
	d = std::move(def);
	ASSERT_EQ(opCount, OpCount(0, 0, 0, 0, 0, n));

	bs_co_t def_move;
	CountedOperationObject co(0);
	opCount.clearCounters();
	for (size_t i = 0; i < n; i++)
	{
		co = CountedOperationObject(i);
		def_move.insert(std::move(co));
	}

	ASSERT_EQ(opCount, OpCount(n, 0, n, 0, n, n));
}

TEST(coperators, with_self)
{
	bs_co_t b = prepare();
	bs_co_t *volatile pb = &b;
	b = *pb;
	ASSERT_EQ(opCount, NO_OP);

	opCount.clearCounters();
	b = std::move(*pb);
	ASSERT_EQ(opCount, NO_OP);
}

TEST(iterators, iter_const_eq)
{
	bs_co_t b = prepare();
	bs_co_t::iterator i = b.begin();
	bs_co_t::const_iterator j = b.cbegin();

	for (; i != b.cend() && j != b.end(); i++, j++)
		ASSERT_EQ(i, j);
	ASSERT_EQ(i, j);
}

TEST(iterators, comparision_lesseq)
{
	bs_co_t b = prepare();
	for (bs_co_t::iterator it = b.begin(); it < b.end(); it++)
		for (bs_co_t::iterator jt = it; jt < b.end(); jt++)
			ASSERT_TRUE(it <= jt);
}

TEST(iterators, comparision_lessge)
{
	bs_co_t b = prepare();
	for (bs_co_t::iterator it = b.begin(); it < b.end(); it++)
		for (bs_co_t::iterator jt = it; jt < b.end(); jt++)
			ASSERT_TRUE(jt >= it);
}

TEST(iterators, member_of_pointer)
{
	bs_string_t b = bs_string_t();
	for (size_t i = 0; i < 100; ++i)
	{
		std::string str = std::to_string(i);
		bs_string_t::iterator it = b.insert(str);
		ASSERT_EQ(b.size(), i + 1);

		EXPECT_EQ(it->size(), str.size());
		ASSERT_EQ((*it).size(), str.size());
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
