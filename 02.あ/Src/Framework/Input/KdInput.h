#pragma once

class InputButton;
class InputAxis;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ボタンの入力状態構造体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
struct ButtonState
{
public:
	enum State
	{
		Free,
		Press,
		Hold = Press << 1,
		Release = Hold << 1
	};

	bool IsFree() const { return m_state == Free; }
	bool IsPress() const { return m_state & Press; }
	bool IsHold() const { return m_state & Hold; }
	bool IsRelease() const { return m_state & Release; }

	short m_state = 0;
};

static ButtonState s_invalidButtonState;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 単一の入力デバイスからの入力を受け付けるクラス
// キーボードやゲームパッド等
// ゲームで使うIndexの管理もここ
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class InputCollector
{
public:

	InputCollector() {}
	~InputCollector() {}

	enum class ButtonIndex
	{
		Action01,
		Action02,
		System01,
		System02
	};

	enum class AxisIndex
	{
		Move,
		Rotate
	};

	static InputCollector& GetInstance()
	{
		static InputCollector instance;
		return instance;
	}

	void Update();

	const Math::Vector2& GetAxis(AxisIndex index) const;

	const ButtonState& GetButtonState(ButtonIndex index) const;

	bool GetEnable() const { return m_isEnable; }
	void SetEnable(bool flag) { m_isEnable = flag; }

	void AddButton(ButtonIndex indx, std::shared_ptr<InputButton> button) { m_buttons[indx] = button; }
	void AddAxis(AxisIndex indx, std::shared_ptr<InputAxis> axis) { m_axes[indx] = axis; }

private:

	std::unordered_map<ButtonIndex, std::shared_ptr<InputButton>> m_buttons;

	std::unordered_map<AxisIndex, std::shared_ptr<InputAxis>> m_axes;

	bool m_isEnable = true;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 様々な入力を管理するクラス：複数のInputCollectorを管理
// キーボードやゲームパッド等
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class InputManager
{
public:

	static InputManager& Instance()
	{
		static InputManager instance;
		return instance;
	}

	enum class InputIndex
	{
		KeyBoard
	};

	void Init();

	void Update();

	Math::Vector2 GetAxis(InputCollector::AxisIndex index) const;

	ButtonState GetButtonState(InputCollector::ButtonIndex index) const;
	bool IsFree(InputCollector::ButtonIndex index) const;
	bool IsPress(InputCollector::ButtonIndex index) const;
	bool IsHold(InputCollector::ButtonIndex index) const;
	bool IsRelease(InputCollector::ButtonIndex index) const;

private:

	InputManager() {}
	~InputManager() {}

	std::unordered_map<InputIndex, std::shared_ptr<InputCollector>> m_inputs;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 単一のボタンの入力状態を受け付けるクラス
// ボタン単体に複数の入力キーを登録できる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

class InputButton
{
public:

	enum InputState
	{
		Free,
		Press,
		Hold = Press << 1,
		Release = Hold << 1
	};

	InputButton() {}
	virtual ~InputButton() {}

	virtual void Update() = 0;

	void NoInput() { m_inputState.m_state = InputState:: Free; }

	const ButtonState& GetState() const { return m_inputState; }

protected:
	ButtonState m_inputState;
};

// GetAsyncKeyStateを利用したキー制御
class InputButtonKey : public InputButton
{
public:
	InputButtonKey(std::initializer_list<int> keyCodeList)
	{
		for (int keyCode : keyCodeList) m_keyCodes.push_back(keyCode);
	}

	InputButtonKey(std::vector<int>& keyCodeList)
	{
		for (int keyCode : keyCodeList) m_keyCodes.push_back(keyCode);
	}

	void Update() override;

private:
	std::list<int>   m_keyCodes;
};


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 単一の軸（方向）入力状態を受け付けるクラス
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class InputAxis
{
public:
	virtual void Update() {};

	void NoInput() { m_axis = Math::Vector2::Zero; }

	const Math::Vector2& GetAxis() const { return m_axis; }

protected:
	Math::Vector2	m_axis;
};

// GetAsyncKeyStateの入力を利用した入力軸の制御
class InputAxisKey : public InputAxis
{
public:

	InputAxisKey(int upCode, int rightCode, int downCode, int leftCode)
	{
		m_dirKeys.push_back(upCode);
		m_dirKeys.push_back(rightCode);
		m_dirKeys.push_back(downCode);
		m_dirKeys.push_back(leftCode);
	}

	void Update() override;

private:

	enum DIR
	{
		Up, Right, Down, Left
	};

	std::vector<int> m_dirKeys;
};