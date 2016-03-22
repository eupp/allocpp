#ifndef ALLOCPP_POINTER_CONCEPT_TAG_H
#define ALLOCPP_POINTER_CONCEPT_TAG_H

namespace allocpp { namespace concepts {

struct nullable_ptr_tag {};
struct object_ptr_tag : public nullable_ptr_tag {};
struct array_ptr_tag : public object_ptr_tag {};
struct random_access_ptr_tag : public array_ptr_tag {};

}}

#endif //ALLOCPP_POINTER_CONCEPT_TAG_H
