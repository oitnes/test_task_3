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

        static void success(std::shared_ptr<StatusApi> &status);

        static void images_folder_was_not_found(std::shared_ptr<StatusApi> &status, std::string_view message);

        static void bad_image(std::shared_ptr<StatusApi> &status, std::string_view message);

        static void system_files_was_not_found(std::shared_ptr<StatusApi> &status, std::string_view message);

        static void unexpected_error(std::shared_ptr<StatusApi> &status, std::string_view message);

    private:
        StatusCode _status_code;
        std::string_view _const_message;
    };

    extern "C" BOOST_SYMBOL_EXPORT StatusWrapper Status;
    StatusWrapper Status;

    class ProcessorWrapper : public ProcessorApi {
    public:
        ~ProcessorWrapper() override = default;

        void init(std::shared_ptr<StatusApi> &status, InitConfig config) noexcept override;

        void
        process(std::shared_ptr<StatusApi> &status, std::string_view path_to_image_folder,
                NotificationCallback notification) noexcept override;

    private:
        std::shared_ptr<detection::HaarDetector> _detector;
        std::set<std::string> _extensions;
    };

    extern "C" BOOST_SYMBOL_EXPORT ProcessorWrapper Processor;
    ProcessorWrapper Processor;

} // namespace processor