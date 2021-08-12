[TOC]

## 1 语法

```
Reading
  file(READ <filename> <out-var> [...])
  file(STRINGS <filename> <out-var> [...])
  file(<HASH> <filename> <out-var>)
  file(TIMESTAMP <filename> <out-var> [...])

Writing
  file({WRITE | APPEND} <filename> <content>...)
  file({TOUCH | TOUCH_NOCREATE} [<file>...])
  file(GENERATE OUTPUT <output-file> [...])

Filesystem
  file({GLOB | GLOB_RECURSE} <out-var> [...] [<globbing-expr>...])
  file(RENAME <oldname> <newname>)
  file({REMOVE | REMOVE_RECURSE } [<files>...])
  file(MAKE_DIRECTORY [<dir>...])
  file({COPY | INSTALL} <file>... DESTINATION <dir> [...])
  file(SIZE <filename> <out-var>)
  file(READ_SYMLINK <linkname> <out-var>)
  file(CREATE_LINK <original> <linkname> [...])

Path Conversion
  file(RELATIVE_PATH <out-var> <directory> <file>)
  file({TO_CMAKE_PATH | TO_NATIVE_PATH} <path> <out-var>)

Transfer
  file(DOWNLOAD <url> <file> [...])
  file(UPLOAD <file> <url> [...])

Locking
  file(LOCK <path> [...])
```

## 2 读文件


## 3 写文件


## 4 文件系统

```
file(GLOB <variable>
     [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS]
     [<globbing-expressions>...])
file(GLOB_RECURSE <variable> [FOLLOW_SYMLINKS]
     [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS]
     [<globbing-expressions>...])
```

Generate a list of files that match the <globbing-expressions> and store it into the <variable>. Globbing expressions are similar to regular expressions, but much simpler. If RELATIVE flag is specified, the results will be returned as relative paths to the given path. The results will be ordered lexicographically.

If the CONFIGURE_DEPENDS flag is specified, CMake will add logic to the main build system check target to rerun the flagged GLOB commands at build time. If any of the outputs change, CMake will regenerate the build system.

By default GLOB lists directories - directories are omitted in result if LIST_DIRECTORIES is set to false.

## 5 路径转换

## 6 传输文件

## 7 锁定文件


