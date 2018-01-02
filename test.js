// hello.js
const sspi = require('./build/Debug/node-sspi-win32');
var xpath = require('xpath')
  , dom = require('xmldom').DOMParser;

buffer = sspi.generate_type1_message();
console.log("Type1: ", buffer.toString('base64'));
buffer = sspi.generate_type2_message(buffer, buffer.length);
console.log("Type2: ", buffer.toString('base64'));
buffer = sspi.generate_type3_message(buffer, buffer.length);
console.log("Type3: ", buffer.toString('base64'));
console.log("Verifying type_3 message");
console.log(buffer.toString('base64'));
buffer = sspi.verify_type3_message(buffer, buffer.length);
