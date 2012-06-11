#include "stdafx.h"
#include "CustomAllocator.h"
#include"util.h"

// All the functions that are implemented in this document uses critical sections,which means only one instance of the object can use the function at a specific time

CAllocator::CAllocator(HRESULT& hr, HWND wnd, IDirect3D9* d3d, IDirect3DDevice9* d3dd)
: m_refCount(1)
, m_D3D(d3d)
, m_D3DDev(d3dd)
, m_window( wnd )
{
    CAutoLock Lock(&m_ObjectLock);
    hr = E_FAIL;

    if( IsWindow( wnd ) == FALSE )
    {
        hr = E_INVALIDARG;
        return;
    }

    if( m_D3D == NULL )
    {
        ASSERT( d3dd ==  NULL ); 

        m_D3D.Attach( Direct3DCreate9(D3D_SDK_VERSION) );
        if (m_D3D == NULL) {
            hr = E_FAIL;
            return;
        }
    }

    if( m_D3DDev == NULL )
    {
        hr = CreateDevice();
    }
}

CAllocator::~CAllocator()
{
    DeleteSurfaces();
}

//CreateDevice function initializes DirectX surfaces and switches to fullscreen mode after window initialization
HRESULT CAllocator::CreateDevice()
{
    HRESULT hr;
    m_D3DDev = NULL;
    D3DDISPLAYMODE dm;

    hr = m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dm);
    D3DPRESENT_PARAMETERS pp,resetParameters;
    ZeroMemory(&pp, sizeof(pp));
	ZeroMemory(&resetParameters, sizeof(resetParameters));
   pp.Windowed=true;				//Enable or disable windowed mode.
	pp.hDeviceWindow=m_window;			//Main window that is initialized in calling method.
	pp.BackBufferCount=1;				//Backbuffer count has to be initialized to 1,for video or image.
	pp.BackBufferWidth=2;				//Temporary resolutions for initializing window.
	pp.BackBufferHeight=2;
	pp.SwapEffect=D3DSWAPEFFECT_COPY;			
	pp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT; // presentation frequency is default,will be changed in stereo mode.
	pp.EnableAutoDepthStencil=false;
	pp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
	pp.MultiSampleType=D3DMULTISAMPLE_NONE;
	pp.Flags=0;	

    FAIL_RET( m_D3D->CreateDevice(  D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    m_window,
                                    64,
                                    &pp,
                                    &m_D3DDev) );

	FAIL_RET(m_D3DDev->CreateOffscreenPlainSurface(10,10,D3DFMT_X8R8G8B8,D3DPOOL_DEFAULT,&m_commSurface,NULL));

	m_commSurface.Release();

	D3DDISPLAYMODE currentMode;
	m_D3DDev->GetDisplayMode(0,&currentMode);

	resetParameters.BackBufferCount=1;
	resetParameters.MultiSampleType=D3DMULTISAMPLE_2_SAMPLES;	//Multisample type has to be 2 in order to stereo work.
	resetParameters.Flags=0;
	resetParameters.FullScreen_RefreshRateInHz = currentMode.RefreshRate;
	resetParameters.BackBufferHeight=currentMode.Height;
	resetParameters.BackBufferWidth=currentMode.Width;
	resetParameters.BackBufferFormat= (D3DFORMAT)currentMode.Format;
	resetParameters.Windowed=false;								//Fullscreen mode must be enabled for stereo mode.
	resetParameters.EnableAutoDepthStencil=false;
	
	resetParameters.SwapEffect=D3DSWAPEFFECT_DISCARD;
	resetParameters.PresentationInterval=2147483648;

	FAIL_RET(m_D3DDev->Reset(&resetParameters));

	
	FAIL_RET(m_D3DDev->CreateRenderTarget(resetParameters.BackBufferWidth,resetParameters.BackBufferHeight,resetParameters.BackBufferFormat,resetParameters.MultiSampleType
		,resetParameters.MultiSampleQuality,false,&m_leftImageBack,NULL));
	FAIL_RET(m_D3DDev->CreateRenderTarget(resetParameters.BackBufferWidth,resetParameters.BackBufferHeight,resetParameters.BackBufferFormat,resetParameters.MultiSampleType
		,resetParameters.MultiSampleQuality,false,&m_rightImageBack,NULL));

	m_D3DDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_leftImageBack);
	m_D3DDev->GetBackBuffer(1,0,D3DBACKBUFFER_TYPE_MONO,&m_rightImageBack);

	

	FAIL_RET(D3DXCreateSprite(m_D3DDev,&m_leftSprite));
	FAIL_RET(D3DXCreateSprite(m_D3DDev,&m_rightSprite));

	

	return hr;

}

//DeleteSurfaces function deletes the surface that is coming from the downstream filter,not the surfaces that are created during runtime to draw the frames to screen.
void CAllocator::DeleteSurfaces()
{
    CAutoLock Lock(&m_ObjectLock);

    // clear out the private texture
    m_sourceTexture = NULL;
	m_leftFrontTexture=NULL;
	m_rightFrontTexture=NULL;

	
    for( size_t i = 0; i < m_surfaces.size(); ++i ) 
    {
        m_surfaces[i] = NULL;
    }


}


//InitializeDevice function creates the texture objects that are drawn to the screen and determines the stretching area regarding to the input pin.
//Left and right splitting rectangles are assigned in this function.
HRESULT CAllocator::InitializeDevice( 
            /* [in] */ DWORD_PTR dwUserID,
            /* [in] */ VMR9AllocationInfo *lpAllocInfo,
            /* [out][in] */ DWORD *lpNumBuffers)
{

	HRESULT hr=S_OK;

	
	 D3DCAPS9 d3dcaps;
    DWORD dwWidth = 1;
    DWORD dwHeight = 1;
    float fTU = 1.f;
    float fTV = 1.f;

	m_D3DDev->GetDeviceCaps( &d3dcaps );
    if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( dwWidth < lpAllocInfo->dwWidth )
            dwWidth = dwWidth << 1;
        while( dwHeight < lpAllocInfo->dwHeight )
            dwHeight = dwHeight << 1;

        fTU = (float)(lpAllocInfo->dwWidth) / (float)(dwWidth);
        fTV = (float)(lpAllocInfo->dwHeight) / (float)(dwHeight);
        lpAllocInfo->dwWidth = dwWidth;
        lpAllocInfo->dwHeight = dwHeight;
    }

	lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;

     DeleteSurfaces();
	 m_surfaces.resize(*lpNumBuffers);

	 hr=m_lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(0) );

	 FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
                                    1, 
                                    D3DUSAGE_RENDERTARGET, 
                                    lpAllocInfo->Format, 
                                    D3DPOOL_DEFAULT /* default pool - usually video memory */, 
                                    & m_leftFrontTexture, NULL ) );

	 	 FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
                                    1, 
                                    D3DUSAGE_RENDERTARGET, 
                                    lpAllocInfo->Format, 
                                    D3DPOOL_DEFAULT /* default pool - usually video memory */, 
                                    & m_rightFrontTexture, NULL ) );

	  leftSourceRect.left=0;leftSourceRect.right=lpAllocInfo->dwWidth;leftSourceRect.top=0;leftSourceRect.bottom=(lpAllocInfo->dwHeight)/2;

	  rightSourceRect.left=0;rightSourceRect.right=lpAllocInfo->dwWidth;
	  rightSourceRect.top=(lpAllocInfo->dwHeight)/2;rightSourceRect.bottom=lpAllocInfo->dwHeight;
	
	 

	 if(FAILED(hr) && !(lpAllocInfo->dwFlags & VMR9AllocFlag_3DRenderTarget))
    {
        exit(1);
    }

	return hr;
	
   
}
            
HRESULT CAllocator::TerminateDevice( 
        /* [in] */ DWORD_PTR dwID)
{
    DeleteSurfaces();
    return S_OK;
}
    
//Allocator calls GetSurface function as long as there are new frames coming from the downstream filter.
HRESULT CAllocator::GetSurface( 
        /* [in] */ DWORD_PTR dwUserID,
        /* [in] */ DWORD SurfaceIndex,
        /* [in] */ DWORD SurfaceFlags,
        /* [out] */ IDirect3DSurface9 **lplpSurface)
{
    if( lplpSurface == NULL )
    {
        return E_POINTER;
    }

    if (SurfaceIndex >= m_surfaces.size() ) 
    {
        return E_FAIL;
    }

    CAutoLock Lock(&m_ObjectLock);

    *lplpSurface = m_surfaces[SurfaceIndex];
    (*lplpSurface)->AddRef();

    return S_OK;

}
    

//Custom allocator uses this method for notification callbacks.
HRESULT CAllocator::AdviseNotify( 
        /* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{
    CAutoLock Lock(&m_ObjectLock);

    HRESULT hr;

    m_lpIVMRSurfAllocNotify = lpIVMRSurfAllocNotify;

    HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );
    FAIL_RET( m_lpIVMRSurfAllocNotify->SetD3DDevice( m_D3DDev, hMonitor ) );

    return hr;
}

//StartPresenting method checks if the device is ready for rendering.

HRESULT CAllocator::StartPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    CAutoLock Lock(&m_ObjectLock);

    ASSERT( m_D3DDev );
    if( m_D3DDev == NULL )
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CAllocator::StopPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    return S_OK;
}

//PresentImage function starts presenting the image to the screen.Actual rendering is done by the helper function;PresentHelper()
HRESULT CAllocator::PresentImage( 
    /* [in] */ DWORD_PTR dwUserID,
    /* [in] */ VMR9PresentationInfo *lpPresInfo)
{
    HRESULT hr;
    CAutoLock Lock(&m_ObjectLock);
	
    // if we are in the middle of the display change
    //if( NeedToHandleDisplayChange() )
    //{
    //     
    //    // The D3DDevice here needs to be switched
    //    // to the device that is using another adapter
    //}
	
    hr = PresentHelper( lpPresInfo );

    // IMPORTANT: device can be lost when user changes the resolution
    // or when (s)he presses Ctrl + Alt + Delete.
    // We need to restore our video memory after that
    if( hr == D3DERR_DEVICELOST)
    {
        if (m_D3DDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) 
        {
            DeleteSurfaces();
            FAIL_RET( CreateDevice() );

            HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

            FAIL_RET( m_lpIVMRSurfAllocNotify->ChangeD3DDevice( m_D3DDev, hMonitor ) );

        }

        hr = S_OK;
    }

    return hr;
}


//Actual rendering is done in this function.
HRESULT CAllocator::PresentHelper(VMR9PresentationInfo *lpPresInfo)
{
	
    // parameter validation
    if( lpPresInfo == NULL )
    {
        return E_POINTER;
    }
    else if( lpPresInfo->lpSurf == NULL )
    {
        return E_POINTER;
    }

	lpPresInfo->lpSurf->GetContainer( __uuidof(IDirect3DTexture9),(LPVOID*)&m_sourceTexture);	//Get seperated frames as a texture.

	m_sourceTexture->GetSurfaceLevel(0,&m_sourceSurfaceTemp);						
	m_leftFrontTexture->GetSurfaceLevel(0,&m_targetSurfaceTemp);
	m_D3DDev->StretchRect(m_sourceSurfaceTemp,&leftSourceRect,m_targetSurfaceTemp,NULL,D3DTEXF_NONE);	//Copy the top side of the frame to the left stream.		

	m_rightFrontTexture->GetSurfaceLevel(0,&m_targetSurfaceTemp);
	m_D3DDev->StretchRect(m_sourceSurfaceTemp,&rightSourceRect,m_targetSurfaceTemp,NULL,D3DTEXF_NONE);	//Copy the bottom side of the frame to the right stream.


    HRESULT hr;
	m_D3DDev->BeginScene();		
	

	hr=m_D3DDev->SetRenderTarget(0,m_leftImageBack);						//Switch to left buffer.

	
	hr=m_leftSprite->Begin(D3DXSPRITE_ALPHABLEND);
	hr=m_leftSprite->Draw(m_leftFrontTexture,NULL,NULL,NULL,0xffffffff);	//Render left image to the left back buffer.
	hr=m_leftSprite->End();
	
	
	m_D3DDev->SetRenderTarget(0,m_rightImageBack);							//Switch to right buffer.						
	
	m_rightSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_rightSprite->Draw(m_rightFrontTexture,NULL,NULL,NULL,0xffffffff);		//Render right image to the right back buffer	
	m_rightSprite->End();
	
	
	m_D3DDev->EndScene();
    FAIL_RET( m_D3DDev->Present( NULL, NULL, NULL, NULL ) );			

    return hr;
}

bool CAllocator::NeedToHandleDisplayChange()
{
    if( ! m_lpIVMRSurfAllocNotify )
    {
        return false;
    }

    D3DDEVICE_CREATION_PARAMETERS Parameters;
    if( FAILED( m_D3DDev->GetCreationParameters(&Parameters) ) )
    {
        ASSERT( false );
        return false;
    }

    HMONITOR currentMonitor = m_D3D->GetAdapterMonitor( Parameters.AdapterOrdinal );

    HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

    return hMonitor != currentMonitor;

}


// IUnknown
HRESULT CAllocator::QueryInterface( 
        REFIID riid,
        void** ppvObject)
{
    HRESULT hr = E_NOINTERFACE;

    if( ppvObject == NULL ) {
        hr = E_POINTER;
    } 
    else if( riid == IID_IVMRSurfaceAllocator9 ) {
        *ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
        AddRef();
        hr = S_OK;
    } 
    else if( riid == IID_IVMRImagePresenter9 ) {
        *ppvObject = static_cast<IVMRImagePresenter9*>( this );
        AddRef();
        hr = S_OK;
    } 
    else if( riid == IID_IUnknown ) {
        *ppvObject = 
            static_cast<IUnknown*>( 
            static_cast<IVMRSurfaceAllocator9*>( this ) );
        AddRef();
        hr = S_OK;    
    }

    return hr;
}

ULONG CAllocator::AddRef()
{
    return InterlockedIncrement(& m_refCount);
}

ULONG CAllocator::Release()
{
	if(m_leftImageBack)
	{
		m_leftImageBack.Release();
		m_leftImageBack=NULL;
	}

	if(m_rightImageBack)
	{
		m_rightImageBack.Release();
		m_rightImageBack=NULL;
	}
	
	if(m_commSurface)
	{
		m_commSurface.Release();
		m_commSurface=NULL;
	}
	if(m_sourceSurfaceTemp)
	{
		m_sourceSurfaceTemp.Release();
		m_sourceSurfaceTemp=NULL;
	}

	if(m_targetSurfaceTemp)
	{
		m_targetSurfaceTemp.Release();
		m_targetSurfaceTemp=NULL;
	}

	m_leftSprite.Release();
	m_rightSprite.Release();

    ULONG ret = InterlockedDecrement(& m_refCount);
    if( ret == 0 )
    {
        delete this;
    }
    return ret;
}
