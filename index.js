const os = require('os');

if (os.platform === 'win32') {
  const native_impl = require('node-sspi-win32.node');
  exports = {
    generate_type1_message: native_impl.generate_type1_message,
    generate_type2_message: native_impl.generate_type2_message,
    generate_type3_message: native_impl.generate_type3_message,
    verify_type3_message: native_impl.verify_type3_message
  }
}
else {
  exports = {
    generate_type1_message: function() {
      return Buffer.from('type1');
    },

    generate_type2_message: function(buffer, len) {
      return Buffer.from('type2');
    },

    generate_type3_message: function(buffer, len) {
      return Buffer.from('type3')
    },

    verify_type3_message: function(buffer, len) {
        return Buffer.from('done');
    }
  }
}
