#pragma once

#include "Serializer.h"

namespace iw {
	// WIP
	class JsonSerializer
		: public Serializer
	{
	public:
		IWUTIL_API
		JsonSerializer();

		IWUTIL_API
		JsonSerializer(
			std::string filePath);

		IWUTIL_API
		virtual void SerializeClass(
			const iw::Class* class_,
			const void* value) override;

		IWUTIL_API
		virtual void SerializeField(
			const iw::Field& field,
			const void* value) override;

		IWUTIL_API
		virtual void SerializeValue(
			const iw::Type* type,
			const void* value) override;
	};
}
