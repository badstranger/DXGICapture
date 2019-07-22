#include "DXGICapture.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
BOOL CDXGICapture::s_isAttach = FALSE;
CDXGICapture::CDXGICapture(int maxWidth, int maxHeight):
m_isInitSuccess(FALSE),
m_pImageData(NULL)
{
	m_pImageData = new unsigned char[maxWidth * maxHeight * 4];
	Init();
}
CDXGICapture::~CDXGICapture()
{

}

BOOL CDXGICapture::Init()
{
	do
	{
		HRESULT hr = S_OK;
		if (m_isInitSuccess)
		{
			//�ѳ�ʼ��
			break;
		}
		//Driver types supported ֧�ֵ��豸����
		//����˵�� https://docs.microsoft.com/zh-cn/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,	//Ӳ��������Ӳ��֧������Direct3D����
			D3D_DRIVER_TYPE_WARP,		//WARP����������һ�������������դ����������դ֧��9_1��10.1�Ĺ��ܼ���
			D3D_DRIVER_TYPE_REFERENCE	//�ο�����������֧��ÿ��Direct3D���ܵ����ʵ��
		};
		unsigned int  numDriverTypes = ARRAYSIZE(driverTypes);
		//����Direct3D�豸����ԵĹ��ܼ� https://docs.microsoft.com/zh-cn/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,		//Direct3D 11.0֧�ֵ�Ŀ�깦�ܣ�������ɫ��ģ��5
			D3D_FEATURE_LEVEL_10_1,		//Direct3D 10.1֧�ֵ�Ŀ�깦�ܣ�������ɫ��ģ��4
			D3D_FEATURE_LEVEL_10_0,		//Direct3D 10.0֧�ֵ�Ŀ�깦�ܣ�������ɫ��ģ��4
			D3D_FEATURE_LEVEL_9_1		//Ŀ�깦��[���ܼ���]��/ windows / desktop / direct3d11 / overviews-direct3d-11-devices-downlevel-intro��9.1֧�֣�������ɫ��ģ��2
		};
		unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);
		D3D_FEATURE_LEVEL featureLevel;
		//1.Create D3D device ����D3D�豸
		/*ע��   
		���������ϴ���Direct3D 11.1����ʱ��pFeatureLevels����ΪNULL��
		��˺������ᴴ��D3D_FEATURE_LEVEL_11_1�豸��Ҫ����D3D_FEATURE_LEVEL_11_1�豸��
		������ʽ�ṩ����D3D_FEATURE_LEVEL_11_1��D3D_FEATURE_LEVEL���顣�����δ��װ
		Direct3D 11.1����ʱ�ļ�������ṩ����D3D_FEATURE_LEVEL_11_1��D3D_FEATURE_LEVEL���飬
		��˺���������ʧ�ܲ���ʾE_INVALIDARG*/
		for (int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
		{
			hr = D3D11CreateDevice(NULL, driverTypes[driverTypeIndex], NULL, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_deviceContext);
			if (SUCCEEDED(hr))
			{
				break;
			}
		}
		if (FAILED(hr))
		{
			break;
		}
		//2.����DXGI�豸
		IDXGIDevice* pDxgiDevice = NULL;
		hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
		if (FAILED(hr))
		{
			break;
		}
		//3.��ȡDXGI adapter
		IDXGIAdapter* pDxgiAdapter = NULL;
		hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
		RELEASE_OBJECT(pDxgiDevice);
		if (FAILED(hr))
		{
			break;
		}
		//4.��ȡIDXGIOutput
		int nOutput = 0;
		IDXGIOutput* pDxgiOutput = NULL;
		//ö��������(��Ƶ��)���
		hr = pDxgiAdapter->EnumOutputs(nOutput, &pDxgiOutput);
		RELEASE_OBJECT(pDxgiAdapter);
		//5.��ȡDXGI_OUTPUT_DESC ����
		pDxgiOutput->GetDesc(&m_dxgiOutputDesc);
		if (FAILED(hr))
		{
			break;
		}
		//6.��ȡIDXGIOutput1
		IDXGIOutput1* pDxgiOutput1 = NULL;
		hr = pDxgiOutput->QueryInterface(_uuidof(pDxgiOutput1), reinterpret_cast<void**>(&pDxgiOutput1));
		RELEASE_OBJECT(pDxgiOutput);
		if (FAILED(hr))
		{
			break;
		}
		//7.������������
		hr = pDxgiOutput1->DuplicateOutput(m_pDevice, &m_pDeskDuplication);
		RELEASE_OBJECT(pDxgiOutput1);
		if (FAILED(hr))
		{
			break;
		}
		//��ʼ���ɹ�
		m_isInitSuccess = TRUE;
	} while (0);
	return m_isInitSuccess;

}

void CDXGICapture::DelInit()
{
	if (!m_isInitSuccess)
	{
		return;
	}
	m_isInitSuccess = FALSE;
	RELEASE_OBJECT(m_pDeskDuplication);
	RELEASE_OBJECT(m_pDevice);
	RELEASE_OBJECT(m_deviceContext)
}	
BOOL CDXGICapture::AttachToThread()
{
	if (s_isAttach)
	{
		return TRUE;
	}
	HDESK hCurrentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (!hCurrentDesktop)
	{
		return FALSE;
	}
	//Attach desktop to this thread
	BOOL isDesktopAttached = SetThreadDesktop(hCurrentDesktop);
	CloseDesktop(hCurrentDesktop);
	hCurrentDesktop = NULL;
	s_isAttach = TRUE;
	return isDesktopAttached;
}
BOOL CDXGICapture::QueryFrame(void** pImageData, int* nImageSize)
{

	*nImageSize = 0;
	if (!m_isInitSuccess || !AttachToThread())
	{
		return FALSE;
	}

	IDXGIResource* pDesktopResource = NULL;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	HRESULT hr = m_pDeskDuplication->AcquireNextFrame(0, &frameInfo, &pDesktopResource);
	if (FAILED(hr))
	{
		//��һЩwin10��ϵͳ�ϣ��������û�з����仯�����
		return TRUE;
	}
	//Query next frame staging buffer
	ID3D11Texture2D* pAcquiredDesktopImage = NULL;
	hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pAcquiredDesktopImage));
	RELEASE_OBJECT(pDesktopResource);
	if (FAILED(hr))
	{
		return FALSE;
	}
	//copy old description
	D3D11_TEXTURE2D_DESC frameDescriptor;
	pAcquiredDesktopImage->GetDesc(&frameDescriptor);

	//create a new stging buffer for fill frame image
	ID3D11Texture2D* pNewDesktopImage = NULL;
	frameDescriptor.Usage = D3D11_USAGE_STAGING;
	frameDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	frameDescriptor.BindFlags = 0;
	frameDescriptor.MiscFlags = 0;
	frameDescriptor.MipLevels = 1;
	frameDescriptor.ArraySize = 1;
	frameDescriptor.SampleDesc.Count = 1;
	hr = m_pDevice->CreateTexture2D(&frameDescriptor, NULL, &pNewDesktopImage);
	if (FAILED(hr))
	{
		RELEASE_OBJECT(pAcquiredDesktopImage);
		m_pDeskDuplication->ReleaseFrame();
		return FALSE;
	}

	//copy next staging buffer to new staging buffer
	m_deviceContext->CopyResource(pNewDesktopImage, pAcquiredDesktopImage);
	RELEASE_OBJECT(pAcquiredDesktopImage);
	m_pDeskDuplication->ReleaseFrame();

	//create staging buffer for map bits
	IDXGISurface* pStagingSurf = NULL;
	hr = pNewDesktopImage->QueryInterface(__uuidof(IDXGISurface), (void**)(&pStagingSurf));
	RELEASE_OBJECT(pNewDesktopImage);
	if (FAILED(hr))
	{
		return FALSE;
	}
	//copy bits to user space 
	DXGI_MAPPED_RECT mappedRect;
	hr = pStagingSurf->Map(&mappedRect, DXGI_MAP_READ);
	if (SUCCEEDED(hr))
	{
		*nImageSize = m_dxgiOutputDesc.DesktopCoordinates.right * m_dxgiOutputDesc.DesktopCoordinates.bottom * 4;
		memcpy((BYTE*)m_pImageData, mappedRect.pBits, *nImageSize);
		*pImageData = m_pImageData;
		pStagingSurf->Unmap();
	}
	RELEASE_OBJECT(pStagingSurf);
	return SUCCEEDED(hr);
}

BOOL CDXGICapture::CaptureImage(void** pImageData, int* nLen)
{
	return QueryFrame(pImageData, nLen);
}