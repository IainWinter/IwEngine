#include "../Serial.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

struct my_type_2
{
	float Y;
	std::vector<int> Ints;
};

struct my_type
{
	int X;
	std::string AString;
	my_type_2 Second;
};

int main()
{
	// meta_class_internal<my_type> my_type_desc;
	// meta_class_internal<my_type_2> my_type2_desc;

	// meta_member_internal<my_type, &my_type::X> my_type_x;
	// meta_member_internal<my_type, &my_type::AString> my_type_string;
	// meta_member_internal<my_type, &my_type::Second> my_type_second;

	// meta_member_internal<my_type_2, &my_type_2::Y> my_type2_y;
	// meta_member_internal<my_type_2, &my_type_2::Ints> my_type2_ints;

	// my_type_second.m_type = &my_type2_desc;

	// my_type_desc.m_children["X"] = &my_type_x;
	// my_type_desc.m_children["AString"] = &my_type_string;
	// my_type_desc.m_children["Second"] = &my_type_second;

	// my_type2_desc.m_children["Y"] = &my_type2_y;
	// my_type2_desc.m_children["Ints"] = &my_type2_ints;

	my_type test;
	test.X = 10;
	test.AString = "a string";
	test.Second.Ints = {1, 2, 3, 4, 5};
	test.Second.Y = 123.456f;

	// meta_type_info* type_my_test = new meta_type_info();
	// type_my_test->m_name = "my_type";
	// type_my_test->m_size = sizeof(my_type);

	// meta_type_info* type_int = new meta_type_info();
	// type_int->m_name = "int";
	// type_int->m_size = sizeof(int);

	// meta_type_info* type_float = new meta_type_info();
	// type_float->m_name = "float";
	// type_float->m_size = sizeof(float);

	// meta_type_info* type_string = new meta_type_info();
	// type_string->m_name = "string";
	// type_string->m_size = sizeof(std::string);



	// meta_class<my_type>* class_my_type = new meta_class<my_type>(type_my_test);
	// class_my_type->m_members.push_back(new meta_class_member<my_type, &my_type::X>(type_int, nullptr, "X"));
	// class_my_type->m_members.push_back(new meta_class_member<my_type, &my_type::AString>(type_float, nullptr, "X"));
	// class_my_type->m_members.push_back(new meta_class_member<my_type, &my_type::X>(type_int, nullptr, "X"));

	// meta_map_rep tree(class_my_type, &test);

	meta_type* type = meta_class_builder<my_type>()
		.name("my type")
		.member<&my_type::X>("X")
		.member<&my_type::AString>("string")
		.member<&my_type::Second>("second")
		.get();

	meta_class_builder<my_type_2>()
		.name("my type 2")
		.member<&my_type_2::Ints>("ints array")
		.member<&my_type_2::Y>("Y");

	meta_walker(type, &test).walk([](meta_type* type, void* data)
	{
		printf("%s\n", type->m_info->m_name.c_str());
	});


	json_writer(std::cout).write(type, &test);


	

	std::stringstream ss;
	bin_writer(ss).write(type, &test);

	std::string s = ss.str();
	std::cout << s;

	std::istringstream ssi(s);

	my_type test2;

	bin_reader(ssi).read(type, &test2);
	
}

// serializer

template<> void meta_serial_write(meta_tag<std::string>, serial_writer* writer, const std::string& instance)
{
	writer->write_length(instance.size());
	writer->write_string(instance.data(), instance.size());
}

template<> void meta_serial_read(meta_tag<std::string>, serial_reader* reader, std::string& instance)
{
	instance.resize(reader->read_length());
	reader->read_string(instance.data(), instance.size());
}

template<typename _t> void meta_serial_write(meta_tag<std::vector<_t>>, serial_writer* writer, const std::vector<_t>& instance)
{
	writer->write_length(instance.size());
	writer->write_array(meta::get_class<_t>(), (void*)instance.data(), instance.size());
}

template<typename _t> void meta_serial_read(meta_tag<std::vector<_t>>, serial_reader* reader, std::vector<_t>& instance)
{
	instance.resize(reader->read_length());
	reader->read_array(meta::get_class<_t>(), instance.data(), instance.size());
}

// typenames

template<> std::string meta_typename(meta_tag<int>)   { return "int"; }
template<> std::string meta_typename(meta_tag<float>) { return "float"; }
template<> std::string meta_typename(meta_tag<std::string>) { return "std::string"; }

template<typename _i>
std::string meta_typename(meta_tag<std::vector<_i>>)
{
	std::stringstream ss;
	ss << "std::vector<" << meta_typename(meta_tag<_i>{}) << ">"; 
	return ss.str();
}









// // #include "../Serial2.h"

// // #include <vector>

// // template<typename _t>
// // meta_class& meta_get_class(meta_tag_class<std::vector<_t>>)
// // {
// // 	static
// // 	meta_class type = meta_class("std::vector", sizeof(std::vector<_t>))
// // 		.member("size", meta_get_type<size_t>())
// // 		.member("items", meta_get_type<_t>())
// // 		.encode(
// // 			[](serializer& serial, const std::vector<_t>& vec)
// // 			{
// // 				serial.write_field      (type.m_members[0], vec.size());
// // 				serial.write_field_array(type.m_members[1], vec.data(), vec.size());
// // 			})
// // 		.decode(
// // 			[](serializer& serial, std::vector<_t>& vec)
// // 			{
// // 				size_t size = 0;
// // 				serial.read(type.m_members[0], &size);
				
// // 				vec.resize(size);
// // 				serial.read_array(type.m_members[1], );

// // 			});

// // 	return type;
// // }

// // int main()
// // {
// // 	meta_type bool_type = meta_get_type<bool>();

// // 	std::vector<int> ints;

// // 	meta_class vector_int_type = meta_get_class<std::vector<int>>();

// // 	// how to serialize this vector

// // 	//serializer serial("/file").write(ints);



// // 	return 0;
// // }


// #include <cstddef>
// #include <stdio.h>
// //#include "../Serial2.h"

// struct test_sub
// {
// 	std::string str = "this is a test";
// 	float f = 123.f;
// };

// struct test
// {
// 	int X;
// 	short _pad;
// 	int Y;

// 	test_sub child;
// };

// // template<>
// // meta_class& meta_get_class(meta_tag_class<test>)
// // {
// // 	static meta_class type = meta_class("test")
// // 		.member<int>("X", offsetof(test, X))
// // 		.member<int>("Y", offsetof(test, Y));
		
// // 	return type;
// // }

// #include <unordered_map>
// #include <functional>
// #include <string>
// #include <iostream>

// struct meta_any
// {
// 	void* m_data;

// 	meta_any(void* data) : m_data(data) {}

// 	template<typename _t>
// 	operator _t&()
// 	{
// 		//static_assert(std::is_convertible<_t, >::value);
// 		// static assert to test if this is valid
// 		return *(_t*)m_data;
// 	}
// };

// // helper to get type from pointer to class member

// template <typename _m>
// struct _ptrtype // https://stackoverflow.com/a/22851790
// {
//    template <typename _t, typename _internal>
//    static _internal get_type(_internal _t::*);

//    typedef decltype(get_type(static_cast<_m>(nullptr))) type;
// };

// template<auto _m>
// using ptrtype = typename _ptrtype<decltype(_m)>::type;

// struct meta_member
// {
// 	std::string m_name;
// 	std::function<meta_any(void*)> m_get;
// 	std::function<void(const meta_member&, const void*, std::ostream&)> m_encode;
// };

// template<typename _t>
// void serialize(const meta_member& member, const _t& instance, std::ostream& out);

// template<typename _t, auto _m>
// meta_member make_member(const std::string& name)
// {
// 	using _m_type = ptrtype<_m>;

// 	meta_member m;
// 	m.m_name = name;
// 	m.m_get = [](void* instance)
// 	{
// 		return meta_any(
// 			&( ((_t*)instance)->*_m )
// 		);
// 	};
// 	m.m_encode = [](const meta_member& member, const void* instance, std::ostream& out) 
// 	{
// 		serialize(member, ((const _t*)instance)->*_m, out);
// 	};

// 	return m;
// }

// template<typename _t>
// struct meta_type
// {
// 	std::string m_name;
// 	std::unordered_map<std::string, meta_member> m_members;

// 	meta_type(const std::string& name)
// 		: m_name(name)
// 	{}

// 	template<auto _m>
// 	meta_type& member(const std::string& name)
// 	{
// 		m_members.emplace(name, make_member<_t, _m>(name));
// 		return *this;
// 	}

// 	meta_any member(_t& instance, const std::string& name)
// 	{
// 		return m_members.at(name).m_get(&instance);
// 	}
// };

// template<typename _t>
// meta_type<_t>& make_meta(const char* name = nullptr)
// {
// 	static meta_type<_t> type(name == nullptr ? "" : name);
// 	return type;
// }

// template<typename _t>
// void serialize(const meta_type<_t>& type, const _t& instance, std::ostream& out)
// {
// 	for (const auto& [_, member] : type.m_members)
// 	{
// 		member.m_encode(member, &instance, out);
// 	}
// }

// template<typename _t>
// void serialize(const meta_member& member, const _t& instance, std::ostream& out)
// {
// 	out << member.m_name << " = " << instance << "\n";
// }

// int main()
// {
// 	test instance;
// 	instance.X = 101;
// 	instance.Y = 202;

// 	meta_type<test>& meta_test = make_meta<test>()
// 		.member<&test::X>("X")
// 		.member<&test::Y>("Y")
// 		.member<&test::child>("child");

// 	int& x = meta_test.member(instance, "X");
// 	x = 303;

// 	serialize(meta_test, instance, std::cout);

// 	return 0;
// }

