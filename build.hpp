#include <string>

namespace build {

    enum class TargetType {

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

    class Target {

    };

    Os GetOs()
    {
        #ifdef __APPLE__
        return Os::MacOs;
        #elif __linux__
        return Os::Linux;
        #elif _WIN32
        return Os::Windows;
        #endif
        return Os::Unknown;
    }

    struct CompilerCommandMap {
        std::string cCompiler;
        std::string cppCompiler;
        std::string compilerOutput;
        std::string includeDirectory;
        std::string define;
        
        std::string linker;
        std::string linkerOutput;
        std::string libraryDirectory;
        std::string library;

        // TODO Add in extra options such as optimization level, warnings, etc...
    };

    static inline CompilerCommandMap clangCommandMap {
        .cCompiler = "clang",
        .cppCompiler = "clang++",
        .compilerOutput = "-o",
        .includeDirectory = "-I",
        .define = "-D",

        // TODO finish filling this out
    };

    static CompilerCommandMap gccCommandMap {
        .cCompiler = "gcc",
        .cppCompiler = "g++",
        .compilerOutput = "-o",
        .includeDirectory = "-I",
        .define = "-D",
        
        // TODO finish filling this out
    };

    // TODO Create a CompilerCommandMap for MSVC

}
