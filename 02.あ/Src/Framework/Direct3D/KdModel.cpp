#include "KdModel.h"
#include "KdGLTFLoader.h"

//コンストラクター
KdModelData::KdModelData()
{
}

//デストラクター
KdModelData::~KdModelData()
{
	Release();
}

//ロード関数
bool KdModelData::Load(const std::string& filename)
{
	Release();

	std::string fileDir = KdGetDirFromPath(filename);
	
	std::shared_ptr<KdGLTFModel> spGltfModel = KdLoadGLTFModel(filename);	// 分かりにくかったので名称変更
	if (spGltfModel == nullptr) { return false; }

	CreateNodes(spGltfModel);

	CreateMaterials(spGltfModel, fileDir);

	CreateAnimations(spGltfModel);

	return true;
}

// ノード作成
void KdModelData::CreateNodes(std::shared_ptr<KdGLTFModel>& spGltfModel)
{
	m_originalNodes.resize(spGltfModel->Nodes.size());

	for (UINT i = 0; i < spGltfModel->Nodes.size(); i++)
	{
		// 入力元ノード
		const KdGLTFNode& rSrcNode = spGltfModel->Nodes[i];

		// 出力先のノード参照
		Node& rDstNode = m_originalNodes[i];

		if (rSrcNode.IsMesh)
		{
			// メッシュ作成
			rDstNode.m_spMesh = std::make_shared<KdMesh>();

			if (rDstNode.m_spMesh)
			{
				rDstNode.m_spMesh->Create(rSrcNode.Mesh.Vertices, rSrcNode.Mesh.Faces, rSrcNode.Mesh.Subsets, rSrcNode.Mesh.IsSkinMesh);
			}

			// メッシュノードリストにインデックス登録
			m_meshNodeIndices.push_back(i);
		}

		// ノード情報セット
		rDstNode.m_name = rSrcNode.Name;

		rDstNode.m_localTransform = rSrcNode.LocalTransform;
		rDstNode.m_worldTransform = rSrcNode.WorldTransform;
		rDstNode.m_boneInverseWorldMatrix = rSrcNode.InverseBindMatrix;

		rDstNode.m_isSkinMesh = rSrcNode.Mesh.IsSkinMesh;

		rDstNode.m_boneIndex = rSrcNode.BoneNodeIndex;

		rDstNode.m_parent = rSrcNode.Parent;
		rDstNode.m_children = rSrcNode.Children;
	}

	for (UINT nodeIdx = 0; nodeIdx < spGltfModel->Nodes.size(); nodeIdx++)
	{
		// ルートノードのIndexリスト
		if (spGltfModel->Nodes[nodeIdx].Parent == -1) { m_rootNodeIndices.push_back(nodeIdx); }

		// ボーンノードのIndexリスト
		int boneIdx = spGltfModel->Nodes[nodeIdx].BoneNodeIndex;

		if (boneIdx >= 0)
		{
			if (boneIdx >= (int)m_boneNodeIndices.size()) { m_boneNodeIndices.resize(boneIdx + 1); }
			
			m_boneNodeIndices[boneIdx] = nodeIdx;
		}
	}
}

// マテリアル作成
void KdModelData::CreateMaterials(std::shared_ptr<KdGLTFModel>& spGltfModel, std::string& fileDir)
{
	//マテリアル配列を受け取れるサイズのメモリを確保
	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		//src = sourceの略
		//dst = destinationの略
		const KdGLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterial.Name;

		// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
		//基本色
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;

		// 基本色テクスチャ
		bool isLoadColorTex = false;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		if (!rSrcMaterial.BaseColorTexture.empty())
		{
			isLoadColorTex = rDstMaterial.BaseColorTex->Load(fileDir + rSrcMaterial.BaseColorTexture);
		}

		// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
		// 法線マップ
		bool isLoadNomalTex = false;
		rDstMaterial.NormalTex = std::make_shared<KdTexture>();

		if (!rSrcMaterial.NormalTexture.empty())
		{
			isLoadNomalTex = rDstMaterial.NormalTex->Load(fileDir + rSrcMaterial.NormalTexture);
		}

		// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
		// 金属性・粗さ
		rDstMaterial.Metallic = rSrcMaterial.Metallic;
		rDstMaterial.Roughness = rSrcMaterial.Roughness;

		// 金属性・粗さマップ
		bool isLoadRoughMetalTex = false;
		rDstMaterial.MetallicRoughnessTex = std::make_shared<KdTexture>();

		if (!rSrcMaterial.MetallicRoughnessTexture.empty())
		{
			isLoadRoughMetalTex = rDstMaterial.MetallicRoughnessTex->Load(fileDir + rSrcMaterial.MetallicRoughnessTexture);
		}

		// 自己発光・エミッシブ
		// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
		rDstMaterial.Emissive = rSrcMaterial.Emissive;

		// 自己発光・エミッシブ
		bool isLoadEmissiveTex = false;
		if (!rSrcMaterial.EmissiveTexture.empty())
		{
			rDstMaterial.EmissiveTex = std::make_shared<KdTexture>();

			if (!rDstMaterial.EmissiveTex->Load(fileDir + rSrcMaterial.EmissiveTexture))
			{
				rDstMaterial.EmissiveTex = nullptr;
			}
		}

		// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
		// BaseColorTextureが読み込めなかった場合は、代わりに白画像を使用
		if (!isLoadColorTex)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}

		// NormalTextureが読み込めなかった場合は、代わりにデフォルト法線画像を使用
		if (!isLoadNomalTex)
		{
			rDstMaterial.NormalTex = D3D.GetNormalTex();
		}

		// RoughMetalTextureが読み込めなかった場合は、代わりに白画像を使用
		if(!isLoadRoughMetalTex)
		{
			rDstMaterial.MetallicRoughnessTex = D3D.GetWhiteTex();
		}
	}
}

// アニメーション作成
void KdModelData::CreateAnimations(std::shared_ptr<KdGLTFModel>& spGltfModel)
{
	// アニメーションデータ
	m_spAnimations.resize(spGltfModel->Animations.size());

	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		const KdGLTFAnimationData& rSrcAnimation = *spGltfModel->Animations[i];

		m_spAnimations[i] = std::make_shared<KdAnimationData>();
		KdAnimationData& rDstAnimation = *(m_spAnimations[i]);

		rDstAnimation.m_name = rSrcAnimation.m_name;

		rDstAnimation.m_maxLength = rSrcAnimation.m_maxLength;

		rDstAnimation.m_nodes.resize(rSrcAnimation.m_nodes.size());

		for (UINT j = 0; j < rDstAnimation.m_nodes.size(); ++j)
		{
			rDstAnimation.m_nodes[j].m_nodeOffset = rSrcAnimation.m_nodes[j]->m_nodeOffset;
			rDstAnimation.m_nodes[j].m_translations = rSrcAnimation.m_nodes[j]->m_translations;
			rDstAnimation.m_nodes[j].m_rotations = rSrcAnimation.m_nodes[j]->m_rotations;
			rDstAnimation.m_nodes[j].m_scales = rSrcAnimation.m_nodes[j]->m_scales;
		}
	}
}

// アニメーションデータ取得：文字列検索
const std::shared_ptr<KdAnimationData> KdModelData::GetAnimation(const std::string& animName) const
{
	for (auto&& anim : m_spAnimations)
	{
		if (anim->m_name == animName)
		{
			return anim;
		}
	}

	return nullptr;
}

// アニメーションデータ取得：番号指定
const std::shared_ptr<KdAnimationData> KdModelData::GetAnimation(UINT index) const
{
	return index >= m_spAnimations.size() ? nullptr : m_spAnimations[index];
}

// 解放
void KdModelData::Release()
{
	m_materials.clear();
	m_originalNodes.clear();

	m_rootNodeIndices.clear();
	m_boneNodeIndices.clear();
	m_meshNodeIndices.clear();
}

bool KdModelData::IsSkinMesh()
{
	for (auto& node : m_originalNodes)
	{
		if (node.m_isSkinMesh) { return true; }
	}

	return false;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 
// KdModelWork
// 
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

bool KdModelWork::Load(const std::string& fileName)
{
	// モデルの読み込み
	std::shared_ptr<KdModelData> data = std::make_shared<KdModelData>();

	if (!data->Load(fileName)) { return false; }

	// モデルのセット
	SetModel(data);

	return true;
}

const KdModelData::Node* KdModelWork::FindDataNode(std::string& name) const
{
	if (m_spData == nullptr) { return nullptr; }

	return m_spData->FindNode(name);
}

// ノード検索：文字列
const KdModelWork::Node* KdModelWork::FindNode(std::string name) const
{
	for (auto&& node : m_coppiedNodes)
	{
		if (node.m_name == name)
		{
			return &node;
		}
	}

	return nullptr;
}

// 可変ノード検索：文字列
KdModelWork::Node* KdModelWork::FindWorkNode(std::string name)
{
	for (auto&& node : m_coppiedNodes)
	{
		if (node.m_name == name)
		{
			return &node;
		}
	}

	return nullptr;
}

// モデル設定：コピーノードの生成
void KdModelWork::SetModel(const std::shared_ptr<KdModelData>& rModel)
{ 
	m_spData = rModel;

	UINT nodeSize = rModel->GetOriginalNodes().size();

	m_coppiedNodes.resize(nodeSize);

	// ノードのコピーを生成
	for (UINT i = 0; i < nodeSize; ++i)
	{
		m_coppiedNodes[i].copy(rModel->GetOriginalNodes()[i]);
	}

	// メッシュを持つノードに関する振り分け処理
	for (int index : rModel->GetMeshNodeIndices())
	{
		// 当たり判定用ノード検索
		if (rModel->GetOriginalNodes()[index].m_name.find("COL") == std::string::npos)
		{
			m_drawMeshNodeIndices.push_back(index);
		}
		else
		{
			m_collisionMeshNodeIndices.push_back(index);
		}
	}

	if(!m_collisionMeshNodeIndices.size())
	{
		m_collisionMeshNodeIndices = rModel->GetMeshNodeIndices();
	}
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ルートノードから各ノードの行列を計算していく
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void KdModelWork::CalcNodeMatrices()
{
	if (!m_spData) { assert(0 && "モデルのないノード行列計算"); return; }

	// 全ボーン行列を書き込み
	for (auto&& nodeIdx : m_spData->GetRootNodeIndices())
	{
		recCalcNodeMatrices(nodeIdx);
	}
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ノード行列計算用の再起用関数
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
void KdModelWork::recCalcNodeMatrices(int nodeIdx, int parentNodeIdx)
{
	auto& data = m_spData->GetOriginalNodes()[nodeIdx];
	auto& work = m_coppiedNodes[nodeIdx];

	// 親との行列を合成
	if (parentNodeIdx >= 0)
	{
		auto& parent = m_coppiedNodes[data.m_parent];

		work.m_worldTransform = work.m_localTransform * parent.m_worldTransform;
	}
	// 親が居ない場合は親は自分自身とする
	else
	{
		work.m_worldTransform = work.m_localTransform;
	}

	for (auto childNodeIdx : data.m_children)
	{
		recCalcNodeMatrices(childNodeIdx, nodeIdx);
	}
}