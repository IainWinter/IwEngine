#pragma once

#include "iw/graphics/QueuedRenderer.h"
#include "iw/input/Devices/Mouse.h"

using render = iw::ref<iw::QueuedRenderer>;

struct UI_Base
{
	float x, y, zIndex, width, height;
	iw::Transform transform;
	std::vector<UI_Base*> children;

	UI_Base()
		: x      (0.f)
		, y      (0.f)
		, zIndex (0.f)
		, width  (0.f)
		, height (0.f)
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

	void AddElement(
		UI_Base* element)
	{
		children.push_back(element);
	}

	virtual void UpdateTransform(
		UI_Base* parent) 
	{
		float x_ = x / parent->width;
		float y_ = y / parent->height;
		float w = width  == 0 ? 1 : width  / parent->width;
		float h = height == 0 ? 1 : height / parent->height;

		transform = iw::Transform();
		transform.Position = glm::vec3(x_, y_, -zIndex);
		transform.Scale    = glm::vec3(w, h, 1);

		transform.SetParent(parent ? &parent->transform : nullptr);
	}

	virtual void Draw(
		render& r,
		UI_Base* parent)
	{
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
		else
		{
			LOG_WARNING << "Button has no label!";
		}

		UI_Base::UpdateTransform(parent);
	}
};

struct UI_Screen : UI_Base
{
	int depth;
	iw::Camera* camera;

	UI_Screen()
		: UI_Base ()
		, depth   (10.f)
		, camera  (nullptr)
	{}

	void UpdateTransform(
		UI_Base* parent) override
	{
		transform = iw::Transform();
		//transform.Scale.x = width;
		//transform.Scale.y = height;
		transform.Scale.z = depth  == 0 ? 1 : 1.f / depth;
		transform.SetParent(parent ? &parent->transform : nullptr);
	}

	void Draw(
		render& r,
		UI_Base* parent = nullptr) override
	{
		r->BeginScene(camera);

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
