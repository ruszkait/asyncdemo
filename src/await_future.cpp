#include "scope_printer.hpp"
#include "future_coroutine.hpp"

#include <future>
#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

std::future<std::string> read_file_async(std::filesystem::path filename)
{
    scope_printer scope(__FUNCTION__);

    std::unique_ptr<std::istream> data_stream = co_await std::async(std::launch::async, [=] {
        log_printer(__FUNCTION__) << "Opening stream " << filename;
        auto data_stream = std::make_unique<std::ifstream>(filename);
        log_printer(__FUNCTION__) << "Stream opened " << filename;
        return data_stream;
    });

    auto data = co_await std::async(std::launch::async, [data_stream = std::move(data_stream)] {
        std::string text;
        text.resize(128);
        log_printer(__FUNCTION__) << "Reading stream";
        data_stream->read(text.data(), text.size());
        log_printer(__FUNCTION__) << "Read stream";
        return text;
    });

    co_return data;
}

void read_file()
{
    separator_printer separator;
    scope_printer scope(__FUNCTION__);
    std::cout << read_file_async("../CMakeLists.txt").get() << '\n';
}


void await_future()
{
    scope_printer scope(__FUNCTION__);
    read_file();
}
