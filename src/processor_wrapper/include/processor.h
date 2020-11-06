#pragma once

#include <boost/config.hpp>

#include <string_view>
#include <functional>
#include <memory>

#include <cstdint>

enum class BOOST_SYMBOL_VISIBLE StatusCode : std::uint8_t {
    SUCCESS = 0,
    IMAGE_FOLDER_IN_NOT_EXISTS = 1,
    BAD_IMAGE = 2,
    INCORRECT_WORKERS_NUMBER = 3,
    SYSTEM_FILES_WAS_NOT_FOUND = 4,
    UNEXPECTED = 255
};

class BOOST_SYMBOL_VISIBLE StatusApi {
public:

    virtual ~StatusApi() = default;

    virtual explicit operator bool() const noexcept = 0;

    virtual bool is_success() const noexcept = 0;

    virtual StatusCode code() const noexcept = 0;

    virtual const std::string_view &message() const noexcept = 0;

};

struct BOOST_SYMBOL_VISIBLE InitConfig {
    std::uint8_t workers_number;
    std::string_view resource_folder_path;
};

using NotificationCallback = std::function<void(std::string_view processed_image_path, std::size_t faces_number)>;

class BOOST_SYMBOL_VISIBLE ProcessorApi {
public:
    virtual ~ProcessorApi() = default;

    virtual std::unique_ptr<StatusApi> init(InitConfig config) noexcept = 0;

    virtual std::unique_ptr<StatusApi> process(std::string_view path_to_image_folder,
                                               NotificationCallback notification) noexcept = 0;
};
