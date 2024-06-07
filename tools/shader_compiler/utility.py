import os
import subprocess
import platform

def is_windows():
    return platform.system() == "Windows"


def is_linux():
    return platform.system() == "Linux"


def is_macos():
    return platform.system() == "Darwin"

def execute_command(command, show_command=False, working_dir=None):
    if show_command:
        print("[ Info ] Command: ", ' '.join(command))
    subprocess.call(command, cwd=working_dir)

def forward(folder):
    p = os.path.dirname(folder)
    if not os.path.exists(p):
        return False
    else:
        return True


def forward_hard(folder):
    p = os.path.dirname(folder)
    if not os.path.exists(p):
        os.makedirs(p)
    return True
        

def abspath(a):
    return os.path.abspath(a)


def relpath(a, common):
    return os.path.relpath(a, common)


def join_path(*argv):
    result = ''
    for arg in argv:
        result = os.path.join(result, arg)
    return result


def get_file_list_of_path(path):
    walkList = os.walk(path)
    file_list = []
    for i in walkList:
        root, dirs, files = i
        for f in files:
            file_list.append(os.path.join(root, f))
    return file_list


def get_folder_list_of_path(path):
    walkList = os.walk(path)
    folder_list = []
    for i in walkList:
        root, dirs, files = i
        for d in dirs:
            folder_list.append(os.path.join(root, d))
            
            
def get_file_name(f):
    return os.path.basename(f)


def get_file_extention(f):
    return os.path.splitext(f)[1]


def get_file_name_without_extention(f):
    return os.path.splitext(os.path.basename(f))[0]