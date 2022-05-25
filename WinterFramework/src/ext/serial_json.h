#pragma once

#include "../Serial.h"

// these are in cpp so user doesn't need json.h

struct json_writer : meta::serial_writer
{
	json_writer(
		std::ostream& out
	)
		: meta::serial_writer(out, false)
	{}

	void write_type(meta::type* type, void* instance) override;
	void write_length(size_t length) override;
	void write_array(meta::type* type, void* instance, size_t repeat) override;
	void write_string(const char* str, size_t length) override;
};

struct json_value_s; // wish this didn't have to be exposed, json.h impls this

struct json_reader : meta::serial_reader
{
	json_reader(
		std::istream& in
	)
		: meta::serial_reader (in, false)
		, m_json              (nullptr)
	{
		init_json();
	}

	bool is_valid() const
	{
		return m_json != nullptr;
	}

	void read_type(meta::type* type, void* instance) override;
	size_t read_length() override;
	void read_array(meta::type* type, void* instance, size_t repeat) override;
	void read_string(char* str, size_t length) override;

private:
	json_value_s* m_json;
	void init_json();
};