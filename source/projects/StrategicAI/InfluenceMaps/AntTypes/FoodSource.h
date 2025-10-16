#pragma once

class FoodSource
{
public:
	FoodSource(Elite::Vector2 pos, int amount);
	virtual ~FoodSource();

	void Render(float dt) const;
	
	int GetAmount() const { return m_amount; }
	void TakeFood() { --m_amount; }
	Elite::Vector2 GetPosition() const { return m_position; }

private:

	Elite::Vector2 m_position;
	int m_amount;
};