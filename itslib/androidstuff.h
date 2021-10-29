#include <sys/stat.h>
#ifdef __cplusplus
extern "C" {
#endif
  
#include <sys/cdefs.h>
#include <sys/types.h>
#include <linux/time.h>
/* POSIX says <sys/time.h> gets you most of <sys/select.h> and may get you all of it. */
#include <sys/select.h>
  
int futimesat(int, const char*, const struct timeval[2]);
int lutimes(const char*, const struct timeval[2]);
#ifdef __cplusplus
}
#endif
