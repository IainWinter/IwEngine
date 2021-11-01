template<>
inline const Class* GetClass(ClassTag<HighscoreRecord>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("HighscoreRecord", sizeof(HighscoreRecord), 4);

		size_t offset = 0;

		c->fields[0] = { "GameId", GetType(TypeTag<std::string>()), offset, false };
		offset += sizeof(std::string);

		c->fields[1] = { "Name", GetType(TypeTag<std::string>()), offset, false };
		offset += sizeof(std::string);

		c->fields[2] = { "Score", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[3] = { "Order", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<
	size_t _iw_array_size>
inline const Class* GetClass(ClassTag<HighscoreRecord[_iw_array_size]>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class(get_array_type_name("HighscoreRecord", _iw_array_size), sizeof(HighscoreRecord[_iw_array_size]), 4);

		size_t offset = 0;

		c->fields[0] = { "GameId", GetType(TypeTag<std::string>()), offset, false };
		offset += sizeof(std::string);

		c->fields[1] = { "Name", GetType(TypeTag<std::string>()), offset, false };
		offset += sizeof(std::string);

		c->fields[2] = { "Score", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[3] = { "Order", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

