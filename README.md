# NativeFS

Replacement functions for filesystem interactions with performance in focus.

## Copy
Progress callback is optional. Progress callback is run at every 1% change.
I.e. every 10th bytes copied of a 1000 bytes large file will trigger a progress update.

```
var nativefs = require('nativefs');

nativefs.copy('original_file', 'target_file', function(copied, total) {
    console.log('Copied ' + copied + ' of ' + total);
}, function(err, result) {
  if (err) throw err;
  // result is always true if there is no error, basically useless argument.
});
```

## License

Licensed under MIT, but please do pull requests if you improve it!

### MIT
Copyright (c) 2015 Patrick Engström

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
