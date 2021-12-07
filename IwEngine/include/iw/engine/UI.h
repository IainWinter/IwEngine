#pragma once

#include "iw/engine/Time.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/graphics/Font.h"
#include "iw/input/Devices/Mouse.h"
#include <array>
#include <tuple>
#include <functional>
#include <iomanip> // required for std::setw

template<
	typename _t>
std::string to_string(const _t& t) // this should go into util.h or something
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << t;
	return ss.str();
}

using render = iw::ref<iw::QueuedRenderer>;

struct UI;
struct UI_Screen;

struct UI_Base
{
	float x, y, zIndex, width, height;
	iw::Transform transform;
	bool active;

	std::vector<UI_Base*> children;
	UI_Base* parent;

	UI_Base()
		: x      (0.f)
		, y      (0.f)
		, zIndex (0.f)
		, width  (0.f)
		, height (0.f)
		, active (true)
		, parent (nullptr)
	{}

	virtual ~UI_Base()
	{
		for (UI_Base* ui : children)
		{
			ui->transform.SetParent(nullptr);
			delete ui;
		}
	}

	template<
		typename _ui = UI>
	_ui* GetElement(
		int index = 0)
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return dynamic_cast<_ui*>(children.at(index));
	}

	template<
		typename _ui = UI,
		typename... _args>
	_ui* CreateElement(
		const _args&... args)
	{
		_ui* ui = new _ui(args...);
		AddElement(ui);
		return ui;
	}

	template<
		typename _ui = UI,
		typename... _args>
	_ui* SetElement(
		const _args&... args)
	{
		RemoveElement(GetElement(0));

		_ui* ui = new _ui(args...);
		AddElement(ui);
		return ui;
	}

	void RemoveElement(
		UI_Base* ui)
	{
		if (children.size() == 0)
		{
			return;
		}

		children.erase(std::find(children.begin(), children.end(), ui));
	}

	template<
		typename _f>
	void WalkTree(
		_f&& functor,
		UI_Base* parent = nullptr)
	{
		if (!active)
		{
			return;
		}

		functor(this, parent);
		for (UI_Base* child : children)
		{
			child->WalkTree(functor, this);
		}
	}

	void AddElement(
		UI_Base* element)
	{
		element->parent = this;
		element->init();
		children.push_back(element);
	}

	std::pair<float, float> TransformPoint(
		float px, float py)
	{
		auto [tx, ty] = parent
					? parent->TransformPoint(px, py)
					:         std::make_pair(px, py);

		tx -= x;
		ty -= y;

		// reverse scaling?

		return std::make_pair(tx, ty);
	}

	bool IsPointOver(/*const*/ iw::vec2& v) { return IsPointOver(v.x(), v.y()); }
	bool IsPointOver(float px, float py)
	{
		auto [tx, ty] = TransformPoint(px, py);

		return  tx > -width  && tx < width
			&& ty > -height && ty < height;
	}

	virtual void UpdateTransform(
		UI_Base* parent) 
	{
		float pw = parent->width  == 0 ? 1 : parent->width;
		float ph = parent->height == 0 ? 1 : parent->height;

		float x_ = x     / pw;
		float y_ = y     / ph;
		float w = width  / pw;
		float h = height / ph;

		transform.Position = glm::vec3(x_, y_, zIndex);
		transform.Scale    = glm::vec3(w, h, 1);

		transform.SetParent(parent ? &parent->transform : nullptr);
	}

	virtual void Draw(
		render& r,
		UI_Base* parent)
	{
		if (!active)
		{
			return;
		}

		UpdateTransform(parent);

		for (UI_Base* child : children)
		{
			child->Draw(r, this);
		}
	}

	virtual void init() {}
};

struct UI_Clickable
{
	std::function<void()> onClick;
	std::function<void()> whileMouseDown;
	std::function<void()> whileMouseHover;

	bool clickActive = true;
};

#define ma(r, g, b, a) m_screen->MakeMesh(iw::Color(r, g, b, a))
#define m(r, g, b) m_screen->MakeMesh(iw::Color(r, g, b, 1))

struct UI_Screen : UI_Base
{
	int depth;
	iw::OrthographicCamera camera;

	iw::Mesh          defaultMesh;
	iw::ref<iw::Font> defaultFont;

	UI_Screen()
		: UI_Base ()
		, depth   (0)
	{}

	iw::Mesh MakeMesh(
		iw::Color& color)
	{
		iw::Mesh mesh = defaultMesh.MakeInstance();
		mesh.Material->Set("color", color);

		return mesh;
	}

	void UpdateTransform(
		UI_Base* parent) override
	{
		transform = iw::Transform();
		transform.Position.z = depth;
		transform.SetParent(parent ? &parent->transform : nullptr);
	}

	void Draw(
		render& r,
		UI_Base* parent = nullptr) override
	{
		float minZ =  FLT_MAX;
		float maxZ = -FLT_MAX;
		UI_Base::WalkTree([&](UI_Base* ui, UI_Base* parent)
		{
			float z = ui->transform.WorldPosition().z;
			if (z < minZ) minZ = z;
			if (z > maxZ) maxZ = z;
		});

		// normalize Z

		camera.NearClip = -10; // this doesnt work for some reason???? seems to clip some things if they are far form 0
		camera.FarClip  =  10;

		camera.Transform.Position.z = maxZ;

		r->BeginScene(&camera);

		UI_Base::Draw(r, parent);

		r->EndScene();
	}

	iw::vec2 LocalMouse()
	{
		iw::vec2 mouse = iw::Mouse::ClientPos();

		return iw::vec2(
				mouse.x() * 2 - width,
			-(mouse.y() * 2 - height)
		);
	}
};

struct UI : UI_Base
{
	iw::Mesh mesh;

	UI()
		: UI_Base()
	{}

	UI(
		const iw::Mesh& mesh
	)
		: UI_Base ()
		, mesh    (mesh)
	{}

	UI_Screen* GetScreen() 
	{
		UI_Base* p = parent;
		while (p->parent)
		{
			p = p->parent;
		}

		return dynamic_cast<UI_Screen*>(p); // return nullptr or screen
	}

	void Draw(
		render& r,
		UI_Base* parent) override
	{
		if (!active)
		{
			return;
		}

		UI_Base::Draw(r, parent);

		if (mesh.Data)
		{
			r->DrawMesh(transform, mesh);
		}
	}
};

struct UI_Image : UI
{
	iw::ref<iw::Texture> texture;

	UI_Image(
		const iw::ref<iw::Texture>& texture
	)
		: UI      ()
		, texture (texture)
	{}

	UI_Image(
		const iw::Mesh& mesh,
		const iw::ref<iw::Texture>& texture
	)
		: UI      (mesh)
		, texture (texture)
	{}

	void init() override
	{
		texture->SetFilter(iw::NEAREST);
		mesh = GetScreen()->defaultMesh.MakeInstance();
		mesh.Material->SetTexture("texture", texture);
		mesh.Material->Set("color", iw::Color(1.f));
	}
};

struct UI_Text : UI
{
	iw::ref<iw::Font> font;
	iw::FontMeshConfig config;
	std::string str;

	UI_Text(
		const std::string& string = "",
		iw::FontMeshConfig config = {},
		const iw::ref<iw::Font>& font = nullptr
	)
		: UI     ()
		, font   (font)
		, config (config)
		, str    (string)
	{}

	void SetString(const std::string& str)
	{
		this->str = str;
		font->UpdateMesh(mesh, str, config);
	}

	void init() override
	{
		if (!font)
		{
			font = GetScreen()->defaultFont;
		}

		mesh = font->GenerateMesh("", config);
		mesh.Material = mesh.Material->MakeInstance();
		mesh.Material->Set("color", iw::Color(1.f));

		SetString(str);
	}
};

inline void position_label(UI_Base* me)
{
	if (me->children.size() > 0)
	{
		UI_Base* label = me->children.at(0);
		if (!label) return;

		UI_Text* text = dynamic_cast<UI_Text*>(label);

		if (text)
		{
			text->x = -me->width + 15; // left anchor + padding
			text->y = me->height - 15;  // top  anchor + padding
			text->width  = me->height;
			text->height = me->height; // keep 1 : 1, only for text rendering this will screw up everything else
		}

		else
		{
			label->x = 0;           // center
			label->y = 0;           // center
			label->width  = me->width;
			label->height = me->height; // keep 1 : 1, only for text rendering this will screw up everything else
		}

		label->zIndex = me->zIndex + 1; // ontop of button
	}
}

// this is very close to the UI_Table_Item struct...
struct UI_Button : UI, UI_Clickable
{
	float offset;
	float offsetTarget;
	float offsetTimeScale;

	std::function<void(UI_Button*)> effect;

	UI_Button(
		const iw::Mesh& bg = iw::Mesh()
	)
		: UI (bg)
	{
		_reset();
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		effect(this);
		position_label(this);
		UI_Base::UpdateTransform(parent);
	}

private:
	void _reset()
	{
		offset          =  0.f;
		offsetTarget    =  0.f;
		offsetTimeScale = 20.f;

		whileMouseHover = [this]()
		{
			offsetTarget = 8;
			offsetTimeScale = 20.f;
		};

		whileMouseDown = [this]()
		{
			offsetTarget = 0;
			offsetTimeScale = 40.f;
		};

		effect = [](UI_Button* button)
		{
			button->offset = iw::lerp(
				button->offset,
				button->offsetTarget,
				button->offsetTimeScale * iw::DeltaTime());

			button->x -= floor(button->offset);
			button->y += floor(button->offset);
			button->offsetTarget = 0;
		};
	}
};

struct UI_Slider : UI, UI_Clickable
{
	float value; // from 0 - 1
	float lastValue;
	std::function<void(float)> onChangeValue;

	UI* slider;
	UI* label_name;
	UI* label_value;

	iw::ref<iw::Font> font_value;
	iw::FontMeshConfig font_config;

	UI_Slider(
		const iw::Mesh& background,
		const iw::Mesh& slider,
		const std::string& name,
		iw::ref<iw::Font> font_value
	)
		: UI         (background)
		, value      (0.f)
		, slider     (CreateElement(slider))
		, font_value (font_value)
	{
		whileMouseDown = [this]()
		{
			iw::vec2 mouse = GetScreen()->LocalMouse();
			auto [x, y] = TransformPoint(mouse.x(), mouse.y());
			UpdateValue((iw::clamp(x / width, -1.f, 1.f) + 1.f) / 2.f);
		};

		font_config = { 360 };

		label_name = CreateElement(
			font_value->GenerateMesh(name, font_config)
		);

		font_config.Anchor = iw::FontAnchor::TOP_RIGHT;

		label_value = CreateElement(
			font_value->GenerateMesh("0", font_config)
		);
	}

	void UpdateValue(
		float newValue)
	{
		// add option for rounding
		value = iw::clamp(
			round(newValue * 100) / 100,
			-1.f,
				1.f
		);

		if (value != lastValue)
		{
			lastValue = value;

			font_value->UpdateMesh(
				label_value->mesh, to_string(value), font_config);

			if (onChangeValue)
			{
				onChangeValue(newValue);
			}
		}
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		label_name->x = -width + 15;
		label_name->y = height - 5;
		label_name->width  = height;
		label_name->height = height;
		label_name->zIndex = zIndex + 1;

		label_value->x = width - 15;
		label_value->y = height - 5;
		label_value->width  = height;
		label_value->height = height;
		label_value->zIndex = zIndex + 1;

		slider->x = -width + 2 * width * value;
		slider->y = 0;
		slider->width  = 10;
		slider->height = 50;
		slider->zIndex = zIndex + 2;

		UI_Base::UpdateTransform(parent);
	}
};

struct UI_Table_Item : UI
{
	UI_Table_Item(
		const iw::Mesh& background
	)
		: UI (background)
	{}

	// can remove if using std::any in table data storage
	UI_Table_Item(
		const iw::Mesh& background,
		const iw::ref<iw::Font>& font,
		const iw::FontMeshConfig& fontConfig,
		const std::string& string
	)
		: UI (background)
	{
		CreateElement<UI_Text>(string, fontConfig, font);
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		position_label(this);
		UI_Base::UpdateTransform(parent);
	}
};

template<
	typename... _cols>
struct UI_Table : UI
{
	// special for font table just for AddRow ease of use
	iw::Mesh background;
	iw::ref<iw::Font> font;
	iw::FontMeshConfig fontConfig;
	// maybe make a seperate class

	using row_t  = std::array<UI_Table_Item*, sizeof...(_cols)>;
	using dataptr_t = std::array<void*, sizeof...(_cols)>;
	using data_t = std::tuple<_cols...>;

	//using data_T = std::array<std::any, sizeof...(_cols)>;

	const int count = sizeof...(_cols);

	struct UI_Row
	{
		int index;
		row_t elements;
		data_t data;

		UI_Row(
			int index,
			const row_t& elems,
			const data_t& data
		)
			: index    (index)
			, elements (elems)
			, data     (data)
		{}

		template<
			typename _t = UI_Table_Item*>
		_t Elem(
			size_t i)
		{
			return (_t)elements.at(i);
		}

		template<
			size_t _i>
		std::tuple_element_t<_i, data_t>& Data()
		{
			return std::get<_i>(data);
		}

		template<
			typename _t>
		_t& Data(
			size_t i)
		{
			// shouldnt need to do this everytime :(
			dataptr_t ptrs = iw::geteach<__get_ptrs, data_t, dataptr_t, sizeof...(_cols)>(data);
			return *(_t*)ptrs.at(i);
		}

		struct __get_ptrs
		{
			template<
				typename _t>
			void* operator()(_t& t)
			{
				return (void*)&t;
			}
		};
	};

	std::vector<UI_Row> rows;

	// table width  is uibase width
	// table height is uibase height

	float scrollOffset;

	float rowHeight;
	float rowPadding;

	std::array<float, sizeof...(_cols)> colWidth;
	std::array<float, sizeof...(_cols)> colPadding;

	std::function<bool(const data_t&, const data_t&)> sort; // return true if 'b' is larger

	UI_Table(
		const iw::Mesh& tableBg = iw::Mesh()
	)
		: UI (tableBg)
	{
		_reset();
	}

private:
	void _reset()
	{
		rowHeight = 0.f;
		rowPadding = 0.f;
		colWidth = {};
		colPadding = {};
		scrollOffset = 0.f;

		for (int i = 0; i < sizeof...(_cols); i++)
		{
			colWidth  [i] = 0;
			colPadding[i] = 0;
		}

		fontConfig.Size = 360;
		fontConfig.Anchor = iw::FontAnchor::TOP_LEFT;
	}

public:
	float WidthRemaining(
		size_t colToNotConsider)
	{
		float widthRemaining = width;

		for (size_t i = 0; i < sizeof...(_cols); i++)
		{
			if (i == colToNotConsider) continue;
			widthRemaining -= colPadding[i] + colWidth[i];
		}

		return widthRemaining;
	}

	int AddRow(
		const _cols&... rowData,
		const std::array<bool, sizeof...(_cols)>& hide = {})
	{
		return AddRow(rowData..., true, hide);
	}

	int AddRow(
		const _cols&... rowData,
		bool makeInstanceOfBackgroundMesh,
		const std::array<bool, sizeof...(_cols)>& hide = {})
	{
		iw::Mesh mesh;

		if (    background.Data
			&& makeInstanceOfBackgroundMesh)
		{
			mesh = background.MakeInstance();
		}

		else
		{
			mesh = background; // if backgound is empty then mesh gets set as empty
		}

		// mesh could be empty and font could be empty

		row_t row = {};

		if (font)
		{
			row = {
				new UI_Table_Item(
					mesh,
					font, fontConfig, to_string(rowData)
				)...
			};
		}

		else
		{
			for (int i = 0; i < sizeof...(_cols); i++)
			{
				if (!hide[i])
				{
					row[i] = new UI_Table_Item(mesh);
				}
			}
		}

		return AddRow(std::make_tuple(rowData...), row);
	}

	int AddRow(
		const data_t& data,
		const row_t& row)
	{
		int id = rows.size();

		for (UI_Base* item : row)
		{
			if (item)
			{
				AddElement(item);
			}
		}

		// custom upper bound that just uses data

		if (sort)
		{
			size_t i = 0;
			for (; i < rows.size(); i++)
			{
				if (sort(rows.at(i).data, data))
				{
					break;
				}
			}

			rows.emplace(rows.begin() + i, id, row, data);
		}

		else
		{
			rows.emplace_back(id, row, data);
		}

		return id;
	}

	void RemoveRow(
		int id)
	{
		for (auto itr = rows.begin(); itr != rows.end(); ++itr)
		{
			if (id == itr->index)
			{
				for (UI_Base* ui : itr->elements)
				{
					RemoveElement(ui);
					delete ui;
				}

				rows.erase(itr);
				break;
			}
		}
	}

	UI_Row* Row(
		int id)
	{
		for (UI_Row& row : rows)
		{
			if (id == row.index)
			{
				return &row;
			}
		}

		return nullptr;
	}

	void UpdateRow(
		int id,
		int col,
		std::string str)
	{
		font->UpdateMesh(
			Row(id)->Elem(col)->GetElement()->mesh,
			str,
			{ 360, iw::FontAnchor::TOP_RIGHT }
		);
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		scrollOffset = iw::clamp(
			scrollOffset,
			0.f,
			iw::max(
				0.f,
				rows.size() * (rowHeight * 2.f + rowPadding) - height * 2.f
			)
		);

		float cursorY = height - rowPadding;

		for (size_t i = 0; i < rows.size(); i++)
		{
			float cursorX = -width + colPadding[0];

			for (size_t j = 0; j < sizeof...(_cols); j++)
			{
				UI_Table_Item* item = rows.at(i).Elem(j);
				if (!item) continue;

				float nextTop = -cursorY + rowPadding - scrollOffset;
				float nextBot = -cursorY - rowPadding - scrollOffset + rowHeight * 2;

				item->active = nextTop < height && nextBot > -height;

				if (!item->active)
				{
					continue;
				}

				item->zIndex = zIndex + 1;

				item->x = cursorX + colWidth[j];
				item->y = cursorY - rowHeight + scrollOffset;

				item->width  = colWidth[j];
				item->height = rowHeight;

				if (j + 1u < sizeof...(_cols)) // annoying flow
				{
					cursorX += item->width * 2 + colPadding[j + 1u];
				}
			}

			cursorY -= rowHeight * 2 + rowPadding;
		}

		UI_Base::UpdateTransform(parent);
	}
};

/*
// UI testing


//struct UIConstraint
//{
//	virtual float Get(int parent) const = 0;
//	virtual ~UIConstraint() = default;
//};

//struct Fixed : UIConstraint
//{
//	float value;

//	Fixed(float value_) {
//		value = value_;
//	}

//	float Get(int parent) const override
//	{
//		return value;
//	}
//};

//struct Ratio : UIConstraint
//{
//	float value;

//	Ratio(float value) : value(value) {}

//	float Get(int parent) const override
//	{
//		return parent * value;
//	}
//};*/
