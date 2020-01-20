#include "Pipeline/FilterTarget.h"

namespace IW {
	FilterTarget::FilterTarget(
		iw::ref<Renderer> r)
		: iw::node(4, 1)
		, renderer(r)
	{}

	void FilterTarget::SetIntermediate(
		iw::ref<RenderTarget>& target)
	{
		set<iw::ref<RenderTarget>>(1, target);
	}

	void FilterTarget::SetShader(
		iw::ref<Shader>& shader)
	{
		set<iw::ref<Shader>>(2, shader);
	}

	void FilterTarget::SetBlur(
		iw::vector2& blur)
	{
		set<iw::vector2>(3, blur);
	}

	iw::ref<RenderTarget>& FilterTarget::GetTarget() {
		return in<iw::ref<RenderTarget>>(0);
	}

	iw::ref<RenderTarget>& FilterTarget::GetIntermediate() {
		return in<iw::ref<RenderTarget>>(1);
	}

	iw::ref<Shader>& FilterTarget::GetShader() {
		return in<iw::ref<Shader>>(2);
	}

	iw::vector2& FilterTarget::GetBlur() {
		return in<iw::vector2>(3);
	}

	void FilterTarget::execute() {
		iw::ref<RenderTarget>& target = GetTarget();
		iw::ref<RenderTarget>& intermediate = GetIntermediate();
		iw::ref<Shader>& shader = GetShader();
		iw::vector2& blur = GetBlur();

		renderer->SetShader(shader);

		shader->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(blur.x, 0, 0), 3);
		renderer->ApplyFilter(shader, target.get(), intermediate.get());

		shader->Program->GetParam("blurScale")->SetAsFloats(&iw::vector3(0, blur.y, 0), 3);
		renderer->ApplyFilter(shader, intermediate.get(), target.get());

		out(0, target->Tex(0));
	}
}
