/*
 * Copyright (C) 2013 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "private/bionic_time_conversions.h"
#include "private/bionic_constants.h"
bool timespec_from_timeval(timespec& ts, const timeval& tv) {
  // Whole seconds can just be copied.
  ts.tv_sec = tv.tv_sec;
  // But we might overflow when converting microseconds to nanoseconds.
  if (tv.tv_usec >= 1000000 || tv.tv_usec < 0) {
    return false;
  }
  ts.tv_nsec = tv.tv_usec * 1000;
  return true;
}
void timespec_from_ms(timespec& ts, const int ms) {
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
}
void timeval_from_timespec(timeval& tv, const timespec& ts) {
  tv.tv_sec = ts.tv_sec;
  tv.tv_usec = ts.tv_nsec / 1000;
}
void monotonic_time_from_realtime_time(timespec& monotonic_time, const timespec& realtime_time) {
  monotonic_time = realtime_time;
  timespec cur_monotonic_time;
  clock_gettime(CLOCK_MONOTONIC, &cur_monotonic_time);
  timespec cur_realtime_time;
  clock_gettime(CLOCK_REALTIME, &cur_realtime_time);
  monotonic_time.tv_nsec -= cur_realtime_time.tv_nsec;
  monotonic_time.tv_nsec += cur_monotonic_time.tv_nsec;
  if (monotonic_time.tv_nsec >= NS_PER_S) {
    monotonic_time.tv_nsec -= NS_PER_S;
    monotonic_time.tv_sec += 1;
  } else if (monotonic_time.tv_nsec < 0) {
    monotonic_time.tv_nsec += NS_PER_S;
    monotonic_time.tv_sec -= 1;
  }
  monotonic_time.tv_sec -= cur_realtime_time.tv_sec;
  monotonic_time.tv_sec += cur_monotonic_time.tv_sec;
}

/*
 * Copyright (C) 2013 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
static int futimesat(int fd, const char* path, const timeval tv[2], int flags) {
  timespec ts[2];
  if (tv && (!timespec_from_timeval(ts[0], tv[0]) || !timespec_from_timeval(ts[1], tv[1]))) {
    errno = EINVAL;
    return -1;
  }
  return utimensat(fd, path, tv ? ts : nullptr, flags);
}
int utimes(const char* path, const timeval tv[2]) {
  return futimesat(AT_FDCWD, path, tv, 0);
}
int lutimes(const char* path, const timeval tv[2]) {
  return futimesat(AT_FDCWD, path, tv, AT_SYMLINK_NOFOLLOW);
}
int futimesat(int fd, const char* path, const timeval tv[2]) {
  return futimesat(fd, path, tv, 0);
}
int futimes(int fd, const timeval tv[2]) {
  timespec ts[2];
  if (tv && (!timespec_from_timeval(ts[0], tv[0]) || !timespec_from_timeval(ts[1], tv[1]))) {
    errno = EINVAL;
    return -1;
  }
  return futimens(fd, tv ? ts : nullptr);
}
