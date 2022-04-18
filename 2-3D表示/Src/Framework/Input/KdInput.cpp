#include "KdInput.h"


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// InputManager
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void InputManager::Init()
{
	// キーボードの入力コレクターの初期化
	// ===== ===== ===== ===== ===== ===== ===== =====
	std::shared_ptr<InputCollector> keyboardCollector = std::make_shared<InputCollector>();

	std::vector<int> keyCodes;

	keyCodes = { 'Z', VK_LBUTTON };
	keyboardCollector->AddButton(InputCollector::ButtonIndex::Action01, std::make_shared<InputButtonKey>(keyCodes));

	keyCodes = { 'X', VK_RBUTTON };
	keyboardCollector->AddButton(InputCollector::ButtonIndex::Action02, std::make_shared<InputButtonKey>(keyCodes));

	std::shared_ptr<InputAxisKey> axis = std::make_shared<InputAxisKey>('W', 'D', 'S', 'A');

	keyboardCollector->AddAxis(InputCollector::AxisIndex::Move, axis);

	m_inputs[InputIndex::KeyBoard] = keyboardCollector;
}

void InputManager::Update()
{
	for (auto& input : m_inputs)
	{
		input.second->Update();
	}
}

Math::Vector2 InputManager::GetAxis(InputCollector::AxisIndex index) const
{
	Math::Vector2 axis = Math::Vector2::Zero;

	for (auto& collector : m_inputs)
	{
		if (collector.second->GetEnable())
		{
			axis += collector.second->GetAxis(index);
		}
	}

	axis.Normalize();

	return axis;
}

ButtonState InputManager::GetButtonState(InputCollector::ButtonIndex index) const
{
	ButtonState buttonState;

	for (auto& collector : m_inputs)
	{
		if (collector.second->GetEnable())
		{
			buttonState.m_state |= collector.second->GetButtonState(index).m_state;
		}
	}

	return buttonState;
}

bool InputManager::IsFree(InputCollector::ButtonIndex index) const
{
	return GetButtonState(index).m_state == InputButton::InputState::Free;
}

bool InputManager::IsPress(InputCollector::ButtonIndex index) const
{
	return GetButtonState(index).m_state & InputButton::InputState::Press;
}

bool InputManager::IsHold(InputCollector::ButtonIndex index) const
{
	return GetButtonState(index).m_state & InputButton::InputState::Hold;
}

bool InputManager::IsRelease(InputCollector::ButtonIndex index) const
{
	return GetButtonState(index).m_state & InputButton::InputState::Release;
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// InputCollector
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void InputCollector::Update()
{
	for (auto& axis : m_axes)
	{
		axis.second->Update();
	}

	for (auto& button: m_buttons)
	{
		button.second->Update();
	}
}

const Math::Vector2& InputCollector::GetAxis(AxisIndex index) const
{
	auto iter = m_axes.find(index);
	if (iter != m_axes.end()) { return iter->second->GetAxis(); }

	return  Math::Vector2::Zero;
}

const ButtonState& InputCollector::GetButtonState(ButtonIndex index) const
{
	auto iter = m_buttons.find(index);
	if (iter != m_buttons.end()) { return iter->second->GetState(); }

	return s_invalidButtonState;
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// InputButtonKey
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void InputButtonKey::Update()
{
	short keyState = 0;

	for (int keyCode : m_keyCodes)
	{
		keyState |= GetAsyncKeyState(keyCode);
	}

	if (keyState & 0x8000)
	{
		if (m_inputState.IsHold())
		{
			m_inputState.m_state &= ~InputState::Press;
		}
		else
		{
			m_inputState.m_state |= InputState::Press | InputState::Hold;
		}
	}
	else
	{
		if (m_inputState.IsHold())
		{
			m_inputState.m_state = InputState::Release;
		}
		else
		{
			m_inputState.m_state &= InputState::Free;
		}
	}
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// InputAxisKey
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void InputAxisKey::Update()
{
	m_axis = Math::Vector2::Zero;

	if (GetAsyncKeyState(m_dirKeys[Up]))	{ m_axis.y += 1.0f; }
	if (GetAsyncKeyState(m_dirKeys[Right]))	{ m_axis.x += 1.0f; }
	if (GetAsyncKeyState(m_dirKeys[Down]))	{ m_axis.y -= 1.0f; }
	if (GetAsyncKeyState(m_dirKeys[Left]))	{ m_axis.x -= 1.0f; }
}