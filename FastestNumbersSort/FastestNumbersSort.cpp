// Самого лучшего алгоритма сортировки нет, алгоритм подбирается под заданные
// ограничения, исходный массив может быть уже отсортирован в прямом или
// обратном порядке или же может быть со случайным порядком чисел, так же выбор
// алгоритма зависит от размера массива, количества сравнений и количества
// перестановок, количества доступной памяти, а так же устройства на котором
// выполняется алгоритм. Одними из самых быстрых алгоритмов сортировки чисел
// являются: быстрая сортировка (Quick Sort), хоть и быстрая и простая, но
// основана на сравнениях и не может выполняться быстрее чем (O(n log n)),
// поразрядная сортировка (Radix Sort), которая не выполняет сравнений и может
// выполняться за линейное время, сортировка вставками (Insertion Sort).
// Insertion Sort (O(n^2)) справится лучше с небольшими массивами, чем Quick
// Sort, Quick Sort (O(n log n)) справится лучше с большими массивами, а Radix
// Sort (O(k * n)) справится лучше с очень большими массивами.

#include <algorithm>
#include <vector>

//*************************** Radix Sort **************************************

void countSort(std::vector<int>& values, const int exp)
{
    int outSize = values.size();
    std::vector<int> output(outSize);
    int i, count[10]{};

    for (i = 0; i < outSize; i++)
        count[(values[i] / exp) % 10]++;

    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];

    for (i = outSize - 1; i >= 0; i--) {
        output[count[(values[i] / exp) % 10] - 1] = values[i];
        count[(values[i] / exp) % 10]--;
    }

    values = std::move(output);
}

void radixsort(std::vector<int>& values)
{
    int maxNumber = *(std::max_element(values.begin(), values.end()));

    for (int exp = 1; maxNumber / exp > 0; exp *= 10)
        countSort(values, exp);
}

//*************************** Quick Sort **************************************

int Partition(std::vector<int>& values, int startIdx, int endIdx)
{
  int pivot = values[endIdx];
  int less = startIdx;

  for (int i = startIdx; i < endIdx; ++i) {
    if (values[i] <= pivot) {
      std::swap(values[i], values[less]);
      ++less;
    }
  }
  std::swap(values[less], values[endIdx]);
  return less;
}

void QuickSortImpl(std::vector<int>& values, int startIdx, int endIdx)
{
  if (startIdx < endIdx) {
    int pivot = Partition(values, startIdx, endIdx);
    QuickSortImpl(values, startIdx, pivot - 1);
    QuickSortImpl(values, pivot + 1, endIdx);
  }
}

void QuickSort(std::vector<int>& values)
{
  if (!values.empty()) {
    QuickSortImpl(values, 0, values.size() - 1);
  }
}

//*************************** Insertion Sort **********************************

void InsertionSort(std::vector<int>& values)
{
  for (size_t i = 1; i < values.size(); ++i)
  {
    int x = values[i];
    size_t j = i;
    while ((j > 0) && (values[j - 1] > x))
    {
      values[j] = values[j - 1];
      --j;
    }
    values[j] = x;
  }
}
