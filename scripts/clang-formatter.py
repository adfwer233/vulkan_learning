import glob
import os
import subprocess
import threading

print("running clang format")

file_extends = ["*.hpp", "*.cpp", "*.cuh", "*.cu"]

target_dirs = [".\\src", ".\\include"]

source_file_list = []

for dir in target_dirs:
    for extend in file_extends:
        res = glob.glob(f"{dir}\\**\\{extend}", recursive=True)
        source_file_list += res


def run_clang_format(file_list):
    for file_path in file_list:
        print(file_list)
        subprocess.call(["clang-format", "-i", file_path, "--style=file"])


def split_list_generator(listTemp, n):
    for i in range(0, len(listTemp), n):
        yield listTemp[i:i + n]


def run_clang_format_dispatcher(total_list):
    for cur_file_list in split_list_generator(total_list, 10):
        thread = threading.Thread(target=run_clang_format, args=(cur_file_list,))
        thread.start()


traverse_thread = threading.Thread(target=run_clang_format_dispatcher, args=(source_file_list,))
traverse_thread.start()

traverse_thread.join()