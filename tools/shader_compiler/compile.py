import utility as util
import argparse
import sys
import os


def log(message, severity="Info"):
    print("[", severity, "]: \"", message, "\"!")


def get_is_embedded(args):
    return args.embedded == True


def get_executable_suffix():
    return ('', '.exe')[util.is_windows()]

def get_shaderc():
    path = util.join_path("shaderc", "shaderc")
    path = path + get_executable_suffix()
    return path

    

def compile_shader(source_path, shader_name, output, platform, extension, profile, is_vertex=False, include=None, opt_level=None, bin2carray=None):
    shaderc = get_shaderc()
    
    print("[ Info ] Searching for shaderc at: ", shaderc)
    
    if os.path.exists(shaderc) == False:
        log("Could not locate \"shaderc\" binary. Please make sure you have compiled it and it is locate in the same directory as this script or in \"shaderc\" folder!", "Error")
        log("Note: To compile \"shaderc\" please clone \"https://github.com/bkaradzic/bgfx.git\" and build bgfx with the GENie tool with the \"--with-tools\" option.", "Info")
        return
    
    
    # Construct full shader source path.
    if is_vertex == True:
        shader_name = "vs_" + shader_name
    else:
        shader_name = "ps_" + shader_name

    full_shader_path = util.join_path(source_path, shader_name)
    full_shader_path = full_shader_path + ".sc"
    
    
    
    if os.path.exists(full_shader_path) == False:
        print("[ Error ]: \"", "Could not locate the shader source at path: \"", full_shader_path, "\"\"!")
        return
    
    # Construct the output shader name.
    name = util.get_file_name_without_extention(shader_name)
    array_name = name
    
    if bin2carray:
        name = name + "." + extension + ".h"
        array_name = array_name + "_" + extension
    else:
        name = name + "." + extension + ".bin"

    shader_type = ""
    if is_vertex:
        shader_type = "v"
    else:
        shader_type = "f"
        
        
    cmd = [shaderc, "-f", full_shader_path, "-o", util.join_path(output, name), "--platform", platform, "--type", shader_type, "--profile", profile]
    
    if include:
        for path in include:
            cmd += ["-i", path]
    if opt_level:
        cmd += ["-O", str(opt_level)]
    if bin2carray:
        cmd += ["--bin2c", array_name]
    
    cmd += ["--varyingdef", util.join_path("source", "varying.def.sc")]
    
    
    # Before executing compiling command make sure the output folder does exist.
    util.forward_hard(util.join_path(output, name))
    
    util.execute_command(cmd, True)
    print("\n")


def check_for_valid_args(args):
    result = True
    if args.file == None:
        log("Please provide a source folder at which the shaders to be compiled are located!", "Error")
        result = False
    return result


def compile_binary_shader(shader_name, include, output):
    source_path = "source"
    source_path = util.join_path(source_path, shader_name)
    
    compile_info = []
    
    if util.is_windows():
        compile_info += [
            ["windows", "s_5_0", 3, "dx11"],
            ["windows", "s_5_0", 3, "dx12"]
            ]
    
    compile_info += [
        ["android", "spirv", None, "spv"],
        ["linux", "spirv", None, "spv"],
        ["osx", "metal", 3, "metal"],
        ["ios", "metal", 3, "metal"],
        ["orbis", "pssl", None, "pssl"],
        ]    
    
    
    shaders = util.get_file_list_of_path(source_path)
    for shader in shaders:
        is_vertex = shader.find("vs") != -1
    
        for info_array in compile_info:
            platform, profile, opt_level, ext = info_array
            
            # Construct full output path
            out_path = output
            out_path = util.join_path(out_path, platform, shader_name)
            
            print("[ Info ] Compiling binary shader at: ", source_path, ", to location: ", out_path)
            
            compile_shader(source_path, shader_name, out_path, platform, ext, profile, is_vertex, ["source"], opt_level)
    

def compile_embedded_shader(shader_name, include, output):
    source_path = "source"
    source_path = util.join_path(source_path, shader_name)
    
    compile_info = []
    
    if util.is_windows():
        compile_info += [
            ["windows", "s_5_0", 3, "dx11"],
            ["windows", "s_5_0", 3, "dx12"]
            ]
    
    compile_info += [
        ["android", "spirv", None, "spv"],
        ["linux", "spirv", None, "spv"],
        ["osx", "metal", 3, "metal"],
        ["ios", "metal", 3, "metal"],
        ["orbis", "pssl", None, "pssl"],
        ]    
    
    
    shaders = util.get_file_list_of_path(source_path)
    for shader in shaders:
        is_vertex = shader.find("vs") != -1
    
        for info_array in compile_info:
            platform, profile, opt_level, ext = info_array
            
            # Construct full output path
            out_path = output
            out_path = util.join_path(out_path, platform, shader_name)
            
            print("[ Info ] Compiling embedded shader at: ", source_path, ", to location: ", out_path)
            
            compile_shader(source_path, shader_name, out_path, platform, ext, profile, is_vertex, ["source"], opt_level, True)
    

def main():
    parser = argparse.ArgumentParser("Mint Engine Shader Compiler")
    
    # Valid arguments for compiler.        
    parser.add_argument("-i", "--include", action="append", nargs="+", type=str, help="additional files to be included for required for shader compilation; can be zero or more")
    parser.add_argument("-embedded", action="store_true", help="Whether the shader should be compiled as an embedded shader; by default we compile to binary")
    parser.add_argument("-f", "--file", type=str)
    
    args = parser.parse_args(sys.argv[1:])
    
    sys.argv.remove(sys.argv[0])
    
    if len(sys.argv) == 0:
        log("No compilation arguments were provided.  Consult the help function: \"python ./compile.py --help\"", "Error")
        return 

    if check_for_valid_args(args) == False:
        log("Invalid arguments provided. Consult the help function: \"python ./compile.py --help\"", "Error")
        return

    if get_is_embedded(args):
        log("Compiling embedded shader...", "Info")
        
        shader_name = args.file
        include = args.include
        out = "embedded"
        
        compile_embedded_shader(shader_name, include, out)        
        
    else:
        log("Compiling binary shader...", "Info")
        
        shader_name = args.file
        include = args.include
        out = "bin"
        
        compile_binary_shader(shader_name, include, out)
    


if __name__ == "__main__":
    main()
    