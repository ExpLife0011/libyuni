/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** gitlab: https://gitlab.com/libyuni/libyuni/
** github: https://github.com/libyuni/libyuni/ {mirror}
*/
#ifndef __CPP_PARSER_JSON_PRODUCTIONS_HXX__
# define __CPP_PARSER_JSON_PRODUCTIONS_HXX__


// Generated by grammar-converter

# include <yuni/yuni.h>


namespace Grammar
{
namespace Private
{
namespace JSON
{
namespace Productions
{

	enum { count = 18 };


	static const uint symbolsForRule0[2] = {18,  (uint) -1 };
	static const uint symbolsForRule1[2] = {14,  (uint) -1 };
	static const uint symbolsForRule2[3] = {7, 8,  (uint) -1 };
	static const uint symbolsForRule3[4] = {7, 17, 8,  (uint) -1 };
	static const uint symbolsForRule4[2] = {19,  (uint) -1 };
	static const uint symbolsForRule5[4] = {19, 3, 17,  (uint) -1 };
	static const uint symbolsForRule6[4] = {12, 4, 20,  (uint) -1 };
	static const uint symbolsForRule7[3] = {5, 6,  (uint) -1 };
	static const uint symbolsForRule8[4] = {5, 15, 6,  (uint) -1 };
	static const uint symbolsForRule9[2] = {20,  (uint) -1 };
	static const uint symbolsForRule10[4] = {20, 3, 15,  (uint) -1 };
	static const uint symbolsForRule11[2] = {12,  (uint) -1 };
	static const uint symbolsForRule12[2] = {11,  (uint) -1 };
	static const uint symbolsForRule13[2] = {18,  (uint) -1 };
	static const uint symbolsForRule14[2] = {14,  (uint) -1 };
	static const uint symbolsForRule15[2] = {13,  (uint) -1 };
	static const uint symbolsForRule16[2] = {9,  (uint) -1 };
	static const uint symbolsForRule17[2] = {10,  (uint) -1 };




	struct Entry
	{
		//! Head symbol index
		const uint head;
		//! Total number of symbols
		const uint symbolCount;
		//! List of symbols
		const uint* symbols;
	};


	static const Entry table[count] = 
	{
		/* 0 */ { 16,  /*symbols*/ 1, symbolsForRule0 },
		/* 1 */ { 16,  /*symbols*/ 1, symbolsForRule1 },
		/* 2 */ { 18,  /*symbols*/ 2, symbolsForRule2 },
		/* 3 */ { 18,  /*symbols*/ 3, symbolsForRule3 },
		/* 4 */ { 17,  /*symbols*/ 1, symbolsForRule4 },
		/* 5 */ { 17,  /*symbols*/ 3, symbolsForRule5 },
		/* 6 */ { 19,  /*symbols*/ 3, symbolsForRule6 },
		/* 7 */ { 14,  /*symbols*/ 2, symbolsForRule7 },
		/* 8 */ { 14,  /*symbols*/ 3, symbolsForRule8 },
		/* 9 */ { 15,  /*symbols*/ 1, symbolsForRule9 },
		/* 10 */ { 15,  /*symbols*/ 3, symbolsForRule10 },
		/* 11 */ { 20,  /*symbols*/ 1, symbolsForRule11 },
		/* 12 */ { 20,  /*symbols*/ 1, symbolsForRule12 },
		/* 13 */ { 20,  /*symbols*/ 1, symbolsForRule13 },
		/* 14 */ { 20,  /*symbols*/ 1, symbolsForRule14 },
		/* 15 */ { 20,  /*symbols*/ 1, symbolsForRule15 },
		/* 16 */ { 20,  /*symbols*/ 1, symbolsForRule16 },
		/* 17 */ { 20,  /*symbols*/ 1, symbolsForRule17 },
	};







} // namespace Productions
} // namespace JSON
} // namespace Private
} // namespace Grammar

#endif // __CPP_PARSER_JSON_PRODUCTIONS_HXX__
