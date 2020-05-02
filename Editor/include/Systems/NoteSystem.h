#pragma once

#include "Components/Note.h"

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"


#include <vector>

class NoteSystem
	: public iw::System<iw::Transform, Note>
{
public:
	struct Components {
		iw::Transform* Transform;
		Note* Item;
	};
private:
	iw::Transform* m_root;

	iw::ref<iw::Font>     m_font;
	iw::ref<iw::Material> m_material;

	std::vector<Note> m_prefabs;

public:
	NoteSystem();

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnNote(
		Note prefab);
};
