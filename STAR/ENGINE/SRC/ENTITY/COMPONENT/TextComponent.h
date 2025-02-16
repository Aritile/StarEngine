#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include <string>

struct TextComponent
{
public:
	void Render();
	void SetActive(bool _Active);
	bool IsActive();
	void RenderText();
	void Release();

private:
	bool activeComponent = true;
	std::string textData = "hello";
	//DirectX::SimpleMath::Vector2 position;
	//float rotation = 0.0f;
	//float scale = 1.0f;
	DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::SimpleMath::Vector2 origin;
	DirectX::SpriteBatch* spriteBatch = nullptr;
	DirectX::SpriteFont* spriteFont = nullptr;
	std::vector<std::pair<std::string, std::string>> items;
	int item_index = 0;
};