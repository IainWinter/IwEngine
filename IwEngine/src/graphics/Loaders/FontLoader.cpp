#include "iw/graphics/Loaders/FontLoader.h"
#include "iw/util/io/File.h"

#undef UNICODE

namespace iw {
namespace Graphics {
	FontLoader::FontLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	enum class State {
		NONE,
		INFO,
		COMMON,
		PAGE,
		CHARS,
		CHAR,
		KERNINGS,
		KERNING
	};

	enum class Type {
		INFO,
		FACE,
		SIZE,
		BOLD,
		ITALIC,
		CHARSET,
		UNICODE,
		STRETCHH,
		SMOOTH,
		AA,
		PADDING,
		SPACING,
		COMMON,
		LINEHEIGHT,
		BASE,
		SCALEW,
		SCALEH,
		PAGES,
		PACKED,
		PAGE,
		ID,
		FILE,
		CHARS,
		COUNT,
		CHAR,
		X,
		Y,
		WIDTH,
		HEIGHT,
		XOFFSET,
		YOFFSET,
		XADVANCE,
		CHNL,
		KERNINGS,
		COUTN,
		KERNING,
		FIRST,
		SECOND,
		AMOUNT
	};

	struct Token {
		Type type;
		std::string value;

		Token(
			Type type,
			std::string value)
			: type(type)
			, value(value) {}
	};

	void ToLower(
		std::string& string)
	{
		for (size_t i = 0; i < string.length(); i++) {
			string[i] = std::tolower(string[i]);
		}
	}

	Font* FontLoader::LoadAsset(
		std::string filepath)
	{
		std::vector<std::string> lines = iw::ReadFileLines(filepath);

		std::vector<Token> tokens;
		for (std::string& line : lines) {
			size_t i = 0;
			size_t end = 0;
			while (end != std::string::npos) {
				end = line.find_first_of(' ', i);

				std::string token = line.substr(i, end - i);

				i = end + 1;

				if (token.find_first_of("=") == std::string::npos) {
					ToLower(token);

					if (token == "info") {
						tokens.emplace_back(Type::INFO, "");
					}

					else if (token == "common") {
						tokens.emplace_back(Type::COMMON, "");
					}

					else if (token == "page") {
						tokens.emplace_back(Type::PAGE, "");
					}

					else if (token == "chars") {
						tokens.emplace_back(Type::CHARS, "");
					}

					else if (token == "char") {
						tokens.emplace_back(Type::CHAR, "");
					}

					else if (token == "kernings") {
						tokens.emplace_back(Type::KERNINGS, "");
					}

					else if (token == "kerning") {
						tokens.emplace_back(Type::KERNING, "");
					}
				}

				else {
					size_t q1 = token.find_first_of('"');
					if (q1 != std::string::npos) {
						size_t q2 = token.find_last_of('"');
						std::string value = token.substr(q1 + 1, q2 - q1 - 1);

						token = token.substr(0, q1 - 1);
						ToLower(token);

						if (token == "face") {
							tokens.emplace_back(Type::FACE, value);
						}

						else if (token == "charset") {
							tokens.emplace_back(Type::CHARSET, value);
						}

						else if (token == "file") {
							tokens.emplace_back(Type::FILE, value);
						}
					}

					else {
						size_t equals = token.find_first_of('=');
						size_t end    = token.find_first_of(' ', equals);
						std::string value = token.substr(equals + 1, end - equals);

						token = token.substr(0, equals);
						ToLower(token);

						if (token == "size") {
							tokens.emplace_back(Type::SIZE, value);
						}

						else if (token == "bold") {
							tokens.emplace_back(Type::BOLD, value);
						}

						else if (token == "italic") {
							tokens.emplace_back(Type::ITALIC, value);
						}

						else if (token == "unicode") {
							tokens.emplace_back(Type::UNICODE, value);
						}

						else if (token == "stretchH") {
							tokens.emplace_back(Type::STRETCHH, value);
						}

						else if (token == "smooth") {
							tokens.emplace_back(Type::SMOOTH, value);
						}

						else if (token == "aa") {
							tokens.emplace_back(Type::AA, value);
						}

						else if (token == "padding") {
							tokens.emplace_back(Type::PADDING, value);
						}

						else if (token == "spacing") {
							tokens.emplace_back(Type::SPACING, value);
						}

						else if (token == "lineheight") {
							tokens.emplace_back(Type::LINEHEIGHT, value);
						}

						else if (token == "base") {
							tokens.emplace_back(Type::BASE, value);
						}

						else if (token == "scalew") {
							tokens.emplace_back(Type::SCALEW, value);
						}

						else if (token == "scaleh") {
							tokens.emplace_back(Type::SCALEH, value);
						}

						else if (token == "pages") {
							tokens.emplace_back(Type::PAGES, value);
						}

						else if (token == "packed") {
							tokens.emplace_back(Type::PACKED, value);
						}

						else if (token == "id") {
							tokens.emplace_back(Type::ID, value);
						}

						else if (token == "file") {
							tokens.emplace_back(Type::FILE, value);
						}

						else if (token == "count") {
							tokens.emplace_back(Type::COUNT, value);
						}

						else if (token == "x") {
							tokens.emplace_back(Type::X, value);
						}

						else if (token == "y") {
							tokens.emplace_back(Type::Y, value);
						}

						else if (token == "width") {
							tokens.emplace_back(Type::WIDTH, value);
						}

						else if (token == "height") {
							tokens.emplace_back(Type::HEIGHT, value);
						}

						else if (token == "xoffset") {
							tokens.emplace_back(Type::XOFFSET, value);
						}

						else if (token == "yoffset") {
							tokens.emplace_back(Type::YOFFSET, value);
						}

						else if (token == "xadvance") {
							tokens.emplace_back(Type::XADVANCE, value);
						}

						else if (token == "page") {
							tokens.emplace_back(Type::PAGE, value);
						}

						else if (token == "chnl") {
							tokens.emplace_back(Type::CHNL, value);
						}

						else if (token == "first") {
							tokens.emplace_back(Type::FIRST, value);
						}

						else if (token == "second") {
							tokens.emplace_back(Type::SECOND, value);
						}

						else if (token == "amount") {
							tokens.emplace_back(Type::AMOUNT, value);
						}
					}
				}
			}
		}

		FontBuilder builder;
		
		// State
		State state = State::NONE;
		unsigned id = 0;
		std::string file = "";
		Character character;
		Kerning kerning;

		for (size_t i = 0; i < tokens.size(); i++) {
			Token token = tokens[i];

			switch (state) {
				case State::INFO: {
					switch (token.type) {
						case Type::FACE: {
							builder.SetName(token.value);
							break;
						}
						case Type::SIZE: {
							builder.SetSize((int)std::stoi(token.value));
							break;
						}
						case Type::BOLD: {
							builder.SetBold((bool)std::stoi(token.value));
							break;
						}
						case Type::ITALIC: {
							builder.SetItalic((bool)std::stoi(token.value));
							break;
						}
						case Type::CHARSET: {
							builder.SetCharSet(token.value);
							break;
						}
						case Type::UNICODE: {
							builder.SetUnicode((bool)std::stoi(token.value));
							break;
						}
						case Type::STRETCHH: {
							builder.SetStretchY((unsigned)std::stoi(token.value));
							break;
						}
						case Type::SMOOTH: {
							builder.SetSmooth((bool)std::stoi(token.value));
							break;
						}
						case Type::AA: {
							builder.SetAnitiAliased((bool)std::stoi(token.value));
							break;
						}
						case Type::PADDING: {
							Padding padding;

							size_t comma = token.value.find_first_of(',');
							size_t comma1 = token.value.find_first_of(',', comma + 1);
							size_t comma2 = token.value.find_first_of(',', comma1 + 1);

							padding.Top = (unsigned)std::stoi(token.value.substr(0, comma));
							padding.Right = (unsigned)std::stoi(token.value.substr(comma + 1, comma1 - comma));
							padding.Bottom = (unsigned)std::stoi(token.value.substr(comma1 + 1, comma2 - comma1));
							padding.Left = (unsigned)std::stoi(token.value.substr(comma2 + 1));

							builder.SetPadding(padding);
							break;
						}
						case Type::SPACING: {
							Spacing spacing;

							size_t comma = token.value.find_first_of(',');

							spacing.X = (unsigned)std::stoi(token.value.substr(0, comma));
							spacing.Y = (unsigned)std::stoi(token.value.substr(comma + 1));

							builder.SetSpacing(spacing);
							break;
						}
						default: {
							state = State::NONE;
							break;
						}
					}
					break;
				}
				case State::COMMON: {
					switch (token.type) {
						case Type::LINEHEIGHT: {
							builder.SetLightHeight((unsigned)std::stoi(token.value));
							break;
						}
						case Type::BASE: {
							builder.SetBase((unsigned)std::stoi(token.value));
							break;
						}
						case Type::SCALEW: {
							builder.SetScaleX((unsigned)std::stoi(token.value));
							break;
						}
						case Type::SCALEH: {
							builder.SetScaleY((unsigned)std::stoi(token.value));
							break;
						}
						case Type::PAGES: {
							builder.SetTextureCount((unsigned)std::stoi(token.value));
							break;
						}
						case Type::PACKED: {
							//builder.SetPacked((bool)std::stoi(token.value));
							break;
						}
						default: {
							state = State::NONE;
							break;
						}
					}
					break;
				}
				case State::PAGE: {
					switch (token.type) {
						case Type::ID: {
							id = std::stoi(token.value);
							break;
						}
						case Type::FILE: {
							file = token.value;
							break;
						}
						default: {
							builder.SetTexture(id, m_asset.Load<Texture>(file));
							id = 0;
							file.clear();
							state = State::NONE;
							break;
						}
					}
					break;
				}
				case State::CHARS: {
					switch (token.type) {
						case Type::COUNT: {
							builder.SetCharacterCount((unsigned)std::stoi(token.value));
							break;
						}
						default: {
							state = State::NONE;
							break;
						}
					}
					break;
				}
				case State::CHAR: {
					switch (token.type) {
						case Type::ID: {
							id = std::stoi(token.value);
							break;
						}
						case Type::X: {
							character.X = std::stoi(token.value);
							break;
						}
						case Type::Y: {
							character.Y = std::stoi(token.value);
							break;
						}
						case Type::WIDTH: {
							character.Width = std::stoi(token.value);
							break;
						}
						case Type::HEIGHT: {
							character.Height = std::stoi(token.value);
							break;
						}
						case Type::XOFFSET: {
							character.Xoffset = std::stoi(token.value);
							break;
						}
						case Type::YOFFSET: {
							character.Yoffset = std::stoi(token.value);
							break;
						}
						case Type::XADVANCE: {
							character.Xadvance = std::stoi(token.value);
							break;
						}
						case Type::PAGE: {
							character.Page = (unsigned)std::stoi(token.value);
							break;
						}
						case Type::CHNL: {
							character.Chnl = (unsigned)std::stoi(token.value);
							break;
						}
						default: {
							builder.SetCharacter(id, character);
							id = 0;
							character.X = 0;
							character.Y = 0;
							character.Width = 0;
							character.Height = 0;
							character.Xoffset = 0;
							character.Yoffset = 0;
							character.Xadvance = 0;
							character.Chnl = 0;
							state = State::NONE;
						}
					}
					break;
				}
				case State::KERNINGS: {
					switch (token.type) {
						case Type::COUNT: {
							builder.SetKerningCount((unsigned)std::stoi(token.value));
							break;
						}
						default: {
							state = State::NONE;
							break;
						}
					}
					break;
				}
				case State::KERNING: {
					switch (token.type) {
						case Type::FIRST: {
							kerning.First = (unsigned)std::stoi(token.value);
							break;
						}
						case Type::SECOND: {
							kerning.Second = (unsigned)std::stoi(token.value);
							break;
						}
						case Type::AMOUNT: {
							kerning.Amount = (int)std::stoi(token.value);
							break;
						}
						default: {
							builder.AddKerning(kerning);
							kerning.First = 0;
							kerning.Second = 0;
							kerning.Amount = 0;
							state = State::NONE;
						}
					}
					break;
				}
				case State::NONE: {
					break;
				}
				default: {
					static_assert("Why are we here?");
				}
			}

			if (state == State::NONE) {
				switch (token.type) {
					case Type::INFO: {
						state = State::INFO;
						break;
					}
					case Type::COMMON: {
						state = State::COMMON;
						break;
					}
					case Type::PAGE: {
						state = State::PAGE;
						break;
					}
					case Type::CHARS: {
						state = State::CHARS;
						break;
					}
					case Type::CHAR: {
						state = State::CHAR;
						break;
					}
					case Type::KERNINGS: {
						state = State::KERNINGS;
						break;
					}
					case Type::KERNING: {
						state = State::KERNING;
						break;
					}
					default: {
						LOG_WARNING << "Invalid token in font file " << filepath;
						return nullptr;
					}
				}
			}

		}

		return builder.Build();
	}
}
}
