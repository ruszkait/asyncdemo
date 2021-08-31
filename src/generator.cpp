#include "scope_printer.hpp"
#include "future_coroutine.hpp"
#include <experimental/generator>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <future>

namespace {

// co_await is not supported yet from a generator
/*
     std::experimental::generator<std::string> read_file_generator_async(std::filesystem::path filename)
     {
         scope_printer scope(__FUNCTION__);

         std::unique_ptr<std::istream> data_stream = co_await std::async(std::launch::async, [=] {
             log_printer(__FUNCTION__) << "Opening stream " << filename;
             auto data_stream = std::make_unique<std::ifstream>(filename);
             log_printer(__FUNCTION__) << "Stream opened " << filename;
             return data_stream;
         });

         for (;;)
         {
             auto data = co_await std::async(std::launch::async, [data_stream = std::move(data_stream)] {
                 std::string text;
                 text.resize(128);
                 log_printer(__FUNCTION__) << "Reading stream";
                 data_stream->read(text.data(), text.size());
                 log_printer(__FUNCTION__) << "Read stream";
                 return text;
             });

             const bool no_more_data = data.empty();
             if (no_more_data)
                 break;

             co_yield data;
         }
     }

     void read_file_async()
     {
         separator_printer separator;
         scope_printer scope(__FUNCTION__);
         auto text_generator = read_file_generator_async("../CMakeLists.txt");
         for (const std::string& text : co_await text_generator)
             std::cout << text;
     }
     */

    std::experimental::generator<std::string> read_file_generator(std::filesystem::path filename)
    {
        scope_printer scope(__FUNCTION__);

        log_printer(__FUNCTION__) << "Opening stream " << filename;
        std::ifstream data_stream(filename);
        log_printer(__FUNCTION__) << "Stream opened " << filename;

        for (;;)
        {
            std::string text;
            text.resize(128);
            log_printer(__FUNCTION__) << "Reading stream";
            const bool no_more_data = data_stream.read(text.data(), text.size()).eof();
            log_printer(__FUNCTION__) << "Read stream";

            co_yield text;

            if (no_more_data)
                break;
        }
    }

    void read_file()
    {
        separator_printer separator;
        scope_printer scope(__FUNCTION__);
        auto text_generator = read_file_generator("../CMakeLists.txt");
        for (const std::string& text : text_generator)
            std::cout << text;
    }

}

void generator()
{
    scope_printer scope(__FUNCTION__);
    read_file();
    //read_file_async();
}
