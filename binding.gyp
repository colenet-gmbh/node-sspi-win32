{
  "targets": [
    {
      "target_name": "node-sspi-win32",
      "defines": [
        "SECURITY_WIN32",
        "_HAS_EXCEPTIONS"
      ],
      "sources": [ "src/main.cpp",
                    "src/type_1.cpp", "src/type_2.cpp", "src/type_3.cpp",  "src/verify_type3.cpp",
                    "src/type_1.h", "src/type_2.h", "src/type_3.h", "src/verify_type3.h",
                    "src/shared.h", "src/shared.cpp"
      ],
      "include_dirs": [
        ".",
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [ "Secur32.lib" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
           "'MultiProcessorCompilation": "false"
        }
      },
      "configurations": {
        "Release": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
            }
          }
        },
        "Debug": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
            }
          }
        }
      }
    }
  ]
}
