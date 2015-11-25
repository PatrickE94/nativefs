#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <string.h>

#include <node.h>

namespace NativeFS {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Function;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Value;

#define THROW_ERROR(err) { isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, err))); return; }

std::string get(v8::Local<v8::Value> value) {
    if (value->IsString()) {
        v8::String::Utf8Value string(value);
        return std::string(*string);
    }
    return "";
}

int doWrite(int fd, char *data, int datalen) {
  int written = write(fd, data, datalen);
  if (written == -1) return -1;
  if (written < datalen) {
    return doWrite(fd, data + written, datalen - written);
  }
  return written;
}

void Copy(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() < 3) THROW_ERROR("Not enough arguments");
  if (!args[0]->IsString()) THROW_ERROR("First argument is not a path");
  if (!args[1]->IsString()) THROW_ERROR("Second argument is not a path");
  if (!args[2]->IsFunction()) THROW_ERROR("Missing result callback");
  if (args.Length() > 3 && !args[3]->IsFunction()) THROW_ERROR("Unknown arguments");

  bool updateProgress = args.Length() > 3;

  std::string pathone = get(args[0]);
  std::string pathtwo = get(args[1]);
  Local<Function> progcb;
  Local<Function> rescb;
  if (updateProgress) {
    progcb = Local<Function>::Cast(args[2]);
    rescb = Local<Function>::Cast(args[3]);
  } else {
    rescb = Local<Function>::Cast(args[2]);
  }

  char buffer[8192];
  struct stat st;
  uint32_t progress = 0;
  ssize_t bytes_read = 0;
  uint32_t bytesPerUpdate = 0;
  uint32_t sinceLastUpdate = 0;
  int in = -1;
  int out = -1;

  in = open(pathone.c_str(), O_RDONLY);
  if (in < 0) goto errOut;
  out = open(pathtwo.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  if (out < 0) goto errOut;

  // Get the input file size
  if (fstat(in, &st) != 0) goto errOut;
  bytesPerUpdate = st.st_size / 100;
  sinceLastUpdate = 0;

  while((bytes_read = read(in, buffer, sizeof(buffer))) > 0)
  {
     int written = doWrite(out, buffer, bytes_read);
     if (written == -1) goto errOut;

     progress += bytes_read;
     sinceLastUpdate += bytes_read;

     if (sinceLastUpdate > bytesPerUpdate && updateProgress) {
       Local<Value> argv[2] = {
         Number::New(isolate, progress),
         Number::New(isolate, st.st_size)
       };
       progcb->Call(Null(isolate), 2, argv);
       sinceLastUpdate = 0;
     }
  }
  if (bytes_read == -1) goto errOut;

  fsync(out); // Flush
  close(in);
  close(out);

  // Result
  {
    Local<Value> argv[2] = { Null(isolate), True(isolate) };
    rescb->Call(Null(isolate), 2, argv);
  }
  return;

errOut:
  remove(pathtwo.c_str()); // remove failed copy
  Local<Value> argv[2] = {
    String::NewFromUtf8(isolate, strerror(errno)),
    False(isolate)
  };
  rescb->Call(Null(isolate), 2, argv);
  return;
}

void Remove(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() < 2) THROW_ERROR("Wrong number of arguments");
  if (!args[0]->IsString()) THROW_ERROR("First argument is not a string");

  std::string path = get(args[0]);
  Local<Function> rescb = Local<Function>::Cast(args[1]);

  if (remove(path.c_str()) < 0) {
    Local<Value> argv[2] = {
      String::NewFromUtf8(isolate, strerror(errno)),
      False(isolate)
    };
    rescb->Call(Null(isolate), 2, argv);
    return;
  }

  Local<Value> argv[2] = { Null(isolate), True(isolate) };
  rescb->Call(Null(isolate), 2, argv);
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "copy", Copy);
  NODE_SET_METHOD(exports, "remove", Remove);
}

NODE_MODULE(native_fs, init)

}  // namespace NativeFS
