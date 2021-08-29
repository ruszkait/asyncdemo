#include "scope_printer.hpp"
#include "boost_future_coroutine.hpp"

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

namespace
{
    boost::future<std::string> read_file_async(std::filesystem::path filename)
    {
        scope_printer scope(__FUNCTION__);

        std::unique_ptr<std::istream> data_stream = co_await boost::async(boost::launch::async, [=]
        {
            log_printer(__FUNCTION__) << "Opening stream " << filename;
            auto data_stream = std::make_unique<std::ifstream>(filename);
            log_printer(__FUNCTION__) << "Opened stream " << filename;
            return data_stream;
        });

        log_printer(__FUNCTION__) << "data stream available";

        auto data = co_await boost::async(boost::launch::async, [data_stream = std::move(data_stream)]
        {
            std::string text;
            text.resize(128);
            log_printer(__FUNCTION__) << "Reading stream";
            data_stream->read(text.data(), text.size());
            log_printer(__FUNCTION__) << "Read stream";
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
