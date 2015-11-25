var expect = require('chai').expect;
var nativefs = require('../nativefs.js');
var fs = require('fs');

describe('NativeFS', function() {

  it("should copy file with progress reports", function(done) {
    var progress_reported = false;
    nativefs.copy('./nativefs.js', 'copied.js', function(copied, total) {
      progress_reported = true;
    }, function(err, result) {
      if (err) throw err;
      expect(result).equal(true);
      expect(progress_reported).equal(true);
      done();
    });
  });

  // since moved is a wrapper around copy, just ensure it's working
  // with an incomplete argument set (optional progress report).
  it("should move file", function(done) {
    nativefs.move('./copied.js', 'moved.js', function(err, result) {
      if (err) throw err;
      expect(result).equal(true);
      done();
    });
  });

  after(function() {
    fs.unlinkSync('moved.js');
  });

});
