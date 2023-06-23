#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define IFERROR(c, msg) ({   \
  if (c) {                   \
    fprintf(stderr, msg ": %s\n", strerror(errno)); \
    return;                  \
  }                          \
})

void run(void) {
  int fd, err;
  uint64_t temp, volt, units;
  fd = open("/dev/intluv", O_RDWR);
  IFERROR(fd < 0, "Could not open /dev/intluv");
  err = pread(fd, (char*) &temp, sizeof(uint64_t), 0x1A2);
  IFERROR(err < 0, "Could not read MSR_TEMPERATURE");
  err = pread(fd, (char*) &volt, sizeof(uint64_t), 0x150);
  IFERROR(err < 0, "Could not read MSR_VOLTAGE");
  err = pread(fd, (char*) &units, sizeof(uint64_t), 0x606);
  IFERROR(err < 0, "Could not read MSR_UNITS");
  printf(
        " SUCCESSFULLY READ MSRS \n"
        "------------------------\n"
        " MSR_TEMPERATURE: %0lX  \n"
        " MSR_VOLTAGE:     %0lX  \n"
        " MSR_UNITS:       %0lX  \n"
        "------------------------\n",
        temp, volt, units);
  volt = 0xABACAB;
  err = pwrite(fd, (char*) &volt, sizeof(uint64_t), 0x150);
  IFERROR(err < 0, "Could not write to MSR_VOLTAGE");
  printf("SUCCESSFULLY WRITTEN TO VOLTAGE MSR\n");
  close(fd);
}

int main(void) {
  run();
  fflush(stderr);
  fflush(stdout);
  return 0;
}
