/*
    Copyright (c) 2017 Christopher A. Taylor.  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Counter nor the names of its contributors may be
      used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#include "../Counter.h"

#include <iostream>

// Compiler-specific debug break
#if defined(_DEBUG) || defined(DEBUG)
    #define COUNTER_DEBUG
    #if defined(_WIN32)
        #define COUNTER_DEBUG_BREAK() __debugbreak()
    #else
        #define COUNTER_DEBUG_BREAK() __builtin_trap()
    #endif
    #define COUNTER_DEBUG_ASSERT(cond) { if (!(cond)) { COUNTER_DEBUG_BREAK(); } }
#else
    #define COUNTER_DEBUG_BREAK() do {} while (false);
    #define COUNTER_DEBUG_ASSERT(cond) do {} while (false);
#endif


//------------------------------------------------------------------------------
// Tests

#define TEST_CHECK(cond) \
    if (!(cond)) { COUNTER_DEBUG_BREAK(); return false; }

template<class CounterT>
bool TestAssignmentF()
{
    CounterT a(2);
    TEST_CHECK(a == 2);

    CounterT b(a);
    TEST_CHECK(b == 2);

    CounterT c = a;
    TEST_CHECK(c == 2);

    CounterT d = 2;
    TEST_CHECK(d == 2);

    CounterT e = CounterT::kMask;
    TEST_CHECK(e == CounterT::kMask);

    CounterT f = CounterT::kMSB;
    TEST_CHECK(f == CounterT::kMSB);

    return true;
}

bool TestAssignment()
{
    TEST_CHECK(TestAssignmentF<Counter64>());
    TEST_CHECK(TestAssignmentF<Counter48>());
    TEST_CHECK(TestAssignmentF<Counter32>());
    TEST_CHECK(TestAssignmentF<Counter24>());
    TEST_CHECK(TestAssignmentF<Counter16>());
    TEST_CHECK(TestAssignmentF<Counter10>());
    TEST_CHECK(TestAssignmentF<Counter8>());
    TEST_CHECK(TestAssignmentF<Counter4>());
    return true;
}


template<class CounterT>
bool TestIncrementF()
{
    CounterT a = CounterT::kMask - 5;
    const CounterT b = a;

    for (unsigned ii = 0; ii < 5; ++ii)
        ++a;

    TEST_CHECK(a == CounterT::kMask);

    for (unsigned ii = 0; ii < 5; ++ii)
        ++a;
    for (unsigned ii = 0; ii < 10; ++ii)
        --a;

    TEST_CHECK(a == b);

    return true;
}

bool TestIncrement()
{
    TEST_CHECK(TestIncrementF<Counter64>());
    TEST_CHECK(TestIncrementF<Counter48>());
    TEST_CHECK(TestIncrementF<Counter32>());
    TEST_CHECK(TestIncrementF<Counter24>());
    TEST_CHECK(TestIncrementF<Counter16>());
    TEST_CHECK(TestIncrementF<Counter10>());
    TEST_CHECK(TestIncrementF<Counter8>());
    TEST_CHECK(TestIncrementF<Counter4>());
    return true;
}


template<class CounterT>
bool TestConstantsF_NoPadding()
{
    static_assert(CounterT::kBits == sizeof(CounterT::ValueType) * 8, "wrong type");

    CounterT::ValueType x = CounterT::kMask;
    TEST_CHECK((CounterT::ValueType)(x + 1) == 0);
    TEST_CHECK((CounterT::kMask >> (CounterT::kBits - 1)) == 1);
    CounterT::ValueType y = CounterT::kMSB;
    TEST_CHECK((CounterT::ValueType)(y + y) == 0);
    TEST_CHECK((CounterT::kMSB >> (CounterT::kBits - 1)) == 1);
    TEST_CHECK((CounterT::ValueType)(CounterT::kMSB << 1) == 0);
    return true;
}

template<class CounterT>
bool TestConstantsF_Padding()
{
    static_assert(CounterT::kBits != sizeof(CounterT::ValueType) * 8, "wrong type");

    TEST_CHECK((CounterT::kMask >> CounterT::kBits) == 0);
    TEST_CHECK((CounterT::kMask >> (CounterT::kBits - 1)) != 0);
    TEST_CHECK((CounterT::kMSB >> CounterT::kBits) == 0);
    TEST_CHECK((CounterT::kMSB >> (CounterT::kBits - 1)) == 1);
    TEST_CHECK(((CounterT::kMSB << 1) & CounterT::kMask) == 0);
    return true;
}

bool TestConstants()
{
    TEST_CHECK(TestConstantsF_NoPadding<Counter64>());
    TEST_CHECK(TestConstantsF_Padding<Counter48>());
    TEST_CHECK(TestConstantsF_NoPadding<Counter32>());
    TEST_CHECK(TestConstantsF_Padding<Counter24>());
    TEST_CHECK(TestConstantsF_NoPadding<Counter16>());
    TEST_CHECK(TestConstantsF_Padding<Counter10>());
    TEST_CHECK(TestConstantsF_NoPadding<Counter8>());
    TEST_CHECK(TestConstantsF_Padding<Counter4>());
    return true;
}


template<class CounterT>
bool TestComparisonsF()
{
    CounterT a = 2, b = 2, c = 3;
    TEST_CHECK(a == b);
    TEST_CHECK(b != c);
    b = c;
    TEST_CHECK(b == c);
    TEST_CHECK(a != b);

    for (unsigned sep = 0; sep < 3; ++sep)
    {
        a = CounterT::kMask - sep;
        b = 0;
        c = 1 + sep;

        for (unsigned ii = 0; ii < 1000; ++ii)
        {
            TEST_CHECK(a == a);
            TEST_CHECK(b == b);
            TEST_CHECK(c == c);
            TEST_CHECK(a != b);
            TEST_CHECK(a != c);
            TEST_CHECK(b != a);
            TEST_CHECK(b != c);
            TEST_CHECK(c != a);
            TEST_CHECK(c != b);

            TEST_CHECK(a < b);
            TEST_CHECK(a < c);
            TEST_CHECK(b < c);
            TEST_CHECK(a <= b);
            TEST_CHECK(a <= c);
            TEST_CHECK(b <= c);
            TEST_CHECK(a <= a);
            TEST_CHECK(b <= b);
            TEST_CHECK(c <= c);

            TEST_CHECK(b > a);
            TEST_CHECK(c > a);
            TEST_CHECK(c > b);
            TEST_CHECK(b >= a);
            TEST_CHECK(c >= a);
            TEST_CHECK(c >= b);
            TEST_CHECK(a >= a);
            TEST_CHECK(b >= b);
            TEST_CHECK(c >= c);

            a++;
            ++b;
            c++;
        }

        for (unsigned ii = 0; ii < 2000; ++ii)
        {
            TEST_CHECK(a == a);
            TEST_CHECK(b == b);
            TEST_CHECK(c == c);
            TEST_CHECK(a != b);
            TEST_CHECK(a != c);
            TEST_CHECK(b != a);
            TEST_CHECK(b != c);
            TEST_CHECK(c != a);
            TEST_CHECK(c != b);

            TEST_CHECK(a < b);
            TEST_CHECK(a < c);
            TEST_CHECK(b < c);
            TEST_CHECK(a <= b);
            TEST_CHECK(a <= c);
            TEST_CHECK(b <= c);
            TEST_CHECK(a <= a);
            TEST_CHECK(b <= b);
            TEST_CHECK(c <= c);

            TEST_CHECK(b > a);
            TEST_CHECK(c > a);
            TEST_CHECK(c > b);
            TEST_CHECK(b >= a);
            TEST_CHECK(c >= a);
            TEST_CHECK(c >= b);
            TEST_CHECK(a >= a);
            TEST_CHECK(b >= b);
            TEST_CHECK(c >= c);

            a--;
            --b;
            c--;
        }
    }

    return true;
}

bool TestComparisons()
{
    TEST_CHECK(TestComparisonsF<Counter64>());
    TEST_CHECK(TestComparisonsF<Counter48>());
    TEST_CHECK(TestComparisonsF<Counter32>());
    TEST_CHECK(TestComparisonsF<Counter24>());
    TEST_CHECK(TestComparisonsF<Counter16>());
    TEST_CHECK(TestComparisonsF<Counter10>());
    TEST_CHECK(TestComparisonsF<Counter8>());
    TEST_CHECK(TestComparisonsF<Counter4>());
    return true;
}


// Example practical application
bool TestGetTickCount()
{
    // Example from README:
    {
        //Counter32 t = ::GetTickCount();
        Counter32 t = 0xffffff00;
        Counter32 kTimeout = t + 300; // Wait for 300 ms
	    // kTimeout == 0x0000002c here.

        unsigned loops = 0;
        while (t < kTimeout)
        {
		    //::Sleep(10);
		    //t = ::GetTickCount();
		    t += 10;
            ++loops;
        }

        TEST_CHECK(loops == 30);
        TEST_CHECK(t == 0x0000002c);
    }

    //Counter32 t0 = ::GetTickCount();
    Counter32 t0 = 0xffffff00;
    Counter32 kTimeout = t0 + 300;

    Counter32 t = t0;
    unsigned loops = 0;
    do
    {
        ++loops;
        TEST_CHECK(loops <= 30);

        //::Sleep(10);
        t += 10;

    } while (t < kTimeout);

    TEST_CHECK(loops == 30);

    //Counter32 t1 = ::GetTickCount();
    Counter32 t1 = 0x0000002c;

    TEST_CHECK(t == t1);

    // Test the cases where normal integers fall down:

    TEST_CHECK(t1 > t0);
    TEST_CHECK(t1 >= t0);
    TEST_CHECK(t0 < t1);

    // Test some other common ways to check for an elapsed timer:

    TEST_CHECK(t1 - t0 > 299);
    TEST_CHECK(t1 - t0 >= 300);
    TEST_CHECK(t1 - t0 < 301);

    TEST_CHECK(t1 > t0 + 299);
    TEST_CHECK(t1 >= t0 + 300);
    TEST_CHECK(t1 < t0 + 301);

    TEST_CHECK(t1 - 299 > t0);
    TEST_CHECK(t1 - 300 >= t0);
    TEST_CHECK(t1 - 301 < t0);

    return true;
}


template<class LargerT, class SmallerT>
bool TestCompressionF(int bias)
{
    const unsigned range = SmallerT::kMSB << 2;

    for (unsigned original = 0; original < range; ++original)
    {
        LargerT fullOriginal = original;
        SmallerT truncatedOriginal = fullOriginal.Truncate<SmallerT>();

        for (unsigned recent = 0; recent < range; ++recent)
        {
            LargerT fullRecent = recent;
            SmallerT truncatedRecent = fullRecent.Truncate<SmallerT>();

            // Skip cases we cannot handle
            if (recent < original)
            {
                unsigned delta = original - recent;
                if (delta >= SmallerT::kMSB - bias)
                    continue;
            }
            else
            {
                unsigned delta = recent - original;
                if (delta > SmallerT::kMSB + bias)
                    continue;
            }

#if 0
            // Useful for debugging
            int changeNeeded = (int)((fullOriginal.Get() >> Counter4::kBits) - (fullRecent.Get() >> Counter4::kBits));
            if (changeNeeded != 0)
            {
                std::cout << "orig=" << (int)fullOriginal.Get();
                std::cout << ", recent=" << (int)fullRecent.Get();
                std::cout << ", (T)orig=" << (int)truncatedOriginal.Get();
                std::cout << ", (T)recent=" << (int)truncatedRecent.Get();
                std::cout << ", delta=" << (int)(truncatedRecent.Get() - truncatedOriginal.Get());
                std::cout << ", Expected=" << changeNeeded;
                std::cout << std::endl;
            }
#endif

            LargerT expanded = LargerT::ExpandFromTruncated(fullRecent, truncatedOriginal, bias);
            TEST_CHECK(expanded.ToUnsigned() == fullOriginal.ToUnsigned());
        }
    }

    return true;
}


bool TestCompression()
{
    for (int bias = -8; bias <= 7; ++bias)
    {
        TEST_CHECK((TestCompressionF<Counter8, Counter4>(bias)));
        TEST_CHECK((TestCompressionF<Counter10, Counter4>(bias)));
        TEST_CHECK((TestCompressionF<Counter10, Counter8>(bias)));
        TEST_CHECK((TestCompressionF<Counter16, Counter4>(bias)));
        TEST_CHECK((TestCompressionF<Counter16, Counter8>(bias)));
        TEST_CHECK((TestCompressionF<Counter16, Counter10>(bias)));
        TEST_CHECK((TestCompressionF<Counter32, Counter4>(bias)));
        TEST_CHECK((TestCompressionF<Counter32, Counter8>(bias)));
        TEST_CHECK((TestCompressionF<Counter32, Counter10>(bias)));
        TEST_CHECK((TestCompressionF<Counter64, Counter4>(bias)));
        TEST_CHECK((TestCompressionF<Counter64, Counter8>(bias)));
        TEST_CHECK((TestCompressionF<Counter64, Counter10>(bias)));
        //TEST_CHECK((TestCompressionF<Counter64, Counter16>(bias)));
    }
    return true;
}


//------------------------------------------------------------------------------
// Test runner

typedef bool(*TestF)();

struct TestDescriptor
{
    TestF Function = nullptr;
    const char* Name = nullptr;

    TestDescriptor(TestF fn = nullptr, const char* name = nullptr)
        : Function(fn)
        , Name(name)
    {
    }
};

#define LIST_TEST(name) {name, #name}

bool RunTests()
{
    TestDescriptor TestList[] = {
        LIST_TEST(TestAssignment),
        LIST_TEST(TestConstants),
        LIST_TEST(TestIncrement),
        LIST_TEST(TestComparisons),
        LIST_TEST(TestGetTickCount),
        LIST_TEST(TestCompression),
        {}
    };

    for (unsigned i = 0; TestList[i].Function != nullptr; ++i)
    {
        if (!TestList[i].Function())
        {
            std::cout << "Test failed: " << TestList[i].Name << std::endl;
            return false;
        }

        std::cout << "Test passed: " << TestList[i].Name << std::endl;
    }

    return true;
}


//------------------------------------------------------------------------------
// Entrypoint

int main()
{
    if (!RunTests())
    {
        std::cout << "Unit test FAILED!" << std::endl;
    }
    else
    {
        std::cout << "Unit tests all passed." << std::endl;
    }

    return 0;
}
