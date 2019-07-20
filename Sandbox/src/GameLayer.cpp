#include "GameLayer.h"

GameLayer::GameLayer() 
	: IwEngine::Layer("Game")
{}

GameLayer::~GameLayer() {
}

int GameLayer::Initialize() {
	IwEntity::Entity player = space.CreateEntity();
	space.CreateComponent<Transform>(player);
	return 0;
}

void GameLayer::Update() {
}
