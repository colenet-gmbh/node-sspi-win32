const os = require('os');
const { spawn } = require('child_process');

if (os.platform === 'win32') {
  console.info('Installing Win32 binaries');
  spawn('node-gyp', ['rebuild']);
}
else {
  console.info('Installing mock');
}
