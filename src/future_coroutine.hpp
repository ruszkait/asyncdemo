# pragma once
#include "scope_printer.hpp"

#include <chrono>
#include <coroutine>
#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

namespace {
// Enable the use of std::future<T> as a coroutine type
// by using a std::promise<T> as the promise type.
template <typename T, typename... Args> requires(!std::is_void_v<T> && !std::is_reference_v<T>) 
struct std::coroutine_traits<std::future<T>, Args...>
{
    // The promise type is the type with which the compiler generated code interacts with
    struct promise_type
    {
        // This is the return value of the coroutine - a future
        std::future<T> get_return_object() noexcept { return promise_.get_future(); }

        // When the coroutine is called (instantiated), then we can immediately suspend at the beginning
        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }

        // If co_return is used inside the coroutine code, then this is the place where we store the value into the
        // promise
        void return_value(const T &value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            promise_.set_value(value);
        }

        void return_value(T &&value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            promise_.set_value(std::move(value));
        }

        // If an exception is thrown from the coroutine code, then it is stored here into the promise
        void unhandled_exception() noexcept { promise_.set_exception(std::current_exception()); }

        std::promise<T> promise_;
    };
};

// Same for std::future<void>.
template <typename... Args>
struct std::coroutine_traits<std::future<void>, Args...>
{
    struct promise_type : std::promise<void>
    {
        std::future<void> get_return_object() noexcept { return this->get_future(); }

        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }

        void return_void() noexcept { this->set_value(); }
        void unhandled_exception() noexcept { this->set_exception(std::current_exception()); }
    };
};

// The co_await operator creates an awaitable wrapper over the future
// this awaitable wrapper provides the interface to be able to co_await of the future
template <typename T>
auto operator co_await(std::future<T> future) noexcept requires(!std::is_reference_v<T>)
{
    scope_printer scope(__FUNCTION__);
    struct future_awaiter
    {
        future_awaiter(std::future<T> &&future)
            : future_(std::move(future))
        {
            log_printer(__FUNCTION__) << "Constructor " << this;
        }

        ~future_awaiter() { log_printer(__FUNCTION__) << "Destructor " << this; }

        // If the awaitable (in this case, the future) is already fulfilled
        // then we do not have to suspend, we can proceed to the await_resume
        bool await_ready() const noexcept
        {
            using namespace std::chrono_literals;
            const auto future_was_fulfilled = future_.wait_for(0s) == std::future_status::ready;
            log_printer(__FUNCTION__) << "Awaitable is ready: " << std::boolalpha << future_was_fulfilled;
            return future_was_fulfilled;
        }

        // Called when suspension is necessary
        // Prepares the condition of the resumption -> when the future is fulfilled
        void await_suspend(std::coroutine_handle<> coroutine_handler) const
        {
            scope_printer scope(__FUNCTION__);
            log_printer(__FUNCTION__) << "Preparing for suspension";

            // Start a new thread
            std::thread([this, coroutine_handler] {
                scope_printer scope(__FUNCTION__);

                log_printer(__FUNCTION__) << "Waiting for the future to finish";
                // the thread is going to wait synchronously for the future to be fulfilled
                future_.wait();
                log_printer(__FUNCTION__) << "Future finished";

                // resume the coroutine from waiting, drive the coroutine from this current thread
                // till the next suspension point
                coroutine_handler.resume();
            }).detach();
        }

        // The awaiting is over or was not necessary,
        // return the result of the awaiting - the content of the future
        T await_resume()
        {
            scope_printer scope(__FUNCTION__);
            return future_.get();
        }

        std::future<T> future_;
    };

    return future_awaiter {std::move(future)};
}

}
