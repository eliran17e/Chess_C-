#ifndef CHESS_EX1_ELIRAN17E_PRIORITYQUEUE_H
#define CHESS_EX1_ELIRAN17E_PRIORITYQUEUE_H

#include <list>
#include <stdexcept>
#include "MyExceptions.h"
/// Comparator for shared pointers to objects with operator* defined.
/// Compares the dereferenced values so that higher scores come first.
template<typename T>
struct MyComparator {
    bool operator()(const T& a, const T& b) const {
        return (*a) > (*b);  //
    }
};

/// A bounded-size priority queue using a sorted std::list.
/// Maintains elements in sorted order with the best at the front.
template<typename T, typename Comparator = MyComparator<T>>
class PriorityQueue {
private:
    std::list<T> data;
    Comparator cmp;
    size_t maxSize = 5;

public:
    void push(const T& item) {
        auto it = data.begin();
        while (it != data.end() && !cmp(item, *it)) {
            ++it;
        }
        data.insert(it, item);

        if (data.size() > maxSize) {
            data.pop_back(); // remove worst (last)
        }
    }

    T poll() {
        if (data.empty()) {
            throw MyExceptions("Queue is empty");
        }
        T top = data.front();
        data.pop_front();
        return top;
    }


    bool empty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }
};

#endif //CHESS_EX1_ELIRAN17E_PRIORITYQUEUE_H