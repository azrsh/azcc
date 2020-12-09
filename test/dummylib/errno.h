#ifndef ERRNO_H
#define ERRNO_H

int *__errno_location();
#define errno (*__errno_location())

#endif
