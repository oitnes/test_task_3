#include "error.hpp"


namespace detection {

    Error::Error(const std::string &message)
            : _message{message} {}


    const char *Error::what() const noexcept {
        return _message.c_str();
    }


    std::ostream &operator<<(std::ostream &os, const Error &error) {
        return os << error.what();
    }

} // namespace detection

