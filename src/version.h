#ifndef CHROME_PLUS_SRC_VERSION_H_
#define CHROME_PLUS_SRC_VERSION_H_

#ifndef RELEASE_VER_MAIN
#define RELEASE_VER_MAIN 1
#endif

#ifndef RELEASE_VER_SUB
#define RELEASE_VER_SUB 18
#endif

#ifndef RELEASE_VER_FIX
#define RELEASE_VER_FIX 2
#endif

#ifndef RELEASE_VER_PRE_SUFFIX
#define RELEASE_VER_PRE_SUFFIX ""
#endif

#define TOSTRING2(arg) #arg
#define TOSTRING(arg) TOSTRING2(arg)

#ifndef RELEASE_VER_STR
#define RELEASE_VER_STR          \
  TOSTRING(RELEASE_VER_MAIN)     \
  "." TOSTRING(RELEASE_VER_SUB)  \
  "." TOSTRING(RELEASE_VER_FIX)  \
  RELEASE_VER_PRE_SUFFIX
#endif

#endif  // CHROME_PLUS_SRC_VERSION_H_
