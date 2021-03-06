#ifndef NODEUV_FS_H
#define NODEUV_FS_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>

#include "./deps/buffer/buffer.h"

extern "C" {
#include "uv.h"
}

namespace nodeuv {

  using namespace std;

  class Readable;
  class Writable;
  class Error;

  // https://github.com/iojs/io.js/blob/master/lib/fs.js#L256
  const unsigned int MAGIC_BUFFER_SIZE = 8192;

  struct Stats {
    uint64_t dev;
    uint64_t mode;
    uint64_t nlink;
    uint64_t uid;
    uint64_t gid;
    uint64_t rdev;
    uint64_t ino;
    uint64_t size;
    uint64_t blksize;
    uint64_t blocks;
    uint64_t flags;
    uint64_t gen;
    uv_timespec_t atime;
    uv_timespec_t mtime;
    uv_timespec_t ctime;
    uv_timespec_t birthtime;
  };

  template <typename... Args>
  using Callback = function<void(Args... params)>;

  class Error {
    public:
      string message = "";
      int code = 0;

      inline operator bool() const {
        return (message.length() > 0 || code != 0);
      }
  };

  //
  // Some reasonable defaults for Read and Write options...
  //
  struct ReadOptions {
    int flags = O_CREAT | O_RDWR;
    int mode = S_IRUSR | S_IWUSR;
  };

  struct WriteOptions {
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  };

  class Filesystem {
    public:
      uv_loop_t* UV_LOOP;
      bool running = false;

      string cwd();

      void stat(string, Callback<Error, Stats>);
      Stats statSync(string);

      Error mkdirSync(string);
      Error mkdirSync(string, int);

      Error rmdirSync(string);

      void open(string, int, int, Callback<Error, uv_file>);
      int openSync(string, int, int);

      void read(uv_file, int64_t, int64_t, Callback<Error, uv_buf_t>);
      int readSync(uv_file fd, uv_buf_t* buffer, int64_t offset, int64_t bytes);

      void write(uv_file, uv_buf_t, int64_t, Callback<Error>);
      int writeSync(uv_file fd, Buffer buffer, int64_t offset, int64_t length);

      void close(uv_file fd, Callback<Error>);
      int closeSync(uv_file fd);

      // 
      // Higher level functions
      //
      void readFile(string, Callback<Error, Buffer>);
      void readFile(string, ReadOptions, Callback<Error, Buffer>);

      Buffer readFileSync(string, ReadOptions);
      Buffer readFileSync(string);

      void writeFile(string, string, Callback<Error>);
      void writeFile(string, string, WriteOptions, Callback<Error>);
      void writeFile(string, uv_buf_t, Callback<Error>);
      void writeFile(string, uv_buf_t, WriteOptions, Callback<Error>);
      int writeFileSync(string, Buffer);
      int writeFileSync(string, Buffer, WriteOptions);

      Filesystem() {
        UV_LOOP = uv_default_loop();
      };
  };
} // namespace fs;

#endif

