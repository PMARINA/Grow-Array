#include <string.h>

#include <chrono>
#include <cstdint>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

typedef int internal_type;
typedef uint64_t index_type;
typedef uint64_t size_type;
// Assuming n = number of elements in array...
class GrowArray {
 private:
  size_type num_elements;
  size_type capacity;
  internal_type* data;
  // O(n) -- moving all the elements is linear time.
  void resize(size_type new_capacity = 0, size_type offset = 0) {
    internal_type* data_old = data;
    capacity = new_capacity <= (capacity << 1)
                   ? (capacity << 1) + 1 + offset  // ignore the given
                   : new_capacity;                 // capacity this saves 1
                                                   // check every time you
                                                   // resize, specifically,
                                                   // is capacity 0
    data = new internal_type[capacity];
    memmove(data + offset, data_old, sizeof(internal_type) * num_elements);
    delete[] data_old;
  }
  // Worst case - O(n) from resize, best case: O(1) for comparison
  void before_add() {
    if (num_elements == capacity) {
      resize();
    } else if (num_elements > capacity) {
      resize();
      cout << "BROKEN: SIZE > CAPACITY" << endl;
    }
  }

 public:
  // O(1) Constant time -- memory reserved is just a single operation.
  // Either the OS gives it to you, or it doesn't have it and kills the
  // program
  GrowArray(size_type capacity = 0) : capacity(capacity), num_elements(0) {
    data = new internal_type[capacity];
  }
  // Worst case:O(n). Best case, O(1). Depends on where you're inserting
  // the element
  void insertAt(index_type position, internal_type element) {
    if (position >= num_elements) {
      if (num_elements >= capacity || position + 1 >= capacity) {
        resize((position + 1) * 2);
      }
      data[position] = element;
      num_elements = position + 1;
    } else {
      before_add();
      memmove(position + 1 + data, position + data,
              sizeof(internal_type) * (num_elements++ - position));
      data[position] = element;
    }
  }
  // O(n) worst case, O(1) best case (if need to resize, n, else 1)
  // On average, logn because resize happens log2(n)
  void addEnd(internal_type element) {
    before_add();
    data[num_elements++] = element;
  }
  // O(n) -- copy everything over 1 to the right
  void addStart(internal_type element) {
    if (num_elements++ >= capacity) {
      resize(0, 1);
    } else {
      memmove(data + 1, data, sizeof(internal_type) * num_elements);
    }
    data[0] = element;
  }
  // O(n)
  internal_type removeStart() {
    internal_type ret = data[0];
    memmove(data, data + 1, sizeof(internal_type) * --num_elements);
    return ret;
  }
  // O(1)
  internal_type removeEnd() { return data[--num_elements]; }
  // O(n)
  internal_type removeFromPosition(size_type position) {
    if (position < num_elements) {
      internal_type ret = data[position];
      memmove(data + position, data + position + 1, --num_elements - position);
      return ret;
    } else {
      return 0;
    }
  }
  // O(1)
  size_type size() { return num_elements; }
  // O(1)
  internal_type at(size_type i) { return i < num_elements ? data[i] : 0; }
};

int main(int argc, char** argv) {
  internal_type num_to_end, num_to_start, location;
  if (argc > 3) {
    num_to_end = atoi(argv[1]);
    num_to_start = atoi(argv[2]);
    location = atoi(argv[3]);
  } else {
    cout << "No console input received, please enter the three numbers now, "
            "separated by spaces"
         << endl;
    cin >> num_to_end >> num_to_start >> location;
  }
  // O(1)
  GrowArray ga = GrowArray();
  cout << "Starting" << endl;
  auto start = std::chrono::high_resolution_clock::now();
  // O(nlogn), assuming n = num_to_end
  for (internal_type i = 1; i <= num_to_end; i++) {
    ga.addEnd(i);  // n times...
  }
  // O(nx), n = number of elements in array, x = num_to_start...
  // tends towards O(n^2)
  for (internal_type i = 1; i <= num_to_start; i++) {
    ga.addStart(i);  // each takes O(n) given n = num elements in array
  }
  // n = number of elements in array, x = num to start
  // O((n-5000)x) = O(nx - 5000x) -> O(nx)
  // O(nx) tends towards O(n^2)
  for (internal_type i = 1; i <= num_to_start; i++) {
    ga.insertAt(location, i);
  }
  // O(n), n = num_to_end
  for (internal_type i = 1; i <= num_to_end; i++) {
    ga.removeEnd();  // O(1)
  }
  // O(n*x) tends towards O(n^2)
  for (internal_type i = 1; i <= num_to_start; i++) {
    ga.removeStart();  // O(n)
  }
  internal_type sum = 0;
  // O(n), n = size
  for (internal_type i = 0; i < ga.size(); i++) {
    sum += ga.at(i);  // O(1)
  }
  auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - start)
                        .count();
  cout << "Sum was " << sum << " in " << time_taken << " ms" << endl;
}
// Time: 8.830 seconds for 10000000 1000 5000.