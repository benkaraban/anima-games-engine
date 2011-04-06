/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>

#include <Gfx/GfxType.h>
#include <Gfx/GfxDx9/DriverDx9.h>

#include <Core/Logger.h>
#include <Core/List.h>
#include <Core/DateAndTime.h>
#include <Core/StopWatch.h>
#include <Core/IO/BufferStream.h>
#include <Core/IO/FileStream.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>
#include <Core/RT/Thread.h>

#include <Images/Image.h>

#include <Window/Window.h>

#include <winsafe.h>    //TODO temporaire


const char VS_SOURCE [] = ""
"bool     TestBool : register(b0);\n"
"row_major float4x4 World;\n"
"row_major float4x4 View;\n"
"row_major float4x4 Projection;\n"
"float4   Color;\n"
"void vertexFn(float4 position   : POSITION,\n"
"              float4 color      : COLOR,\n"
"              float2 TexCoord   : TEXCOORD0,\n"
"          out float4 oPosition  : POSITION,\n"
"          out float4 oColor     : COLOR,\n"
"          out float2 oTexCoord  : TEXCOORD0)\n"
"{\n"
"    oPosition = mul(World, position);\n"
"    oPosition = mul(View, oPosition);\n"
"    oPosition = mul(Projection, oPosition);\n"
"#ifdef TEST_MACRO\n"
"    oColor = color * TEST_MACRO;\n"
"#else\n"
"    oColor = color;\n"
"#endif\n"
"    if(TestBool)\n"
"        oColor = Color + oColor;\n"
"    oTexCoord = TexCoord;\n"
"}\n";


const char PS_SOURCE [] = ""
"sampler2D baseMap;\n"
"float4 pixelFn(float4 color : COLOR,\n"
"               float2 TexCoord : TEXCOORD0) : COLOR\n"
"{\n"
"    color = tex2D(baseMap, TexCoord.xy);\n"
"    return color;\n"
"}\n";



//pour configurer les etat du rendu
Gfx::IVertexFormatPtr pVertexFormat;
Gfx::IRSRasterizerPtr pStateRasterizer;
Gfx::IRSDepthStencilPtr pStateDepthStencil;

//la fenetre de rendu
Ptr<Window::Window> pRenderWindow1;
Ptr<Window::Window> pRenderWindow2;

//la geometrie
Gfx::IVertexBufferPtr pVertexBuf;
Gfx::IIndexBufferPtr pIndexBuf;

//les textures
Gfx::ITexture2DPtr pTex1;
Gfx::ITexture2DPtr pTex2;
Gfx::ITexture2DPtr pTex3;

//les fonts
Ptr<Gfx::IFont>    pFont1;

//render target view
Gfx::IRenderTargetViewPtr pTex1RenderTargetView;    //render target sur texture
Gfx::IRenderTargetViewPtr pWindowRenderTargetView1; //render target sur window 1
Gfx::IRenderTargetViewPtr pWindowRenderTargetView2; //render target sur window 2

//depth stencil view
Gfx::IDepthStencilViewPtr pTex2DepthStencilView;    //pour le moment tous les rendertargetView utilise le même detphStencilView

//ShaderResourceView
Gfx::IShaderResourceViewPtr  pTex1ShaderResourceView;
Gfx::IShaderResourceViewPtr  pTex3ShaderResourceView;

//les shaders
Gfx::IVertexShaderPtr pVShader;
Gfx::IPixelShaderPtr pPShader;
Gfx::ShaderConstantTablePtr variableVShader;
Gfx::ShaderConstantTablePtr variablePShader;


Core::Matrix4f matWorld;
Core::Matrix4f matView;
Core::Matrix4f matProjection;
Core::Vector4f bgColor (0, 0, 0, 0);

Gfx::IDriverPtr pDriver;    //le driver pour recuperer les info du meteriel graphique
Gfx::IDevicePtr pDevice;    //le device est separe du driver pour gerer les cas ou plusieurs exe utilise  D3D (ex: le jeux et l'editeur de niveaux, ...)


Gfx::ISwapChainPtr pSwapChain1;
Gfx::ISwapChainPtr pSwapChain2;

bool initialize(HINSTANCE hInst);
void cleanUpDevice();
void render();


Core::Ptr<Images::Image> createImageFromTexture2D(const Gfx::ITexture2DPtr& pTex2D);

class WindowHandler : public Window::IWindowMessageHandler
{
public:
    virtual bool onPaint(void * pHandle)
    {
        PAINTSTRUCT ps;
        HDC hdc;
        HWND hWnd(static_cast<HWND>(pHandle));
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return true;
    };
    virtual bool onDestroy(void * pHandle)
    {
        PostQuitMessage(0);
        return true;
    };
    virtual bool onLeftButtonDown(void * pHandle)
    {
        PostQuitMessage(0);
        return true;
    };
    virtual bool onLeftButtonDoubleClick(void * pHandle)
    {
        return true;
    };
    virtual bool onMiddleButtonDown(void * pHandle)
    {
        return true;
    };
    virtual bool onRightButtonDown(void * pHandle)
    {
        return false;
    };
    virtual bool onLeftButtonUp(void * pHandle)
    {
        PostQuitMessage(0);
        return true;
    };
    virtual bool onMiddleButtonUp(void * pHandle)
    {
        return true;
    };
    virtual bool onRightButtonUp(void * pHandle)
    {
        return false;
    };
    virtual bool onMouseMove(void * pHandle, int32 x, int32 y, int32 flags)
    {
        return false;
    };
    virtual bool onMouseWheel(void * pHandle, int32 roll, int32 flags)
    {
        return false;
    };
    virtual bool onKeyDown(void * pHandle, int32 keyCode)
    {
        return false;
    };
    virtual bool onChar(void * pHandle, wchar_t c)
    {
        return false;
    };
    virtual bool onKeyUp(void * pHandle, int32 keyCode)
    {
        return false;
    };
    virtual bool onResize(void * pHandle, int32 width, int32 height)
    {
        return false;
    };
    virtual bool onActivate(void * pHandle, bool active)
    {
        return false;
    };
    virtual bool onMinimize(void * pHandle, bool minimized)
    {
        return false;
    };
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Core::String workDir(LM_WORKING_DIRECTORY);
	SetCurrentDirectory(Core::String8(workDir).c_str());
    DBG << L"Start";
    Core::Thread::quickSleep(584);

    if(initialize(hInst) == false)
    {
        cleanUpDevice();
        return 0;
    }

    //LM_LDBG << driver->getAdapterInfo();
    //Gfx::AdapterInformation ai = pDriver->getAdapters(0);
    //LM_LDBG << Core::toString(ai);
    //LM_LDBG << Gfx::toString(pDriver->getGraphicCapabilities());

    pDevice->setState(pStateRasterizer);
    pDevice->setState(pStateDepthStencil);
    pDevice->setVertexFormat(pVertexFormat);

    //LM_LDBG << Core::toString(variableVShader->map);


    MSG msg = {0};
    int32 nbFrames = 0;
    Core::Ptr<Images::Image> pImg4;
    Core::Ptr<Images::Image> pImg5;

    pDevice->PSSetShaderResources(0, 1, &pTex3ShaderResourceView);


    bgColor = Core::Vector4f (125, 125, 125, 0);

    INF << L"Start";
    pDevice->setRenderTarget(pTex1RenderTargetView, pTex2DepthStencilView);
    while( WM_QUIT != msg.message )
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            render();
            nbFrames++;

            if (nbFrames==1)
            {
                //on copie en RAM l'image du rendu sur Texture
                Ptr<Core::Bitmap> pBitmap = pDevice->toBitmap(pTex1RenderTargetView);
                pImg4 = Images::toImage(pBitmap);

                //on sauvegarde la texture applique au cube
                Core::Ptr<Images::Image> pImg = createImageFromTexture2D(pTex3);
                pImg->saveFile(L"Local/TVGfxDx9_TextureCube.png");
            }

            if (nbFrames%2 == 0)
            {
                pSwapChain2->present();

                int32 vcSampler = variablePShader->getConstantIndex(L"baseMap");
                variablePShader->setSampler2D(vcSampler, pTex3ShaderResourceView);

                bgColor = Core::Vector4f (125, 125, 125, 0);
                pDevice->setRenderTarget(pWindowRenderTargetView1, pTex2DepthStencilView);
            }
            else
            {
                pSwapChain1->present();
                pDevice->PSSetShaderResources(0, 1, &pTex1ShaderResourceView);
                bgColor = Core::Vector4f (0, 0, 0, 0);
                pDevice->setRenderTarget(pWindowRenderTargetView2, pTex2DepthStencilView);
            }

        }
    }

    //on sauvegarde l'image du rendu sur Texture
    pImg4->saveFile(L"Local/TVGfxDx9_textureRenderTarget.bmp");

    cleanUpDevice();


    return (int)msg.wParam;
}

bool initialize(HINSTANCE hInst)
{
    //creation 1ere fenetre
    Ptr<WindowHandler> pWinHandler = Ptr<WindowHandler>(new WindowHandler());
    pRenderWindow1 = Ptr<Window::Window>(new Window::Window(
            hInst,
            L"TVGfxDX9_1",
            0, 0,    // position
            800, 600,    // dimension
            false,  // fullscreen
            pWinHandler));

    //creation 2nd fenetre
    Ptr<WindowHandler> pWinHandler2 = Ptr<WindowHandler>(new WindowHandler());
    pRenderWindow2 = Ptr<Window::Window>(new Window::Window(
             hInst,
             L"TVGfxDX9_2",
             700, 400,    // position
             800, 600,    // dimension
             false,  // fullscreen
             pWinHandler2));

    pDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());

    //description swapChain fenetre 1
    Gfx::SwapChainDesc scDesc;
    scDesc.width = pRenderWindow1->getWidth();
    scDesc.height = pRenderWindow1->getHeight();
    scDesc.format = Gfx::DPF_X8R8G8B8;
    scDesc.bufferCount = 2;
    scDesc.handleWindow = pRenderWindow1->getHandle();
    scDesc.screenMode = Gfx::WINDOWED_MODE;

    //description swapChain fenetre 2
    Gfx::SwapChainDesc scDesc2;
    scDesc2.width = pRenderWindow2->getWidth();
    scDesc2.height = pRenderWindow2->getHeight();
    scDesc2.format = Gfx::DPF_X8R8G8B8;
    scDesc2.bufferCount = 2;
    scDesc2.handleWindow = pRenderWindow2->getHandle();
    scDesc2.screenMode = Gfx::WINDOWED_MODE;


    //creation du device et du 1er swapChain
    pDriver->createDevice(
            Gfx::DEFAULT_ADAPTER,
            Gfx::SOFTWARE_PROCESSING,
            scDesc,
            pSwapChain1,
            pDevice);


    //creation de la texture DepthStencil
    Gfx::Texture2DDesc tex2Desc;
    tex2Desc.width = scDesc.width;
    tex2Desc.height = scDesc.height;
    tex2Desc.mipLevels = 0;
    tex2Desc.format = Gfx::TF_D24S8;
    tex2Desc.bind = Gfx::TB_DEPTH_STENCIL_SURFACE;
    pTex2 = pDevice->createTexture2D(tex2Desc);

    //creation DepthStencilView
    Gfx::DepthStencilViewDesc tex2_dsvDesc;
    tex2_dsvDesc.texture2D.mipSlice = 0;
    pTex2DepthStencilView = pDevice->createDepthStencilView(pTex2, tex2_dsvDesc);

    //creation du second SwapChain
    pSwapChain2 = pDevice->createSwapChain(scDesc2);

    //-- : render target
    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.swapChain.idBuffer = 0;
    pWindowRenderTargetView1 = pDevice->createRenderTargetView(pSwapChain1, rtDesc);

    Gfx::RenderTargetViewDesc rtDesc2;
    rtDesc2.swapChain.idBuffer = 0;
    pWindowRenderTargetView2 = pDevice->createRenderTargetView(pSwapChain2, rtDesc2);

    //-- : on modifie la surface de rendue
    pDevice->setRenderTarget(pWindowRenderTargetView1, pTex2DepthStencilView);

    //-- : ViewPort
    Gfx::Viewport vp(0, 0, pRenderWindow1->getWidth(), pRenderWindow1->getHeight(), 0.0, 1.0);
    pDevice->setViewport(vp);

    //-- : geometrie
    Gfx::VertexFormatDesc vertexFormat;

    {
        int32 nbVertex = 8;
        int32 nbTriangle = 12;

        float taille = 1.0f;

        //input element
        vertexFormat.clear();
        vertexFormat.addAttribut(0, Gfx::VAT_FLOAT4, Gfx::VAU_POSITION);
        vertexFormat.addAttribut(0, Gfx::VAT_FLOAT4, Gfx::VAU_COLOR);
        vertexFormat.addAttribut(0, Gfx::VAT_FLOAT2, Gfx::VAU_TEXTURE_COORD);

        //vertex buffer
        int32 VertexSize = vertexFormat.getVertexSize(0);
        Gfx::VertexBufferDesc vBufDesc(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, VertexSize*nbVertex);
        pVertexBuf = pDevice->createVertexBuffer(vBufDesc);

        //index buffer
        Gfx::IndexBufferDesc iBufDesc(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, nbTriangle*3*sizeof(uint16));
        pIndexBuf = pDevice->createIndexBuffer(iBufDesc);

        //on renseigne le vertexbuffer
        int32 inc = 0;
        float* vbuf = (float*)pVertexBuf->lock(Gfx::LOCK_WRITE, 0, 0);
        vbuf[inc++]=-taille; vbuf[inc++]=+taille; vbuf[inc++]=-taille; vbuf[inc++]=1.f;   vbuf[inc++]=0; vbuf[inc++]=0; vbuf[inc++]=1; vbuf[inc++]=1;  vbuf[inc++]=0.0f; vbuf[inc++]=0.0f;
        vbuf[inc++]=+taille; vbuf[inc++]=+taille; vbuf[inc++]=-taille; vbuf[inc++]=1.f;   vbuf[inc++]=0; vbuf[inc++]=1; vbuf[inc++]=0; vbuf[inc++]=1;  vbuf[inc++]=1.0f; vbuf[inc++]=0.0f;
        vbuf[inc++]=+taille; vbuf[inc++]=+taille; vbuf[inc++]=+taille; vbuf[inc++]=1.f;   vbuf[inc++]=0; vbuf[inc++]=1; vbuf[inc++]=1; vbuf[inc++]=1;  vbuf[inc++]=1.0f; vbuf[inc++]=1.0f;
        vbuf[inc++]=-taille; vbuf[inc++]=+taille; vbuf[inc++]=+taille; vbuf[inc++]=1.f;   vbuf[inc++]=1; vbuf[inc++]=0; vbuf[inc++]=0; vbuf[inc++]=1;  vbuf[inc++]=0.0f; vbuf[inc++]=1.0f;
        vbuf[inc++]=-taille; vbuf[inc++]=-taille; vbuf[inc++]=-taille; vbuf[inc++]=1.f;   vbuf[inc++]=1; vbuf[inc++]=0; vbuf[inc++]=1; vbuf[inc++]=1;  vbuf[inc++]=0.0f; vbuf[inc++]=0.0f;
        vbuf[inc++]=+taille; vbuf[inc++]=-taille; vbuf[inc++]=-taille; vbuf[inc++]=1.f;   vbuf[inc++]=1; vbuf[inc++]=1; vbuf[inc++]=0; vbuf[inc++]=1;  vbuf[inc++]=1.0f; vbuf[inc++]=0.0f;
        vbuf[inc++]=+taille; vbuf[inc++]=-taille; vbuf[inc++]=+taille; vbuf[inc++]=1.f;   vbuf[inc++]=1; vbuf[inc++]=1; vbuf[inc++]=1; vbuf[inc++]=1;  vbuf[inc++]=1.0f; vbuf[inc++]=1.0f;
        vbuf[inc++]=-taille; vbuf[inc++]=-taille; vbuf[inc++]=+taille; vbuf[inc++]=1.f;   vbuf[inc++]=0; vbuf[inc++]=0; vbuf[inc++]=0; vbuf[inc++]=1;  vbuf[inc++]=0.0f; vbuf[inc++]=1.0f;
        pVertexBuf->unlock();

        //on renseigne l'indexbuffer
        inc = 0;
        int16* ibuf = (int16*)pIndexBuf->lock(Gfx::LOCK_WRITE, 0, 0);
        ibuf[inc++]=3; ibuf[inc++]=1; ibuf[inc++]=0;
        ibuf[inc++]=2; ibuf[inc++]=1; ibuf[inc++]=3;
        ibuf[inc++]=0; ibuf[inc++]=5; ibuf[inc++]=4;
        ibuf[inc++]=1; ibuf[inc++]=5; ibuf[inc++]=0;
        ibuf[inc++]=7; ibuf[inc++]=0; ibuf[inc++]=4;
        ibuf[inc++]=3; ibuf[inc++]=0; ibuf[inc++]=7;
        ibuf[inc++]=2; ibuf[inc++]=5; ibuf[inc++]=1;
        ibuf[inc++]=6; ibuf[inc++]=5; ibuf[inc++]=2;
        ibuf[inc++]=7; ibuf[inc++]=2; ibuf[inc++]=3;
        ibuf[inc++]=6; ibuf[inc++]=2; ibuf[inc++]=7;
        ibuf[inc++]=4; ibuf[inc++]=6; ibuf[inc++]=7;
        ibuf[inc++]=5; ibuf[inc++]=6; ibuf[inc++]=4;
        pIndexBuf->unlock();
    }

    pDevice->setVertexBuffer(0, pVertexBuf, vertexFormat.getVertexSize(0), 0);
    pDevice->setIndexBuffer(pIndexBuf); //placer le type d'un index (16 ou 32)

    //-- : shader
    try
    {
        Core::List<Gfx::ShaderMacro> macros;
        macros.push_back(Gfx::ShaderMacro(L"TEST_MACRO", L"float4(1.0, 0.0, 0.0, 1.0)"));

        Gfx::IVertexShaderBinObjectPtr pVSCode = pDevice->compileVShader(String(VS_SOURCE), Gfx::VS_V2_0, String(L"vertexFn"), macros, null);
        Gfx::IPixelShaderBinObjectPtr pPSCode = pDevice->compilePShader(String(PS_SOURCE), Gfx::PS_V2_0, L"pixelFn", macros, null);

        Core::BufferStream buffer;

        pVSCode->write(buffer);
        Gfx::IVertexShaderBinObjectPtr pVSCode2 = pDevice->readPrecompiledVShader(buffer);

        pPSCode->write(buffer);
        Gfx::IPixelShaderBinObjectPtr pPSCode2 = pDevice->readPrecompiledPShader(buffer);

        pVShader = pDevice->createVertexShader(pVSCode2);
        pPShader = pDevice->createPixelShader(pPSCode2);
    }
    catch(Gfx::GfxException &)
    {
        return false;
    }

    variableVShader = pVShader->getConstantTable();
    variablePShader = pPShader->getConstantTable();

    pDevice->setVertexShader(pVShader);
    pDevice->setPixelShader(pPShader);

    //-- : state
    pVertexFormat = pDevice->createVertexFormat(vertexFormat, pVShader);

    Gfx::RSRasterizerDesc rsDesc;
    rsDesc.cullMode = Gfx::CM_FRONT;
    rsDesc.frontFaceIsCounterClockwise = true;
    //rsDesc.fillMode = Gfx::FM_WIREFRAME; //pour passer en fil de fer
    pStateRasterizer = pDevice->createState(rsDesc);


    Gfx::RSDepthStencilDesc dsDesc (true, true, Gfx::COMPARISON_ALWAYS);
    dsDesc.isStencilEnabled = true;
    pStateDepthStencil = pDevice->createState(dsDesc);

    //-- : matrice
    matWorld.setIdentity();

    Core::Vector3f vecEye(10.f, 10.f, 10.f);
    Core::Vector3f vecAt(0.f, 0.f, 0.f);
    Core::Vector3f vecUp(0.f, 1.f, 0.f);
    matView.setLookAt(vecEye, vecAt, vecUp);
    matView.inverseSelf();

    float fov = f_PI * 0.25f;
    float aspectRatio = pRenderWindow1->getWidth() / (float) pRenderWindow1->getHeight();
    float near = 4.0f;
    float far = 40.f;
    matProjection.setPerspectiveProj(fov, aspectRatio, near, far);

    /*
    float w = 4.0;
    float h = aspectRatio * w;
    matProjection.setFrustumProj(-w, w, -h, h, near, far);
    */

    //-- : test Texture

    Gfx::Texture2DDesc tex1Desc;
    tex1Desc.width = 800;
    tex1Desc.height = 600;
    tex1Desc.mipLevels = 1;
    tex1Desc.format = Gfx::TF_R5G6B5;
    //tex1Desc.usage = Gfx::BUFFER_USAGE_DYNAMIC;
    tex1Desc.bind = Gfx::TB_RENDER_TARGET;
    pTex1 = pDevice->createTexture2D(tex1Desc);



    Gfx::RenderTargetViewDesc tex1_rtvDesc;
    tex1_rtvDesc.texture2D.mipSlice = 0;
    pTex1RenderTargetView = pDevice->createRenderTargetView(pTex1, tex1_rtvDesc);




    Gfx::Texture2DDesc tex3Desc;
    tex3Desc.width =512;
    tex3Desc.height = 512;
    tex3Desc.mipLevels = 1;
    tex3Desc.format = /*Gfx::TF_DXT5;//*/Gfx::TF_X8R8G8B8;
    tex3Desc.bind = Gfx::TB_BUFFER_DEFAULT;
    tex3Desc.usage = Gfx::BUFFER_USAGE_DYNAMIC;
    tex3Desc.cpuAccess = Gfx::BCA_ONLY_READ;
    pTex3 = pDevice->createTexture2D(tex3Desc);

    pTex3ShaderResourceView = pTex3->getShaderResourceView();

    Gfx::MappedTexture2D mapTex = pTex3->lock(Gfx::LOCK_WRITE, 0);
    for (int y=0; y<512/2; y++)
    {
        for (int x=0; x<mapTex.rowStep/2; x+=4)
        {
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+1] = 255;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+2] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+3] = 0;
        }
        for (int x=mapTex.rowStep/2; x<mapTex.rowStep; x+=4)
        {
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+1] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+2] = 255;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+3] = 0;
        }
    }
    for (int y=512/2; y<512; y++)
    {
        for (int x=0; x<mapTex.rowStep/2; x+=4)
        {
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+1] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+2] = 255;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+3] = 0;
        }
        for (int x=mapTex.rowStep/2; x<mapTex.rowStep; x+=4)
        {
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+1] = 255;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+2] = 0;
            ((uint8*)mapTex.pData)[y*mapTex.rowStep+x+3] = 0;
        }
    }
    pTex3->unlock(0);

    pTex1ShaderResourceView  = pTex1->getShaderResourceView();

    Gfx::FontDesc fontDesc;
    fontDesc.name = L"Arial";
    fontDesc.pixHeight = 48;
    fontDesc.bold = false;
    fontDesc.italic = false;

    pFont1 = pDevice->createFont(fontDesc);

    return true;
}


void cleanUpDevice()
{
    pVertexFormat = null;
    pStateRasterizer = null;
    pStateDepthStencil = null;

    //la fenetre de rendu
    pRenderWindow1 = null;
    pRenderWindow2 = null;

    //la geometrie
    pVertexBuf = null;
    pIndexBuf = null;

    //les textures
    pTex1 = null;
    pTex2 = null;
    pTex3 = null;

    //render target view
    pTex1RenderTargetView = null;    //render target sur texture
    pWindowRenderTargetView1 = null; //render target sur window 1
    pWindowRenderTargetView2 = null; //render target sur window 2

    //depth stencil view
    pTex2DepthStencilView = null;

    //ShaderResourceView
    pTex1ShaderResourceView = null;
    pTex3ShaderResourceView = null;

    //les shaders
    pVShader = null;
    pPShader = null;
    variableVShader = null;
    variablePShader = null;


    pSwapChain1 = null;
    pSwapChain2 = null;

    pDevice = null;
    pDriver = null;
}

void render()
{
    float angle = ( float )f_PI * 0.005f;

    Core::Matrix4f matRotYaw = Core::Matrix4f::makeRotationYaw(angle);
    Core::Matrix4f matRotPitch = Core::Matrix4f::makeRotationPitch(angle);
    matWorld = matWorld * matRotYaw * matRotPitch;

    pDevice->beginDraw();

    pDevice->clearCurrentRenderTargetView(bgColor);

    int32 vcWorldConst = variableVShader->getConstantIndex(L"World");
    int32 vcViewConst = variableVShader->getConstantIndex(L"View");
    int32 vcProjectionConst = variableVShader->getConstantIndex(L"Projection");
    int32 vcColor = variableVShader->getConstantIndex(L"Color");
    int32 vcTestBool = variableVShader->getConstantIndex(L"TestBool");

    pDevice->setVertexShader(pVShader);
    pDevice->setPixelShader(pPShader);


    variableVShader->setConstant(vcWorldConst, matWorld);
    variableVShader->setConstant(vcViewConst, matView);
    variableVShader->setConstant(vcProjectionConst, matProjection);
    variableVShader->setConstant(vcColor, Core::Vector4f(0.0, 1.0, 0.0, 1.0));
    variableVShader->setConstant(vcTestBool, true);

    pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, 8, 0, 12);

    pFont1->drawText(L"Hello world!", 200, 200, Core::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));

    pDevice->endDraw();

    Sleep(5);
}

Core::Ptr<Images::Image> createImageFromTexture2D(const Gfx::ITexture2DPtr& pTex2D)
{
    Gfx::Texture2DDesc desc = pTex2D->getDesc();

    Core::Ptr<Images::Image> pImg =
            (Core::Ptr<Images::Image>) new Images::Image(Core::RGB_U8, desc.width, desc.height);

    uint8* buffer = (uint8*) pImg->getData();
    uint8* pt = NULL;

    Gfx::MappedTexture2D mapTex = pTex2D->lock(Gfx::LOCK_READ, 0);

    for (int y = 0; y < desc.height; y++)
    {
        pt = &buffer[y * desc.width * 3];
        for (int x = 0; x < desc.width; x++)
        {
            *pt = ((uint8*) mapTex.pData)[y * mapTex.rowStep + x * 4];
            pt++;
            *pt = ((uint8*) mapTex.pData)[y * mapTex.rowStep + x * 4 + 1];
            pt++;
            *pt = ((uint8*) mapTex.pData)[y * mapTex.rowStep + x * 4 + 2];
            pt++;
        }
    }

    pTex2D->unlock(0);

    return pImg;
}
