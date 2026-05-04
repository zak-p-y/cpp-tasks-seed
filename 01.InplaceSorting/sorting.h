#ifndef SORTING_H
#define SORTING_H


#include "collvalue.h"

//  Пузырёк
template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end)
{
    auto n = end - begin;
    if (n < 2)
    {
        return;
    }

    for (decltype(n) pass = 0; pass + 1 < n; ++pass)
    {
        bool swapped = false;

        for (decltype(n) i = 0; i + 1 < n - pass; ++i)
        {
            Iterator left = begin + i;
            Iterator right = left + 1;

            if (*right < *left)
            {
                std::swap(*left, *right);
                swapped = true;
            }
        }

        if (!swapped)
        {
            break;
        }
    }
}

//  QuickSort 
template <typename Iterator>
  Iterator partition(Iterator begin, Iterator end)
  {
      Iterator pivot_iter = end - 1;
      Iterator i = begin;

      for (Iterator item = begin; item != pivot_iter; ++item)
      {
          if (*item < *pivot_iter)
          {
              std::swap(*item, *i);
              ++i;
          }
      }

      std::swap(*pivot_iter, *i);
      return i;
  }

  template <typename Iterator>
  void quick_sort(Iterator begin, Iterator end)
  {
      if (end - begin < 2) return;

      Iterator pivot = partition(begin, end);
      quick_sort(begin, pivot);
      quick_sort(pivot + 1, end);
  }

  
#endif // SORTING_H
