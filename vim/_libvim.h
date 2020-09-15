

// libvim was built with autoconf, but I manually copied the headers over.
// This definition is needed to tell vim autoconf ran. This will cause vim
// to define things differently. Without this, many compile errors occur.
#define HAVE_CONFIG_H 1

// wrap in extern C just once, instead of every time we include libvim
extern "C" {
#include <libvim.h>
}
