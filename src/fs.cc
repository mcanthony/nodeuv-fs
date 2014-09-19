#include "../fs.h"

namespace fs {

  void Filesystem::read(long fd, unsigned int bytes, int64_t offset, Callback<Error, uv_buf_t> cb) {

    uv_fs_t read_req;
    uv_buf_t buffer;
    char *buf = (char *) malloc(bytes);
    buffer = uv_buf_init(buf, sizeof(buf));
    Error err;

    static function<void(uv_fs_t* req)> on_read;

    on_read = [&](uv_fs_t* req) {
      auto result = req->result;
      uv_fs_req_cleanup(req);

      if (result > 0) {
        cb(err, buffer);
      }
      else if (result < 0) {
        err.message = uv_err_name(result);
        err.code = result;
        cb(err, buffer);
      }
    };

    uv_fs_read(UV_LOOP, &read_req, fd, &buffer, buffer.len, offset, [](uv_fs_t* req) {
      on_read(req);
    });

    if (!running) {
      uv_run(UV_LOOP, UV_RUN_DEFAULT);
    }
  }

  void Filesystem::open(const char* path, Callback<Error, long> cb) {

    uv_fs_t open_req;
    static function<void(uv_fs_t* req)> on_open;
    Error err;

    on_open = [&](uv_fs_t* req) {
      auto result = req->result;
      uv_fs_req_cleanup(req);

      if (result != -1) {
        cb(err, result);
      }
      else {
        err.message = uv_err_name(result);
        err.code = result;
 
        cb(err, 0);
      }
    };

    uv_fs_open(UV_LOOP, &open_req, "./test.txt", 0, 0, [](uv_fs_t* req) {
      on_open(req);
    });

    if (!running) {
      uv_run(UV_LOOP, UV_RUN_DEFAULT);
    }
  }

  void Filesystem::close(long fd, Callback<Error> cb) {
    uv_fs_t close_req;
    static function<void(uv_fs_t* req)> on_close;
    Error err;

    on_close = [&](uv_fs_t* req) {
      int result = req->result;
      if (result == -1) {
        err.message = uv_err_name(result);
        err.code = result;
      }
      cb(err);
    };

    uv_fs_close(UV_LOOP, &close_req, fd, [](uv_fs_t* req) {
      on_close(req);
    });

    if (!running) {
      uv_run(UV_LOOP, UV_RUN_DEFAULT);
    }
  }

  void Filesystem::stat(const char* path, Callback<Error, Stats> cb) {

    uv_fs_t stat_req;
    static function<void(uv_fs_t* req)> on_stat;
    Error err;

    on_stat = [&](uv_fs_t* req) {
      int result = req->result;
      Stats stats;

      if (result >= 0) {

        auto s = static_cast<const uv_stat_t*>(req->ptr);

        stats.dev = s->st_dev;
        stats.mode = s->st_mode;
        stats.nlink = s->st_nlink;
        stats.uid = s->st_uid;
        stats.gid = s->st_gid;
        stats.rdev = s->st_rdev;
        stats.size = STAT_GET_DOUBLE(size);
        stats.ino = STAT_GET_DOUBLE(ino);
        stats.atime = STAT_GET_DATE(atim);
        stats.mtime = STAT_GET_DATE(mtim);
        stats.ctime = STAT_GET_DATE(ctim);
      }

      uv_fs_req_cleanup(req);
  
      if (result >= 0) {
       cb(err, stats);
      }
      else {
        err.message = uv_err_name(result);
        err.code = result;
        cb(err, stats);
      }
    };

    uv_fs_stat(UV_LOOP, &stat_req, path, [](uv_fs_t* req) {
      on_stat(req);
    });

    if (!running) {
      uv_run(UV_LOOP, UV_RUN_DEFAULT);
    }
  }

  void Filesystem::readFile(const char* path, Callback<Error, string> cb) {

    stat(path, [&](auto err, auto stats) {

      if (err) {
        cb(err, "");
        return;
      }

      auto size = stats.size;

      open(path, [&](auto err, auto fd) {

        if (err) {
          cb(err, "");
          return;
        }

        int64_t offset = 0;
        stringstream ss;
        static function<void()> reader;

        reader = [&]() {

          if (offset < size) {
            read(fd, MAGIC_BUFFER_SIZE, offset, [&](auto err, auto buf) {
              offset += (int64_t) buf.len;
              ss << buf.base;
              reader();
            });
            return;
          }
          else {
            close(fd, [&](auto err) {
              cb(err, ss.str());
            });
          }
        }; 

        reader();
      });

    });

    if (!running) {
      uv_run(UV_LOOP, UV_RUN_DEFAULT);
    }
  }

} // namespace fs

