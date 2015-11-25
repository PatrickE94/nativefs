(function(){
  "use strict";

  var native_fs = require('./build/Release/native_fs');
  var fs = require('fs');

  module.exports = {};
  module.exports.copy = function() {
    return native_fs.copy.apply(null, arguments);
  };

  module.exports.move = function(pathone, pathtwo, progcb, rescb) {
    if (rescb === undefined) {
      rescb = progcb;
      progcb = null;
    }

    var tmpcb = function(err, res) {
      if (err) return rescb(err, res);
      fs.unlink(pathone, function(err) {
        if (err) return rescb(err, false);
        rescb(null, true);
      });
    };

    if (progcb) native_fs.copy.call(null, pathone, pathtwo, progcb, tmpcb);
    else native_fs.copy.call(null, pathone, pathtwo, tmpcb);
  };

  return;

})();
