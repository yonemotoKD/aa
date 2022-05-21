#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

// テクスチャ
Texture2D g_tex : register(t0);

Texture2D g_dissolveTex : register(t11); // ディゾルブマップ

// サンプラ
SamplerState g_ss : register(s0);

float4 main(VSOutputNoLighting In) : SV_Target0
{
	// ディゾルブによる描画スキップ
	float discardValue = g_dissolveTex.Sample(g_ss, In.UV).r;
	if (discardValue < g_dissolveValue)
	{
		discard;
	}

	float4 color = g_tex.Sample(g_ss, In.UV) * In.Color * g_BaseColor;
	
	// Alphaテスト
	if (color.a < 0.05f)
	{
		discard;
	}
	
	// 全体の明度：環境光に1が設定されている場合は影響なし
	// 環境光の不透明度を下げる事により、明度ライトの周り以外は描画されなくなる
	float totalBrightness = g_AmbientLight.a;

	//-------------------------
	// 点光
	//-------------------------
	for (int i = 0; i < g_PointLightNum.x; i++)
	{
		// ピクセルから点光への方向
		float3 dir = g_PointLights[i].Pos - In.wPos;
		
		// 距離を算出
		float dist = length(dir);
		
		// 正規化
		dir /= dist;
		
		// 点光の判定以内
		if (dist < g_PointLights[i].Radius)
		{
			// 半径をもとに、距離の比率を求める
			float atte = 1.0 - saturate(dist / g_PointLights[i].Radius);
			
			// 明度の追加
			totalBrightness += (1 - pow(1 - atte, 2)) * g_PointLights[i].IsBright;
		}
	}
	
	totalBrightness = saturate(totalBrightness);
	color.rgb *= totalBrightness;
	
	return color;
}