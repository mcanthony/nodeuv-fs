# SYNOPSIS
A friendly c++1y wrapper for libuv's filesystem operations

# USAGE

```cpp
#include "../fs.h"

using namespace std;
using namespace fs;

int main() {
 
  Filesystem fs;

  fs.readFile("test.txt", [](auto err, auto data) {

    if (err) {
      cout << err.message << endl;
      return;
    }

    cout << data << endl;
  });
}
```

# API

## CONSTRUCTOR
### Filesystem fs;
Creates a loop for file operations.

## METHODS

### fs.readFile(path, callback);
Stats, Opens, reads whole file, callback provides (`error`, `streambuf`).

### fs.writeFile(path, buffer, callback);
Opens, writes whole buffer, callback provides (`error`).

### fs.readFileSync(path);
Stats, Opens, reads whole file sync.

### fs.writeFileSync(path, buffer);
Opens, writes whole buffer sync.

### fs.stat(path, callback);
Callback provides (`error`, `stats`), where stats is a struct containing 
the following members...

### fs.statSync(path);
Returns a Stats object.

```cpp
int stats.dev
int stats.mode
int stats.nlink
int stats.uid
int stats.gid
int stats.rdev
double stats.size
double stats.ino
double stats.atime
double stats.mtime
double stats.ctime
```

### fs.cwd();
Returns a string that represents the current working directory.

### fs.mkdirSync(path, [mode]);
Create a directory sync, optionally pass the mode as an octal.

### fs.rmdirSync(path);
Remove a direcory sync.

### fs.open(path, callback);
Callback provides (`error`, `fd`) where fd is a file descriptor.

### fs.openSync(path, [mode]);
Open a file sync, optionally pass the mode as an octal.

### fs.read(fd, bufferSize, offset, callback);
Callback provides (`error`, `uv_buf_t`).

### fs.write(fd, buffer, offset, callback);
Callback provides (`error`, `uv_buf_t`).

### fs.writeSync(fd, buffer, offset, length);
Write to a file sync, returns the number of bytes written.

### fs.close(fd, callback);
Close a file. Callback provides (`error`).

### fs.closeSync(fd);
Close a file sync.

### fs.createBuffer(string);
Returns a new `uv_buf_t` from a string.

