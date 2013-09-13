/*! \file traits.hpp
    \brief Internal type trait support
    \ingroup Internal */
/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_DETAILS_TRAITS_HPP_
#define CEREAL_DETAILS_TRAITS_HPP_

#include <type_traits>
#include <typeindex>
#include <memory>

#include <cereal/access.hpp>

namespace cereal
{
  namespace traits
  {
    typedef std::true_type yes;
    typedef std::false_type no;

    //! Creates a test for whether a non const member function exists
    /*! This creates a class derived from std::integral_constant that will be true if
        the type has the proper member function for the given archive. */
    #define CEREAL_MAKE_HAS_MEMBER_TEST(name)                                                                                      \
    namespace detail                                                                                                               \
    {                                                                                                                              \
      template <class T, class A>                                                                                                  \
      struct has_member_##name##_impl                                                                                              \
      {                                                                                                                            \
        template <class TT, class AA>                                                                                              \
        static auto test(int) -> decltype( cereal::access::member_##name( std::declval<AA&>(), std::declval<TT&>() ), yes());      \
        template <class, class>                                                                                                    \
        static no test(...);                                                                                                       \
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;                                               \
      };                                                                                                                           \
    } /* end namespace detail */                                                                                                   \
    template <class T, class A>                                                                                                    \
    struct has_member_##name : std::integral_constant<bool, detail::has_member_##name##_impl<T, A>::value> {};

    //! Creates a test for whether a non const non-member function exists
    /*! This creates a class derived from std::integral_constant that will be true if
        the type has the proper non-member function for the given archive. */
    #define CEREAL_MAKE_HAS_NON_MEMBER_TEST(name)                                                                                  \
    namespace detail                                                                                                               \
    {                                                                                                                              \
      template <class T, class A>                                                                                                  \
      struct has_non_member_##name##_impl                                                                                          \
      {                                                                                                                            \
        template <class TT, class AA>                                                                                              \
        static auto test(int) -> decltype( name( std::declval<AA&>(), std::declval<TT&>() ), yes());                               \
        template <class, class>                                                                                                    \
        static no test( ... );                                                                                                     \
        static const bool value = std::is_same<decltype( test<T, A>( 0 ) ), yes>::value;                                           \
      };                                                                                                                           \
    } /* end namespace detail */                                                                                                   \
    template <class T, class A>                                                                                                    \
    struct has_non_member_##name : std::integral_constant<bool, detail::has_non_member_##name##_impl<T, A>::value> {};

    // ######################################################################
    // Member load_and_allocate
    template<typename T, typename A>
    struct has_member_load_and_allocate :
      std::integral_constant<bool,  std::is_same<decltype( access::load_and_allocate<T>( std::declval<A&>() ) ), T*>::value> {};

    // ######################################################################
    // Non Member load_and_allocate
    template<typename T, typename A>
    struct has_non_member_load_and_allocate : std::integral_constant<bool,
      std::is_same<decltype( LoadAndAllocate<T>::load_and_allocate( std::declval<A&>() ) ), T*>::value> {};

    // ######################################################################
    // Has either a member or non member allocate
    template<typename T, typename A>
    struct has_load_and_allocate : std::integral_constant<bool,
      has_member_load_and_allocate<T, A>::value || has_non_member_load_and_allocate<T, A>::value>
    { };

    // ######################################################################
    // Member Serialize
    CEREAL_MAKE_HAS_MEMBER_TEST(serialize);

    // ######################################################################
    // Non Member Serialize
    CEREAL_MAKE_HAS_NON_MEMBER_TEST(serialize);

    // ######################################################################
    // Member Load
    CEREAL_MAKE_HAS_MEMBER_TEST(load);

    // ######################################################################
    // Non Member Load
    CEREAL_MAKE_HAS_NON_MEMBER_TEST(load);

    // ######################################################################
    // Member Save
    namespace detail
    {
      template <class T, class A>
      struct has_member_save_impl
      {
        template <class TT, class AA>
        static auto test(int) -> decltype( cereal::access::member_save( std::declval<AA&>(), std::declval<TT const &>() ), yes());
        template <class, class>
        static no test(...);
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;

        template <class TT, class AA>
        static auto test2(int) -> decltype( cereal::access::member_save_non_const( std::declval<AA &>(), std::declval<typename std::remove_const<TT>::type&>() ), yes());
        template <class, class>
        static no test2(...);
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value;
      };
    } // end namespace detail

    template <class T, class A>
    struct has_member_save : std::integral_constant<bool, detail::has_member_save_impl<T, A>::value>
    {
      typedef typename detail::has_member_save_impl<T, A> check;
      static_assert( check::value || !check::not_const_type,
        "cereal detected a non-const save.\n"
        "save member functions must always be const" );
    };

    // ######################################################################
    // Non-const Member Save
    namespace detail
    {
      template <class T, class A>
      struct has_non_member_save_impl
      {
        template <class TT, class AA>
        static auto test(int) -> decltype( save( std::declval<AA&>(), std::declval<TT const &>() ), yes());
        template <class, class>
        static no test(...);
        static const bool value = std::is_same<decltype(test<T, A>(0)), yes>::value;

        template <class TT, class AA>
        static auto test2(int) -> decltype( save( std::declval<AA &>(), std::declval<typename std::remove_const<TT>::type&>() ), yes());
        template <class, class>
        static no test2(...);
        static const bool not_const_type = std::is_same<decltype(test2<T, A>(0)), yes>::value;
      };
    } // end namespace detail

    template <class T, class A>
    struct has_non_member_save : std::integral_constant<bool, detail::has_non_member_save_impl<T, A>::value>
    {
      typedef typename detail::has_non_member_save_impl<T, A> check;
      static_assert( check::value || !check::not_const_type,
        "cereal detected a non-const type parameter in non-member save.\n"
        "save non-member functions must always pass their types as const" );
    };

    // ######################################################################
    template <class T, class InputArchive, class OutputArchive>
    struct has_member_split : std::integral_constant<bool,
      has_member_load<T, InputArchive>::value && has_member_save<T, OutputArchive>::value> {};

    // ######################################################################
    template <class T, class InputArchive, class OutputArchive>
    struct has_non_member_split : std::integral_constant<bool,
      has_non_member_load<T, InputArchive>::value && has_non_member_save<T, OutputArchive>::value> {};

    // ######################################################################
    template <class T, class OutputArchive>
    struct is_output_serializable : std::integral_constant<bool,
      has_member_save<T, OutputArchive>::value ^
      has_non_member_save<T, OutputArchive>::value ^
      has_member_serialize<T, OutputArchive>::value ^
      has_non_member_serialize<T, OutputArchive>::value> {};

    // ######################################################################
    template <class T, class InputArchive>
      struct is_input_serializable : std::integral_constant<bool,
          has_member_load<T, InputArchive>::value ^
          has_non_member_load<T, InputArchive>::value ^
          has_member_serialize<T, InputArchive>::value ^
          has_non_member_serialize<T, InputArchive>::value> {};

    // ######################################################################
    namespace detail
    {
      template <class T, class A>
      struct is_specialized_member_serialize : std::integral_constant<bool,
        !std::is_base_of<std::false_type, specialize<A, T, specialization::member_serialize>>::value> {};

      template <class T, class A>
      struct is_specialized_member_load_save : std::integral_constant<bool,
        !std::is_base_of<std::false_type, specialize<A, T, specialization::member_load_save>>::value> {};

      template <class T, class A>
      struct is_specialized_non_member_serialize : std::integral_constant<bool,
        !std::is_base_of<std::false_type, specialize<A, T, specialization::non_member_serialize>>::value> {};

      template <class T, class A>
      struct is_specialized_non_member_load_save : std::integral_constant<bool,
        !std::is_base_of<std::false_type, specialize<A, T, specialization::non_member_load_save>>::value> {};

      // Considered an error if specialization exists for more than one type
      template <class T, class A>
      struct is_specialized_error : std::integral_constant<bool,
        (is_specialized_member_serialize<T, A>::value +
         is_specialized_member_load_save<T, A>::value +
         is_specialized_non_member_serialize<T, A>::value +
         is_specialized_non_member_load_save<T, A>::value) <= 1> {};
    } // namespace detail

    template <class T, class A>
    struct is_specialized : std::integral_constant<bool,
      detail::is_specialized_member_serialize<T, A>::value ||
      detail::is_specialized_member_load_save<T, A>::value ||
      detail::is_specialized_non_member_serialize<T, A>::value ||
      detail::is_specialized_non_member_load_save<T, A>::value>
    {
      static_assert(detail::is_specialized_error<T, A>::value, "More than one explicit specialization detected for type.");
    };

    template <class T, class A>
    struct is_specialized_member_serialize : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_member_serialize<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_member_serialize<T, A>::value && has_member_serialize<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_member_serialize<T, A>::value),
                     "cereal detected member serialization specialization but no member serialize function" );
    };

    template <class T, class A>
    struct is_specialized_member_load : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value && has_member_load<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value),
                     "cereal detected member load specialization but no member load function" );
    };

    template <class T, class A>
    struct is_specialized_member_save : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value && has_member_save<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_member_load_save<T, A>::value),
                     "cereal detected member save specialization but no member save function" );
    };

    template <class T, class A>
    struct is_specialized_non_member_serialize : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_non_member_serialize<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_non_member_serialize<T, A>::value && has_non_member_serialize<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_non_member_serialize<T, A>::value),
                     "cereal detected non-member serialization specialization but no non-member serialize function" );
    };

    template <class T, class A>
    struct is_specialized_non_member_load : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value && has_non_member_load<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value),
                     "cereal detected non-member load specialization but no non-member load function" );
    };

    template <class T, class A>
    struct is_specialized_non_member_save : std::integral_constant<bool,
      is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value>
    {
      static_assert( (is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value && has_non_member_save<T, A>::value)
                     || !(is_specialized<T, A>::value && detail::is_specialized_non_member_load_save<T, A>::value),
                     "cereal detected non-member save specialization but no non-member save function" );
    };

    // ######################################################################
    namespace detail
    {
      struct base_class_id
      {
        template<class T>
          base_class_id(T const * const t) :
          type(typeid(T)),
          ptr(t),
          hash(std::hash<std::type_index>()(typeid(T)) ^ (std::hash<void const *>()(t) << 1))
          { }

          bool operator==(base_class_id const & other) const
          { return (type == other.type) && (ptr == other.ptr); }

          std::type_index type;
          void const * ptr;
          size_t hash;
      };
      struct base_class_id_hash { size_t operator()(base_class_id const & id) const { return id.hash; }  };
    } // namespace detail


    // ######################################################################
    //! A macro to use to restrict which types of archives your function will work for.
    /*! This requires you to have a template class parameter named Archive and replaces the void return
        type for your function.

        INTYPE refers to the input archive type you wish to restrict on.
        OUTTYPE refers to the output archive type you wish to restrict on.

        For example, if we want to limit a serialize to only work with binary serialization:

        @code{.cpp}
        template <class Archive>
        CEREAL_ARCHIVE_RESTRICT(BinaryInputArchive, BinaryOutputArchive)
        serialize( Archive & ar, MyCoolType & m )
        {
          ar & m;
        }
        @endcode

        If you need to do more restrictions in your enable_if, you will need to do this by hand.
     */
    #define CEREAL_ARCHIVE_RESTRICT(INTYPE, OUTTYPE) \
    typename std::enable_if<std::is_same<Archive, INTYPE>::value || std::is_same<Archive, OUTTYPE>::value, void>::type
  } // namespace traits

  // ######################################################################
  namespace detail
  {
    template <class T, class A, bool Member = traits::has_member_load_and_allocate<T, A>::value, bool NonMember = traits::has_non_member_load_and_allocate<T, A>::value>
    struct Load
    {
      static_assert( !sizeof(T), "Cereal detected both member and non member load_and_allocate functions!" );
      static T * load_andor_allocate( A & ar )
      { return nullptr; }
    };

    template <class T, class A>
    struct Load<T, A, false, false>
    {
      static_assert( std::is_default_constructible<T>::value,
                     "Trying to serialize a an object with no default constructor.\n\n"
                     "Types must either be default constructible or define either a member or non member Construct function.\n"
                     "Construct functions generally have the signature:\n\n"
                     "template <class Archive>\n"
                     "static T * load_and_allocate(Archive & ar)\n"
                     "{\n"
                     "  var a;\n"
                     "  ar & a\n"
                     "  return new T(a);\n"
                     "}\n\n" );
      static T * load_andor_allocate( A & ar )
      { return new T(); }
    };

    template <class T, class A>
    struct Load<T, A, true, false>
    {
      static T * load_andor_allocate( A & ar )
      {
        return access::load_and_allocate<T>( ar );
      }
    };

    template <class T, class A>
    struct Load<T, A, false, true>
    {
      static T * load_andor_allocate( A & ar )
      {
        return LoadAndAllocate<T>::load_and_allocate( ar );
      }
    };
  } // namespace detail
} // namespace cereal

#endif // CEREAL_DETAILS_TRAITS_HPP_
