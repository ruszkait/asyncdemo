#include "scope_printer.hpp"

#include <boost/thread/future.hpp>

#include <future>

namespace
{
    void discarded_future()
    {
        separator_printer separator;
        scope_printer scope(__FUNCTION__);

        (void)std::async(std::launch::async, []
        {
            log_printer(__FUNCTION__) << "calculating";
            return 42;
        });

        // even though the future was discarded, it waits in its destructor for its completion
        log_printer(__FUNCTION__) << "future discarded";
    }

    void waited_future()
    {
        separator_printer separator;
        scope_printer scope(__FUNCTION__);

        auto result_future = std::async(std::launch::async, []
        {
            log_printer(__FUNCTION__) << "calculating";
            return 42;
        });

        log_printer(__FUNCTION__) << "future result: " << result_future.get();
    }

    void continued_future()
    {
        separator_printer separator;
        scope_printer scope(__FUNCTION__);

        auto result_future = boost::async(boost::launch::async, []
        {
            log_printer(__FUNCTION__) << "calculating";
            return 42;
        });

        auto final_result_future = result_future.then([](auto parent_future)
        {
            log_printer(__FUNCTION__) << "doubling";
            return parent_future.get() * 2;
        });

        log_printer(__FUNCTION__) << "final result: " << final_result_future.get();
    }
}


void std_async()
{
    scope_printer scope(__FUNCTION__);

    discarded_future();
    waited_future();
    continued_future();
}
