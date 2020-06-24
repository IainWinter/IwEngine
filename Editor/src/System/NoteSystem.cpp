#include "Systems/NoteSystem.h"
#include "Events/ActionEvents.h"

#include "iw/engine/Time.h"
#include "iw/engine/Components/UiElement_temp.h"
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/input/Devices/Keyboard.h"

NoteSystem::NoteSystem()
	: iw::System<iw::Transform, Note>("Note")
{
}

int NoteSystem::Initialize() {
	m_font = Asset->Load<iw::Font>("fonts/arial.fnt");

	m_material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/font.shader"));

	m_material->Set("color", iw::vector3(1));
	m_material->SetTexture("fontMap", m_font->GetTexture(0));

	m_prefabs.push_back(Note { "A scroll with the words 'slow mo is bound to c.\nsave this for a hard level idiot' written on it.\nWhat could it be??",     1.5f, 0.0f });
	m_prefabs.push_back(Note { "A letter to the generals,\n\nThe king doesn't want to play our hand so soon, bring back\nyour men immediately.\n\n\ - Inr", 1.5f, 0.0f });
	m_prefabs.push_back(Note { "A scroll with the words 'charged kill is bound to c.\nsave ths for a hard level idiot' written on it.\nCould this be the answer???", 1.5f, 0.0f });

	return 0;
}

void NoteSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, note] = entity.Components.Tie<Components>();

		note->Timer += iw::Time::DeltaTime();

		if (note->Timer > note->Time) {
			if (   iw::Keyboard::KeyDown(iw::X)
				|| iw::Keyboard::KeyDown(iw::UP)
				|| iw::Keyboard::KeyDown(iw::DOWN)
				|| iw::Keyboard::KeyDown(iw::LEFT)
				|| iw::Keyboard::KeyDown(iw::RIGHT))
			{
				Space->FindComponent<iw::Transform>(entity.Handle)->SetParent(nullptr);
				QueueDestroyEntity(entity.Index);

				//Bus->push<iw::EntityDestroyEvent>(entity.Handle);
				Bus->push<GameStateEvent>(SOFT_RUN);
			}
		}
	}
}

bool NoteSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::SPAWN_NOTE)) {
		SpawnNoteEvent& event = e.as<SpawnNoteEvent>();
		iw::Transform* note = SpawnNote(m_prefabs.at(event.Index));

		Bus->push<GameStateEvent>(SOFT_PAUSE);
	}

	return false;
}

iw::Transform* NoteSystem::SpawnNote(
	Note prefab)
{
	iw::Mesh mesh = m_font->GenerateMesh(prefab.Text, 0.005f, 1.0f);
	mesh.SetMaterial(m_material);

	iw::Entity note = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement, Note>();

	iw::Transform* t = note.Set<iw::Transform>(iw::vector3(-4, 2, 0));
					   note.Set<iw::Mesh>(mesh);
	                   note.Set<Note>(prefab);

	return t;
}
