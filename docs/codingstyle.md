Coding style for libyuni
========================



GENERAL
-------

 * Do not use Java-like braces

   GOOD:
		if ()
		{
			doSomething();
		}

   BAD:
		if () {
			doSomething();
		}

 * Use UTF-8 in all files

 * Use tabs instead of whitespaces (we usually set our editors to 4
   whitespaces for 1 tab, but the choice is up to you)

 * Do not end your namespaces or switch statements with a semicolon

 * Extensions for files:

   - header: `.h`
   - source code: `.cpp`
   - source code for template: `.hxx`
   - Text files: `.txt`
   - Markdown: `.md`

 * Always leave one space before and after binary and ternary operators
   Only leave one space after semi-colons in "for" statements

   GOOD:
		if (10 == a && 42 == b)
		{}

   BAD:
		if (a==10&&b==42)

   GOOD:
		for (int i = 0; i != 10; ++i)

   BAD:
		for(int i=0;i<10;++i)

 * Keywords are not function calls.
   Function names are not separated from the first parenthesis:

   GOOD:
		foo();
		myObject.foo(24);

   BAD:
		foo ();

   Keywords are separated from the first parenthesis by one space:

   GOOD:
		if (true)
		while (true)

   BAD:
		if(myCondition)

   Return values should not be enclosed in parentheses:

   GOOD:
		return true;
		return VALUE_OK == result;

   BAD:
		return (VALUE_OK == result);


 * Use the following indenting for "switch" statements:

	switch (test)
	{
		case 1:
		{
			// Do something
			break;
		}
		default:
			// Do something else
	} // No semi-colon here


 * Avoid magic numbers

   GOOD:
		if (foo < YUNI_I_CAN_PUSH_ON_THE_RED_BUTTON)
			startThermoNuclearWar();

   BAD:
		if (foo < 42.7)
			startThermoNuclearWar();


 * Use the following indenting for constructor definitions:
		inline MyClass::MyClass(bool test)
			: pTest(test)
			, pValue(0)
			, pName("Default name")
		{}

 * Use the keyword `nullptr` instead of `0` or `NULL` for pointers

 * Prefer enums for integer constants






NAMING CONVENTIONS
------------------

 * namespaces (Pascal case: camel case + begins with an upper case)
   GOOD:
		namespace HereIsANewNamespace
		{
		}

		namespace // anonymous
		{
		}

 * Classes (camel case):

   GOOD:
		class IAmAClass
		{};

   BAD:
		class iAmClass
		{};
		class I_am_class
		{};

 * methods (camel case + begins with a lower case)
   method parameters (camel case + begins with a lower case)

   GOOD:
		void myMethod(uint myVeryLongParameter);

 * private/protected variables.
   Attributes should be separated only by one space from their type.
   Private attributes must start with 'p' then use camel case.
   Public attributes must start with a lower case letter followed by camel case.

   GOOD:
		public:
			int publicAttribute;
		private:
			int pPrivateAttribute;
			float pAccount;

   BAD:
		int	   pTest;
		int PTest;
		int m_test;

 * Always prefer a variable name that describes what the variable is used for

   GOOD:
		if (hours < 24 && minutes < 60 && seconds < 60)

   BAD:
		if (a < 24 && b < 60 && c < 60)

 * Following the same reasoning, template parameters naming should indicate the
   kind of types expected: (assuming: `Yuni::String pName;`)

   GOOD:
		template<class StringT> void name(const StringT& name) { pName = name; }

   BAD:
		template<class T> void name(const T& name) { pName = name; }
		void name(const char* name) { pName = name; }


 * Macros and defines are all in uppercase
   All Macros that define an exported header of yuni must have the prefix YUNI_

 * The symbol to indicate a pointer or a reference must be attached to the
   type because it is a part of it:

   GOOD:
		void foo(const String& s);

   BAD:
		void foo(const String & s);
		void foo(const String &s);

 * In any thread-safe class, the methods providing thread-unsafe access
   to the instance (usually methods that are equivalent to thread-safe methods
   not using locks on purpose) must be suffixed with "WL" (stands for
   "without lock").

 * When a class acts as a single entry point for a Yuni module, and
   offers a continuous service during the program's execution, this
   class should be named Service, e.g.:
   "Yuni::Audio::QueueService"





COMMENTS
--------

 * Use C++ Doyxgen style

   GOOD:
		/*!
		** \brief Some brief description here
		** \param paramName Description
		*/

   BAD:
		/*!
		** @brief
		*/

		/**
		** \brief
		*/

 * One liners may use the C++ style comments (omitting the `\brief` keyword):
		//! Some description here

* Use the `in`/`out` keyword for doxygen if needed:
		/*!
		** \brief An useless routine
		** \param a The first value
		** \param b The second value
		** \param[out] ret The result of the addition of `a` and `b`
		*/
		void uselessMethod(const int a, int b, int& ret)
		{
			ret = a + b;
		}






BEST PRACTICES
--------------

 * Prefer this test (for strings):
		if (not s.empty())
   to
		if (s != "")

 * Prefer this form:
		++i
   to
		i++
   (It does not change anything for builtin types but it would bring consistency)

 * Use and abuse `assert` tests.

 * Prefer this form (assuming `int i;`):
		if (42 == i)
   to
		if (i == 42)

 * Don't put an else right after a return.
   Delete the else clause, it's unnecessary and increases indent level.

   BAD:
		if (...)
			return;
		else
			return;

 * Avoid using pointers. Prefer references. You might need the variable to
   be assigned a NULL value: in this case the NULL value has semantics and must
   be checked. Wherever possible, use a SmartPtr instead of old-school pointers.

 * Avoid using const char* or char*. Use Yuni::String instead (or better the good
   template parameter when possible, or even AnyString).

 * Header file guards: use pragma once
   If 'pragma once' can not be used, the header guards name should
   follow the directory structure: e.g. `__YUNI_CORE_ATOMIC_INT_H__`

 * Avoid using copy constructors whenever it's possible (see `Yuni::NonCopyable`).

 * If your class has copy semantics, define both a copy constructor and an
   assignment operator.

 * Be Const Correct: Use and abuse the const keyword from the start. It is a
   nightmare to add it in later.

 * Don't place any "using namespace" directives in headers

 * All headers must be self-sufficient.

 * Prefer inlines to macros. In C++ macros are not needed for code efficiency

 * In cpp files, the system headers should be included before any other include.

 * In header files, always include "yuni.h" first

 * Avoid as much as possible Return By Value for compound types. Use Return By
   Reference instead.

 * Compile time is without incidence. Increasing compile time to reduce execution
   time (e.g. with templates) is encouraged.

 * Code legibility and length is less important than easy and fast end-user experience.


