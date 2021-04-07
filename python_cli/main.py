import ctypes
import os
import argparse
import json

import cv2

DLL_FILE_NAME = "libdetection_processor_wrapper.dylib"  # TODO: platform dependence


def image_post_process_callback(char_ptr: bytes):
    result_json = json.loads(char_ptr.decode('utf8'))
    image_path = result_json['image_path']
    image = cv2.imread(image_path)
    if image is None:
        print(f"can't open image by path: {image_path}")
        return

    detections_counter = 0
    detections = result_json['detections']
    if type(detections) is not str:
        for detection in detections:
            detections_counter += 1
            current_image_path = f"{image_path}face_{detections_counter}.jpg"
            x = int(detection['x'])
            y = int(detection['y'])
            width = int(detection['width'])
            height = int(detection['height'])
            face_roi = image[y:y + height, x:x + width]
            flopped_face_roi = cv2.flip(face_roi, 0)
            cv2.imwrite(current_image_path, flopped_face_roi)
            detection["image_path"] = current_image_path

    print(f"{detections_counter} detections by path: {image_path}")

    with open(image_path + ".result.json", "wt") as output_json_file:
        json.dump(result_json, output_json_file)


def process(library_path: str, workers_number: int, detector_description_file: str, images_dir: str):
    # load and setup library
    if not os.path.exists(library_path):
        print(f"The processor library was not found by path: {library_path}")
        exit(1)

    print(f"Loading the processor library by path: {library_path}")
    test_lib = ctypes.CDLL(library_path)

    # set up init fn
    init_lib_fn = test_lib.init
    init_lib_fn.restype = ctypes.c_int
    init_lib_fn.argtypes = [ctypes.c_int, ctypes.c_char_p]

    # call init fn
    if 100 != init_lib_fn(workers_number, detector_description_file.encode('utf8')):
        print(f"Library init failed")
        exit(1)

    # set up process fn
    process_lib_fn = test_lib.process
    process_lib_fn.restype = ctypes.c_int
    process_lib_fn.argtypes = [ctypes.c_char_p, ctypes.c_void_p]

    # call process fn
    process_callback_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
    if 200 != process_lib_fn(images_dir.encode("utf8"), process_callback_type(image_post_process_callback)):
        print(f"Library image process failed")
        exit(1)

    exit(0)


def main():
    argument_parser = argparse.ArgumentParser("Computation options")
    argument_parser.add_argument('--detector_description_file', '-d',
                                 default=os.path.join(os.getcwd(), "haar_detector_description.json"), type=str,
                                 help="set up detector description file path")
    argument_parser.add_argument('--library_path', '-l',
                                 default=os.path.join(os.getcwd(), DLL_FILE_NAME), type=str,
                                 help="set processor library path")
    argument_parser.add_argument('--images_dir', '-i',
                                 required=True, type=str,
                                 help="set images folder path")
    argument_parser.add_argument('--workers_number', '-w',
                                 default=2, type=int,
                                 help="set process worker number")

    args = argument_parser.parse_args()

    if os.path.dirname(os.path.realpath(__file__)) != os.path.dirname(os.path.realpath(args.library_path)):
        print(f"Warning: be sure that you correctly set up paths to resource files and inner paths.")

    process(args.library_path, args.workers_number, args.detector_description_file, args.images_dir)


if __name__ == "__main__":
    main()
