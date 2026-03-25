#pragma once

#include <coroutine>
#include <exception>

namespace win_ai_agent::nexus_pro {

template <typename T>
class Task {
 public:
  struct promise_type {
    T value_{};
    std::exception_ptr error_{};
    Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_value(T value) { value_ = std::move(value); }
    void unhandled_exception() { error_ = std::current_exception(); }
  };

  explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
  Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = {}; }
  ~Task() { if (handle_) handle_.destroy(); }

  T Get() {
    if (handle_.promise().error_) std::rethrow_exception(handle_.promise().error_);
    return std::move(handle_.promise().value_);
  }

 private:
  std::coroutine_handle<promise_type> handle_{};
};

}  // namespace win_ai_agent::nexus_pro
