# node-sspi-win32

A very thin wrapper around Win32 SSPI.
Itented to be used by proxy for NTLM oder kerberos authentication

*Works only with 32 bit version of node.js*

## Prerequisits

- [node.js](https://nodejs.org/dist/v6.10.0/node-v6.10.0-x86.msi) for Windows 32 bit
- [windows-build-tools](https://github.com/felixrieseberg/windows-build-tools) using `npm install --global --production windows-build-tools` from an elevated PowerShell or CMD.exe (run as Administrator).
- [node-gyp](https://github.com/nodejs/node-gyp) via `npm install -g node-gyp`
- [jasmine](https://jasmine.github.io/2.0/node.html) using `npm install --global jasmine`

## Compilation and test

1. change to the cloned dir
2. run in a nodejs PowerShell `node-gyp init`
3. `node-gyp configure`
4. `node-gyp build`
4. `jasmine`

## Debugging

1. Open the `binding.sln` in the build directory.
2. Select the node-sspi-win32 project properties
3. under `Debugging` use `node.exe` as the command (browse for it under `C:\Program Files (x86)\nodejs\node.exe`)
4. Working directory should be set to `$(ProjectDir)..`
5. Program Arguments can be set to `test.js`

This has to be done every done `node-gyp configure` or `node-gyp rebuild` has
been called.

Happy coding.
