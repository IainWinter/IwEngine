#pragma once

#include "iw/graphics/QueuedRenderer.h"
#include "iw/graphics/Font.h"
#include "iw/input/Devices/Mouse.h"
#include <array>
#include <tuple>
#include <functional>

template<
	typename _t>
std::string to_string(const _t& t) // this should go into util.h or something
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

using render = iw::ref<iw::QueuedRenderer>;

struct UI;

struct UI_Base
{
	float x, y, zIndex, width, height;
	iw::Transform transform;
	std::vector<UI_Base*> children;

	bool active;

	UI_Base()
		: x      (0.f)
		, y      (0.f)
		, zIndex (0.f)
		, width  (0.f)
		, height (0.f)
		, active (true)
	{}

	virtual ~UI_Base()
	{
		for (UI_Base* ui : children)
		{
			delete ui;
		}
	}

	template<
		typename _ui = UI,
		typename... _args>
	_ui* CreateElement(
		const _args&... args)
	{
		_ui* ui = new _ui(args...);
		children.push_back(ui);
		return ui;
	}

	template<
		typename _f>
	void WalkTree(
		_f&& functor)
	{
		if (!active)
		{
			return;
		}

		functor(this);
		for (UI_Base* child : children)
		{
			child->WalkTree(functor);
		}
	}

	void AddElement(
		UI_Base* element)
	{
		children.push_back(element);
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

		transform = iw::Transform();
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
};

struct UI : UI_Base
{
	iw::Mesh mesh;

	UI(
		const iw::Mesh& mesh
	)
		: UI_Base ()
		, mesh    (mesh)
	{}

	void Draw(
		render& r,
		UI_Base* parent) override
	{
		if (!active)
		{
			return;
		}

		UI_Base::Draw(r, parent);
		r->DrawMesh(transform, mesh);
	}
	
	bool IsPointOver(/*const*/ iw::vec2& v) { return IsPointOver(v.x(), v.y()); }
	bool IsPointOver(float tx, float ty)
	{
		return tx > x - width  && tx < x + width
			&& ty > y - height && ty < y + height;
	}
};

struct UI_Button : UI
{
	float offset;
	std::function<void()> onClick;

	UI_Button(
		const iw::Mesh& button,
		const iw::Mesh& label
	)
		: UI     (button)
		, offset (0.f)
	{
		CreateElement(label);
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		UI_Base* label = children.at(0);
		if (label)
		{
			label->x = -width  + 15;    // left anchor + padding
			label->y =  height - 5;     // top  anchor + padding
			label->width  = height;
			label->height = height;      // keep 1 : 1
			label->zIndex = zIndex + 1; // ontop of button
		}
		//else
		//{
		//	LOG_WARNING << "Button has no label!";
		//}

		UI_Base::UpdateTransform(parent);
	}
};

//struct UI_Table_Row_Base
//{
//	int Id;
//};
//
//template<
//	typename... _t>
//struct UI_Table_Row : UI_Table_Row_Base
//{
//	std::tuple<_t...> data;
//
//
//};

// test hard coding for 3 elements
// 

struct UI_Table_Item : UI
{
	UI_Table_Item(
		const iw::Mesh& background,
		const iw::Mesh& element
	)
		: UI (background)
	{
		CreateElement(element);
	}

	UI* GetElement()
	{
		return (UI*)children.at(0);
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		UI* label = GetElement();
		if (label)
		{
			label->x =  width;    // left anchor + padding
			label->y =  height;     // top  anchor + padding
			label->width  = height;
			label->height = height;     // keep 1 : 1
			label->zIndex = zIndex + 1; // ontop of element
		}

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
	// maybe make a seperate class

	using row_t  = std::array<UI_Table_Item*, sizeof...(_cols)>;
	using data_t = std::tuple<_cols...>;

	//using sort_func = std::function<>

	std::vector<
		std::tuple<
			int,    // id
			data_t,
			row_t>> rows;

	// table width  is uibase width
	// table height is uibase height

	float scrollOffset;

	float rowHeight;
	float rowPadding;

	std::array<float, sizeof...(_cols)> colWidth;
	std::array<float, sizeof...(_cols)> colPadding;

	std::function<bool(const data_t&, const data_t&)> sort; // return true if 'b' is larger

	UI_Table(
		const iw::Mesh& background1,
		const iw::Mesh& background2,
		iw::ref<iw::Font> font
	)
		: UI           (background1)
		, background   (background2)
		, font         (font)

		, rowHeight    (0.f)
		, rowPadding   (0.f)
		, colWidth     ()
		, colPadding   ()

		, scrollOffset (0.f)
	{
		for (int i = 0; i < sizeof...(_cols); i++)
		{
			colWidth  [i] = 0;
			colPadding[i] = 0;
		}
	}

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
		bool makeInstanceOfBackgroundMesh = false)
	{
		iw::Mesh mesh;
		if (makeInstanceOfBackgroundMesh)
		{
			mesh = background.MakeInstance();
		}

		else
		{
			mesh = background;
		}

		row_t row
		{
			new UI_Table_Item(
				mesh,
				font->GenerateMesh(
					to_string(rowData),
					{ 360, iw::FontAnchor::TOP_RIGHT }
				)
			)...
		};

		return AddRow(std::make_tuple(rowData...), row);
	}

	int AddRow(
		const data_t& data,
		const row_t& row)
	{
		int id = rows.size();

		for (UI_Table_Item* item : row)
		{
			children.push_back(item);
		}

		// custom upper bound that just uses data

		if (sort)
		{
			int i = 0;
			for (; i < (int)rows.size(); i++)
			{
				if (sort(std::get<1>(rows.at(i)), data))
				{
					break;
				}
			}

			rows.emplace(rows.begin() + i, id, data, row);
		}

		return id;
	}

	row_t* GetRow(
		int id)
	{
		row_t* row = nullptr;

		for (auto& [rid, rdata, rarray] : rows)
		{
			if (id == rid)
			{
				row = &rarray;
				break;
			}
		}

		return row;
	}

	void UpdateRow(
		int id,
		int col,
		std::string str)
	{
		font->UpdateMesh(
			GetRow(id)->at(col)->GetElement()->mesh,
			str,
			{ 360, iw::FontAnchor::TOP_RIGHT }
		);
	}

	void UpdateTransform(
		UI_Base* parent)
	{
		float cursorY = height - rowPadding;

		for (size_t i = 0; i < rows.size(); i++)
		{
			float cursorX = -width + colPadding[0];

			for (size_t j = 0; j < sizeof...(_cols); j++)
			{
				UI_Table_Item* item = std::get<2>(rows[i])[j];

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

struct UI_Screen : UI_Base
{
	int depth;
	iw::OrthographicCamera camera;

	UI_Screen()
		: UI_Base ()
		, depth   (0)
	{}

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
		UI_Base::WalkTree([&](UI_Base* ui)
		{
			if (ui->zIndex < minZ) minZ = ui->zIndex;
			if (ui->zIndex > maxZ) maxZ = ui->zIndex;
		});

		// normalize Z

		camera.NearClip = minZ - 1; // this doesnt work for some reason????
		camera.FarClip  = maxZ + 1;

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
