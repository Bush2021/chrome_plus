#ifndef CHROME_PLUS_SRC_VERSION_H_
#define CHROME_PLUS_SRC_VERSION_H_

#define RELEASE_VER_MAIN 1
#define RELEASE_VER_SUB 16
#define RELEASE_VER_FIX 0
#define RELEASE_VER_PRE_SUFFIX ""

#define TOSTRING2(arg) #arg
#define TOSTRING(arg) TOSTRING2(arg)

#define RELEASE_VER_STR                         \
  TOSTRING(RELEASE_VER_MAIN)                    \
  "." TOSTRING(RELEASE_VER_SUB)                 \
  "." TOSTRING(RELEASE_VER_FIX)                 \
  RELEASE_VER_PRE_SUFFIX

#endif  // CHROME_PLUS_SRC_VERSION_H_
