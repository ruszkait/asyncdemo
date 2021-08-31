#include "scope_printer.hpp"
#include "future_coroutine.hpp"

#include <future>
#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

std::future<std::unique_ptr<std::ifstream>> open_file_async(std::filesystem::path filename)
{
    auto open_file_algorithm = [=] {
        log_printer(__FUNCTION__) << "Opening stream " << filename;
        auto data_stream = std::make_unique<std::ifstream>(filename);
        log_printer(__FUNCTION__) << "Stream opened " << filename;
        return data_stream;
    };

    auto stream_future = std::async(std::launch::async, open_file_algorithm);
    return stream_future;
}

std::future<std::string> read_async(std::istream& data_stream)
{
    auto read_algorithm = [&data_stream] {
        std::string text;
        text.resize(128);
        log_printer(__FUNCTION__) << "Reading stream";
        data_stream.read(text.data(), text.size());
        log_printer(__FUNCTION__) << "Read stream";
        return text;
    };

    auto stream_future = std::async(std::launch::async, read_algorithm);
    return stream_future;
}

std::future<std::string> read_file_async(std::filesystem::path filename)
{
    scope_printer scope(__FUNCTION__);

    std::unique_ptr<std::ifstream> data_stream = co_await open_file_async(filename);

    auto data = co_await read_async(*data_stream);

    co_return data;
}

void read_file()
{
    separator_printer separator;
    scope_printer scope(__FUNCTION__);
    auto file_content = read_file_async("../CMakeLists.txt");
    file_content.wait();
    std::cout << file_content.get() << '\n';
}


void await_future()
{
    scope_printer scope(__FUNCTION__);
    read_file();
}
