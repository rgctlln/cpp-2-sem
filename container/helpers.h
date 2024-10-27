#include "bucket_storage.hpp"

#include <exception>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

class NoCopy
{
  public:
	int m_value;
	NoCopy(int value) noexcept : m_value(value) {}
	NoCopy(NoCopy &&) = default;
	NoCopy(const NoCopy &) { throw -1; }

	NoCopy &operator=(NoCopy &&) = default;
	NoCopy &operator=(const NoCopy &) { throw -2; }
};

class OpCount
{
  public:
	OpCount() = default;
	size_t creationCount = 0;
	size_t ctorCount = 0;
	size_t mtorCount = 0;
	size_t copCount = 0;
	size_t mopCount = 0;
	size_t dtorCount = 0;
	OpCount(size_t creation_count, size_t ctor_count, size_t mtor_count, size_t cop_count, size_t mop_count, size_t dtor_count) :
		creationCount(creation_count), ctorCount(ctor_count), mtorCount(mtor_count), copCount(cop_count),
		mopCount(mop_count), dtorCount(dtor_count)
	{
	}
	bool operator==(const OpCount &rhs) const
	{
		return creationCount == rhs.creationCount && ctorCount == rhs.ctorCount && mtorCount == rhs.mtorCount &&
			   copCount == rhs.copCount && mopCount == rhs.mopCount && dtorCount == rhs.dtorCount;
	}
	void clearCounters()
	{
		creationCount = 0;
		ctorCount = 0;
		mtorCount = 0;
		copCount = 0;
		mopCount = 0;
		dtorCount = 0;
	}
	friend std::ostream &operator<<(std::ostream &os, const OpCount &stat)
	{
		os << "creationCount: " << stat.creationCount << " ctorCount: " << stat.ctorCount << " mtorCount: " << stat.mtorCount
		   << " copCount: " << stat.copCount << " mopCount: " << stat.mopCount << " dtorCount: " << stat.dtorCount;
		return os;
	}
};

OpCount opCount;
const OpCount NO_OP = OpCount(0, 0, 0, 0, 0, 0);

class CountedOperationObject
{
  public:
	size_t number;
	explicit CountedOperationObject(size_t number) noexcept : number(number) { opCount.creationCount++; }
	CountedOperationObject(const CountedOperationObject &oth) noexcept : number(oth.number) { opCount.ctorCount++; }
	CountedOperationObject(CountedOperationObject &&oth) noexcept : number(oth.number) { opCount.mtorCount++; }
	CountedOperationObject &operator=(const CountedOperationObject &) noexcept
	{
		opCount.copCount++;
		return *this;
	};
	CountedOperationObject &operator=(CountedOperationObject &&) noexcept
	{
		opCount.mopCount++;
		return *this;
	};
	~CountedOperationObject() noexcept { opCount.dtorCount++; }
	bool operator==(const CountedOperationObject &rhs) const { return number == rhs.number; }
};

BucketStorage< CountedOperationObject > prepare()
{
	size_t n = 1000;
	auto b = BucketStorage< CountedOperationObject >();
	for (size_t i = 0; i < n; ++i)
	{
		b.insert(CountedOperationObject(i));
	}
	opCount.clearCounters();
	return b;
}

#define RETURNS(...)                                                                                                   \
	noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__)                                                             \
	{                                                                                                                  \
		return __VA_ARGS__;                                                                                            \
	}

#define METHOD(...) [](auto &&self, auto &&...args) RETURNS(decltype(self)(self).__VA_ARGS__(decltype(args)(args)...))

using bs_sizet_t = BucketStorage< size_t >;
using bs_string_t = BucketStorage< std::string >;
using bs_nc_t = BucketStorage< NoCopy >;
using bs_co_t = BucketStorage< CountedOperationObject >;
