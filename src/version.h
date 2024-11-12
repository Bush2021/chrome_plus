#ifndef VERSION_H_
#define VERSION_H_

#define RELEASE_VER_MAIN 1
#define RELEASE_VER_SUB 11
#define RELEASE_VER_FIX 0

#define TOSTRING2(arg) #arg
#define TOSTRING(arg) TOSTRING2(arg)

#define RELEASE_VER_STR      \
  TOSTRING(RELEASE_VER_MAIN) \
  "." TOSTRING(RELEASE_VER_SUB) "." TOSTRING(RELEASE_VER_FIX)

#endif  // VERSION_H_
