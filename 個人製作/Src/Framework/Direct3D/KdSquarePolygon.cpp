#include "KdSquarePolygon.h" 

bool FileExistance(const std::string& filename)
{
	std::ifstream ifs(filename);

	return ifs.is_open();
}

KdSquarePolygon::KdSquarePolygon(
	const Math::Vector2* _pScale, const Math::Color* _pColor, const Math::Rectangle* _pRect)
{
	// 描画の幅・高さの初期化
	if (_pScale)
	{
		SetScale(*_pScale);
	}
	else
	{
		InitScale();
	}

	// 指定があればカラーの初期化
	if (_pColor)
	{
		SetColor(*_pColor);
	}

	// テクスチャ内の描画エリア
	if (_pRect)
	{
		SetUVRect(*_pRect);
	}
	else
	{
		InitRect();
	}
}

// 描画の幅と高さの設定
void KdSquarePolygon::SetScale( const Math::Vector2& _scale, PivotType _pivot )
{
	float halfX = _scale.x * 0.5f;
	float halfY = _scale.y * 0.5f;

	//float pivotX = 0.0f;
	//float pivotY = 0.0f;
	float pivotX = (static_cast<int>(_pivot) / 10 - 1) * halfX;
	float pivotY = (static_cast<int>(_pivot) % 10 - 1) * halfY;

	// 頂点座標
	m_vertex[0].pos = { -halfX + pivotX, -halfY + pivotY, 0.0f };	// 左上
	m_vertex[1].pos = { -halfX + pivotX,  halfY + pivotY, 0.0f };	// 左下
	m_vertex[2].pos = {  halfX + pivotX, -halfY + pivotY, 0.0f };	// 右上
	m_vertex[3].pos = {  halfX + pivotX,  halfY + pivotY, 0.0f };	// 右下
}

void KdSquarePolygon::SetPivot(PivotType pivot)
{
	Math::Vector2 scale;
	scale.x = m_vertex[2].pos.x - m_vertex[0].pos.x;
	scale.y = m_vertex[1].pos.y - m_vertex[0].pos.y;

	SetScale(scale, pivot);
}

// テクスチャの描画合成色の設定
void KdSquarePolygon::SetColor(const Math::Color& color)
{
	unsigned int col = 0;
	unsigned char r = static_cast<unsigned int>(color.R() * 255);
	unsigned char g = static_cast<unsigned int>(color.G() * 255);
	unsigned char b = static_cast<unsigned int>(color.B() * 255);
	unsigned char a = static_cast<unsigned int>(color.A() * 255);

	col = (a << 24) | (b << 16) | (g << 8) | r;

	//color.BGRA

	// 頂点カラーの設定
	for (auto& vertex : m_vertex)
	{
		vertex.color = col;
	}
}

// テクスチャ内の描画エリアの設定 分割フレームから
void KdSquarePolygon::SetUVRect(int FrameCnt)
{
	// マス座標
	int x = FrameCnt % m_splitX;
	int y = FrameCnt / m_splitX;

	float w = 1.0f / m_splitX;
	float h = 1.0f / m_splitY;

	Math::Vector2 uvMin, uvMax;

	uvMin.x = x * w;
	uvMin.y = y * h;

	uvMax.x = uvMin.x + w;
	uvMax.y = uvMin.y + h;

	SetUVRect(uvMin, uvMax);
}

// テクスチャ内の描画エリアの設定
void KdSquarePolygon::SetUVRect(const Math::Rectangle& _rect)
{
	if (!m_spMaterial || !m_spMaterial->BaseColorTex) { return; }

	Math::Vector2 uvMin, uvMax;
	ConvertRectToUV(m_spMaterial->BaseColorTex.get(), _rect, uvMin, uvMax);

	// UV座標
	SetUVRect(uvMin, uvMax);
}

void KdSquarePolygon::SetUVRect(const Math::Vector2& _uvMin, const Math::Vector2& _uvMax)
{
	// UV座標
	m_vertex[0].UV = { _uvMin.x, _uvMax.y };
	m_vertex[1].UV = { _uvMin.x, _uvMin.y };
	m_vertex[2].UV = { _uvMax.x, _uvMax.y };
	m_vertex[3].UV = { _uvMax.x, _uvMin.y };
}

void KdSquarePolygon::SetInfo(const Math::Vector2* pScale, const Math::Color* pColor, const Math::Rectangle* pRect)
{
	// 描画の幅・高さの初期化
	if (pScale)
	{
		SetScale(*pScale);
	}

	// 指定があればカラーの初期化
	if (pColor)
	{
		SetColor(*pColor);
	}

	// テクスチャ内の描画エリア
	if (pRect)
	{
		SetUVRect(*pRect);
	}
}

void KdSquarePolygon::SetMaterial(const std::string_view filename)
{
	if (!m_spMaterial) { m_spMaterial = std::make_shared<KdMaterial>(); }

	m_spMaterial->BaseColorTex = std::make_shared<KdTexture>();

	if (!m_spMaterial->BaseColorTex->Load(filename.data())) { return; }

	// 拡張子なしファイルパス
	std::string nonExtFilePass = filename.data();

	// 拡張子の場所
	size_t fileExtPos = nonExtFilePass.find_last_of('.');

	nonExtFilePass = nonExtFilePass.substr(0, fileExtPos);
	
	// 金属性マップ読込
	if (FileExistance(nonExtFilePass + "rm.png"))
	{
		m_spMaterial->NormalTex = std::make_shared<KdTexture>();

		m_spMaterial->NormalTex->Load(nonExtFilePass + "rm.png");
	}

	// 発光色マップ読込
	if (FileExistance(nonExtFilePass + "ems.png"))
	{
		m_spMaterial->NormalTex = std::make_shared<KdTexture>();

		m_spMaterial->NormalTex->Load(nonExtFilePass + "ems.png");
	}

	// 法線マップ読込
	if (FileExistance(nonExtFilePass + "nml.png"))
	{
		m_spMaterial->NormalTex = std::make_shared<KdTexture>();

		m_spMaterial->NormalTex->Load(nonExtFilePass + "nml.png");
	}
}

void KdSquarePolygon::InitScale()
{
	// 頂点座標
	m_vertex[0].pos = { -0.5f, -0.5f, 0 };
	m_vertex[1].pos = { -0.5f,  0.5f, 0 };
	m_vertex[2].pos = {  0.5f, -0.5f, 0 };
	m_vertex[3].pos = {  0.5f,  0.5f, 0 };
}

void KdSquarePolygon::InitRect()
{
	// UV座標
	m_vertex[0].UV = { 0, 1 };
	m_vertex[1].UV = { 0, 0 };
	m_vertex[2].UV = { 1, 1 };
	m_vertex[3].UV = { 1, 0 };
}

void KdSquarePolygon::Draw(const Math::Matrix& mWorld)
{
	// 法線の平行光ビルボード化
	m_vertex[0].normal = Math::Vector3::TransformNormal(-D3D.GetShaderManager().m_cb8_Light.Get().DirLight_Dir, mWorld.Invert());
	m_vertex[3].normal = m_vertex[2].normal = m_vertex[1].normal = m_vertex[0].normal;

	m_vertex[0].normal.Cross(Math::Vector3::Up, m_vertex[0].tangent);
	m_vertex[3].tangent = m_vertex[2].tangent = m_vertex[1].tangent = m_vertex[0].tangent;

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &m_vertex, sizeof(Vertex));
}