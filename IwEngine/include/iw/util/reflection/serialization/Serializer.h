#pragma once

#include "iw/util/reflection/Type.h"
#include <iostream>
#include <sstream>
#include <fstream>

namespace iw {
namespace Reflect {
	class Serializer {
	protected:
		std::iostream* m_stream;

	public:
		Serializer()
			: m_stream(new std::stringstream())
		{}

		Serializer(
			std::string source
		)
			: m_stream(new std::stringstream())
		{
			*m_stream << source;
		}

		Serializer(
			std::string filePath,
			bool overwrite)
		{
			std::fstream* fstream = new std::fstream();

			fstream->open(filePath, overwrite ? std::fstream::in | std::fstream::out | std::ios::trunc : 3);
			if (!fstream->is_open()) {
				fstream->open(filePath, std::fstream::in | std::fstream::out | std::ios::trunc);
			}

			m_stream = fstream;
		}

		~Serializer() {
			delete m_stream;
		}

		void Flush() {
			m_stream->flush();
		}

		std::string Str() const {
			return std::string(
				std::istreambuf_iterator<char>(*m_stream),
				std::istreambuf_iterator<char>());
		}

		virtual void ResetCursor() {
			m_stream->seekg(0);
		}

		void Write(
			const char* str,
			size_t size)
		{
			m_stream->write(str, size);
		}

		void Read(
			char* str,
			size_t size)
		{
			m_stream->read(str, size);
		}

		template<
			typename _t>
		void Write(
			const _t& value)
		{
			const iw::Type* type = GetType<_t>();
			SerializeType(type, &value);
		}

		template<
			typename _t>
		void Read(
			_t& value)
		{
			const iw::Type* type = GetType<_t>();
			DeserializeType(type, &value);
		}

		virtual void SerializeClass(
			const iw::Class* class_,
			const void* value)
		{
			if (class_->serialize) {
				class_->serialize(*this, value);
			}

			else {
				for (int i = 0; i < class_->fieldCount; i++) {
					const iw::Field& f = class_->fields[i];
					SerializeField(f, (char*)value + f.offset);
				}
			}
		}

		virtual void SerializeField(
			const iw::Field& field,
			const void* value)
		{
			SerializeType(field.type, value);
		}

		virtual void SerializeFieldArray(
			const iw::Field& field,
			const iw::Type* elemType,
			const void* value,
			size_t size)
		{
			SerializeArray(elemType, value, size);
		}

		virtual void SerializeType(
			const iw::Type* type,
			const void* value)
		{
			if (type->isArray) {
				SerializeArray(type, value, type->count);
			}

			else {
				void* ptr = (char*)value;
				if (type->isClass) {
					const iw::Class* class_ = type->AsClass();
					SerializeClass(class_, ptr);
				}

				else {
					SerializeValue(type, ptr);
				}
			}
		}

		virtual void SerializeArray(
			const iw::Type* type,
			const void* value,
			size_t count)
		{
			if (type->type == IntegralType::CHAR) {
				Write((char*)value, count);
				m_stream->put('\0');
			}

			else {
				for (int i = 0; i < count; i++) {
					void* ptr = (char*)value + i * type->size;
					SerializeType(type, ptr);
				}
			}
		}

		virtual void SerializeValue(
			const iw::Type* type,
			const void* value)
		{
			Write((char*)value, type->size);
		}

		virtual void DeserializeClass(
			const iw::Class* class_,
			void* value)
		{
			if (class_->deserialize) {
				class_->deserialize(*this, value);
			}

			else {
				for (int i = 0; i < class_->fieldCount; i++) {
					const iw::Field& f = class_->fields[i];
					DeserializeField(f, (char*)value + f.offset);
				}
			}
		}

		virtual void DeserializeField(
			const iw::Field& field,
			void* value)
		{
			DeserializeType(field.type, value);
		}

		virtual void DeserializeFieldArray(
			const iw::Field& field,
			const iw::Type* elemType,
			void* value,
			size_t size)
		{
			DeserializeArray(elemType, value, size);
		}

		virtual void DeserializeType(
			const iw::Type* type,
			void* value)
		{
			if (type->isArray) {
				DeserializeArray(type, value, type->count);
			}

			else {
				void* ptr = (char*)value;
				if (type->isClass) {
					const iw::Class* class_ = type->AsClass();
					DeserializeClass(class_, ptr);
				}

				else {
					DeserializeValue(type, ptr);
				}
			}
		}

		virtual void DeserializeArray(
			const iw::Type* type,
			void* value,
			size_t count)
		{
			if (type->type == IntegralType::CHAR) {
				Read((char*)value, count);
				m_stream->get(); // remove null char
			}

			else {
				for (int i = 0; i < count; i++) {
					void* ptr = (char*)value + i * type->size;
					DeserializeType(type, ptr);
				}
			}
		}

		virtual void DeserializeValue(
			const iw::Type* type,
			void* value)
		{
			Read((char*)value, type->size);
		}
	};
}

	using namespace Reflect;
}
