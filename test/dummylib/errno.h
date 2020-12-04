#ifndef ERRNO_H
#define ERRNO_H

int *__errno_location(void);
#define errno (*__errno_location())

#endif
