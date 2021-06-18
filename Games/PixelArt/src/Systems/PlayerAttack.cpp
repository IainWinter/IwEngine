#include "PlayerAttack.h"

int PlayerAttackSystem::Initialize()
{
	using namespace glm;

	auto keepPlayerWithTarget = [&](
		iw::EntityHandle playerHandle)
	{
		iw::Rigidbody* body = Space->FindComponent<iw::Rigidbody>(playerHandle);
		Player* player = Space->FindComponent<Player>(playerHandle);

		if (!player->OnGround) {
			body->Velocity.y = iw::max(body->Velocity.y, 2.0f);
		}

		return false;
	};

	// Forward normal slash

	AttackProps nForwardSlash;
	nForwardSlash.func_deleteIf = keepPlayerWithTarget;
	nForwardSlash.Hitbox = iw::MakeSquareCollider();
	nForwardSlash.Knockback = vec2(1, 5);
	nForwardSlash.Transform = iw::Transform(vec2(3, 0), vec2(1.2, .5));

	AttackProps cForwardSlash = nForwardSlash;
	cForwardSlash.Knockback = vec2(10, 15);

	PlayerComboAttack forwardSlash;
	forwardSlash.NeededComboCount = 3;
	forwardSlash.Normal = nForwardSlash;
	forwardSlash.Combo  = cForwardSlash;

	// Upper normal slash

	AttackProps nUpperSlash;
	nForwardSlash.func_deleteIf = keepPlayerWithTarget;
	nUpperSlash.Hitbox = iw::MakeSquareCollider();
	nUpperSlash.Knockback = vec2(0, 10);
	nUpperSlash.Transform = iw::Transform(vec2(0, 2), vec2(2, .25));

	AttackProps cUpperSlash = nUpperSlash;
	nForwardSlash.func_deleteIf = keepPlayerWithTarget;
	cUpperSlash.Knockback = vec2(5, 20);

	PlayerComboAttack upperSlash;
	upperSlash.NeededComboCount = 2;
	upperSlash.Normal = nUpperSlash;
	upperSlash.Combo  = cUpperSlash;

	// Bounce into air

	AttackProps nBounceUp;
	nBounceUp.Hitbox = iw::MakeSquareCollider();
	nBounceUp.Knockback = vec2(2, 25);
	nBounceUp.Transform = iw::Transform(vec2(2, 0), vec2(.5, 1.5));

	PlayerComboAttack bounceUp;
	bounceUp.Normal = nBounceUp;

	m_attacks.emplace(AttackType::GROUND_LIGHT_FORWARD, forwardSlash);
	m_attacks.emplace(AttackType::   AIR_LIGHT_FORWARD, forwardSlash);

	m_attacks.emplace(AttackType::GROUND_LIGHT_UP, upperSlash);
	m_attacks.emplace(AttackType::   AIR_LIGHT_UP, upperSlash);
	
	m_attacks.emplace(AttackType::GROUND_LIGHT_DOWN, bounceUp);

	return 0;
}

void PlayerAttackSystem::Update()
{
	// Reset combo after some time

	Space->Query<iw::Timer, Player>().Each([](
		iw::EntityHandle entity,
		iw::Timer* timer,
		Player* player)
	{
		if (timer->Can("reset")) {
			player->PreviousAttacks.clear();
		}
	});
}

bool PlayerAttackSystem::On(
	iw::ActionEvent& e)
{
	if (e.Type == iw::val(myActionEvents::ATTACK))
	{
		event_Attack& e_attack = e.as<event_Attack>();
		Player* player = Space->FindComponent<Player>(e_attack.Entity);
		if (player)
		{
			auto itr = m_attacks.find(e_attack.Type);
			if (itr == m_attacks.end()) return true;

			iw::Timer* timer = Space->FindComponent<iw::Timer>(e_attack.Entity);
			if (timer) {
				timer->GetTimer("reset") = 0;
			}

			size_t currentTick = iw::Ticks();

			PlayerComboAttack& attack = itr->second;
			bool combo = IsCombo(player, e_attack.Type, attack.NeededComboCount);

			AttackProps props = combo ? attack.Combo : attack.Normal;
			props.func_deleteIf = [=](
				iw::EntityHandle e)
			{
				if (   player->PreviousAttacks.size() > 0
					&& player->PreviousAttacks.back().second == currentTick)
				{
					return false;
				}

				player->PreviousAttacks.emplace_back(e_attack.Type, currentTick); // this should only run hit
				
				return props.func_deleteIf 
					&& props.func_deleteIf(e);
			};

			e_attack.Props = props;
		}
	}

	return false;
}

bool PlayerAttackSystem::IsCombo(
	Player* player,
	AttackType attack,
	int neededCount)
{
	if (neededCount == -1) return false;

	int previousCount = 0;

	for (size_t i = 0; i < player->PreviousAttacks.size(); i++)
	{
		if (player->PreviousAttacks[i].first == attack) {
			previousCount++;
		}

		else {
			previousCount = 0;
		}

		if (previousCount == neededCount - 1) {
			return i == player->PreviousAttacks.size() - 1; // if chain is last
		}
	}

	return false;
}
