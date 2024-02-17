#include <iterator>
#pragma once

namespace cls08 {

    // fill
    template<typename Iterator, typename T>
    void fill(Iterator begin, Iterator end, const T &value) {
        for (Iterator it = begin; it != end; ++it) {
            *it = value;
        }
    }

    // generate
    template<typename Iterator, typename Generator>
    void generate(Iterator begin, Iterator end, Generator gen) {
        for (Iterator it = begin; it != end; ++it) {
            *it = gen();
        }
    }

    // copy
    template<typename SourceIterator, typename DestinationIterator>
    void copy(SourceIterator srcBegin, SourceIterator srcEnd, DestinationIterator destBegin) {
        while (srcBegin != srcEnd) {
            *destBegin++ = *srcBegin++;
        }
    }

    // for_each
    template<typename Iterator, typename Functor>
    void for_each(Iterator begin, Iterator end, Functor f) {
        for (Iterator it = begin; it != end; ++it) {
            f(*it);
        }
    }

}
