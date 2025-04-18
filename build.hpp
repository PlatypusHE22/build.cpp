#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>

namespace build {
//// Definitions

    enum class TargetType {
        Executable = 0,
        StaticLibrary,
        SharedLibrary,
    };

    enum class Os {
        Unknown = 0,
        MacOs,
        Linux,
        Windows,
        FreeBSD,
        OpenBSD,
        NetBSD,
    };

    class Project {

    };

    struct Target {
        Target() = delete;
        Target(const std::string& name, TargetType type = TargetType::Executable, const std::vector<std::string>& sources = {});

        std::string name;
        TargetType type;
        std::vector<std::string> sources;
        std::vector<std::string> includeDirectories = {};
        std::vector<std::string> libraryDirectories = {};
        std::vector<std::string> libraries = {};
        std::vector<std::string> defines = {};
        struct CompilerCommandMap& compilerCommandMap;

    private:
        bool built = false;
    };

    int Build(Target& target);

    static Os GetOs();
    static CompilerCommandMap& GetDefaultCompiler();

    struct CompilerCommandMap {
        // Compile options
        std::string cCompiler;
        std::string cppCompiler;
        std::string compilerOutput;
        std::string includeDirectory;
        std::string define;
        
        // Linker options
        std::string cLinker;
        std::string cppLinker;
        std::string linkerOutput;
        std::string libraryDirectory;
        std::string library;

        // Compilation steps
        std::string compileOnly;
        // TODO Add in extra options such as optimization level, warnings, etc...
    };

    static CompilerCommandMap clangCommandMap {
        .cCompiler = "clang",
        .cppCompiler = "clang++",
        .compilerOutput = "-o",
        .includeDirectory = "-I",
        .define = "-D",

        .cLinker = "clang",
        .cppLinker = "clang++",
        .linkerOutput = "-o",
        .libraryDirectory = "-L",
        .library = "-l",

        .compileOnly = "-c",

        // TODO finish filling this out
    };

    static CompilerCommandMap gccCommandMap {
        .cCompiler = "gcc",
        .cppCompiler = "g++",
        .compilerOutput = "-o",
        .includeDirectory = "-I",
        .define = "-D",

        .compileOnly = "-c",
        
        // TODO finish filling this out
    };
    
    // TODO Create a CompilerCommandMap for MSVC

//// Implementation
    static Os GetOs()
    {
        #ifdef __APPLE__
        return Os::MacOs;
        #elif __linux__
        return Os::Linux;
        #elif _WIN32
        return Os::Windows;
        #endif
        // TODO Add more checks for additional OSes

        return Os::Unknown;
    }

    static CompilerCommandMap& GetDefaultCompiler()
    {
        std::cerr << "GetDefaultCompiler is not implemented yes so it only returns clang :P\n";
        return clangCommandMap;
    }

    Target::Target(const std::string& name, TargetType type, const std::vector<std::string>& sources)
    :name(name), type(type), sources(sources), compilerCommandMap(GetDefaultCompiler())
    {

    }

    int Build(Target& target)
    {
        std::filesystem::path buildDirectory = "out/";
        std::filesystem::path objectOutputDirectory = buildDirectory / "obj/";
        std::filesystem::path binaryOutputDirectory = buildDirectory;

        // Creating necessary directories
        if (!std::filesystem::exists(buildDirectory))
            std::filesystem::create_directory(buildDirectory);
        if (!std::filesystem::exists(objectOutputDirectory))
            std::filesystem::create_directory(objectOutputDirectory);
        if (!std::filesystem::exists(binaryOutputDirectory))
            std::filesystem::create_directory(binaryOutputDirectory);


        /// Compiling
        std::cout << "Building target: " << target.name << "\n";
        std::vector<std::string> objects; 
        for (const std::string& source : target.sources)
        {
            std::cout << "Compiling " << source << "\n";
            std::string object = source.substr(0, source.find(".cpp")) + ".o";

            std::filesystem::file_time_type objectLastWrite = std::filesystem::file_time_type::min();
            if (std::filesystem::exists(objectOutputDirectory / object))
                objectLastWrite = std::filesystem::last_write_time(objectOutputDirectory / object);

            if (objectLastWrite >= std::filesystem::last_write_time(source))
            {
                objects.push_back(objectOutputDirectory / object);
                std::cout << source << " is up to date, skipping\n";
                continue;
            }

            std::ostringstream commmand;
            // clang++ -c -o main.o main.cpp
            commmand << target.compilerCommandMap.cppCompiler << " " << target.compilerCommandMap.compileOnly << " " << target.compilerCommandMap.compilerOutput << (objectOutputDirectory/object) << " " << source;
            std::filesystem::path tmp = std::filesystem::temp_directory_path() / "buildcpptmp.txt";
            int result = system(commmand.str().c_str());
            if (result != 0)
            {
                std::cerr << "Failed to build: " << source << ", exiting\n";
                return result;
            }
            objects.push_back(objectOutputDirectory / object);
            std::cout << "Successfully compiled " << source << "\n";
        }
        std::cout << "Successfully compiled target: " << target.name << "\n";


        // Linking
        // This part should only run when building an executable
        if (target.type == TargetType::Executable)
        {
            std::cout << "Linking target: " << target.name << "\n";
            std::ostringstream command;
            command << target.compilerCommandMap.cppLinker << " " << target.compilerCommandMap.linkerOutput << target.name;

            // TODO Add linking to static libraries

            for (const auto& object : objects)
            {
                command << " " << object;
            }
            
            std::cout << command.str() << "\n";
            int result = system(command.str().c_str());
            if (result != 0)
            {
                std::cerr << "Failed to link: " << target.name << ", exiting\n";
                return result;
            }
            std::cout << "Successfully linked target: " << target.name << "\n";
            return 0;
        }

        // TODO Add support for compiling static libraries
        if (target.type == TargetType::StaticLibrary)
        {
            std::cerr << "Compiling static libraries is not supported yet...\n";
            return 1;
        }


        // TODO Add support for compiling shared libraries
        if (target.type == TargetType::SharedLibrary)
        {
            std::cerr << "Compiling shared libraries is not supported yet...\n";
            return 1;
        }

        return 0;
    }


}

