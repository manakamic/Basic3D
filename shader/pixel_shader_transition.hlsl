// ピクセルシェーダーの入力
struct PS_INPUT {
	float4 diff       : COLOR0;
	float2 texCoords0 : TEXCOORD0; // テクスチャ座標
};

// ピクセルシェーダーの出力
struct PS_OUTPUT {
	float4 color0 : SV_TARGET0;	// 出力カラー
};

// ===================================================================================================================
// DX ライブラリが用意している Constant Buffer
// ===================================================================================================================
struct DX_D3D11_CONST_MATERIAL {
	float4 Diffuse;          // ディフューズカラー
	float4 Specular;         // スペキュラカラー
	float4 Ambient_Emissive; // マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

	float Power;             // スペキュラの強さ
	float TypeParam0;        // マテリアルタイプパラメータ0
	float TypeParam1;        // マテリアルタイプパラメータ1
	float TypeParam2;        // マテリアルタイプパラメータ2
};

struct DX_D3D11_VS_CONST_FOG {
	float LinearAdd; // フォグ用パラメータ end / ( end - start )
	float LinearDiv; // フォグ用パラメータ -1  / ( end - start )
	float Density;   // フォグ用パラメータ density
	float E;         // フォグ用パラメータ 自然対数の低

	float4 Color;    // カラー
};

struct DX_D3D11_CONST_LIGHT {
	int    Type;        // ライトタイプ( DX_LIGHTTYPE_POINT など )
	int3   Padding1;    // パディング１

	float3 Position;    // 座標( ビュー空間 )
	float  RangePow2;   // 有効距離の２乗

	float3 Direction;   // 方向( ビュー空間 )
	float  FallOff;     // スポットライト用FallOff

	float3 Diffuse;     // ディフューズカラー
	float  SpotParam0;  // スポットライト用パラメータ０( cos( Phi / 2.0f ) )

	float3 Specular;    // スペキュラカラー
	float  SpotParam1;  // スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	float4 Ambient;     // アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの

	float Attenuation0; // 距離による減衰処理用パラメータ０
	float Attenuation1; // 距離による減衰処理用パラメータ１
	float Attenuation2; // 距離による減衰処理用パラメータ２
	float Padding2;     // パディング２
};

struct DX_D3D11_CONST_BUFFER_COMMON {
	DX_D3D11_CONST_LIGHT    Light[6]; // ライトパラメータ
	DX_D3D11_CONST_MATERIAL Material; // マテリアルパラメータ
	DX_D3D11_VS_CONST_FOG   Fog;      // フォグパラメータ
};

// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE {
	float4 FactorColor;        // アルファ値等

	float  MulAlphaColor;      // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float  AlphaTestRef;       // アルファテストで使用する比較値
	float2 Padding1;

	int    AlphaTestCmpMode;   // アルファテスト比較モード( DX_CMP_NEVER など )
	int3   Padding2;

	float4 IgnoreTextureColor; // テクスチャカラー無視処理用カラー
};

cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0) {
	DX_D3D11_CONST_BUFFER_COMMON g_Common;
};

cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1) {
	DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};
// ===================================================================================================================

SamplerState g_DiffuseMapSampler : register(s0);  // ディフューズマップサンプラ
Texture2D    g_DiffuseMapTexture : register(t0);  // ディフューズマップテクスチャ

PS_OUTPUT main(PS_INPUT PSInput) {
	float4 diffuse = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, PSInput.texCoords0);
	float alpha = step(PSInput.diff.r, diffuse.r);

    PS_OUTPUT PSOutput;

	PSOutput.color0.rgb = diffuse.rgb;
	PSOutput.color0.a = alpha * diffuse.a * PSInput.diff.a;

	return PSOutput;
}