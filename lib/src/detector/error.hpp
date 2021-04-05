#pragma once


#include <boost/current_function.hpp>

#include <exception>
#include <string>
#include <iostream>


namespace detection {

    class Error : public std::exception {
    public:
        Error() = default;

        Error(const std::string &message);

        Error(const Error &) = default;

        Error(Error &&) = default;

        ~Error() override = default;

        Error &operator=(const Error &) = default;

        Error &operator=(Error &&) = default;

        const char *what() const noexcept override;

    private:
        std::string _message;
    };


    class CreationError : public Error {
        using Error::Error;
    };


    class ProcessingError : public Error {
        using Error::Error;
    };


    std::ostream &operator<<(std::ostream &os, const Error &error);


#define RAISE_ERROR(error_type, message)                                                                               \
    throw error_type(std::string{ __FILE__ } + std::string{ ":" } + std::to_string(__LINE__) + std::string{ " :: " } + \
                     std::string{ BOOST_CURRENT_FUNCTION } + std::string{ " :: " } + (message))

} // namespace detection