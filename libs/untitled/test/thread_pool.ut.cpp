//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#include "untitled/thread_pool.hpp"

#include <iostream>
#include <random>
#include <string>

#include <boost/test/unit_test.hpp>

static size_t get_number_between(int min, int max) {
  assert(min >= 0);
  assert(max > 0);
  assert(min < max);
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution<> dist{min, max};
  return dist(gen);
}

template <typename F, typename... Args>
static void wait_until(F&& f, Args&&... args) {
  while (!f()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

BOOST_AUTO_TEST_SUITE(t_untitled)
BOOST_AUTO_TEST_SUITE(thread_pool)

BOOST_AUTO_TEST_CASE(can_default_create_thread_pool) {
  untitled::thread_pool pool;
  BOOST_CHECK_EQUAL(pool.size(), std::thread::hardware_concurrency());
}

BOOST_AUTO_TEST_CASE(can_create_thread_pool) {
  auto n_threads = std::thread::hardware_concurrency() + 1;
  untitled::thread_pool pool{n_threads};
  BOOST_CHECK_EQUAL(pool.size(), n_threads);
}

BOOST_AUTO_TEST_CASE(can_do_work_on_thread_pool) {
  size_t n_threads = 2;
  size_t n_tasks   = 8 * 2;

  untitled::thread_pool pool{n_threads};
  BOOST_CHECK_EQUAL(pool.size(), n_threads);

  untitled::monitor<int> m;

  for (int i = 0; i < n_tasks; ++i) {
    pool.submit([]() {
      auto sleep_time = get_number_between(100, 250);
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    });
  }
}

BOOST_AUTO_TEST_CASE(can_sum_vector_on_thread_pool) {

  struct accumulator {
    int sum   = 0;
    int count = 0;
  };

  size_t n_threads = 4;
  untitled::thread_pool pool{n_threads};
  BOOST_CHECK_EQUAL(pool.size(), n_threads);

  size_t n_items  = 10'000'000;
  size_t n_chunks = 80;
  std::vector<int> v(n_items * n_chunks, 1);

  {
    untitled::monitor<accumulator> a;

    for (int i = 0; i < n_chunks; ++i) {
      pool.submit([i, n_items, &v, &a]() {
        size_t lower = i * (n_items);
        size_t upper = lower + n_items - 1;
        int sum      = 0;
        for (size_t j = lower; j <= upper; ++j) {
          sum += v[j];
        }

        a([sum](accumulator& a) {
          a.sum += sum; // Accumulate the sum
          a.count++;    // Count the number of handled chunks
        });
      });
    }

    // Acts as a barrier, and waits until all chunks are handled -- after, all threads in the pool are waiting for more work
    wait_until([&a, n_chunks]() { return a.get().count == n_chunks; });
    BOOST_REQUIRE_EQUAL(a.get().sum, n_items * n_chunks);
  }

  {
    untitled::monitor<accumulator> a;

    for (int i = 0; i < n_chunks; ++i) {
      pool.submit([i, n_items, &v, &a]() {
        size_t lower = i * (n_items);
        size_t upper = lower + n_items - 1;
        int sum      = 0;
        for (size_t j = lower; j <= upper; ++j) {
          sum += v[j];
        }

        a([sum](accumulator& a) {
          a.sum += sum; // Accumulate the sum
          a.count++;    // Count the number of handled chunks
        });
      });
    }

    // Acts as a barrier, and waits until all chunks are handled -- after, all threads in the pool are stopped/gone
    pool.stop();
    BOOST_REQUIRE_EQUAL(a.get().sum, n_items * n_chunks);
  }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
