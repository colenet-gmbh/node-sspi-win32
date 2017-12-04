{
  "targets": [
    {
      "target_name": "node-sspi-win32",
      "defines": [
        "SECURITY_WIN32"
      ],
      "sources": [ "src/main.cpp" ],
      "include_dirs": [
        ".",
      ],
      "libraries": [ "Secur32.lib" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
           "'MultiProcessorCompilation": "false"
        }
      }
    }
  ]
}
