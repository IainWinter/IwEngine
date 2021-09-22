#pragma once

#include "iw/graphics/QueuedRenderer.h"

struct UI
{
	float x, y, z, width, height;
	iw::Mesh mesh;

	UI(const iw::Mesh& mesh_)
	{
		mesh = mesh_;
		x = 0;
		y = 0;
		z = 0;
		width = 0;
		height = 0;
	}

	iw::Transform GetTransform(int screenWidth, int screenHeight, int screenDepth)
	{
		iw::Transform transform;
		transform.Position.x = floor(x) / screenWidth;
		transform.Position.y = floor(y) / screenHeight;
		transform.Position.z = z / screenDepth;
		transform.Scale.x = floor(width) / screenWidth;
		transform.Scale.y = floor(height) / screenHeight;

		return transform;
	}
};

struct UIScreen
{
	std::vector<UI*> elements;
	int width, height, depth;

	UIScreen(int width_ = 0, int height_ = 0, int depth_ = 10)
	{
		width = width_;
		height = height_;
		depth = depth_;
	}

	~UIScreen()
	{
		for (UI* ui : elements)
		{
			delete ui;
		}
	}

	UI* CreateElement(const iw::Mesh& mesh)
	{
		return elements.emplace_back(new UI(mesh));
	}

	void AddElement(UI* element)
	{
		elements.push_back(element);
	}

	void Draw(iw::Camera* camera, iw::ref<iw::QueuedRenderer>& renderer)
	{
		renderer->BeginScene(camera);

		for (UI* ui : elements)
		{
			renderer->DrawMesh(ui->GetTransform(width, height, depth), ui->mesh);
		}

		renderer->EndScene();
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
