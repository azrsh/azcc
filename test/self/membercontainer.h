#ifndef MEMBERCONTAINER
#define MEMBERCONTAINER

#include "container.h"
#include "variable.h"

typedef struct MemberContainer MemberContainer;
MemberContainer *new_member_container();
Variable *member_container_get(MemberContainer *container, String name);
bool member_container_push(MemberContainer *container, Variable *varibale);
size_t member_container_align(MemberContainer *container);
size_t member_container_aligned_size(MemberContainer *container);

#endif
