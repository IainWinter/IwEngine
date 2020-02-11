#include "iw/util/serialization/Serializer.h"
#include <sstream>
#include <fstream>

namespace iw {
	Serializer::Serializer()
		: m_stream(new std::stringstream())
	{}

	Serializer::Serializer(
		std::string filePath,
		bool overwrite)
		: m_stream(nullptr)
	{
		std::fstream* fstream = new std::fstream();

		fstream->open(filePath, overwrite ? std::fstream::in | std::fstream::out | std::ios::trunc : 3);
		if (!fstream->is_open()) {
			fstream->open(filePath, std::fstream::in | std::fstream::out | std::ios::trunc);
		}

		m_stream = fstream;
	}

	Serializer::~Serializer() {
		delete m_stream;
	}

	void Serializer::Flush() {
		m_stream->flush();
	}

	std::string Serializer::Str() const {
		return std::string(
			std::istreambuf_iterator<char>(*m_stream),
			std::istreambuf_iterator<char>());
	}

	void Serializer::ResetCursor() {
		m_stream->seekg(0);
	}

	void Serializer::SerializeClass(
		const iw::Class* class_,
		const void* value)
	{
		if (strcmp("std::vector", class_->name) == 0) {
			SerializeVector(class_, value);
		}

		else if (strcmp("std::string", class_->name) == 0) {
			SerializeString(class_, value);
		}

		else {
			for (int i = 0; i < class_->fieldCount; i++) {
				const iw::Field& f = class_->fields[i];
				SerializeField(f, (char*)value + f.offset);
			}
		}
	}

	void Serializer::SerializeField(
		const iw::Field& field,
		const void* value)
	{
		SerializeType(field.type, value);
	}

	void Serializer::SerializeType(
		const iw::Type* type,
		const void* value)
	{
		for (int e = 0; e < type->count; e++) {
			void* ptr = (char*)value + e * type->size;
			if (type->isClass) {
				SerializeClass(type->AsClass(), ptr);
			}

			else {
				SerializeValue(type, ptr);
			}
		}
	}

	void Serializer::SerializeValue(
		const iw::Type* type,
		const void* value)
	{
		WriteToStream(value, type->size);
	}

	void Serializer::DeserializeClass(
		const iw::Class* class_, 
		void* value)
	{
		if (strcmp("std::vector", class_->name) == 0) {
			DeserializeVector(class_, value);
		}

		else if (strcmp("std::string", class_->name) == 0) {
			DeserializeString(class_, value);
		}

		else {
			for (int i = 0; i < class_->fieldCount; i++) {
				const iw::Field& f = class_->fields[i];
				DeserializeField(f, (char*)value + f.offset);
			}
		}
	}

	void Serializer::DeserializeField(
		const iw::Field& field, 
		void* value)
	{
		DeserializeType(field.type, value);
	}

	void Serializer::DeserializeType(
		const iw::Type* type, 
		void* value)
	{
		for (int e = 0; e < type->count; e++) {
			void* ptr = (char*)value + e * type->size;
			if (type->isClass) {
				DeserializeClass(type->AsClass(), ptr);
			}

			else {
				DeserializeValue(type, ptr);
			}
		}
	}

	void Serializer::DeserializeValue(
		const iw::Type* type, 
		void* value)
	{
		ReadFromStream(value, type->size);
	}

	void Serializer::SerializeVector(
		const iw::Class* class_, 
		const void* value)
	{
		std::vector<char>& vector = *(std::vector<char>*)value;

		size_t elementSize = class_->fields[1].type->size;
		size_t size = vector.size() / elementSize;

		SerializeValue(class_->fields[0].type, &size);

		for (size_t i = 0; i < size; i++) {
			SerializeType(class_->fields[1].type, &vector.at(i * elementSize));
		}
	}

	void Serializer::SerializeString(
		const iw::Class* class_, 
		const void* value)
	{
		std::string& string = *(std::string*)value;

		size_t size = string.size();

		SerializeValue(class_->fields[0].type, &size);

		WriteToStream(string.data(), size);
	}

	void Serializer::DeserializeVector(
		const iw::Class* class_,
		void* value)
	{
		std::vector<char>& vector = *(std::vector<char>*)value;

		size_t elementSize = class_->fields[1].type->size;
		size_t size;

		DeserializeValue(class_->fields[0].type, &size);

		vector.resize(size * elementSize);

		for (size_t i = 0; i < size; i++) {
			DeserializeType(class_->fields[1].type, &vector.at(i * elementSize));
		}
	}

	void Serializer::DeserializeString(
		const iw::Class* class_, 
		void* value)
	{
		std::string& string = *(std::string*)value;

		size_t size;

		DeserializeValue(class_->fields[0].type, &size);

		string.resize(size);

		ReadFromStream(string.data(), size);
	}

	void Serializer::WriteToStream(
		const void* value,
		size_t size)
	{
		m_stream->write((char*)value, size);
	}

	void Serializer::ReadFromStream(
		void* value,
		size_t size)
	{
		m_stream->read((char*)value, size);
	}
}
