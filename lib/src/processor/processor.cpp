#include "processor.hpp"

#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <set>


// TODO: add logging

namespace {

    template<typename DataType, std::size_t MAX_POOL_SIZE>
    class TaskQueue {
    public:

        enum class TASK_TYPE : std::uint8_t {
            PROCESS = 1,
            STOP = 2
        };

        struct Task {
            TASK_TYPE type;
            DataType data;

            operator bool() const {
                return type != TASK_TYPE::STOP;
            }
        };

        explicit TaskQueue() : _has_task{false}, _opened{true} {
        }

        ~TaskQueue() {
            close();
        }

        void add(DataType &&data) {
            if (!_opened) {
                _task_conditional_variable.notify_all();
                return; // TODO: throw exception
            }

            std::lock_guard<std::mutex> rw_lock{_rw_mutex};

            if (_awaiting_data_pool.size() >= MAX_POOL_SIZE) {
                return; // TODO: add waiting until queue become smaller
            }

            _awaiting_data_pool.emplace_back(std::move(data));

            _has_task = true;
            _task_conditional_variable.notify_one();
        }

        Task wait_for_task() {
            std::unique_lock rw_lock{_rw_mutex};

            _task_conditional_variable.wait(rw_lock, [this] {
                return (_has_task || !_opened);
            });

            if (_has_task && !_awaiting_data_pool.empty()) {
                auto front_item = _awaiting_data_pool.front();
                _awaiting_data_pool.pop_front();

                if (!_awaiting_data_pool.empty()) {
                    _has_task = true;
                } else {
                    _has_task = false;
                }

                _task_conditional_variable.notify_one();
                return Task{TASK_TYPE::PROCESS, front_item};
            }

            if (_opened) {
                _has_task = false;
            }
            _task_conditional_variable.notify_one();
            return Task{TASK_TYPE::STOP, DataType{}};
        }

        void close() {
            _opened = false;
            _task_conditional_variable.notify_all();
        }

    private:
        std::mutex _rw_mutex;
        std::condition_variable _task_conditional_variable;

        std::atomic<bool> _has_task;
        std::atomic<bool> _opened;

        std::list<DataType> _awaiting_data_pool;
    };

}


namespace processing {

    RESULT_CODE Processor::init(const InitConfig &config) noexcept {

        if ((config.workers_number < 1) || (config.workers_number > _MAX_WORKER_COUNT)) {
            return RESULT_CODE::INIT_INCORRECT_WORKER_NUMBER;
        }

        if (!std::filesystem::exists(config.detector_description_file_path)) {
            return RESULT_CODE::INIT_FILES_WAS_NOT_FOUND;
        }

        std::ifstream file(config.detector_description_file_path);
        std::stringstream buffer;
        if (file) {
            buffer << file.rdbuf();
            file.close();
        } else {
            return RESULT_CODE::INIT_BAD_SETTINGS_FILE;
        }

        boost::property_tree::ptree detector_settings;
        try {
            boost::property_tree::read_json(buffer, detector_settings);
        }
        catch (std::exception const &e) {
            return RESULT_CODE::INIT_BAD_SETTINGS_FILE;
        }

        for (int i = 0; i < config.workers_number; i++) {
            try {
                _detectors_pool.emplace_back(detection::create_detector(detector_settings));
            } catch (...) {
                _detectors_pool.clear();
                return RESULT_CODE::INIT_BAD_DATA_FILE;
            }
        }

        return RESULT_CODE::INIT_SUCCESS;
    }


    RESULT_CODE
    Processor::process(const std::string &path_to_image_folder, NotificationCallback &&notification) noexcept {
        std::set<std::string> extensions{".jpg", ".bmp", ".jpeg"};

        if (!std::filesystem::exists(path_to_image_folder)) {
            return RESULT_CODE::PROCESS_IMAGE_FOLDER_IS_NOT_EXISTS;
        }

        TaskQueue<std::string, 1000> paths_queue;
        std::vector<std::thread> workers;
        for (auto &detector: _detectors_pool) {
            workers.emplace_back([&detector, notification, &paths_queue]() {
                while (auto task = paths_queue.wait_for_task()) {
                    try {
                        auto img = cv::imread(task.data, cv::IMREAD_COLOR);
                        if (!img.empty()) {
                            auto detections = detector->detect(img);
                            notification(task.data, detections);
                        }
                    } catch (...) {
                        continue; // pass
                    }
                }
            });
        }

        for (auto itEntry = std::filesystem::recursive_directory_iterator(path_to_image_folder);
             itEntry != std::filesystem::recursive_directory_iterator();
             ++itEntry) {
            if (itEntry->is_regular_file()) {
                if (extensions.count(itEntry->path().filename().extension().string())) {
                    paths_queue.add(itEntry->path().string());
                }
            }
        }
        paths_queue.close();

        for (auto &worker: workers) {
            worker.join();
        }

        return RESULT_CODE::PROCESS_SUCCESS;
    }

} // namespace processing