#ifndef PROCESSOR_H
#define PROCESSOR_H

extern "C"
{

enum RESULT_CODE {

    SUCCESS = 0,
    UNEXPECTED_ERROR = SUCCESS + 1,

    INIT_SUCCESS = 100,
    INIT_UNEXPECTED_ERROR = INIT_SUCCESS + 1,
    INIT_FILES_WAS_NOT_FOUND = INIT_SUCCESS + 2,
    INIT_BAD_SETTINGS_FILE = INIT_SUCCESS + 3,
    INIT_BAD_DATA_FILE = INIT_SUCCESS + 4,
    INIT_INCORRECT_WORKER_NUMBER = INIT_SUCCESS + 5,
    INIT_DOUBLE_INITIALIZATION = INIT_SUCCESS + 6,

    PROCESS_SUCCESS = 200,
    PROCESS_UNEXPECTED_ERROR = PROCESS_SUCCESS + 1,
    PROCESS_IMAGE_FOLDER_IS_NOT_EXISTS = PROCESS_SUCCESS + 2,
    PROCESS_UNINITIALIZED_LIB = PROCESS_SUCCESS + 3

};

RESULT_CODE init(int workers_number, const char *detector_description_file_path);

using NotificationFunction = void (*)(const char *);
RESULT_CODE process(const char *path_to_image_folder, NotificationFunction notification_fn_ptr);

}

#endif //PROCESSOR_H
