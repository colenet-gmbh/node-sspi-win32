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
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [ "Secur32.lib" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
           "'MultiProcessorCompilation": "false"
        }
      },
      'configurations': {
        'Release': {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1,
            }
          }
        }
      }
    }
  ]
}
