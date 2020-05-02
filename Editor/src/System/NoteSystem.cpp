#include "Systems/NoteSystem.h"
#include "Events/ActionEvents.h"

#include "iw/engine/Time.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/engine/Components/UiElement_temp.h"

#include "iw/input/Devices/Keyboard.h"

NoteSystem::NoteSystem()
	: iw::System<iw::Transform, Note>("Note")
{
}

int NoteSystem::Initialize() {
	m_root = new iw::Transform();

	m_font = Asset->Load<iw::Font>("fonts/arial.fnt");

	m_material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/font.shader"));

	m_material->Set("color", iw::vector3(1));
	m_material->SetTexture("fontMap", m_font->GetTexture(0));

	m_prefabs.push_back(Note { "The greatest lore of all time. People will be\nmaking 4 hours long videos decrypting\nthe secrets that these notes\nare bouta hold. You've never een anything \ncleaner than this", 2.0f, 0.0f });

	return 0;
}

void NoteSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, note] = entity.Components.Tie<Components>();

		note->Timer += iw::Time::DeltaTime();

		if (note->Timer > note->Time) {
			if (iw::Keyboard::KeyDown(iw::X)) {
				QueueDestroyEntity(entity.Index);
				Bus->push<GameStateEvent>(RUNNING);
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
		note->SetParent(m_root);

		Bus->push<GameStateEvent>(PAUSED);
	}

	return false;
}

iw::Transform* NoteSystem::SpawnNote(
	Note prefab)
{
	iw::Mesh mesh = m_font->GenerateMesh(prefab.Text, 0.005f, 1.0f);
	mesh.SetMaterial(m_material);

	iw::Entity note = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement, Note>();

	iw::Transform* t = note.Set<iw::Transform>(iw::vector3(-5, 3, 0));
					   note.Set<iw::Mesh>(mesh);
	                   note.Set<Note>(prefab);

	return t;
}
