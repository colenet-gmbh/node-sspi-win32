// hello.js
const sspi = require('./build/Release/node-sspi-win32');
var xpath = require('xpath')
  , dom = require('xmldom').DOMParser

buffer = sspi.generate_type1_message()
console.log(buffer.toString('base64'));
buffer = Buffer.from("test");
buffer = sspi.generate_type3_message(buffer, buffer.length)
