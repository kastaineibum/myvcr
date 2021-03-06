//////////////////////////////////////////
//MyVCR Core Class For Audio&Video Capture
//
//////////////////////////////////////////

#include "StdAfx.h"
#include "Cortez.h"
#include <io.h>

#include "Log4c.h"

bool Cortez::m_isRascalEnabled = false;

bool Cortez::bind2device(DWORD dwDevIndex)
{
	CComPtr<ICreateDevEnum> pCreateDevEnum; 
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, 
			IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR) 
		return false;

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0); 
	if (hr != NOERROR) 
		return false; 

	pEm->Reset(); 
	ULONG cFetched; 
	IMoniker *pM; 
	int nIndx = 0; 
	while(hr = pEm-> Next(1, &pM, &cFetched), hr==S_OK) 
	{ 
		IPropertyBag *pBag; 
		hr = pM-> BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag); 
		if(SUCCEEDED(hr)) 
		{ 
			VARIANT var; 
			var.vt = VT_BSTR; 
			hr = pBag-> Read(L"FriendlyName", &var, NULL); 
			if (hr == NOERROR) 
			{ 
				CString sztmp = var.bstrVal;
				if( nIndx == dwDevIndex )
				{ 
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pCap); 
					break;
				} 
				SysFreeString(var.bstrVal); 
			} 
			pBag-> Release(); 
			nIndx++; 
		} 
		pM-> Release(); 
	}
	return true;
}

bool Cortez::bind2deviceAu(DWORD dwDevIndex)
{
	if(m_dwDevNumAu == 0)
		return false;

	CComPtr<ICreateDevEnum> pCreateDevEnum; 
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, 
			IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR) 
		return false;

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,&pEm, 0); 
	if (hr != NOERROR) 
		return false; 

	pEm->Reset(); 
	ULONG cFetched; 
	IMoniker *pM; 
	int nIndx = 0; 
	while(hr = pEm-> Next(1, &pM, &cFetched), hr==S_OK) 
	{ 
		IPropertyBag *pBag; 
		hr = pM-> BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag); 
		if(SUCCEEDED(hr)) 
		{ 
			VARIANT var; 
			var.vt = VT_BSTR; 
			hr = pBag-> Read(L"FriendlyName", &var, NULL); 
			if (hr == NOERROR) 
			{ 
				CString sztmp = var.bstrVal;
				if( nIndx == dwDevIndex )
				{ 
					pM-> BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pCapAu); 
					break;
				} 
				SysFreeString(var.bstrVal); 
			} 
			pBag-> Release(); 
			nIndx++; 
		} 
		pM-> Release(); 
	}
	return true;
}

int Cortez::initDevice(void)
{
	int ret = 0;
	if(!enumDevices())
		ret |= NO_VIDEO_DEVICE;

	if(!enumDevicesAu())
	{
		ret |= NO_AUDIO_DEVICE;
		setAudioDisabled(true);
	}
	return ret;
}

bool Cortez::enumDevices(void)
{
	CComPtr<ICreateDevEnum> pCreateDevEnum; 
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, 
			IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR) 
		return false;

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0); 
	if (hr != NOERROR) 
		return false; 

	pEm->Reset(); 
	ULONG cFetched; 
	IMoniker* pM; 

	while(hr = pEm->Next(1, &pM, &cFetched), hr == S_OK) 
	{ 
		IPropertyBag *pBag; 
		hr = pM-> BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag); 
		if(SUCCEEDED(hr)) 
		{ 
			VARIANT var; 
			var.vt = VT_BSTR; 
			hr = pBag-> Read(L"FriendlyName", &var, NULL); 

			if (hr == NOERROR) 
			{ 
				m_devList[m_dwDevNum++] = var.bstrVal  ;
				Log4c::logMsg(L"#ViDev:%s",var.bstrVal);
				SysFreeString(var.bstrVal); 
			} 
			pBag-> Release(); 
		} 
		pM-> Release(); 
	}

	if( m_dwDevNum == 0 )
		return false;
	return true;
}


bool Cortez::enumDevicesAu(void)
{
	CComPtr<ICreateDevEnum> pCreateDevEnum; 
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, 
			IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR) 
		return false;

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,&pEm, 0); 
	if (hr != NOERROR) 
		return false; 

	pEm->Reset(); 
	ULONG cFetched; 
	IMoniker *pM; 
	
	while(hr = pEm->Next(1, &pM, &cFetched), hr == S_OK) 
	{ 
		IPropertyBag *pBag; 
		hr = pM-> BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag); 
		if(SUCCEEDED(hr)) 
		{ 
			VARIANT var; 
			var.vt = VT_BSTR; 
			hr = pBag-> Read(L"FriendlyName", &var, NULL); 
			if (hr == NOERROR) 
			{ 
				m_devListAu[m_dwDevNumAu++] = var.bstrVal  ;
				Log4c::logMsg(L"#AuDev:%s",var.bstrVal);
				SysFreeString(var.bstrVal); 
			} 
			pBag-> Release(); 
		} 
		pM-> Release(); 
	}

	if( m_dwDevNumAu == 0 )
		return false;
	return true;
}

Resolution* Cortez::getHighResolution()
{
	return &m_ResolutionHigh;
}

Resolution* Cortez::getLowResolution()
{
	return &m_ResolutionLow;
}

Resolution* Cortez::getCurrentResolution()
{
	return m_pResolution;
}

void Cortez::setVideoResolution(Resolution* resolution)
{
	if(resolution != &m_ResolutionHigh && 
		resolution != &m_ResolutionLow)
	{
		m_pResolution = &m_ResolutionLow;
	}
	m_pResolution = resolution;
}

bool Cortez::setVideoFormat()
{
	IAMStreamConfig *pConfig = NULL;
	m_pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pCap, IID_IAMStreamConfig, (void **)&pConfig);
	// Query the output pin for IAMStreamConfig (not shown).
	AM_MEDIA_TYPE *pmt = NULL;
	HRESULT hr = pConfig->GetFormat(&pmt); 

	if (SUCCEEDED(hr))
	{
		VIDEOINFOHEADER *vh = (VIDEOINFOHEADER *)pmt->pbFormat;
		if(vh->bmiHeader.biCompression == BI_COMPRESSION_YUV2)
			m_isYUV2 = true;

		vh->bmiHeader.biBitCount = DEFAULT_VIDEO_BITCOUNT;
		//vh->bmiHeader.biCompression = BI_RGB;
		vh->bmiHeader.biWidth = m_pResolution->getWidth();
		vh->bmiHeader.biHeight = m_pResolution->getHeight(); 
		vh->bmiHeader.biSizeImage = pmt->lSampleSize =  \
		vh->bmiHeader.biWidth *      \
		vh->bmiHeader.biHeight *     \
		vh->bmiHeader.biPlanes *     \
		vh->bmiHeader.biBitCount / 8;  
		pmt->pbFormat = (BYTE *)vh;
		hr = pConfig->SetFormat(pmt);
	} 

	pConfig->Release();
	return true;
}

Cortez::Cortez(void):
  m_dwDevNum(0)
, m_pMediaControl(NULL)
, m_pMoniker(NULL)
, m_pCap(NULL)
, m_pMux(NULL)
, m_pCompressor(NULL)
, m_pSink(NULL)
, m_pGraph(NULL)
, m_Completed(NULL)
, m_isInCaptureProgress(NULL)
, m_TotalTime(NULL)
, m_pBuilder(NULL)
, m_isCapturePaused(false)
, m_pCapAu(NULL)
, m_pAviDec(NULL)
, m_pRascal(NULL)
, m_pRascalPreview(NULL)
, m_dwDevNumAu(0)
, m_pCompressorAu(NULL)
, m_isAudioDisabled(false)
, m_isYUV2(false)
, m_ResolutionLow(320, 240)
, m_ResolutionHigh(640, 480)
, m_pResolution(&m_ResolutionLow)
{
	CoInitialize(NULL);
}

Cortez::~Cortez(void)
{
	CoFreeUnusedLibraries();
	CoUninitialize();
}

void Cortez::buildDefaultFileName(CString& strFile)
{
	SYSTEMTIME    systemTime, localTime;
	GetSystemTime(&systemTime);
	SystemTimeToTzSpecificLocalTime(NULL, &systemTime, &localTime);
	strFile.Empty();
	strFile.Format(L"%4d-%02d-%02d-(%02d.%02d.%02d).avi", 
		localTime.wYear, 
		localTime.wMonth, 
		localTime.wDay, 
		localTime.wHour, 
		localTime.wMinute, 
		localTime.wSecond
	);
}

bool Cortez::setOutputFile(CString& strFileName)
{
	m_output.SetString(strFileName);
	return true;
}

IPin* Cortez::findPin(IBaseFilter *pFilter, PIN_DIRECTION dir)
{
	IEnumPins* pEnumPins;
	IPin* pOutpin;
	PIN_DIRECTION pDir;
	pFilter->EnumPins(&pEnumPins);
	while (pEnumPins->Next(1,&pOutpin,NULL)==S_OK)
	{
		pOutpin->QueryDirection(&pDir);
		if (pDir==dir)
		{
			return pOutpin;
		}
	}
	return 0;
}

bool Cortez::initFilters()
{
	HRESULT hret;
	m_pBuilder = NULL;
	hret = CoCreateInstance(CLSID_CaptureGraphBuilder2,0,CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2,(void**)&m_pBuilder);
	hret = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
        IID_IGraphBuilder, (void **)&m_pGraph);

	m_pBuilder->SetFiltergraph(m_pGraph);
	setVideoFormat();

	m_pGraph->AddFilter( m_pCap, L"MyVCR" );
	m_pGraph->AddFilter( m_pCapAu, L"MyVCR_Mic");
	m_pGraph->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
	if(initCompressor())
		m_pGraph->AddFilter(m_pCompressor,L"MyVCR_Encoder");
	else
		return false;

	if(initCompressorAu())
		m_pGraph->AddFilter(m_pCompressorAu,L"MyVCR_AudioEncoder");
	else
		return false;

	IPin* pCompressOut;
	IAMVideoCompression* pAMCompress ;
	pCompressOut =  findPin(m_pCompressor,PINDIR_OUTPUT);
	pCompressOut->QueryInterface(IID_IAMVideoCompression,(void**)&pAMCompress);
	hret = pAMCompress->put_KeyFrameRate(KEY_FRAME_RATE);
	m_pBuilder->SetOutputFileName( &MEDIASUBTYPE_Avi, m_output.AllocSysString(), &m_pMux, NULL);

	return true;
}

bool Cortez::renderAudioStream()
{
	//Render the audio stream
	HRESULT hret;
	if( !m_isAudioDisabled )
		hret = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, 
			m_pCapAu, m_pCompressorAu, m_pMux);
	return true;
}

bool Cortez::renderVideoStreamForPreview()
{
	HRESULT hret;
	if(m_isYUV2)
		initAviDec();

	hret = m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,
			m_pCap, m_pRascalPreview, NULL);

	if( hret != VFW_S_NOPREVIEWPIN && hret != S_OK )
		return false;
	return true;
}

bool Cortez::renderVideoStream()
{
	HRESULT hret;
	if(m_isRascalEnabled)
	{
		if(m_isYUV2)
			initAviDec();

		hret = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,
			m_pCap,m_pAviDec,m_pRascal);
		hret = m_pBuilder->RenderStream(NULL,&MEDIATYPE_Video,
			m_pRascal,m_pCompressor,m_pMux);

	}
	else
	{
		hret = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,
			m_pCap,m_pCompressor,m_pMux);
	}
	
	hret = m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, 
		m_pCap, m_pRascalPreview, NULL);
	//hret = S_OK;
	if( hret != VFW_S_NOPREVIEWPIN && hret != S_OK )
		return false;
	return true;
}

bool Cortez::initPreviewWin()
{
	if( (m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&m_pVWin)) == S_OK )
	{ 
		m_pVWin->put_Owner((OAHWND)m_prvwWin->m_hWnd); 
		m_pVWin->put_WindowStyle(WS_CHILD); 
		m_pVWin->SetWindowPosition(9, 15, 
			m_pResolution->getWidth(), 
			m_pResolution->getHeight()
		);
	}
	else
		return false;

	//HRESULT hret = m_pMux->QueryInterface(IID_IMediaSeeking, (void**)&m_pSeek);

    //m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pEvent);
	
	if(m_pVWin)
		m_pVWin->put_MessageDrain((OAHWND)m_prvwWin->m_hWnd);
	return true;
}

bool Cortez::beginPreview(void)
{
	initFilters();
	if(m_isRascalEnabled)
	{
		initRascal();
	}

	renderVideoStreamForPreview();
	initPreviewWin();
	m_pMediaControl->Run();
	m_isInCaptureProgress = true;	
	return true;
}

bool Cortez::beginCapture(void)
{
	if(m_output.IsEmpty())
		return false;

	initFilters();
	if(m_isRascalEnabled)
	{
		initRascal();
	}
	
	renderVideoStream();
	renderAudioStream();
	initPreviewWin();

	m_pMediaControl->Run();
	m_isInCaptureProgress = true;	

	return true;
}

bool Cortez::bind2ui(CWnd* prvwWin)
{
	m_prvwWin =  prvwWin;
	m_prvwWin->ModifyStyle( 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0 );

	return true;
}

bool Cortez::stopCapture(void)
{
	if( m_isInCaptureProgress )
	{
		m_pMediaControl->Stop();
		m_pMediaControl->Release();
		m_pVWin->put_Visible(OAFALSE);
		m_pVWin->put_Owner(NULL);
		m_pVWin->Release();
	}

	SAFE_RELEASE(m_pCap);
	SAFE_RELEASE(m_pCapAu);
	SAFE_RELEASE(m_pMux);
	SAFE_RELEASE(m_pCompressor);
	SAFE_RELEASE(m_pCompressorAu);
	SAFE_RELEASE(m_pRascal);
	SAFE_RELEASE(m_pRascalPreview);
	SAFE_RELEASE(m_pSink);
	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pBuilder);
	SAFE_RELEASE(m_pAviDec);

	m_isInCaptureProgress = false;
	m_isCapturePaused = false;
	return true;
}

bool Cortez::pauseCapture(void)
{
	m_isCapturePaused = true;
	m_pMediaControl->Pause();
	return true;
}

bool Cortez::resumeCapture(void)
{
	m_isCapturePaused = false;
	m_pMediaControl->Run();
	
	return false;
}

bool Cortez::initAviDec()
{
	HRESULT hr = CoCreateInstance(CLSID_AVIDec, NULL, 
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pAviDec);
	if (FAILED(hr))
    {
		Log4c::logMsg(L"#Cortez::initAviDec() Failed");
		return false;
    }
	m_pGraph->AddFilter(m_pAviDec, L"MyVCR_AviDec");
	return true;
}

bool Cortez::initCompressor()
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, 
		IID_ICreateDevEnum, (void **)&pDevEnum);
	ULONG cFetched;
	pDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pClassEnum, 0);

	while (pClassEnum->Next(1, &m_pMoniker, &cFetched) == S_OK)     
	{
		IPropertyBag* pProp= NULL;
		m_pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProp);
		VARIANT varName;
		varName.vt = VT_BSTR;
		pProp->Read(L"FriendlyName", &varName,0);
		CString str = varName.bstrVal;

		if (str.Find(DEFAULT_VIDEO_CODEC,0)!= -1)
		{
			
			m_pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pCompressor);
			m_pMoniker->Release(); 
			break;
		}
		
		VariantClear(&varName);
	}

	pClassEnum->Release();
	m_pMoniker->Release();

	if (m_pCompressor == NULL)
		return false;
	return true;
}

bool Cortez::initCompressorAu()
{
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker*	pClassEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, 
		IID_ICreateDevEnum, (void **)&pDevEnum);
	ULONG cFetched;
	pDevEnum->CreateClassEnumerator(CLSID_AudioCompressorCategory, &pClassEnum, 0);

	while (pClassEnum->Next(1, &m_pMoniker, &cFetched) == S_OK)     
	{
		IPropertyBag* pProp= NULL;
		m_pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pProp);
		VARIANT varName;
		varName.vt = VT_BSTR;
		pProp->Read(L"FriendlyName", &varName,0);
		CString str = varName.bstrVal;

		Log4c::logMsg(L"#AudioEncoder:%s",str.GetBuffer());
		if (str.Find(DEFAULT_AUDIO_CODEC,0)!= -1)
		{
			m_pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pCompressorAu);
			m_pMoniker->Release(); 
			break;
		}
		VariantClear(&varName);
	}

	pClassEnum->Release();
	m_pMoniker->Release();

	if (m_pCompressorAu == NULL)
		return false;
	else
		return true;
}


bool Cortez::isInCaptureProgress(void)
{
	return this->m_isInCaptureProgress;
}

bool Cortez::isCapturePaused(void)
{
	return this->m_isCapturePaused;
}

void Cortez::setAudioDisabled(bool audioDisabled)
{
	this->m_isAudioDisabled = audioDisabled;
}

void Cortez::getDeviceList(CArray<CString>& deviceList)
{
	//Make sure it's empty...
	deviceList.RemoveAll();
	for(DWORD dwIndex = 0; dwIndex < m_dwDevNum; dwIndex++)
		deviceList.Add(m_devList[dwIndex]);
}

void Cortez::getDeviceListAu(CArray<CString>& deviceList)
{
	deviceList.RemoveAll();
	for(DWORD dwIndex = 0; dwIndex < m_dwDevNumAu; dwIndex++)
		deviceList.Add(m_devListAu[dwIndex]);
}

void Cortez::setRascalEnabled(bool rascalEnabled)
{
	m_isRascalEnabled = rascalEnabled;
}

bool Cortez::initRascal()
{
	HRESULT hr = CoCreateInstance(CLSID_RASCAL, NULL, 
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pRascal);
	if (FAILED(hr))
    {
		Log4c::logMsg(L"#Cortez::initRascal() Failed");
		return false;
    }

	hr = CoCreateInstance(CLSID_RASCAL, NULL, 
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pRascalPreview);
	if (FAILED(hr))
    {
		Log4c::logMsg(L"#Cortez::initRascal()_Preview Failed");
		return false;
    }

	m_pGraph->AddFilter(m_pRascal, L"MyVCR_RASCAL");
	m_pGraph->AddFilter(m_pRascalPreview, L"MyVCR_RASCAL_PREVIEW");
	return true;
}