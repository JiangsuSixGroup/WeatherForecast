
// WeatherForecastDlg.cpp : implementation file
//
#include "stdafx.h"
#include "WeatherForecast.h"
#include "WeatherForecastDlg.h"
#include "afxdialogex.h"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "CHttpClient.h"
#include "fstream"
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWeatherForecastDlg dialog

int flag=0;


CWeatherForecastDlg::CWeatherForecastDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWeatherForecastDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//VERIFY(font.CreateFont(
	//   12,                        // nHeight
	//   0,                         // nWidth
	//   0,                         // nEscapement
	//   0,                         // nOrientation
	//   FW_SEMIBOLD,                 // nWeight
	//   FALSE,                     // bItalic
	//   FALSE,                     // bUnderline
	//   0,                         // cStrikeOut
	//   GB2312_CHARSET,              // nCharSet
	//   OUT_DEFAULT_PRECIS,        // nOutPrecision
	//   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	//   DEFAULT_QUALITY,           // nQuality
	//   DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
	//   L"宋体"));                 // lpszFacename


}

void CWeatherForecastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PROVINCE, m_cbProvince);
	DDX_Control(pDX, IDC_COMBO_CITY, m_cbCity);
}

BEGIN_MESSAGE_MAP(CWeatherForecastDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CWeatherForecastDlg::OnBnClickedButtonQuery)
	ON_CBN_SELCHANGE(IDC_COMBO_PROVINCE, &CWeatherForecastDlg::OnCbnSelchangeComboProvince)
	ON_CBN_SELCHANGE(IDC_COMBO_CITY, &CWeatherForecastDlg::OnCbnSelchangeComboCity)
	ON_BN_CLICKED(IDCANCEL, &CWeatherForecastDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//字符串分割函数
std::vector<std::string> stringSplit(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;//扩展字符串以方便操作
	int size=str.size();

	for(int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

std::wstring s2ws(const std::string& s){

 int len;

 int slength = (int)s.length() + 1;

 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);

 wchar_t* buf = new wchar_t[len];

 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);

 std::wstring r(buf);

 delete[] buf;

 return r.c_str();

}

std::string& trim(std::string &s)   
{  
    if (s.empty())   
    {  
        return s;  
    }  
  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}  

// CWeatherForecastDlg message handlers

BOOL CWeatherForecastDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	font.CreatePointFont(400,L"微软雅黑");

	string ProvinceConfigFilePath = "province.ini";
	ifstream infile(ProvinceConfigFilePath.c_str());

	string line, province, id;
	string pattern="\t";
	std::vector<std::string> result;

	while (getline(infile, line)) 
	{
		if (line.substr(0,1)=="#" || line=="")
		{
			continue;
		}
		result=stringSplit(line,pattern);
		if (2==result.size())
		{
			id=result[0];			
			province=result[1];
			trim(id);
			trim(province);
			vector<string> vTmp;
			vTmp.push_back(id);
			vTmp.push_back(province);
			m_VectorProvince.push_back(vTmp);
		}
		else
		{
			continue;
		}
	}
	for (int i=0; i<m_VectorProvince.size(); i++)
	{
		std::wstring stemp = s2ws( m_VectorProvince[i][1] ); 
		LPCWSTR result = stemp.c_str();
		m_cbProvince.InsertString(-1,result );	
	}
	//读取city配置文件
	string CityConfigFilePath = "city.ini";
	ifstream infile1(CityConfigFilePath.c_str());
	string city, cityid, provinceid;

	while (getline(infile1, line)) 
	{
		if (line.substr(0,1)=="#" || line=="")
		{
			continue;
		}
		result=stringSplit(line,pattern);
		if (3==result.size())
		{
			cityid=result[0];
			city=result[1];
			provinceid=result[2];
			trim(cityid);
			trim(city);
			trim(provinceid);
			vector<string> vTmp;
			vTmp.push_back(cityid);
			vTmp.push_back(city);
			vTmp.push_back(provinceid);
			m_VectorCity.push_back(vTmp);
			vTmp.clear();
		}
		else
		{
			continue;
		}
	}

	nProvinceSelected = 9;
	nCitySelected = 0;


	getCityNum= 173;//1340;
	m_cbProvince.SetCurSel(nProvinceSelected);
	for (int i =0 ; i<m_VectorCity.size(); i++)
	{	
		if (m_VectorCity[i][2] == m_VectorProvince[nProvinceSelected][nCitySelected])
		{
			std::wstring stemp = s2ws( m_VectorCity[i][1] ); 
			LPCWSTR result = stemp.c_str();
			m_cbCity.InsertString(-1,result );	
		}
	}
	m_cbCity.SetCurSel(0);
	
	flag =1;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWeatherForecastDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CWeatherForecastDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
	

		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		if (flag ==1)
		{
			flag =0;
		}

	}
	else
	{
		//修改窗体背景颜色
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);

		//设置为白色背景
		dc.FillSolidRect(rect,RGB(255,255,255)); 
		CDialogEx::OnPaint();
	}

	queryshowFun( );
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWeatherForecastDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void UTF8ToGBK( char *&szOut )
{
 unsigned short *wszGBK;
 char *szGBK;
 //长度
 int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)szOut, -1, NULL, 0);
 wszGBK = new unsigned short[len+1];
 memset(wszGBK, 0, len * 2 + 2);
 MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)szOut, -1, (LPWSTR)wszGBK, len);
 //长度
 len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
 szGBK = new char[len+1];
 memset(szGBK, 0, len + 1);
 WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
 //szOut = szGBK; //这样得到的szOut不正确，因为此句意义是将szGBK的首地址赋给szOut，当delete []szGBK执行后szGBK的内
                             //存空间将被释放，此时将得不到szOut的内容
 memset(szOut,'/0',strlen(szGBK)+1); //改将szGBK的内容赋给szOut ，这样即使szGBK被释放也能得到正确的值
 memcpy(szOut,szGBK,strlen(szGBK));

 delete []szGBK;
 delete []wszGBK;
}
void GBKToUTF8(char* &szOut)
{
 char* strGBK = szOut;
 int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK, -1, NULL,0);
 unsigned short * wszUtf8 = new unsigned short[len+1];
 memset(wszUtf8, 0, len * 2 + 2);
 MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK, -1, (LPWSTR)wszUtf8, len);
 len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
 char *szUtf8=new char[len + 1];
 memset(szUtf8, 0, len + 1);
 WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
 //szOut = szUtf8;
 memset(szOut,'/0',strlen(szUtf8)+1);
 memcpy(szOut,szUtf8,strlen(szUtf8));
 delete[] szUtf8;
 delete[] wszUtf8;
}
string UTF8ToGBK(const std::string& strUTF8)  
{  
	if (&strUTF8 == NULL)
	{
		return ""; 
	}

	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);  
	unsigned short * wszGBK = new unsigned short[len + 1];  
	memset(wszGBK, 0, len * 2 + 2);  
	MultiByteToWideChar(CP_UTF8, 0,LPCSTR(strUTF8.c_str()), -1, LPWSTR(wszGBK), len);  

	len = WideCharToMultiByte(CP_ACP, 0,LPCWSTR(wszGBK), -1, NULL, 0, NULL, NULL);  
	char *szGBK = new char[len + 1];  
	memset(szGBK, 0, len + 1);  
	WideCharToMultiByte(CP_ACP,0, LPCWSTR(wszGBK), -1, szGBK, len, NULL, NULL);  
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);  
	delete[]szGBK;  
	delete[]wszGBK;  
	return strTemp;  
}  
string GBKToUTF8(const std::string& strGBK)  
{  

	if (&strGBK == NULL)
	{
		return ""; 
	}
	string strOutUTF8 = "";  
	WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];  
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);  
	strOutUTF8 = str2;  
	delete[]str1;  
	str1 = NULL;  
	delete[]str2;  
	str2 = NULL;  
	return strOutUTF8;  
}  

int write_string_to_file_append(const std::string & file_string, const std::string str )  
{  
    std::ofstream   OsWrite(file_string,std::ofstream::app);  
    OsWrite<<str;  
    OsWrite<<std::endl;  
    OsWrite.close();  
   return 0;  
}  
  

vector<vector<string> > CWeatherForecastDlg::ReadXML(string Response )
{
	TiXmlDocument* myDocument = new TiXmlDocument();
	myDocument->Parse(Response.c_str());

	//string xmlPath="D:\\Microsoft Visual Studio 2010\\Project\\WeatherForecast\\20171113.xml";
	//TiXmlDocument * myDocument = new TiXmlDocument(xmlPath.c_str());
	myDocument->LoadFile(TIXML_ENCODING_UTF8); 
	TiXmlElement* root = myDocument->RootElement();                             //XML的根节点  
	TiXmlNode* currentNode;
	currentNode = root->FirstChild("wendu");
	const char* szX  = currentNode->ToElement()->GetText();
	nowTemp = szX;
	nowTemp = nowTemp +"℃";

	//读取指数信息
	zhishuInfo.clear();
	currentNode = root->FirstChild("zhishus");
	currentNode = currentNode->FirstChild("zhishu");
	while (currentNode)                                                 //currentNode节点循环  
	{
		TiXmlNode* tmpNode;
		tmpNode = currentNode->FirstChild("name");
		const char* tmpname  = tmpNode->ToElement()->GetText();
		string name= tmpname;
		name =UTF8ToGBK(name);

		tmpNode = currentNode->FirstChild("value");
		const char* tmpvalue  = tmpNode->ToElement()->GetText();
		string value= tmpvalue;
		value =UTF8ToGBK(value);
		vector<string> tmp;
		tmp.push_back(name);
		tmp.push_back(value);
		zhishuInfo.push_back(tmp);
		currentNode=currentNode->NextSibling("zhishu");
	}

	////读取environment信息
	//environmentInfo.clear();
	//TiXmlNode* tmpCurrentNode ;

	//currentNode = root->FirstChild("environment");
	//tmpCurrentNode = currentNode->FirstChild("aqi");	
	//const char* aqiTemp  = tmpCurrentNode->ToElement()->GetText();
	//string aqi= aqiTemp;
	//aqi =UTF8ToGBK(aqi);
	//environmentInfo.push_back(aqi );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("pm25");
	//const char* pmTemp  = currentNode->ToElement()->GetText();
	//string pm= pmTemp;
	//pm =UTF8ToGBK(pm);
	//environmentInfo.push_back(pm );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("suggest");
	//const char* suggestTemp  = currentNode->ToElement()->GetText();
	//string suggest= suggestTemp;
	//suggest =UTF8ToGBK(suggest);
	//environmentInfo.push_back(suggest );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("quality");
	//const char* qualityTemp  = currentNode->ToElement()->GetText();
	//string quality= qualityTemp;
	//quality =UTF8ToGBK(quality);
	//environmentInfo.push_back(quality );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("MajorPollutants");
	//const char* MajorPollutantsTemp  = currentNode->ToElement()->GetText();
	//string MajorPollutants= MajorPollutantsTemp;
	//MajorPollutants =UTF8ToGBK(MajorPollutants);
	//environmentInfo.push_back(MajorPollutants );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("o3");
	//const char* o3Temp  = currentNode->ToElement()->GetText();
	//string o3= o3Temp;
	//o3 =UTF8ToGBK(o3);
	//environmentInfo.push_back(o3 );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("co");
	//const char* coTemp  = currentNode->ToElement()->GetText();
	//string co= coTemp;
	//co =UTF8ToGBK(co);
	//environmentInfo.push_back(co );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("pm10");
	//const char* pm10Temp  = currentNode->ToElement()->GetText();
	//string pm10= pm10Temp;
	//pm10 =UTF8ToGBK(pm10);
	//environmentInfo.push_back(pm10 );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("so2");
	//const char* so2Temp  = currentNode->ToElement()->GetText();
	//string so2= so2Temp;
	//so2 =UTF8ToGBK(so2);
	//environmentInfo.push_back(so2 );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("no2");
	//const char* no2Temp  = currentNode->ToElement()->GetText();
	//string no2= no2Temp;
	//no2 =UTF8ToGBK(no2);
	//environmentInfo.push_back(no2 );

	//currentNode = root->FirstChild("environment");
	//currentNode = currentNode->FirstChild("time");
	//const char* timeTemp  = currentNode->ToElement()->GetText();
	//string time= timeTemp;
	//time =UTF8ToGBK(time);
	//environmentInfo.push_back(time );
	
	vector<vector<string> >  date_weather;
	currentNode = root->FirstChild("forecast");
	currentNode = currentNode->FirstChild("weather");
	while (currentNode)                                                 //currentNode节点循环  
	{
		TiXmlNode* tmpNode;
		tmpNode = currentNode->FirstChild("date");
		const char* tmpdate  = tmpNode->ToElement()->GetText();
		string date= tmpdate;
		date =UTF8ToGBK(date);

		tmpNode = currentNode->FirstChild("high");
		const char* tmphighTemp  = tmpNode->ToElement()->GetText();
		string highTemp= tmphighTemp;
		highTemp =UTF8ToGBK(highTemp);

		tmpNode = currentNode->FirstChild("low");
		const char* tmplowTemp  = tmpNode->ToElement()->GetText();
		string lowTemp= tmplowTemp;
		lowTemp =UTF8ToGBK(lowTemp);
		
		tmpNode = currentNode->FirstChild("day");
		tmpNode = tmpNode->FirstChild("type");
		const char* tmpDayType  = tmpNode->ToElement()->GetText();
		string dayType= tmpDayType;
		dayType =UTF8ToGBK(dayType);

		vector<string> p;
		p.push_back(date);
		p.push_back(dayType);
		p.push_back(highTemp);
		p.push_back(lowTemp);
		date_weather.push_back(p);
		p.clear();
		currentNode=currentNode->NextSibling("weather");

	}  

	return date_weather;
}

void CWeatherForecastDlg::OnBnClickedButtonQuery()
{
	queryshowFun( );
}

//************************************
// 方法说明:    显示JPG和GIF、BMP图片
// 参数说明:    CDC * pDC           设备环境对象
// 参数说明:    CString strPath     要显示的图片路径 
// 参数说明:    int x               要显示的X位置
// 参数说明:    int y               要显示的Y位置
// 返回值:      BOOL                成功返回TRUE,否则返回FALSE
//************************************
BOOL CWeatherForecastDlg::ShowJpgGif(CDC* pDC,CString strPath, int x, int y)

{
    CFileStatus fstatus;  
    CFile file;  
    ULONGLONG cb;  

    // 打开文件并检测文件的有效性
     if (!file.Open(strPath,CFile::modeRead))
     {
         return FALSE;
     }
     if (!file.GetStatus(strPath,fstatus))
     {
         return FALSE;
     }
     if ((cb =fstatus.m_size)<=0)
     {
         return FALSE;
     }

     // 根据文件大小分配内存空间,记得释放内存
     HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (unsigned int)cb);  
      if (hGlobal== NULL) 
      {
          return FALSE;
      }

      // 锁定刚才分配的内存空间
      LPVOID pvData = NULL;  
      pvData = GlobalLock(hGlobal);
      if (pvData == NULL)  
      {  
            GlobalFree(hGlobal);  // 记得释放内存
            return FALSE;
      } 

      // 将文件放到流中
      IStream *pStm;  
      file.Read(pvData,(unsigned int)cb);  
      GlobalUnlock(hGlobal);  
      CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);  
 
    // 从流中加载图片
    IPicture *pPic; 
    if(OleLoadPicture(pStm,(LONG)fstatus.m_size,TRUE,IID_IPicture,(LPVOID*)&pPic)!=S_OK) 
    { 
        GlobalFree(hGlobal);  // 记得释放内存
        return FALSE;
    }

    //获取图像宽和高
    OLE_XSIZE_HIMETRIC hmWidth;  
    OLE_YSIZE_HIMETRIC hmHeight;  
    pPic->get_Width(&hmWidth);  
    pPic->get_Height(&hmHeight);  

    //use render function display image
    RECT rtWnd;
    pDC->GetWindow()->GetWindowRect(&rtWnd);
    int iWndWidth=rtWnd.right-rtWnd.left;
    int iWndHeight=rtWnd.bottom-rtWnd.top;
    
    if(FAILED(pPic->Render(*pDC,x,y,iWndWidth,iWndHeight,0,hmHeight,hmWidth,-hmHeight,NULL)))  
    {
        pPic->Release();
        GlobalFree(hGlobal);  // 记得释放内存
        return false;
    }

    pPic->Release(); 
    GlobalFree(hGlobal);  // 记得释放内存
    return true;
}

void CWeatherForecastDlg::OnCbnSelchangeComboProvince()
{
	nProvinceSelected=m_cbProvince.GetCurSel();
	
	m_cbCity.ResetContent();

	string a = m_VectorProvince[nProvinceSelected][0];
	for (int i =0 ; i<m_VectorCity.size(); i++)
	{	
		if (m_VectorCity[i][2] == m_VectorProvince[nProvinceSelected][0])
		{
			std::wstring stemp = s2ws( m_VectorCity[i][1] ); 
			LPCWSTR result = stemp.c_str();
			m_cbCity.InsertString(-1,result );	
		}
	}


	m_cbCity.SetCurSel(0);
	CString str; 
	m_cbCity.GetWindowText(str);

	string provinceCode = m_VectorProvince[nProvinceSelected][0];
	int provinceStartNum = -1;
	for (int i =0 ; i<m_VectorCity.size(); i++)
	{	
		if (m_VectorCity[i][2] == provinceCode )
		{
			provinceStartNum = i;
			break;
		}
	}
	getCityNum = provinceStartNum +nCitySelected;

}


void CWeatherForecastDlg::OnCbnSelchangeComboCity()
{

	nCitySelected=m_cbCity.GetCurSel();
	//获取用户选择城市代码
	CString str; 
	m_cbCity.GetWindowText(str);

	string provinceCode = m_VectorProvince[nProvinceSelected][0];
	int provinceStartNum = -1;
	for (int i =0 ; i<m_VectorCity.size(); i++)
	{	
		if (m_VectorCity[i][2] == provinceCode )
		{
			provinceStartNum = i;
			break;
		}
	}
	getCityNum = provinceStartNum +nCitySelected;

}



void CWeatherForecastDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}



void CWeatherForecastDlg::queryshowFun( )
{

	//1、首先通过HTTP获取最新的天气预报信息 2、保存成xml格式 3、读取xml  4、展示xml
	 //获得城市名称
	std::string URL = "http://wthrcdn.etouch.cn/WeatherApi?citykey=";
	CHttpClient httpClient;
	std::string Response;
	std::string urls=URL+m_VectorCity[getCityNum][0];
	
	httpClient.Gets(urls,Response );


	vector<vector<string> > date_weather;
	date_weather = ReadXML(Response );
	
	//当前位置
	SetDlgItemText(IDC_STATIC_ADDRESS,CString( m_VectorCity[getCityNum][1].c_str()));
	
	//当天指数
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME0,CString( zhishuInfo[0][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME1,CString(  zhishuInfo[1][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME2,CString( zhishuInfo[2][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME3,CString(  zhishuInfo[3][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME4,CString(  zhishuInfo[4][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME5,CString(  zhishuInfo[5][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME6,CString( zhishuInfo[6][0].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_NAME7,CString(  zhishuInfo[7][0].c_str()));

	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE0,CString(  zhishuInfo[0][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE1,CString(  zhishuInfo[1][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE2,CString( zhishuInfo[2][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE3,CString(  zhishuInfo[3][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE4,CString(  zhishuInfo[4][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE5,CString(  zhishuInfo[5][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE6,CString( zhishuInfo[6][1].c_str()));
	SetDlgItemText(IDC_STATIC_ZHISHU_VALUE7,CString(  zhishuInfo[7][1].c_str()));



	
	////当天环境
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV1,CString( environmentInfo[0].c_str()));
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV2,CString( environmentInfo[1].c_str()));
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV3,CString( environmentInfo[2].c_str()));
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV4,CString( environmentInfo[3].c_str()));
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV5,CString( environmentInfo[4].c_str()));
	//SetDlgItemText(IDC_STATIC_CURRENT_ENV6,CString( environmentInfo[5].c_str()));



	//当前天气
	CWnd* pWnd = GetDlgItem(IDC_STATIC_NOWTEMP);
	pWnd->SetFont(&font);
	

	SetDlgItemText(IDC_STATIC_NOWTEMP,CString( nowTemp.c_str()));

	SetDlgItemText(IDC_STATIC_CURRENT_DATE,CString((date_weather[0][0]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_WEATHERTYPE,CString((date_weather[0][1]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_WEATHERTYPE2,CString((date_weather[0][1]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_HIGHTEMP,CString((date_weather[0][2]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_LOWTEMP,CString((date_weather[0][3]).c_str()));

	SetDlgItemText(IDC_STATIC_DATE1,CString((date_weather[1][0]).c_str()));
	SetDlgItemText(IDC_STATIC_WEATHERTYPE1,CString((date_weather[1][1]).c_str()));
	SetDlgItemText(IDC_STATIC_HIGHTEMP1,CString((date_weather[1][2]).c_str()));
	SetDlgItemText(IDC_STATIC_LOWTEMP1,CString((date_weather[1][3]).c_str()));

	SetDlgItemText(IDC_STATIC_DATE2,CString((date_weather[2][0]).c_str()));
	SetDlgItemText(IDC_STATIC_WEATHERTYPE2,CString((date_weather[2][1]).c_str()));
	SetDlgItemText(IDC_STATIC_HIGHTEMP2,CString((date_weather[2][2]).c_str()));
	SetDlgItemText(IDC_STATIC_LOWTEMP2,CString((date_weather[2][3]).c_str()));

	SetDlgItemText(IDC_STATIC_DATE3,CString((date_weather[3][0]).c_str()));
	SetDlgItemText(IDC_STATIC_WEATHERTYPE3,CString((date_weather[3][1]).c_str()));
	SetDlgItemText(IDC_STATIC_HIGHTEMP3,CString((date_weather[3][2]).c_str()));
	SetDlgItemText(IDC_STATIC_LOWTEMP3,CString((date_weather[3][3]).c_str()));

	SetDlgItemText(IDC_STATIC_DATE4,CString((date_weather[4][0]).c_str()));
	SetDlgItemText(IDC_STATIC_WEATHERTYPE4,CString((date_weather[4][1]).c_str()));
	SetDlgItemText(IDC_STATIC_HIGHTEMP4,CString((date_weather[4][2]).c_str()));
	SetDlgItemText(IDC_STATIC_LOWTEMP4,CString((date_weather[4][3]).c_str()));

	//画天气图
	string str_srcImageFolderPath = ".\\pic\\";

	string str_srcImagePath = str_srcImageFolderPath + date_weather[0][1]+".jpg";
	CString cstr(str_srcImagePath.c_str());
	CDC* pDC = GetDlgItem( IDC_STATIC_CURRENT_WEATHER )->GetDC();
	ShowJpgGif(pDC,cstr,0,0);
	ReleaseDC( pDC );

	str_srcImagePath = str_srcImageFolderPath + date_weather[0][1]+".jpg";
	CString cstr0(str_srcImagePath.c_str());
	pDC = GetDlgItem( IDC_STATIC_CURRENT_WEATHER2 )->GetDC();
	ShowJpgGif(pDC,cstr0,0,0);
	ReleaseDC( pDC );

	//1天
	str_srcImagePath = str_srcImageFolderPath + date_weather[1][1]+".jpg";
	CString cstr1(str_srcImagePath.c_str());
	pDC = GetDlgItem( IDC_STATIC_WEATHER1 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	ShowJpgGif(pDC,cstr1,0,0);
	ReleaseDC( pDC );

	//2天
	str_srcImagePath = str_srcImageFolderPath + date_weather[2][1]+".jpg";
	CString cstr2(str_srcImagePath.c_str());
	pDC = GetDlgItem( IDC_STATIC_WEATHER2 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	ShowJpgGif(pDC,cstr2,0,0);
	ReleaseDC( pDC );

	//3天
	str_srcImagePath = str_srcImageFolderPath + date_weather[3][1]+".jpg";
	CString cstr3(str_srcImagePath.c_str());
	pDC = GetDlgItem( IDC_STATIC_WEATHER3 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	ShowJpgGif(pDC,cstr3,0,0);
	ReleaseDC( pDC );

	//4天
	str_srcImagePath = str_srcImageFolderPath + date_weather[4][1]+".jpg";
	CString cstr4(str_srcImagePath.c_str());
	pDC = GetDlgItem( IDC_STATIC_WEATHER4 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	ShowJpgGif(pDC,cstr4,0,0);
	ReleaseDC( pDC );
}


HBRUSH CWeatherForecastDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd-> GetDlgCtrlID()==IDC_STATIC||nCtlColor==CTLCOLOR_STATIC||nCtlColor==CTLCOLOR_BTN)
	{
		//pDC-> SetTextColor(RGB(255,0,0));   //设置字体颜色
		pDC-> SetBkMode(TRANSPARENT); //设置字体背景为透明
		// TODO: Return a different brush if the default is not desired
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);   // 设置背景色
	}
	else
	{
		return hbr;
	}
}
