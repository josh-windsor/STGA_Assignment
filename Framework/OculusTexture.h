
//------------------------------------------------------------
// ovrSwapTextureSet wrapper class that also maintains the render target views
// needed for D3D11 rendering.
//Modified by Josh Windsor to support deferred rendering
#include "OVR_CAPI_D3D.h"
#include <vector>
#include "CommonHeader.h"
#include <sstream>
#ifndef VALIDATE
#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(nullptr, (msg), "JW_STGA", MB_ICONERROR | MB_OK); exit(-1); }
#endif

struct OculusTexture
{
	ovrSession               Session;
	ovrTextureSwapChain      TextureChain;
	ovrTextureSwapChain      DepthTextureChain;
	std::vector<ID3D11RenderTargetView*> TexRtv;
	std::vector<ID3D11DepthStencilView*> TexDsv;
	std::vector<ID3D11ShaderResourceView*> TexDtv;

	OculusTexture() :
		Session(nullptr),
		TextureChain(nullptr),
		DepthTextureChain(nullptr)
	{
	}

	bool Init(ovrSession session, int sizeW, int sizeH, bool createDepth, ID3D11Device* pD3DDevice)
	{
		Session = session;
		int sampleCount = 1;

		// create color texture swap chain first
		{
			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Width = sizeW;
			desc.Height = sizeH;
			desc.MipLevels = 1;
			desc.SampleCount = sampleCount;
			desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.MiscFlags = ovrTextureMisc_DX_Typeless | ovrTextureMisc_AutoGenerateMips;
			desc.BindFlags = ovrTextureBind_DX_RenderTarget;
			desc.StaticImage = ovrFalse;

			ovrResult result = ovr_CreateTextureSwapChainDX(session, pD3DDevice, &desc, &TextureChain);
			if (!OVR_SUCCESS(result))
				panicF("%i", result);


			int textureCount = 0;
			ovr_GetTextureSwapChainLength(Session, TextureChain, &textureCount);
			for (int i = 0; i < textureCount; ++i)
			{
				ID3D11Texture2D* tex = nullptr;
				ovr_GetTextureSwapChainBufferDX(Session, TextureChain, i, IID_PPV_ARGS(&tex));

				D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
				rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				rtvd.ViewDimension = (sampleCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS
					: D3D11_RTV_DIMENSION_TEXTURE2D;
				ID3D11RenderTargetView* rtv;
				HRESULT hr = pD3DDevice->CreateRenderTargetView(tex, &rtvd, &rtv);
				VALIDATE((hr == ERROR_SUCCESS), "Error creating render target view");
				TexRtv.push_back(rtv);
				tex->Release();
			}
		}

		// if requested, then create depth swap chain
		if (createDepth)
		{
			ovrTextureSwapChainDesc desc = {};
			desc.Width = sizeW;
			desc.Height = sizeH;
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.MipLevels = 1;
			desc.SampleCount = sampleCount;
			desc.Format = OVR_FORMAT_D24_UNORM_S8_UINT;
			desc.MiscFlags = ovrTextureMisc_DX_Typeless;
			desc.BindFlags = ovrTextureBind_DX_DepthStencil;
			desc.StaticImage = ovrFalse;

			ovrResult result = ovr_CreateTextureSwapChainDX(session, pD3DDevice, &desc, &DepthTextureChain);
			if (!OVR_SUCCESS(result))
				return false;

			int textureCount = 0;

			ovr_GetTextureSwapChainLength(Session, DepthTextureChain, &textureCount);
			for (int i = 0; i < textureCount; ++i)
			{
				ID3D11Texture2D* tex = nullptr;
				ovr_GetTextureSwapChainBufferDX(Session, DepthTextureChain, i, IID_PPV_ARGS(&tex));

				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.ViewDimension = (sampleCount > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS
					: D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;

				ID3D11DepthStencilView* dsv;
				HRESULT hr = pD3DDevice->CreateDepthStencilView(tex, &dsvDesc, &dsv);
				VALIDATE((hr == ERROR_SUCCESS), "Error creating depth stencil view");
				TexDsv.push_back(dsv);

				ovr_GetTextureSwapChainBufferDX(Session, DepthTextureChain, i, IID_PPV_ARGS(&tex));


				//Doesnt work right now 
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // View suitable for decoding full 24bits of depth to red channel.
				srvDesc.ViewDimension = (sampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS
					: D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;

				ID3D11ShaderResourceView* dtv;
				hr = pD3DDevice->CreateShaderResourceView(tex, &srvDesc, &dtv);
				VALIDATE((hr == ERROR_SUCCESS), "Failure in Oculus SRV");

				TexDtv.push_back(dtv);



				tex->Release();

			}
		}

		return true;
	}*/

	bool Init(ovrSession session, int sizeW, int sizeH, ID3D11Device* pD3DDevice)
	{
		Session = session;

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Width = sizeW;
		desc.Height = sizeH;
		desc.MipLevels = 1;
		desc.SampleCount = 1;
		desc.MiscFlags = ovrTextureMisc_DX_Typeless;
		desc.StaticImage = ovrFalse;
		desc.BindFlags = ovrTextureBind_DX_RenderTarget;

		ovrResult result = ovr_CreateTextureSwapChainDX(session, pD3DDevice, &desc, &TextureChain);
		if (!OVR_SUCCESS(result))
			return false;

		int textureCount = 0;
		ovr_GetTextureSwapChainLength(Session, TextureChain, &textureCount);
		VALIDATE(textureCount == TextureCount, "TextureCount mismatch.");

		for (int i = 0; i < TextureCount; ++i)
		{
			ID3D11Texture2D* tex = nullptr;
			ovr_GetTextureSwapChainBufferDX(Session, TextureChain, i, IID_PPV_ARGS(&tex));
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			pD3DDevice->CreateRenderTargetView(tex, &rtvd, &TexRtv[i]);
			tex->Release();
		}

		return true;
	}


	~OculusTexture()
	{
		for (int i = 0; i < TextureCount; ++i)
		{
			TexRtv[i]->Release();
		}

		/*for (int i = 0; i < (int)TexRtv.size(); ++i)
		{
			TexRtv[i]->Release();
		}*/
		/*for (int i = 0; i < (int)TexDsv.size(); ++i)
		{
			TexDsv[i]->Release();
		}*/
		if (TextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, TextureChain);
		}
		if (DepthTextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, DepthTextureChain);
		}
	}

	ID3D11RenderTargetView* GetRTV()
	{
		int index = 0;
		ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &index);
		return TexRtv[index];
	}
	ID3D11DepthStencilView* GetDSV()
	{
		int index = 0;
		ovr_GetTextureSwapChainCurrentIndex(Session, DepthTextureChain, &index);
		return TexDsv[index];
	}
	ID3D11ShaderResourceView* GetDTV()
	{
		int index = 0;
		ovr_GetTextureSwapChainCurrentIndex(Session, DepthTextureChain, &index);
		return TexDtv[index];
	}

	// Commit changes
	void Commit()
	{
		ovr_CommitTextureSwapChain(Session, TextureChain);
	}
};

// ========================================================