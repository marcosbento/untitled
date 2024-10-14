//
// Copyright (c) 2024 Marcos Bento
//
// Distributed under multiple licenses: Apache, MIT, Boost, Unlicense.
//
// See https://github.com/marcosbento/untitled
//

#ifndef UNTITLED_THREAD_POOL_H
#define UNTITLED_THREAD_POOL_H

#include <cstdint>
#include <iostream>
#include <thread>

namespace untitled {

template <typename T>
struct thread_safe_queue {
public:
  thread_safe_queue() : q_{}, m_{}, c_{} {}
  ~thread_safe_queue() {}

  void push(T v) {
    std::lock_guard<std::mutex> lock(m_);

    q_.push(v);
    c_.notify_one();
  }

  bool pop(T& v, bool wait = true) {
    std::unique_lock<std::mutex> lock(m_);

    if (wait) {
      c_.wait(lock, [this] { return !q_.empty() || cancel_; });
    }
    else if (q_.empty()) {
      return false;
    }

    if (q_.empty() && cancel_) {
      // We only really terminate early when the queue is empty
      return false;
    }

    v = q_.front();
    q_.pop();
    return true;
  }

  // Releases all waiting threads, and prevents further waiting
  void cancel() {
    std::unique_lock<std::mutex> lock(m_);

    cancel_ = true;
    c_.notify_all();
  }

  bool empty() {
    std::unique_lock<std::mutex> lock(m_);
    return q_.empty();
  }

private:
  std::queue<T> q_;
  mutable std::mutex m_;
  std::condition_variable c_;
  bool cancel_ = false;
};

class thread_pool {
public:
  using task_t = std::function<void()>;

  explicit thread_pool(size_t num_threads = std::thread::hardware_concurrency()) {
    for (size_t i = 0; i < num_threads; ++i) {
      t_.emplace_back([i, this] {
        while (true) {
          task_t task;
          {
            auto found = q_.pop(task);
            if (!found) {
              break; // n.b. this means the task is done
            }
          }
          task();
        }
      });
    }
  }

  ~thread_pool() { stop(); }

  size_t size() const { return t_.size(); }

  void stop() {
    q_.cancel();
    for (auto& t : t_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

  void submit(task_t task) { q_.push(task); }

private:
  std::vector<std::thread> t_;
  thread_safe_queue<task_t> q_;
};

template <typename T>
class monitor {
public:
  monitor() : m_{}, v_{} {}
  ~monitor() = default;

  template <typename F, typename... Args>
  auto operator()(F f, Args... args) {
    std::lock_guard<std::mutex> lock(m_);
    return f(v_, args...);
  }

  T get() const {
    std::lock_guard<std::mutex> lock(m_);
    return v_;
  }

private:
  mutable std::mutex m_;
  T v_;
};

} // namespace untitled

#endif // UNTITLED_THREAD_POOL_H
