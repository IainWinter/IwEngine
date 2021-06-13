#include "PlayerAttack.h"

int PlayerAttackSystem::Initialize()
{
	PlayerComboAttack forwardSlash;

	forwardSlash.Normal.Hitbox = iw::MakeSquareCollider();
	forwardSlash.Normal.Knockback = glm::vec2(2, .5);
	
	forwardSlash.Combo = forwardSlash.Normal;
	forwardSlash.Combo.Knockback *= 2;

	m_attacks.emplace(AttackType::GROUND_LIGHT_FORWARD, forwardSlash);
}

void PlayerAttackSystem::Update()
{
	// Reset combo after some time

	Space->Query<iw::Transform, PlayerAttackState>().Each([](
		iw::EntityHandle entity,
		iw::Transform* transform,
		PlayerAttackState* props)
	{
		if (props->PreviousAttackTime > props->PreviousAttackDelay) {
			props->PreviousAttacks.clear();
		}
	});
}

bool PlayerAttackSystem::On(
	iw::ActionEvent& e)
{
	if (e.Type == iw::val(myActionEvents::ATTACK))
	{
		event_Attack& e_attack = e.as<event_Attack>();
		PlayerAttackState* state = Space->FindComponent<PlayerAttackState>(e_attack.Entity);
		if (state) 
		{
			PlayerComboAttack attack = m_attacks.at(e_attack.Type);
			bool combo = IsCombo(state, e_attack.Type, attack .NeededComboCount);
			
			e_attack.Props = combo ? attack.Combo : attack.Normal;
		}
	}

	return false;
}

bool PlayerAttackSystem::IsCombo(
	PlayerAttackState* player,
	AttackType attack,
	int neededCount)
{
	int previousCount = 0;

	for (size_t i = 0; i < player->PreviousAttacks.size(); i++)
	{
		if (player->PreviousAttacks[i] == attack) {
			previousCount++;
		}

		else {
			previousCount = 0;
		}

		if (previousCount == neededCount) {
			return i == player->PreviousAttacks.size() - 1; // if chain is last
		}
	}
}
