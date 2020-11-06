#pragma once

#include "processor.h"

#include "processor/detector.hpp"

#include <boost/config.hpp>

#include <string_view>
#include <functional>

#include <cstdint>
#include <set>

namespace processor {

    class StatusWrapper : public StatusApi {
    public:

        StatusWrapper();

        StatusWrapper(StatusCode status_code, std::string_view message);

        virtual ~StatusWrapper() override = default;

        explicit operator bool() const noexcept override;

        bool is_success() const noexcept override;

        StatusCode code() const noexcept override;

        const std::string_view &message() const noexcept override;

        static std::unique_ptr<StatusApi> success();

        static std::unique_ptr<StatusApi> images_folder_was_not_found(std::string_view message);

        static std::unique_ptr<StatusApi> bad_image(std::string_view message);

        static std::unique_ptr<StatusApi> system_files_was_not_found(std::string_view message);

        static std::unique_ptr<StatusApi> unexpected_error(std::string_view message);

    private:
        StatusCode _status_code;
        std::string_view _const_message;
    };

    extern "C" BOOST_SYMBOL_EXPORT StatusWrapper Status;
    StatusWrapper Status;

    class ProcessorWrapper : public ProcessorApi {
    public:
        ~ProcessorWrapper() override = default;

        std::unique_ptr<StatusApi> init(InitConfig config) noexcept override;

        std::unique_ptr<StatusApi> process(std::string_view path_to_image_folder,
                                           NotificationCallback notification) noexcept override;

    private:
        std::shared_ptr<detection::HaarDetector> _detector;
        std::set<std::string> _extensions;
    };

    extern "C" BOOST_SYMBOL_EXPORT ProcessorWrapper Processor;
    ProcessorWrapper Processor;

} // namespace processor