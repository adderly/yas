
// Copyright (c) 2010-2014 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef _yas__binary__fusion_tuple_serializer_hpp
#define _yas__binary__fusion_tuple_serializer_hpp

#include <yas/detail/config/config.hpp>

#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/selector.hpp>
#include <yas/detail/io/serialization_exception.hpp>
#include <yas/detail/preprocessor/preprocessor.hpp>

#include <boost/fusion/tuple.hpp>

namespace yas {
namespace detail {

/***************************************************************************/

#define YAS__BINARY__WRITE_BOOST_FUSION_TUPLE_ITEM(unused, idx, type) \
	if ( is_fundamental_and_sizeof_is<YAS_PP_CAT(type, idx), 1>::value ) \
		ar.write(&boost::fusion::at_c<idx>(tuple), sizeof(YAS_PP_CAT(type, idx))); \
	else \
		ar & boost::fusion::at_c<idx>(tuple);

#define YAS__BINARY__READ_BOOST_FUSION_TUPLE_ITEM(unused, idx, type) \
	if ( is_fundamental_and_sizeof_is<YAS_PP_CAT(type, idx), 1>::value ) \
		ar.read(&boost::fusion::at_c<idx>(tuple), sizeof(YAS_PP_CAT(type, idx))); \
	else \
		ar & boost::fusion::at_c<idx>(tuple);

#define YAS__BINARY__GENERATE_EMPTY_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	template<> \
	struct serializer<type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::binary, direction::out, boost::fusion::tuple<> > \
	{ \
		template<typename Archive> \
		static Archive& apply(Archive& ar, const boost::fusion::tuple<>&) { return ar; } \
	};

#define YAS__BINARY__GENERATE_EMPTY_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	template<> \
	struct serializer<type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::binary, direction::in, boost::fusion::tuple<> > \
	{ \
		template<typename Archive> \
		static Archive& apply(Archive& ar, boost::fusion::tuple<>&) { return ar; } \
	};

#define YAS__BINARY__GENERATE_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC(unused, count, text) \
	template<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), typename T)> \
	struct serializer<type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::binary, direction::out, \
		boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)> > \
	{ \
		template<typename Archive> \
		static Archive& apply(Archive& ar, \
			const boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)>& tuple) \
		{ \
			ar.write((std::uint8_t)YAS_PP_INC(count)); \
			YAS_PP_REPEAT( \
				YAS_PP_INC(count), \
				YAS__BINARY__WRITE_BOOST_FUSION_TUPLE_ITEM, \
				T \
			) \
			return ar; \
		} \
	};

#define YAS__BINARY__GENERATE_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(count) \
	YAS__BINARY__GENERATE_EMPTY_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	YAS_PP_REPEAT( \
		count, \
		YAS__BINARY__GENERATE_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC, \
		~ \
	)

#define YAS__BINARY__GENERATE_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC(unused, count, text) \
	template<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), typename T)> \
	struct serializer<type_prop::not_a_pod,ser_method::use_internal_serializer, \
		archive_type::binary, direction::in, \
		boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)> > \
	{ \
		template<typename Archive> \
		static Archive& apply(\
			Archive& ar, \
			boost::fusion::tuple<YAS_PP_ENUM_PARAMS(YAS_PP_INC(count), T)>& tuple) \
		{ \
			std::uint8_t size = 0; \
			ar.read(size); \
			if ( size != YAS_PP_INC(count) ) YAS_THROW_BAD_SIZE_ON_DESERIALIZE_FUSION("fusion::tuple"); \
			YAS_PP_REPEAT( \
				YAS_PP_INC(count), \
				YAS__BINARY__READ_BOOST_FUSION_TUPLE_ITEM, \
				T \
			) \
			return ar; \
		} \
	};

#define YAS__BINARY__GENERATE_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(count) \
	YAS__BINARY__GENERATE_EMPTY_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_SPEC() \
	YAS_PP_REPEAT( \
		count, \
		YAS__BINARY__GENERATE_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTION_SPEC, \
		~ \
	)

/***************************************************************************/

YAS__BINARY__GENERATE_SAVE_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(FUSION_MAX_VECTOR_SIZE)
YAS__BINARY__GENERATE_LOAD_SERIALIZE_BOOST_FUSION_TUPLE_FUNCTIONS_SPEC(FUSION_MAX_VECTOR_SIZE)

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // _yas__binary__fusion_tuple_serializer_hpp
