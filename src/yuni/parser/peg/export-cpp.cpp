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
#include "grammar.h"
#include "../../io/file.h"
#include <vector>




namespace Yuni
{
namespace Parser
{
namespace PEG
{

	namespace // anonymous
	{

		#include "yuni/private/parser/peg/__parser.include.cpp.hxx"

		class CPPConverter final
		{
		public:
			CPPConverter(const AnyString& root, const Node::Map& rules);
			bool initialize(const AnyString& name);
			void startHeaderheaderGuardID();
			void endHeaderheaderGuardID();
			void generateH();
			void generateHXX();
			void generateCPP();
			bool writeFiles() const;

		public:
			//! Original rules
			const Node::Map& rules;
			//! Code in the .h file
			Clob h;
			//! Code in the .hxx file
			Clob hxx;
			//! Code in the .cpp file
			Clob cpp;
			//! Header guard ID
			YString headerGuardID;
			//! Class name
			YString classname;
			//! Namespace
			std::vector<YString> namespaces;
			//! Root filename
			YString rootfilename;
			//! Local include
			YString localInclude;

		}; // class CPPConverter


		template<class YStringT>
		static inline void EnumIDToClassname(YStringT& out, const YString& enumID)
		{
			out = "Node";
			out << enumID;
			if (not enumID.empty())
				out[4] = (char) YString::ToUpper(out[4]);
		}

		inline CPPConverter::CPPConverter(const AnyString& root, const Node::Map& rules) :
			rules(rules),
			rootfilename(root)
		{
			IO::ExtractFileName(localInclude, root);
		}

		inline bool CPPConverter::initialize(const AnyString& name)
		{
			name.split(namespaces, "/\\:");
			return true;
		}

		inline void CPPConverter::generateH()
		{
			const Node::Map::const_iterator end = rules.end();

			h << "#pragma once\n";
			h << "#include <yuni/yuni.h>\n";
			h << "#include <yuni/core/string.h>\n";
			h << "#include <yuni/core/bind.h>\n";
			h << "#include <yuni/core/dictionary.h>\n";
			h << "#include <yuni/core/smartptr/intrusive.h>\n";
			h << "#include <vector>\n";
			h << "#include <memory>\n";
			h << "#include <initializer_list>\n";
			h << '\n';
			h << '\n';

			for (uint32_t i = 0; i != namespaces.size(); ++i)
				h << "namespace " << namespaces[i] << "\n{\n";
			h << '\n';
			h << "	enum Rule {\n";
			h << "		//! Unknown rule\n";
			h << "		rgUnknown = 0,\n";
			uint32_t enumIndex = 1;
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i, ++enumIndex)
			{
				h << "		//! enum for the rule `" << i->first << '`';
				if (i->second.attributes.inlined)
					h << " [inline]";
				h << '\n';
				h << "		" << i->second.enumID << ", // = " << enumIndex << '\n';
			}
			h << "		//! enum for the final rule\n";
			h << "		rgEOF // " << (enumIndex++) << '\n';
			h << "	};\n";
			// rule count
			h << '\n';
			h << "	//! The total number of rules\n";
			h << "	constexpr const uint32_t ruleCount = " << enumIndex << ";\n";
			h << "\n";
			h << "	//! Convert a rule id into its text representation\n";
			h << "	YUNI_DECL AnyString ruleToString(enum Rule);\n";
			h << '\n';
			h << "	//! Get if a rule is an error\n";
			h << "	YUNI_DECL bool ruleIsError(enum Rule ruleid);\n";
			h << '\n';
			h << "	//! Get if the rule should be ignored when duplucating an AST (starting from 'tk-' and some special rules)\n";
			h << "	YUNI_DECL bool shouldIgnoreForDuplication(enum Rule);\n";
			h << '\n';
			h << '\n';
			h << "	enum class Error {\n";
			h << "		//! No error\n";
			h << "		none,\n";
			h << "		//! Parse error\n";
			h << "		parse,\n";
			h << "	};\n";
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Notification final {\n";
			h << "	public:\n";
			h << "		//! Start offset\n";
			h << "		uint32_t offset = 0;\n";
			h << "		//! Line Index\n";
			h << "		uint32_t line = 0;\n";
			h << "		//! Filename\n";
			h << "		YString message;\n";
			h << "		//! Filename\n";
			h << "		YString filename;\n";
			h << "	}; // class Notification\n";
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Node;\n";
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL NodeVector final {\n";
			h << "	public:\n";
			h << "		using T = Node;\n";
			h << '\n';
			h << "		struct Iterator final: public std::iterator<std::input_iterator_tag, T> {\n";
			h << "			Iterator(T** array, uint32_t index) :m_array(array), m_index{index} {}\n";
			h << "			Iterator(const Iterator&) = default;\n";
			h << "			Iterator& operator++() { ++m_index; return *this; }\n";
			h << "			Iterator operator++(int) {Iterator tmp(*this); operator++(); return tmp;}\n";
			h << "			bool operator==(const Iterator& rhs) const { return rhs.m_index == m_index; };\n";
			h << "			bool operator!=(const Iterator& rhs) const { return rhs.m_index != m_index; };\n";
			h << "			T& operator * () { return *(m_array[m_index]); }\n";
			h << '\n';
			h << "		private:\n";
			h << "			T** const m_array;\n";
			h << "			uint32_t m_index = 0;\n";
			h << "			friend class NodeVector;\n";
			h << "		};\n";
			h << '\n';
			h << "		struct ConstIterator final: public std::iterator<std::input_iterator_tag, T> {\n";
			h << "			ConstIterator(T** array, uint32_t index) :m_array(array), m_index{index} {}\n";
			h << "			ConstIterator(const ConstIterator&) = default;\n";
			h << "			ConstIterator& operator++() { ++m_index; return *this; }\n";
			h << "			ConstIterator operator++(int) {ConstIterator tmp(*this); operator++(); return tmp;}\n";
			h << "			bool operator==(const ConstIterator& rhs) const { return rhs.m_index == m_index; };\n";
			h << "			bool operator!=(const ConstIterator& rhs) const { return rhs.m_index != m_index; };\n";
			h << "			const T& operator * () const { return *(m_array[m_index]); }\n";
			h << '\n';
			h << "		private:\n";
			h << "			T** const m_array;\n";
			h << "			uint32_t m_index = 0;\n";
			h << "			friend class NodeVector;\n";
			h << "		};\n";
			h << '\n';
			h << "	public:\n";
			h << "		NodeVector() = default;\n";
			h << "		NodeVector(const NodeVector&) = delete;\n";
			h << "		NodeVector(NodeVector&&) = delete;\n";
			h << "		~NodeVector();\n";
			h << '\n';
			h << "		Iterator begin();\n";
			h << "		Iterator end();\n";
			h << "		ConstIterator begin() const;\n";
			h << "		ConstIterator end() const;\n";
			h << "		ConstIterator cbegin() const;\n";
			h << "		ConstIterator cend() const;\n";
			h << '\n';
			h << "		void push_back(T* const element);\n";
			h << "		void push_back(yuni::Ref<T> element);\n";
			h << "		void push_front(T* const element);\n";
			h << "		void push_front(yuni::Ref<T> element);\n";
			h << "		void pop_back();\n";
			h << "		void clear();\n";
			h << '\n';
			h << "		void erase(uint32_t index);\n";
			h << "		void erase(const Iterator& it);\n";
			h << "		void erase(const ConstIterator& it);\n";
			h << '\n';
			h << "		void resize(uint32_t count);\n";
			h << "		bool empty() const;\n";
			h << "		uint32_t size() const;\n";
			h << "		uint32_t capacity() const;\n";
			h << "		void shrink_to_fit();\n";
			h << '\n';
			h << "		//! Retrieve the last element\n";
			h << "		T& back();\n";
			h << "		//! Retrieve the last element (const)\n";
			h << "		const T& back() const;\n";
			h << '\n';
			h << "		//! Retrive the first element\n";
			h << "		T& front();\n";
			h << "		//! Retrive the first element (const)\n";
			h << "		const T& front() const;\n";
			h << '\n';
			h << "		void swap(NodeVector&);\n";
			h << '\n';
			h << "		//! Retrieve the Nth element\n";
			h << "		T& operator [] (uint32_t i);\n";
			h << "		//! Retrieve the Nth element (const)\n";
			h << "		const T& operator [] (uint32_t i) const;\n";
			h << "		//! Move operator\n";
			h << "		NodeVector& operator = (NodeVector&&) = delete;\n";
			h << "		//! Copy operator\n";
			h << "		NodeVector& operator = (const NodeVector&);\n";
			h << '\n';
			h << "	private:\n";
			h << "		static void deleteElement(T* const ptr);\n";
			h << "		void grow();\n";
			h << '\n';
			h << "	private:\n";
			h << "		static constexpr uint32_t preAllocatedCount = 2;\n";
			h << "		uint32_t m_size = 0;\n";
			h << "		uint32_t m_capacity = preAllocatedCount;\n";
			h << "		T** m_pointer = &(m_innerstorage[0]);\n";
			h << "		T* m_innerstorage[preAllocatedCount];\n";
			h << "		friend struct Iterator;\n";
			h << "	};\n";
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Node final {\n";
			h << "	public:\n";
			h << "		//! Callback definition for export a node\n";
			h << "		using ExportCallback = void (*)(const Node& node, YString& tmp);\n";
			h << '\n';
			h << "	public:\n";
			h << "		//! Export the tree node\n";
			h << "		static void Export(Yuni::Clob& out, const Node& node);\n";
			h << "		//! Export the tree node (with color output)\n";
			h << "		static void Export(Yuni::Clob& out, const Node& node, bool color, ExportCallback callback = nullptr);\n";
			h << "		//! Export the tree node to HTML\n";
			h << "		static void ExportToHTML(Yuni::Clob& out, const Node& node);\n";
			h << '\n';
			h << "		//! Export the tree node into a JSON object\n";
			h << "		static void ExportToJSON(Yuni::Clob& out, const Node& node);\n";
			h << "		//! Export the tree node into a JSON object (with callback for additional data)\n";
			h << "		static void ExportToJSON(Yuni::Clob& out, const Node& node, void (*callback)(Yuni::Dictionary<AnyString, YString>::Unordered&, const Node&));\n";
			h << '\n';
			h << "	public:\n";
			h << "		//! Default constructor\n";
			h << "		Node() = default;\n";
			h << "		//! Default constructor with a pre-defined rule\n";
			h << "		explicit Node(enum Rule);\n";
			h << "		//! Default constructor with a pre-defined rule and a given text\n";
			h << "		Node(enum Rule, const AnyString& text);\n";
			h << "		//! Copy constructor\n";
			h << "		Node(const Node& rhs) = delete;\n";
			h << "		//! Destructor\n";
			h << "		~Node() = default;\n";
			h << '\n';
			h << "		void clear();\n";
			h << "		bool empty() const;\n";
			h << '\n';
			h << "		//! Iterate through all child nodes\n";
			h << "		template<class F> bool each(const F& callback);\n";
			h << '\n';
			h << "		//! Iterate through all child nodes (const)\n";
			h << "		template<class F> bool each(const F& callback) const;\n";
			h << "		template<class F> bool each(enum Rule rule, const F& callback);\n";
			h << "		template<class F> bool each(enum Rule rule, const F& callback) const;\n";
			h << '\n';
			h << "		template<class StringT> bool extractFirstChildText(StringT& out, enum Rule rule) const;\n";
			h << '\n';
			h << "		template<class StringT> bool extractChildText(StringT& out, enum Rule rule, const AnyString& separator = nullptr) const;\n";
			h << '\n';
			h << "		uint32_t findFirst(enum Rule rule) const;\n";
			h << '\n';
			h << "		bool exists(enum Rule rule) const;\n";
			h << '\n';
			h << "		const Node* xpath(std::initializer_list<enum Rule> path) const;\n";
			h << '\n';
			h << "		Node& operator = (const Node& rhs);\n";
			h << '\n';
			h << "		void toText(YString& out) const;\n";
			h << '\n';
			h << "		const Node& firstChild() const;\n";
			h << "		Node& firstChild();\n";
			h << '\n';
			h << "		const Node& lastChild() const;\n";
			h << "		Node& lastChild();\n";
			h << '\n';
			h << "		bool hasAttributeImportant() const;\n";
			h << "		bool hasAttributeCapture() const;\n";
			h << "		AnyString attributeCapturedText() const;\n";
			h << '\n';
			h << "		Node& append(Rule);\n";
			h << "		Node& append(Rule, Rule);\n";
			h << "		Node& append(Rule, Rule, Rule);\n";
			h << '\n';
			h << "	public:\n";
			h << "		//! The rule ID\n";
			h << "		enum Rule rule = rgUnknown;\n";
			h << "		//! Start offset\n";
			h << "		uint32_t offset = 0;\n";
			h << "		//! End offset\n";
			h << "		uint32_t offsetEnd = 0;\n";
			h << "		//! Text associated to the node (if any)\n";
			h << "		AnyString text;\n";
			h << "		//! Parent node, if any\n";
			h << "		Node* parent = nullptr;\n";
			h << "		//! Children\n";
			h << "		NodeVector children;\n";
			h << '\n';
			h << "		void addRef() {\n";
			h << "			++m_refcount;\n";
			h << "		}\n";
			h << '\n';
			h << "		bool release() {\n";
			h << "			return --m_refcount == 0;\n";
			h << "		}\n";
			h << '\n';
			h << "	private:\n";
			h << "		uint32_t m_refcount = 0;\n";
			h << '\n';
			h << "	}; // struct Node\n";
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Parser final {\n";
			h << "	public:\n";
			h << "		using OnURILoading = Yuni::Bind<bool (Yuni::Clob& out, const AnyString& uri)>;\n";
			h << "		using OnError = Yuni::Bind<bool (const AnyString& filename, uint32_t line, uint32_t offset, Error, const std::vector<YString>&)>;\n";
			h << '\n';
			h << "	public:\n";
			h << "		Parser();\n";
			h << "		Parser(const Parser&) = delete;\n";
			h << "		~Parser();\n";
			h << '\n';
			h << "		void clear();\n";
			h << "		bool loadFromFile(const AnyString& filename);\n";
			h << "		bool load(const AnyString& content);\n";
			h << "		void translateOffset(uint& column, uint& line, const Node&) const;\n";
			h << "		void translateOffset(uint& column, uint& line, uint32_t offset) const;\n";
			h << "		uint32_t translateOffsetToLine(const Node& node) const;\n";
			h << "		AnyString firstSourceContent() const;\n";
			h << '\n';
			h << "		Parser& operator = (const Parser&) = delete;\n";
			h << '\n';
			h << "	public:\n";
			h << "		//! Event: load another include file\n";
			h << "		OnURILoading onURILoading;\n";
			h << "		//! Event: load another include file\n";
			h << "		OnError  onError;\n";
			h << "		//! The root node, if any\n";
			h << "		yuni::Ref<Node> root;\n";
			h << "		//! Notifications\n";
			h << "		std::vector<std::shared_ptr<Notification>> notifications;\n";
			h << '\n';
			h << "	private:\n";
			h << "		void* pData = nullptr;\n";
			h << '\n';
			h << "	}; // class Parser\n";
		}

		inline void CPPConverter::generateHXX()
		{
			hxx << "#pragma once\n";
			hxx << "\n\n\n\n";
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				hxx << "namespace " << namespaces[i] << "\n{\n";
			hxx << '\n';
			hxx << "	inline NodeVector::T& NodeVector::back() {\n";
			hxx << "		assert(m_size != 0);\n";
			hxx << "		return *(m_pointer[m_size - 1]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const NodeVector::T& NodeVector::back() const {\n";
			hxx << "		assert(m_size != 0);\n";
			hxx << "		return *(m_pointer[m_size - 1]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::T& NodeVector::front() {\n";
			hxx << "		assert(m_size != 0);\n";
			hxx << "		return *(m_pointer[0]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const NodeVector::T& NodeVector::front() const {\n";
			hxx << "		assert(m_size != 0);\n";
			hxx << "		return *(m_pointer[0]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline bool NodeVector::empty() const {\n";
			hxx << "		return (0 == m_size);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline uint32_t NodeVector::size() const {\n";
			hxx << "		return m_size;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline uint32_t NodeVector::capacity() const {\n";
			hxx << "		return m_capacity;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::T& NodeVector::operator [] (uint32_t i) {\n";
			hxx << "		assert(i < m_size);\n";
			hxx << "		return *(m_pointer[i]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const NodeVector::T& NodeVector::operator [] (uint32_t i) const {\n";
			hxx << "		assert(i < m_size);\n";
			hxx << "		return *(m_pointer[i]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::erase(const Iterator& it) {\n";
			hxx << "		erase(it.m_index);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::erase(const ConstIterator& it) {\n";
			hxx << "		erase(it.m_index);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::push_back(yuni::Ref<T> element) {\n";
			hxx << "		push_back(yuni::Ref<T>::WeakPointer(element));\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::push_front(yuni::Ref<T> element) {\n";
			hxx << "		push_front(yuni::Ref<T>::WeakPointer(element));\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::Iterator NodeVector::begin() {\n";
			hxx << "		return Iterator(m_pointer, 0);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::Iterator NodeVector::end() {\n";
			hxx << "		return Iterator(m_pointer, m_size);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::ConstIterator NodeVector::begin() const {\n";
			hxx << "		return ConstIterator(m_pointer, 0);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::ConstIterator NodeVector::end() const {\n";
			hxx << "		return ConstIterator(m_pointer, m_size);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::ConstIterator NodeVector::cbegin() const {\n";
			hxx << "		return ConstIterator(m_pointer, 0);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector::ConstIterator NodeVector::cend() const {\n";
			hxx << "		return ConstIterator(m_pointer, m_size);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::deleteElement(T* const element) {\n";
			hxx << "		if (element->release())\n";
			hxx << "			delete element;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline NodeVector& NodeVector::operator = (const NodeVector& rhs) {\n";
			hxx << "		clear();\n";
			hxx << "		for (uint32_t i = 0; i != rhs.m_size; ++i)\n";
			hxx << "			push_back(rhs.m_pointer[i]);\n";
			hxx << "		return *this;";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline void NodeVector::swap(NodeVector& other) {\n";
			hxx << "		std::swap(m_size, other.m_size);\n";
			hxx << "		std::swap(m_capacity, other.m_capacity);\n";
			hxx << "		std::swap(m_innerstorage, other.m_innerstorage);\n";
			hxx << "		std::swap(m_pointer, other.m_pointer);\n";
			hxx << "		if (m_capacity == preAllocatedCount)\n";
			hxx << "			m_pointer = &(m_innerstorage[0]);\n";
			hxx << "		if (other.m_capacity == preAllocatedCount)\n";
			hxx << "			other.m_pointer = &(other.m_innerstorage[0]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	//! References for tokens\n";
			hxx << "	static constexpr const bool isToken[] = {\n";
			hxx << "		false, // rgUnknown\n";
			for (auto& pair: rules)
			{
				if (pair.first.startsWith("tk-"))
					hxx << "		true, // " << pair.first << '\n';
				else
					hxx << "		false, // " << pair.first << '\n';
			}
			hxx << "	};\n";
			hxx << "\n";
			hxx << "	inline bool shouldIgnoreForDuplication(enum Rule rule) {\n";
			hxx << "		//assert(static_cast<uint32_t>(rule) < " << (rules.size() + 1) << ");\n";
			hxx << "		return isToken[static_cast<uint32_t>(rule)];\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	static constexpr const bool isAttributeError[] = {\n";
			hxx << "		false, // rgUnknown\n";
			for (auto& pair: rules)
			{
				bool error = pair.first == "error" or pair.first.startsWith("error-");
				hxx << "		" << (error ? "true" : "false") << ", // " << pair.second.enumID << '\n';
			}
			hxx << "		false, // rgEOF\n";
			hxx << "	};\n";
			hxx << '\n';
			hxx << "	inline bool ruleIsError(enum Rule ruleid) {\n";
			hxx << "		// assert((uint) ruleid < (uint) ruleCount);\n";
			hxx << "		return isAttributeError[static_cast<uint32_t>(ruleid)];\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node::Node(enum Rule rule)\n";
			hxx << "		: rule(rule) {\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node::Node(enum Rule rule, const AnyString& text)\n";
			hxx << "		: rule(rule)\n";
			hxx << "		, text(text)\n";
			hxx << "	{}\n";
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(const F& callback) {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			if (not callback(children[i]))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(const F& callback) const {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			if (not callback(children[i]))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(enum Rule rule, const F& callback) {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			Node& subnode = children[i];\n";
			hxx << "			if (subnode.rule == rule and not callback(subnode))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(enum Rule rule, const F& callback) const {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			const Node& subnode = children[i];\n";
			hxx << "			if (subnode.rule == rule and not callback(subnode))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	template<class StringT> inline bool Node::extractFirstChildText(StringT& out, enum Rule rule) const {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			const Node& subnode = children[i];\n";
			hxx << "			if (subnode.rule == rule) {\n";
			hxx << "				out += subnode.text;\n";
			hxx << "				return true;\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return false;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	template<class StringT> inline bool Node::extractChildText(StringT& out, enum Rule rule, const AnyString& separator) const {\n";
			hxx << "		bool somethingFound = false;\n";
			hxx << "		if (separator.empty()) {\n";
			hxx << "			for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "				const Node& subnode = children[i];\n";
			hxx << "				if (subnode.rule == rule) {\n";
			hxx << "					out += subnode.text;\n";
			hxx << "					somethingFound = true;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		else {\n";
			hxx << "			for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "				const Node& subnode = children[i];\n";
			hxx << "				if (subnode.rule == rule) {\n";
			hxx << "					if (not out.empty())\n";
			hxx << "						out += separator;\n";
			hxx << "					out += subnode.text;\n";
			hxx << "					somethingFound = true;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return somethingFound;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline uint32_t Node::findFirst(enum Rule rule) const {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			if (children[i].rule == rule)\n";
			hxx << "				return i;\n";
			hxx << "		}\n";
			hxx << "		return (uint)-1;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline bool Node::empty() const {\n";
			hxx << "		return children.empty();\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline bool Node::exists(enum Rule rule) const {\n";
			hxx << "		for (uint32_t i = 0; i != children.size(); ++i) {\n";
			hxx << "			if (children[i].rule == rule)\n";
			hxx << "				return true;\n";
			hxx << "		}\n";
			hxx << "		return false;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const Node& Node::firstChild() const {\n";
			hxx << "		return children.front();\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node& Node::firstChild() {\n";
			hxx << "		return children.front();\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const Node& Node::lastChild() const {\n";
			hxx << "		return children.back();\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node& Node::lastChild() {\n";
			hxx << "		return children.back();\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline const Node* Node::xpath(std::initializer_list<enum Rule> path) const {\n";
			hxx << "		if (path.size() > 0) {\n";
			hxx << "			auto it = path.begin();\n";
			hxx << "			for (auto& child: children) {\n";
			hxx << "				if (child.rule == *it) {\n";
			hxx << "					const Node* result = &child;\n";
			hxx << "					++it;\n";
			hxx << "					for (; it != path.end(); ++it) {\n";
			hxx << "						bool found = false;\n";
			hxx << "						auto& subnode = *result;\n";
			hxx << "						for (uint32_t j = 0; j != subnode.children.size(); ++j) {\n";
			hxx << "							if (subnode.children[j].rule == *it) {\n";
			hxx << "								result = &(subnode.children[j]);\n";
			hxx << "								found = true;\n";
			hxx << "								break;\n";
			hxx << "							}\n";
			hxx << "						}\n";
			hxx << "						if (not found)\n";
			hxx << "							return nullptr;\n";
			hxx << "					}\n";
			hxx << "					return result;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return nullptr;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node& Node::append(Rule r1) {\n";
			hxx << "		Node* node = new Node{r1};\n";
			hxx << "		children.push_back(node);\n";
			hxx << "		return *node;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node& Node::append(Rule r1, Rule r2) {\n";
			hxx << "		return append(r1).append(r2);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << "	inline Node& Node::append(Rule r1, Rule r2, Rule r3) {\n";
			hxx << "		return append(r1).append(r2).append(r3);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
		}

		static inline void GenerateFunctionForEachRule(Clob& cpp, uint& sp, const Node::Map& rules, const AnyString& name, const Node& node)
		{
			Clob body;
			// subnodes
			{
				// We have to reserve from the begining this vector, to avoir future calls to
				// std::vector::resize, and to invalidate references to internal elements
				std::vector<Clob> helpers;
				std::vector<String> datatext;
				helpers.reserve(node.treeCount());

				// skip the first AND block, which is useless actually
				if (node.rule.type == Node::asAND)
				{
					for (uint32_t i = 0; i != (uint) node.children.size(); ++i)
						node.children[i].exportCPP(body, rules, helpers, datatext, 1, true, sp);
				}
				else
					node.exportCPP(body, rules, helpers, datatext, 1, true, sp);

				// helpers
				bool foundHelper = false;

				if (not datatext.empty())
				{
					for (uint32_t i = 0; i != (uint) datatext.size(); ++i)
						cpp << '	' << datatext[i] << '\n';
					cpp << "\n\n\n";
				}

				for (uint32_t i = 0; i != (uint) helpers.size(); ++i)
				{
					if (not helpers[i].empty())
					{
						if (not foundHelper)
						{
							foundHelper = true;
							cpp << "\n\n";
						}
						// extract forward declaration
						uint32_t linefeed = helpers[i].find('\n');
						if (linefeed < helpers[i].size())
							cpp << AnyString(helpers[i], 0, linefeed) << ";\n";
					}
				}

				foundHelper = false;
				for (uint32_t i = 0; i != (uint) helpers.size(); ++i)
				{
					if (not helpers[i].empty())
					{
						if (not foundHelper)
						{
							foundHelper = true;
							cpp << "\n\n";
						}
						cpp << helpers[i] << '\n';
					}
				}
			}

			cpp << "//! Rule " << name << '\n';
			cpp << "inline bool yy" << node.enumID << "(Datasource& ctx) {\n";
			cpp << "	(void) ctx;\n";
			cpp << "	TRACE(\"entering " << node.enumID;
			if (node.attributes.inlined)
				cpp << " [inline]";
			cpp << "\");\n";

			if (not node.attributes.inlined)
				cpp << "	uint32_t _ruleOffset = ctx.enterRule(" << node.enumID << ");\n";
			cpp << '\n';
			cpp << body;
			cpp << '\n';
			if (not node.attributes.inlined)
				cpp << "	ctx.commit(_ruleOffset, " << node.enumID << ");\n";
			cpp << "	return true;\n";
			cpp << "}\n";
			cpp << '\n';
		}

		inline void CPPConverter::generateCPP()
		{
			const Node::Map::const_iterator end = rules.end();
			cpp << '\n';
			cpp << "#include \"" << localInclude << "h\"\n";
			cpp << "#include <cassert>\n";
			cpp << "#include <yuni/core/string.h>\n";
			cpp << "#include <yuni/io/file.h>\n";
			cpp << "#include <yuni/io/directory.h>\n";
			cpp << "#include <yuni/core/dictionary.h>\n";
			cpp << "#include <yuni/core/noncopyable.h>\n";
			cpp << "#include <yuni/datetime/timestamp.h>\n";
			cpp << "#include <yuni/core/system/console/console.h>\n";
			cpp << "#include <iostream>\n";
			cpp << '\n';
			cpp << "using namespace Yuni;\n";
			cpp << "\n\n";

			cpp << "std::ostream& operator << (std::ostream& out, const ";
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				cpp << "::" << namespaces[i];
			cpp << "::Node& node) {\n";
			cpp << "	Clob content;\n";
			cpp << "	";
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				cpp << "::" << namespaces[i];
			cpp << "::Node::Export(content, node);";
			cpp << "out << content;\n";
			cpp << "	return out;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "std::ostream& operator << (std::ostream& out, const yuni::Ref<";
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				cpp << "::" << namespaces[i];
			cpp << "::Node>& node) {\n";
			cpp << "	if (!!node)\n";
			cpp << "		out << (*node);\n";
			cpp << "	else\n";
			cpp << "		out << \"<invalid ast node>\";\n";
			cpp << "	return out;\n";
			cpp << "}\n";
			cpp << '\n';
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				cpp << "namespace " << namespaces[i] << "\n{\n";
			cpp << "namespace // anonymous\n{\n";
			cpp << '\n';
			cpp << "constexpr const bool _attrAttributeCapture[] = {\n";
			cpp << "	false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "	" << (i->second.attributes.capture ? "true" : "false") << ", // " << i->second.enumID << '\n';
			cpp << "	false, // rgEOF\n";
			cpp << "};\n";
			cpp << '\n';
			cpp << "bool ruleAttributeCapture(enum Rule ruleid) {\n";
			cpp << "	assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "	return _attrAttributeCapture[(uint) ruleid];\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "constexpr const bool _attrAttributeImportant[] = {\n";
			cpp << "	false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "	" << (i->second.attributes.important ? "true" : "false") << ", // " << i->second.enumID << '\n';
			cpp << "	false, // rgEOF\n";
			cpp << "};\n";
			cpp << '\n';
			cpp << "bool ruleAttributeImportant(enum Rule ruleid) {\n";
			cpp << "	assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "	return _attrAttributeImportant[(uint) ruleid];\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "constexpr const char* const _attrAttributeSimpleTextCapture[] = {\n";
			cpp << "		nullptr, // rgUnknown\n";
			String textCapture;
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
			{
				cpp << "		";
				if (i->second.children.size() == 1 and i->second.children[0].isSimpleTextCapture())
				{
					textCapture = i->second.children[0].rule.text;
					textCapture.replace("\\", "\\\\");
					textCapture.replace("\"", "\\\"");
					cpp << '"' << textCapture << '"';
				}
				else
					cpp << "nullptr";

				cpp << ", // " << i->second.enumID << '\n';
			}
			cpp << "		nullptr, // rgEOF\n";
			cpp << "	};\n";
			cpp << '\n';
			cpp << "AnyString ruleAttributeSimpleTextCapture(enum Rule ruleid) {\n";
			cpp << "	assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "	return _attrAttributeSimpleTextCapture[(uint) ruleid];\n";
			cpp << "}\n\n";
			PrepareCPPInclude(cpp);
			cpp << "	// Forward declarations\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "inline bool yy" << i->second.enumID << "(Datasource& ctx);\n";
			cpp << '\n';
			// generate all rules
			{
				uint32_t sp = 0;
				for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
					GenerateFunctionForEachRule(cpp, sp, rules, i->first, i->second);
			}
			cpp << "} // anonymous namespace\n";
			cpp << '\n';
			cpp << "NodeVector::~NodeVector() {\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	for (uint32_t i = 0; i != size; ++i)\n";
			cpp << "		deleteElement(m_pointer[i]);\n";
			cpp << "	if (m_capacity != preAllocatedCount)\n";
			cpp << "		free(m_pointer);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::shrink_to_fit() {\n";
			cpp << "	if (m_capacity != preAllocatedCount) {\n";
			cpp << "		if (m_size == 0) {\n";
			cpp << "			free(m_pointer);\n";
			cpp << "			m_pointer = &(m_innerstorage[0]);\n";
			cpp << "			m_capacity = preAllocatedCount;\n";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::push_front(NodeVector::T* const element) {\n";
			cpp << "	assert(element != nullptr);\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	if (size == 0)\n";
			cpp << "		push_back(element);\n";
			cpp << "	uint32_t newsize = size + 1;\n";
			cpp << "	if (not (newsize < m_capacity))\n";
			cpp << "		grow();\n";
			cpp << "	uint32_t i = size;\n";
			cpp << "	while (i-- > 0)\n";
			cpp << "		m_pointer[i + 1] = m_pointer[i];\n";
			cpp << "	m_size = newsize;\n";
			cpp << "	element->addRef();\n";
			cpp << "	m_pointer[0] = element;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::resize(uint32_t count) {\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	if (count != size) {\n";
			cpp << "		if (count > size) {\n";
			cpp << "			for (uint32_t i = size; i != count; ++i)\n";
			cpp << "				push_back(new T);\n";
			cpp << "		}\n";
			cpp << "		else {\n";
			cpp << "			for (uint32_t i = count; i != size; ++i)\n";
			cpp << "				deleteElement(m_pointer[i]);\n";
			cpp << "		}\n";
			cpp << "		m_size = count;\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::erase(uint32_t index) {\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	if (index < size) {\n";
			cpp << "		deleteElement(m_pointer[index]);\n";
			cpp << "		for (uint32_t i = index + 1; i < size; ++i)\n";
			cpp << "			m_pointer[i - 1] = m_pointer[i];\n";
			cpp << "		m_size = --size;\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::clear() {\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	if (size != 0) {\n";
			cpp << "		for (uint32_t i = 0; i != size; ++i)\n";
			cpp << "			deleteElement(m_pointer[i]);\n";
			cpp << "		m_size = 0;\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::pop_back() {\n";
			cpp << "	uint32_t size = m_size;\n";
			cpp << "	if (size != 0) {\n";
			cpp << "		--size;\n";
			cpp << "		deleteElement(m_pointer[size]);\n";
			cpp << "		m_size = size;\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::push_back(NodeVector::T* const element) {\n";
			cpp << "	assert(element != nullptr);\n";
			cpp << "	uint32_t oldsize = m_size;\n";
			cpp << "	uint32_t newsize = oldsize + 1;\n";
			cpp << "	if (not (newsize < m_capacity))\n";
			cpp << "		grow();\n";
			cpp << "	m_size = newsize;\n";
			cpp << "	element->addRef();\n";
			cpp << "	m_pointer[oldsize] = element;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void NodeVector::grow() {\n";
			cpp << "	uint32_t newcapacity;\n";
			cpp << "	T** newptr;\n";
			cpp << "	if (m_capacity == preAllocatedCount) {\n";
			cpp << "		newcapacity = 8;\n";
			cpp << "		newptr = (T**) malloc(sizeof(T*) * newcapacity);\n";
			cpp << "		if (!newptr) {\n";
			cpp << "			#if __cpp_exceptions >= 199711\n";
			cpp << "			throw std::bad_alloc();\n";
			cpp << "			#else\n";
			cpp << "			abort();\n";
			cpp << "			#endif\n";
			cpp << "		}\n";
			cpp << "		newptr[0] = m_innerstorage[0];\n";
			cpp << "		newptr[1] = m_innerstorage[1];\n";
			cpp << "	}\n";
			cpp << "	else {\n";
			cpp << "		newcapacity = m_capacity + 8;\n";
			cpp << "		newptr = (T**) realloc(m_pointer, sizeof(T*) * newcapacity);\n";
			cpp << "		if (!newptr) {\n";
			cpp << "			#if __cpp_exceptions >= 199711\n";
			cpp << "			throw std::bad_alloc();\n";
			cpp << "			#else\n";
			cpp << "			abort();\n";
			cpp << "			#endif\n";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << "	m_capacity = newcapacity;\n";
			cpp << "	m_pointer = newptr;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "AnyString ruleToString(Rule ruleid) {\n";
			cpp << "	switch (ruleid) {\n";
			cpp << "		case rgUnknown: return \"<unknown>\";\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "		case " << i->second.enumID << ": return \"" << i->first << "\";\n";
			cpp << "		case rgEOF: return \"EOF\";\n";
			cpp << "	}\n";
			cpp << "	return \"<error>\";\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "Parser::Parser() {\n";
			cpp << "	onURILoading.bind(& StandardURILoaderHandler);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "Parser::~Parser() {\n";
			cpp << "	delete (Datasource*) pData;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Parser::clear() {\n";
			cpp << "	root = nullptr;\n";
			cpp << "	delete (Datasource*) pData;\n";
			cpp << "	pData = nullptr;\n";
			cpp << "	if (not notifications.empty()) {\n";
			cpp << "		notifications.clear();\n";
			cpp << "		notifications.shrink_to_fit();\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "AnyString Parser::firstSourceContent() const {\n";
			cpp << "	if (pData) {\n";
			cpp << "		Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "		if (not ctx.contents.empty())\n";
			cpp << "			return ctx.contents.front();\n";
			cpp << "	}\n";
			cpp << "	return AnyString();\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "bool Parser::loadFromFile(const AnyString& filename) {\n";
			cpp << "	if (!pData)\n";
			cpp << "		pData = new Datasource(notifications);\n";
			cpp << "	Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "	switch (ctx.open(filename)) {\n";
			cpp << "		case Datasource::OpenFlag::opened: {\n";
			cpp << "			DATASOURCE_PARSE(ctx);\n";
			cpp << "			return ctx.success;\n";
			cpp << "		}\n";
			cpp << "		case Datasource::OpenFlag::ignore: {\n";
			cpp << "			return true;";
			cpp << "		}\n";
			cpp << "		case Datasource::OpenFlag::error: {\n";
			cpp << "			return false;";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << "	return false;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "bool Parser::load(const AnyString& content) {\n";
			cpp << "	if (!pData)\n";
			cpp << "		pData = new Datasource(notifications);\n";
			cpp << "	Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "	ctx.openContent(content);\n";
			cpp << "	DATASOURCE_PARSE(ctx);\n";
			cpp << "	return ctx.success;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Parser::translateOffset(uint& column, uint& line, const Node& node) const {\n";
			cpp << "	column = 0;\n";
			cpp << "	line = 0;\n";
			cpp << "	if (YUNI_LIKELY(pData)) {\n";
			cpp << "		Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "		ctx.translateOffset(column, line, node.offset);\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Parser::translateOffset(uint& column, uint& line, uint32_t offset) const {\n";
			cpp << "	column = 0;\n";
			cpp << "	line = 0;\n";
			cpp << "	if (YUNI_LIKELY(pData)) {\n";
			cpp << "		Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "		ctx.translateOffset(column, line, offset);\n";
			cpp << "	}\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "uint32_t Parser::translateOffsetToLine(const Node& node) const {\n";
			cpp << "	uint32_t column;\n";
			cpp << "	uint32_t line;\n";
			cpp << "	translateOffset(column, line, node);\n";
			cpp << "	return line;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::ExportToHTML(Clob& out, const Node& node) {\n";
			cpp << "	String tmp;\n";
			cpp << "	String indent;\n";
			cpp << "	InternalNodeExportHTML(out, node, indent, tmp);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::Export(Clob& out, const Node& node, bool color, ExportCallback callback) {\n";
			cpp << "	String tmp;\n";
			cpp << "	String indent;\n";
			cpp << "	if (not color)\n";
			cpp << "		InternalNodeExportConsole<false>(out, node, false, indent, tmp, callback);\n";
			cpp << "	else\n";
			cpp << "		InternalNodeExportConsole<true>(out, node, false, indent, tmp, callback);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::ExportToJSON(Yuni::Clob& out, const Node& node, void (*callback)(Yuni::Dictionary<AnyString, YString>::Unordered&, const Node&)) {\n";
			cpp << "	String tmp;\n";
			cpp << "	String indent;\n";
			cpp << "	out << \"{ \\\"data\\\": [\\n\";\n";
			cpp << "	InternalNodeExportJSON(out, node, false, indent, tmp, callback);\n";
			cpp << "	out << \"\t{}\\n\";\n";
			cpp << "	out << \"] }\\n\";\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::ExportToJSON(Yuni::Clob& out, const Node& node) {\n";
			cpp << "	ExportToJSON(out, node, nullptr);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::Export(Clob& out, const Node& node) {\n";
			cpp << "	Export(out, node, ::Yuni::System::Console::IsStdoutTTY());\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "Node& Node::operator = (const Node& rhs) {\n";
			cpp << "	rule = rhs.rule;\n";
			cpp << "	offset = rhs.offset;\n";
			cpp << "	offsetEnd = rhs.offsetEnd;\n";
			cpp << "	text = rhs.text;\n";
			cpp << "	children = rhs.children;\n";
			cpp << "	return *this;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::toText(YString& out) const {\n";
			cpp << "	if (not text.empty()) {\n";
			cpp << "		if (not out.empty())\n";
			cpp << "			out += ' ';\n";
			cpp << "		out += text;\n";
			cpp << "		out.trimRight();\n";
			cpp << "	}\n";
			cpp << "	for (auto& child: children)\n";
			cpp << "		child.toText(out);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "void Node::clear() {\n";
			cpp << "	children.clear();\n";
			cpp << "	text.clear();\n";
			cpp << "	offset = 0;\n";
			cpp << "	offsetEnd = 0;\n";
			cpp << "	rule = rgUnknown;\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "bool Node::hasAttributeImportant() const {\n";
			cpp << "	return ruleAttributeImportant(rule);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "bool Node::hasAttributeCapture() const {\n";
			cpp << "	return ruleAttributeCapture(rule);\n";
			cpp << "}\n";
			cpp << '\n';
			cpp << "AnyString Node::attributeCapturedText() const {\n";
			cpp << "	return ruleAttributeSimpleTextCapture(rule);\n";
			cpp << "}\n";
		}

		inline void CPPConverter::startHeaderheaderGuardID()
		{
			headerGuardID = "__HEADER";
			for (uint32_t i = 0; i != namespaces.size(); ++i)
				headerGuardID << '_' << namespaces[i];
			headerGuardID << "_GRAMMAR";
			headerGuardID.toUpper();
		}

		inline void CPPConverter::endHeaderheaderGuardID()
		{
			h << "\n\n\n\n\n";
			hxx << "\n\n\n\n\n";
			cpp << "\n\n\n\n\n";
			if (not namespaces.empty())
			{
				uint32_t i = (uint) namespaces.size();
				do
				{
					--i;
					h << "} // namespace " << namespaces[i] << '\n';
					hxx << "} // namespace " << namespaces[i] << '\n';
					cpp << "} // namespace " << namespaces[i] << '\n';
				}
				while (i > 0);
			}
			h << '\n' << "#include \"" << localInclude << "hxx\"\n";
		}

		inline bool CPPConverter::writeFiles() const
		{
			AnyString root = rootfilename;
			YString file;
			file.clear() << root << "h";
			if (not IO::File::SetContent(file, h))
				return false;
			file.clear() << root << "hxx";
			if (not IO::File::SetContent(file, hxx))
				return false;
			file.clear() << root << "cpp";
			if (not IO::File::SetContent(file, cpp))
				return false;
			return true;
		}

	} // anonymous namespace


	bool Grammar::exportToCPP(const AnyString& rootfilename, const AnyString& name) const
	{
		CPPConverter data(rootfilename, pRules);
		if (YUNI_UNLIKELY(not data.initialize(name)))
			return false;

		data.startHeaderheaderGuardID();
		data.generateH();
		data.generateHXX();
		data.generateCPP();
		data.endHeaderheaderGuardID();
		return data.writeFiles();
	}

} // namespace PEG
} // namespace Parser
} // namespace Yuni
