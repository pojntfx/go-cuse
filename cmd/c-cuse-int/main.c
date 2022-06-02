#define FUSE_USE_VERSION 31

#include <cuse_lowlevel.h>
#include <errno.h>
#include <string.h>

enum {
  FIOC_GET_SIZE = 0,
  FIOC_SET_SIZE = 1,
};

int v = 0;

static void handle_open(fuse_req_t req, struct fuse_file_info *fi) {
  fuse_reply_open(req, fi);
}

static void handle_ioctl(fuse_req_t req, int cmd, void *arg,
                         struct fuse_file_info *fi, unsigned flags,
                         const void *in_buf, size_t in_bufsz,
                         size_t out_bufsz) {
  switch (cmd) {
  case FIOC_GET_SIZE:
    if (out_bufsz == 0) {
      struct iovec iov = {arg, sizeof(int)};

      fuse_reply_ioctl_retry(req, NULL, 0, &iov, 1);
    } else
      fuse_reply_ioctl(req, 0, &v, sizeof(v));

    break;

  case FIOC_SET_SIZE:
    if (in_bufsz == 0) {
      struct iovec iov = {arg, sizeof(int)};

      fuse_reply_ioctl_retry(req, &iov, 1, NULL, 0);
    } else {
      v = *(int *)in_buf;

      fuse_reply_ioctl(req, 0, NULL, 0);
    }

    break;

  default:
    fuse_reply_err(req, EINVAL);
  }
}

int main(int argc, char **argv) {
  struct cuse_info ci;
  memset(&ci, 0, sizeof(ci));

  const char *dev_info_argv[] = {"DEVNAME=c-cuse-int"};
  ci.dev_major = 70;
  ci.dev_minor = 72;
  ci.dev_info_argc = 1;
  ci.dev_info_argv = dev_info_argv;
  ci.flags = CUSE_UNRESTRICTED_IOCTL;

  struct cuse_lowlevel_ops clop;
  memset(&clop, 0, sizeof(clop));
  clop.open = handle_open;
  clop.ioctl = handle_ioctl;

  return cuse_lowlevel_main(argc, argv, &ci, &clop, NULL);
}
