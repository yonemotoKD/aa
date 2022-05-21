#pragma once

struct KdFrameAnimationData
{
public:

	struct AnimData
	{
		AnimData(int start, int end, bool loop) :
			m_startFrame(start), 			
			// 最後のコマを表示するため+1
			// 終了フレームになった瞬間終了判定になってしまっては、最後のコマが描画される時間が無くなる
			m_endFrame(end + 1),
			m_isLoop(loop) {}

		int m_startFrame = 0;
		int m_endFrame = 0;

		bool m_isLoop = false;
	};

	void Load(std::string_view);

	void AddAnimation(const std::string_view animName, const AnimData& data);

	void AddAnimation(const std::string_view animName, int start, int end, bool isLoop);

	const std::shared_ptr<AnimData> GetAnimation(const std::string& name);

private:

	std::unordered_map<std::string, std::shared_ptr<AnimData>> m_animations;

	static const float s_endFrameTime;
};

class FrameAnimator
{
public:

	void SetAnimation(const std::shared_ptr<KdFrameAnimationData::AnimData>& animData, bool restart = true);

	// コマアニメーションを進行させる
	// ・speed		… 進行速度 1.0で１コマずつ
	void AdvanceTime(float speed);

	int GetFrame() { return static_cast<int>(m_nowAnimPos); }

	// アニメーションの再生が終わった？
	bool IsAnimationEnd() const;

private:

	float	m_nowAnimPos = 0;	// 現在のアニメーション位置

	std::shared_ptr<KdFrameAnimationData::AnimData> m_spNowAnimation = nullptr;
};