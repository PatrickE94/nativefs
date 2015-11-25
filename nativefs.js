(function(){
  "use strict";

  var native_fs = require('./build/Release/native_fs');

  module.exports = {};
  module.exports.copy = function() {
    return native_fs.copy.apply(null, arguments);
  };

  module.exports.remove = function() {
    return native_fs.remove.apply(null, arguments);
  };

  return;

})();
