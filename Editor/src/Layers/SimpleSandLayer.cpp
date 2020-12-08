#include "Layers/SimpleSandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

namespace iw {
	SimpleSandLayer::SimpleSandLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, camera(nullptr)
		, target(nullptr)
	{
		srand(time(nullptr));
	}

	float scale = 7;

	int SimpleSandLayer::Initialize() {
		shader = Asset->Load<Shader>("shaders/texture.shader");

		//camera = PushSystem<EditorCameraControllerSystem>()->GetCamera();
		
		if (int err = Layer::Initialize()) {
			return err;
		}

		//camera->SetPosition(vector3(0, 2, 10));
		//camera->SetRotation(quaternion::from_look_at(camera->Position()).inverted());

		texture = REF<Texture>(Renderer->Width() / scale, Renderer->Height() / scale, iw::TextureType::TEX_2D, iw::TextureFormat::RG);
		texture->CreateColors();
		texture->Clear();
		texture->SetFilter(iw::TextureFilter::NEAREST);

		target = REF<RenderTarget>();
		target->AddTexture(texture);

		return 0;
	}

	unsigned char _CLEAR = 0;
	unsigned char _SAND  = -1;
	unsigned char _LASER  = 200;
	unsigned char _ROCK = 50;

	bool outOfBounds(ref<Texture> texture, size_t x, size_t y) {
		return x >= texture->Width() || y >= texture->Height();
	}

	size_t getCoords(ref<Texture> texture, size_t x, size_t y) {
		if (outOfBounds(texture, x, y)) {
			return 0;
		}

		return x * texture->Channels() + y * texture->Channels() * texture->Width();
	}

	bool isColor(ref<Texture> texture, size_t x, size_t y, unsigned char color) {
		return texture->Colors()[getCoords(texture, x, y)] == color;
	}

	bool isEmpty(ref<Texture> texture, size_t x, size_t y) {
		if (outOfBounds(texture, x, y)) {
			return false;
		}

		return isColor(texture, x, y, _CLEAR);
	}

	void setColor(ref<Texture> texture, size_t x, size_t y, unsigned char color) {
		if (outOfBounds(texture, x, y)) {
			return;
		}

		texture->Colors()[getCoords(texture, x, y)    ] = color;
		texture->Colors()[getCoords(texture, x, y) + 1] = -1; // full life
	}

	unsigned char getColor(ref<Texture> texture, size_t x, size_t y) {
		if (outOfBounds(texture, x, y)) {
			return _ROCK;
		}

		return texture->Colors()[getCoords(texture, x, y)];
	}

	unsigned char getLifetime(ref<Texture> texture, size_t x, size_t y) {
		if (outOfBounds(texture, x, y)) {
			return 0;
		}

		return texture->Colors()[getCoords(texture, x, y) + 1];
	}

	void setLifetime(ref<Texture> texture, size_t x, size_t y, unsigned char life) {
		if (outOfBounds(texture, x, y)) {
			return;
		}

		texture->Colors()[getCoords(texture, x, y) + 1] = life;
	}

	int imgx = -1;

	void SimpleSandLayer::PostUpdate() {
		vector2 pos = Mouse::ScreenPos() / scale;
		pos.y = texture->Height() - pos.y;

		LOG_INFO << 1 / iw::DeltaTime();

		//LOG_INFO << pos << (int)getColor(texture, pos.x, pos.y);
		
		//if (++imgx < texture->Width()) {
		//	setColor(texture, imgx, texture->Height() - 1, _LASER);
		//}

		if (Keyboard::KeyDown(iw::S)) {
			setColor(texture, pos.x, pos.y, _SAND);
			//setColor(texture, pos.x - 1, pos.y, _SAND);
			//setColor(texture, pos.x + 1, pos.y, _SAND);
			//setColor(texture, pos.x, pos.y + 1, _SAND);
			//setColor(texture, pos.x, pos.y + 2, _SAND);
			//setColor(texture, pos.x, pos.y + 3, _SAND);
			//setColor(texture, pos.x, pos.y + 4, _SAND);
			//setColor(texture, pos.x, pos.y + 5, _SAND);
		}

		else if (Keyboard::KeyDown(iw::Z)) {
			setColor(texture, pos.x, pos.y, _LASER);
		}

		else if (Keyboard::KeyDown(iw::R)) {
			setColor(texture, pos.x, pos.y, _ROCK);
		}

		for (size_t x = 0; x < texture->Width();  x++)
		for (size_t y = 0; y < texture->Height(); y++) {
			if (isColor(texture, x, y, _SAND)) {
				bool down = isEmpty(texture, x,     y - 1);
				bool left = isEmpty(texture, x - 1, y - 1);
				bool rght = isEmpty(texture, x + 1, y - 1);
			
				if (down || left || rght) {
					setColor(texture, x, y, 0);
				}

				if      (down) setColor(texture, x,     y - 1, _SAND);
				else if (left) setColor(texture, x - 1, y - 1, _SAND);
				else if (rght) setColor(texture, x + 1, y - 1, _SAND);
			}

			else if (isColor(texture, x, y, _LASER)) {
				bool down = isEmpty(texture, x, y - 1);
				unsigned char life = getLifetime(texture, x, y);

				if (down) {
					setColor(texture, x, y - 1, _LASER); // Expand laser
					setLifetime(texture, x, y - 1, life);
				}

				else if (getColor(texture, x, y - 1) != _LASER) {
					setLifetime(texture, x, y,  life/2);

					if (getColor(texture, x, y - 1) != _ROCK) {
						setColor(texture, x, y - 1, _CLEAR); // Destroy stuff hit by it
					}
				}

				if (life != 0) {
					setLifetime(texture, x, y, life * .6);
				}

				else {
					setColor(texture, x, y, _CLEAR);
				}
			}

			//if (x == pos.x && y == pos.y) {
			//	setColor(texture, x, y, _SAND);
			//}
		}

		texture->Update(Renderer->Device); // should be auto in apply filter

		Renderer->ApplyFilter(shader, target, nullptr);
	}
}

