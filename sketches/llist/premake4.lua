solution "linearisation-test"
  buildoptions {
    "-std=c11",
    "-march=native",
    "-Qunused-arguments"
    }

  linkoptions {
    "-lm",
    "-Qunused-arguments"
    }

  configurations {"Debug", "Release"}

  configuration "Debug"
    targetdir "bin/debug"
    objdir "obj/debug"
    buildoptions {
      "-ggdb",
      }

  configuration "Release"
    targetdir "bin/release"
    objdir "obj/release"
    defines "NDEBUG"
    buildoptions {
      "-O3",
      "-flto"
      }
    linkoptions {
      "-flto",
      "-fuse-ld=gold",
      }

project "baseline"
  kind "ConsoleApp"
  language "C"
  files {
    "list.c",
    "main.c"
  }

project "linear1"
  kind "ConsoleApp"
  language "C"
  files {
    "linear1.c",
    "main.c"
  }

project "linear2"
  kind "ConsoleApp"
  language "C"
  files {
    "linear2.c",
    "main.c"
  }


