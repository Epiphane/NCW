//
// DereferenceIterator.h
//
// Simple iterator subclass for unique/shared pointers
//
// It calls .get() on the smart pointers before returning them,
//    so the caller doesn't have to worry about the smart
//    pointer internals and can just work with the pointer itself.
//
// Reference: https://jonasdevlieghere.com/containers-of-unique-pointers/
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

namespace CubeWorld
{

namespace Shared
{

template <class BaseIterator> class DereferenceIterator : public BaseIterator {
public:
   using value_type = typename BaseIterator::value_type::element_type;
   using pointer = value_type *;
   using reference = value_type &;

   DereferenceIterator(const BaseIterator &other) : BaseIterator(other) {}

   reference operator*() const { return *(BaseIterator::operator*()); }
   pointer operator->() const { return this->BaseIterator::operator*().get(); }
   pointer GetPointer() const { return this->BaseIterator::operator*().get(); }
   reference operator[](size_t n) const {
      return *(BaseIterator::operator[](n));
   }
};

template <typename Iterator> DereferenceIterator<Iterator> MakeDereferenceIterator(Iterator t) {
   return DereferenceIterator<Iterator>(t);
}

} // namespace Shared

} // namespace CubeWorld
