#include <vacall_r.h>
#define FUSE_USE_VERSION 31

#include <callback.h>
#include <cuse_lowlevel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void init_handler(void *device, struct vacall_alist *alist) {
  void *userdata = va_arg_ptr(alist, void *);
  struct fuse_conn_info *conn = va_arg_struct(alist, struct fuse_conn_info *);

  gocuse_init(device, userdata, conn);
}

void init_handler_done(void *device, struct vacall_alist *alist) {
  void *userdata = va_arg_ptr(alist, void *);

  gocuse_init_done(device, userdata);
}

void destroy_handler(void *device, struct vacall_alist *alist) {
  void *userdata = va_arg_ptr(alist, void *);

  gocuse_destroy(device, userdata);
}

void open_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_open(device, req, fi);
}

void read_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  size_t size = va_arg_struct(alist, size_t);
  off_t off = va_arg_struct(alist, off_t);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_read(device, req, (unsigned long)size, off, fi);
}

void write_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  const char *buf = va_arg_struct(alist, const char *);
  size_t size = va_arg_struct(alist, size_t);
  off_t off = va_arg_struct(alist, off_t);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_write(device, req, buf, (unsigned long)size, off, fi);
}

void flush_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_flush(device, req, fi);
}

void release_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_release(device, req, fi);
}

void fsync_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  int datasync = va_arg_int(alist);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);

  gocuse_fsync(device, req, datasync, fi);
}

void ioctl_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  int cmd = va_arg_int(alist);
  void *arg = va_arg_ptr(alist, void *);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);
  unsigned int flags = va_arg_uint(alist);
  const void *in_buf = va_arg_ptr(alist, const void *);
  size_t in_bufsz = va_arg_struct(alist, size_t);
  size_t out_bufsz = va_arg_struct(alist, size_t);

  gocuse_ioctl(device, req, cmd, arg, fi, flags, in_buf,
               (unsigned long)in_bufsz, (unsigned long)out_bufsz);
}

void poll_handler(void *device, struct vacall_alist *alist) {
  fuse_req_t *req = va_arg_struct(alist, fuse_req_t *);
  struct fuse_file_info *fi = va_arg_struct(alist, struct fuse_file_info *);
  struct fuse_pollhandle *ph = va_arg_struct(alist, struct fuse_pollhandle *);

  gocuse_poll(device, req, fi, ph);
}

int gocuse_start(void *device, unsigned int major, unsigned int minor,
                 char *name, int argc, char **argv) {
  struct cuse_info ci;
  memset(&ci, 0, sizeof(ci));

  const char *dev_info_argv[] = {name};
  ci.dev_major = major;
  ci.dev_minor = minor;
  ci.dev_info_argc = 1;
  ci.dev_info_argv = dev_info_argv;
  ci.flags = CUSE_UNRESTRICTED_IOCTL;

  void *handle_init = alloc_callback(&init_handler, device);
  void *handle_init_done = alloc_callback(&init_handler_done, device);
  void *handle_destroy = alloc_callback(&destroy_handler, device);
  void *handle_open = alloc_callback(&open_handler, device);
  void *handle_read = alloc_callback(&read_handler, device);
  void *handle_write = alloc_callback(&write_handler, device);
  void *handle_flush = alloc_callback(&flush_handler, device);
  void *handle_release = alloc_callback(&release_handler, device);
  void *handle_fsync = alloc_callback(&fsync_handler, device);
  void *handle_ioctl = alloc_callback(&ioctl_handler, device);
  void *handle_poll = alloc_callback(&poll_handler, device);

  struct cuse_lowlevel_ops clop;
  memset(&clop, 0, sizeof(clop));
  clop.init = handle_init;
  clop.init_done = handle_init_done;
  clop.destroy = handle_destroy;
  clop.open = handle_open;
  clop.read = handle_read;
  clop.write = handle_write;
  clop.flush = handle_flush;
  clop.release = handle_release;
  clop.fsync = handle_fsync;
  clop.ioctl = handle_ioctl;
  clop.poll = handle_poll;

  struct fuse_session *se;
  int multithreaded;
  int res;

  se = cuse_lowlevel_setup(argc, argv, &ci, &clop, &multithreaded, NULL);
  if (se == NULL) {
    return 1;
  }

  if (multithreaded) {
    res = fuse_session_loop_mt(se, 0);
  } else {
    res = fuse_session_loop(se);
  }

  cuse_lowlevel_teardown(se);

  free_callback(handle_init);
  free_callback(handle_init_done);
  free_callback(handle_destroy);
  free_callback(handle_open);
  free_callback(handle_read);
  free_callback(handle_write);
  free_callback(handle_flush);
  free_callback(handle_release);
  free_callback(handle_fsync);
  free_callback(handle_ioctl);
  free_callback(handle_poll);

  if (res == -1) {
    return 1;
  }

  return 0;
}
