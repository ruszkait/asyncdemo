#include "scope_printer.hpp"

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

#include <coroutine>
#include <exception>
#include <iostream>
#include <type_traits>
#include <fstream>
#include <memory>
#include <filesystem>
#include <string>
#include <iomanip>

namespace {

    // Enable the use of std::future<T> as a coroutine type
    // by using a std::promise<T> as the promise type.
    template <typename T, typename... Args> requires(!std::is_void_v<T> && !std::is_reference_v<T>)
        struct std::coroutine_traits<boost::future<T>, Args...>
    {
        // The promise type is the type with which the compiler generated code interacts with
        struct promise_type
        {
            // This is the return value of the coroutine - a future
            boost::future<T> get_return_object() noexcept { return promise_.get_future(); }

            // When the coroutine is called (instantiated), then we can immediately suspend at the beginning
            std::suspend_never initial_suspend() const noexcept { return {}; }
            std::suspend_never final_suspend() const noexcept { return {}; }

            // If co_return is used inside the coroutine code, then this is the place where we store the value into the promise
            void return_value(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) { promise_.set_value(value); }
            void return_value(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) { promise_.set_value(std::move(value)); }

            // If an exception is thrown from the coroutine code, then it is stored here into the promise
            void unhandled_exception() noexcept { promise_.set_exception(std::current_exception()); }

            boost::promise<T> promise_;
        };
    };

    // The co_await operator creates an awaitable wrapper over the future
    // this awaitable wrapper provides the interface to be able to co_await of the future
    template <typename T>
    auto operator co_await(boost::future<T> future) noexcept requires(!std::is_reference_v<T>)
    {
        scope_printer scope(__FUNCTION__);
        struct future_awaiter
        {
            future_awaiter(boost::future<T>&& future)
                : future_(std::move(future))
            {
                log_printer(__FUNCTION__) << "Constructor " << this;
            }

            ~future_awaiter()
            {
                log_printer(__FUNCTION__) << "Destructor " << this;
            }

            // If the awaitable (in this case, the future) is already fulfilled
            // then we do not have to suspend, we can proceed to the await_resume
            bool await_ready() const noexcept
            {
                const auto future_was_fulfilled = future_.is_ready();
                log_printer(__FUNCTION__) << "Awaitable is ready: " << std::boolalpha << future_was_fulfilled;
                return future_was_fulfilled;
            }

            // Called when suspension is necessary
            // Prepares the condition of the resumption -> when the future is fulfilled
            void await_suspend(std::coroutine_handle<> coroutine_handler) const
            {
                scope_printer scope(__FUNCTION__);
                log_printer(__FUNCTION__) << "Preparing for suspension";

                //continuation_future_ = std::move(future_.then(boost::launch::async, [this, coroutine_handler](boost::future<T> parent_future) {
                (void)future_.then(boost::launch::async, [this, coroutine_handler](boost::future<T> parent_future) {
                    scope_printer scope(__FUNCTION__);
                    log_printer(__FUNCTION__) << "Resume coroutine";
                    // resume the coroutine from waiting, drive the coroutine from this current thread
                    // till the next suspension point
                    value_ = std::move(parent_future.get());
                    coroutine_handler.resume();
                    });
            }

            // The awaiting is over or was not necessary,
            // return the result of the awaiting - the content of the future
            T await_resume()
            {
                scope_printer scope(__FUNCTION__);
                return std::move(value_);
            }

            mutable T value_;
            mutable boost::future<T> future_;
        };

        return future_awaiter{ std::move(future) };
    }



    boost::future<std::string> read_file_async(std::filesystem::path filename)
    {
        scope_printer scope(__FUNCTION__);

        std::unique_ptr<std::istream> data_stream = co_await boost::async(boost::launch::async, [=] {
                log_printer(__FUNCTION__) << "Opening stream " << filename;
                auto data_stream = std::make_unique<std::ifstream>(filename);
                log_printer(__FUNCTION__)  << "Opened stream " << filename;
                return data_stream;
            });

        log_printer(__FUNCTION__) << "data stream available";

        auto data = co_await boost::async(boost::launch::async, [data_stream = std::move(data_stream)]{
                std::string text;
                text.resize(128);
                log_printer(__FUNCTION__)  << "Reading stream";
                data_stream->read(text.data(), text.size());
                log_printer(__FUNCTION__)  << "Read stream";
                return text;
            });

        log_printer(__FUNCTION__) << "data filled";

        co_return data;
    }

    void read_file()
    {
        separator_printer separator;
        scope_printer scope(__FUNCTION__);
        std::cout << read_file_async("../CMakeLists.txt").get() << '\n';
    }
}


void await_boost_future()
{
    scope_printer scope(__FUNCTION__);

    read_file();
}