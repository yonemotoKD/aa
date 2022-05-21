#include "KdFrameAnimation.h"

void KdFrameAnimationData::Load(std::string_view fileName)
{
	KdCSVData data;

	if (data.Load(std::string("Asset/Data/") + fileName.data()))
	{
		for (int i = 0; ; ++i)
		{
			const std::vector<std::string>& anim = data.GetLineData(i);

			if (!anim.size()) { break; }

			AddAnimation(anim[0], atoi(anim[1].c_str()), atoi(anim[2].c_str()), atoi(anim[3].c_str()));
		}
	}
}

void KdFrameAnimationData::AddAnimation(const std::string_view animName, const AnimData& data)
{
	m_animations[animName.data()] = std::make_shared<AnimData>(data);
}

void KdFrameAnimationData::AddAnimation(const std::string_view animName, int start, int end, bool isLoop)
{
	m_animations[animName.data()] = std::make_shared<AnimData>(start, end, isLoop);
}

const std::shared_ptr<KdFrameAnimationData::AnimData> KdFrameAnimationData::GetAnimation(const std::string& name)
{
	auto& dataItr = m_animations.find(name);

	if ( dataItr == m_animations.end() ) { return nullptr; }

	return dataItr->second;
}

void FrameAnimator::SetAnimation(const std::shared_ptr<KdFrameAnimationData::AnimData>& animData, bool restart)
{
	if (!m_spNowAnimation)
	{
		m_spNowAnimation = animData;
	}

	// アニメーションの進捗を初期化しない
	if (!restart)
	{
		float nowProgress = m_nowAnimPos - m_spNowAnimation->m_startFrame;

		m_nowAnimPos = std::min(animData->m_startFrame + nowProgress, static_cast<float>(animData->m_endFrame));
	}
	else
	{
		m_nowAnimPos = static_cast<float>(animData->m_startFrame);
	}

	m_spNowAnimation = animData;
}

void FrameAnimator::AdvanceTime(float speed)
{
	if (!m_spNowAnimation) { return; }

	// アニメーション位置を進める
	m_nowAnimPos += speed;

	// 終了判定
	if (IsAnimationEnd())
	{
		if (m_spNowAnimation->m_isLoop)
		{
			m_nowAnimPos = static_cast<float>(m_spNowAnimation->m_startFrame);
		}
		else
		{
			// 最後のコマにする
			m_nowAnimPos = static_cast<float>(m_spNowAnimation->m_endFrame) - 0.001f;
		}
	}
}

bool FrameAnimator::IsAnimationEnd() const
{
	if (!m_spNowAnimation) { return true; }

	// 終了判定
	if (m_nowAnimPos >= m_spNowAnimation->m_endFrame - 0.001f) { return true; }

	return false;
}