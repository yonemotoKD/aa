﻿#include "KdTrailPolygon.h"

void KdTrailPolygon::SetInfo(const std::shared_ptr<KdTexture>& tex, Trail_Pattern pattern)
{
	SetTexture(tex);

	SetPattern(pattern);
}

void KdTrailPolygon::Draw() const
{
	std::vector<Vertex> vertices;

	CreateVertices(vertices);

	// 頂点数が3より少なければポリゴンが形成できないので描画不能
	if (vertices.size() < 3) { return; }

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertices.size(), &vertices[0], sizeof(Vertex));
}

void KdTrailPolygon::CreateVertices(std::vector<Vertex>& vertex) const
{
	switch (m_pattern)
	{
		case Trail_Pattern::eDefault:		CreateWithDefaultPattern(vertex);	break;
		case Trail_Pattern::eBillboard:		CreateWithBillboardPattern(vertex); break;
		case Trail_Pattern::eVertices:		CreateWithVerticesPattern(vertex);	break;
	}
}

void KdTrailPolygon::CreateWithDefaultPattern(std::vector<Vertex>& vertices) const
{
	// ポイントが２つ以下の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	// 軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// ポイント数分確保
	vertices.resize(m_pointList.size() * 2);

	//--------------------------
	// 頂点データ作成
	//--------------------------
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		const Math::Matrix& mat = m_pointList[i];

		// ２つの頂点の参照(ショートカット)
		Vertex& v1 = vertices[i * 2];
		Vertex& v2 = vertices[i * 2 + 1];

		// X方向
		Math::Vector3 axisX = mat.Right();
		float width = axisX.Length();

		axisX.Normalize();


		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		float uvY = std::clamp(i / sliceCount, 0.0f, 0.99f);

		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };
	}
}

void KdTrailPolygon::CreateWithBillboardPattern(std::vector<Vertex>& vertices) const
{
	// ポイントが２つ以下の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	// カメラの情報
	Math::Matrix mCam = D3D.WorkShaderManager().m_cb7_Camera.Get().mView.Invert();

	// 軌跡画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// ポイント数分確保
	vertices.resize(m_pointList.size() * 2);

	//--------------------------
	// 頂点データ作成
	//--------------------------
	Math::Vector3 prevPos;
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		const Math::Matrix& mat = m_pointList[i];

		// ２つの頂点の参照(ショートカット)
		Vertex& v1 = vertices[i * 2];
		Vertex& v2 = vertices[i * 2 + 1];

		// ラインの向き
		Math::Vector3 vDir;
		if (i == 0)
		{
			// 初回時のみ、次のポイントを使用
			vDir = m_pointList[1].Translation() - mat.Translation();
		}
		else
		{
			// 二回目以降は、前回の座標から向きを決定する
			vDir = mat.Translation() - prevPos;
		}

		// カメラからポイントへの向き
		Math::Vector3 v = mat.Translation() - mCam.Translation();
		Math::Vector3 axisX = DirectX::XMVector3Cross(vDir, v);
		float width = axisX.Length();

		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		float uvY = std::clamp(i / sliceCount, 0.0f, 0.99f);

		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };

		// 座標を記憶しておく
		prevPos = mat.Translation();
	}
}

// 頂点情報をそのまま繋げてポリゴンを描画
void KdTrailPolygon::CreateWithVerticesPattern(std::vector<Vertex>& vertices) const
{
	UINT pointListSize = m_pointList.size();
	if (pointListSize < 4) { return; }

	// 頂点配列
	vertices.resize(pointListSize);

	// 軌跡画像の分割数
	float sliceNum = pointListSize * 0.5f;

	// 頂点データ作成
	for (UINT i = 0; i < pointListSize; i++)
	{
		Vertex& rVertex = vertices[i];

		// 頂点座標
		rVertex.Pos = m_pointList[i].Translation();

		// UV
		rVertex.UV.x = (float)(i % 2);
		rVertex.UV.y = std::clamp((i * 0.5f) / sliceNum, 0.0f, 0.99f);
	}
}