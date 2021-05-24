#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <mutex>

inline std::mutex cout_mutex_;

class separator_printer
{
public:
    separator_printer()
    {
        std::lock_guard lock(cout_mutex_);
        std::cout << "===================================\n";
    }

    ~separator_printer()
    {
        std::lock_guard lock(cout_mutex_);
        std::cout << "===================================\n";
    }
};

class scope_printer
{
public:
    scope_printer(std::string_view function_name)
        : m_function_name(function_name)
    {
        std::lock_guard lock(cout_mutex_);
        std::cout << m_function_name << " starts on " << std::this_thread::get_id() << '\n';
    }

    ~scope_printer()
    {
        std::lock_guard lock(cout_mutex_);
        std::cout << m_function_name << " ends on " << std::this_thread::get_id() << '\n';
    }

private:
    std::string_view m_function_name;
};

class log_printer : public std::ostringstream
{
public:
    log_printer(std::string_view function_name)
        : function_name_(function_name) {}

    ~log_printer()
    {
        std::lock_guard lock(cout_mutex_);
        std::cout << function_name_ << " on " << std::this_thread::get_id() << ": " << this->str() << '\n';
    }

private:
    std::string_view function_name_;
};
