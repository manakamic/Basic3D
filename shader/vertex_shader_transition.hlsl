// 頂点シェーダーの入力
// モデルの頂点情報を合わないと不正な値が入るので注意
struct VS_INPUT {
	float3 pos        : POSITION;  // 座標(ローカル空間)
	float3 norm       : NORMAL0;   // 法線(ローカル空間)
	float4 diff       : COLOR0;    // ディフューズカラー
	float4 spe        : COLOR1;    // スペキュラカラー
	float2 texCoords0 : TEXCOORD0; // テクスチャ座標
};

// 頂点シェーダーの出力
struct VS_OUTPUT {
	float4 diff       : COLOR0;
	float2 texCoords0 : TEXCOORD0;   // テクスチャ座標
	float4 pos        : SV_POSITION;
};

// ===================================================================================================================
// DX ライブラリが用意している Constant Buffer
// ===================================================================================================================
// 基本パラメータ
struct DX_D3D11_VS_CONST_BUFFER_BASE {
	float4 AntiViewportMatrix[4]; // アンチビューポート行列
	float4 ProjectionMatrix[4];   // ビュー　→　プロジェクション行列
	float4 ViewMatrix[3];         // ワールド　→　ビュー行列
	float4 LocalWorldMatrix[3];   // ローカル　→　ワールド行列
	float4 ToonOutLineSize;       // トゥーンの輪郭線の大きさ
	float DiffuseSource;          // ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float SpecularSource;         // スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float MulSpecularColor;       // スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float Padding;
};

// その他の行列
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX {
	float4 ShadowMapLightViewProjectionMatrix[3][4]; // シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
	float4 TextureMatrix[3][2];                      // テクスチャ座標操作用行列
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1) {
	DX_D3D11_VS_CONST_BUFFER_BASE g_Base;
};

// その他の行列
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX : register(b2) {
	DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX g_OtherMatrix;
};

cbuffer cbADDPOSITION_CBUFFER : register(b4) {
	float4 g_FadeColor;
};
// ===================================================================================================================

// main関数
VS_OUTPUT main(VS_INPUT VSInput) {
	float4 localPosition;

	localPosition.xyz = VSInput.pos.xyz; 
	localPosition.w = 1.0f;

	float4 worldPosition;

	worldPosition.x = dot(localPosition, g_Base.LocalWorldMatrix[0]);
	worldPosition.y = dot(localPosition, g_Base.LocalWorldMatrix[1]);
	worldPosition.z = dot(localPosition, g_Base.LocalWorldMatrix[2]);
	worldPosition.w = 1.0f;

	float4 viewPosition;

	viewPosition.x = dot(worldPosition, g_Base.ViewMatrix[0]);
	viewPosition.y = dot(worldPosition, g_Base.ViewMatrix[1]);
	viewPosition.z = dot(worldPosition, g_Base.ViewMatrix[2]);
	viewPosition.w = 1.0f;

	VS_OUTPUT VSOutput;

	VSOutput.pos.x = dot(viewPosition, g_Base.ProjectionMatrix[0]);
	VSOutput.pos.y = dot(viewPosition, g_Base.ProjectionMatrix[1]);
	VSOutput.pos.z = dot(viewPosition, g_Base.ProjectionMatrix[2]);
	VSOutput.pos.w = dot(viewPosition, g_Base.ProjectionMatrix[3]);

    // テクスチャ座標変換行列による変換を行った結果のテクスチャ座標をセット
	VSOutput.texCoords0.x = dot(VSInput.texCoords0, g_OtherMatrix.TextureMatrix[0][0]);
	VSOutput.texCoords0.y = dot(VSInput.texCoords0, g_OtherMatrix.TextureMatrix[0][1]);

	VSOutput.diff = g_FadeColor;

    return VSOutput;
}