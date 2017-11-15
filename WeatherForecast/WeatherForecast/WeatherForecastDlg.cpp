
// WeatherForecastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WeatherForecast.h"
#include "WeatherForecastDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include "CvvImage/CvvImage.h"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include "CHttpClient.h"

//#include "string"
#include "fstream"

using namespace cv;

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//读取配置文件
	//string ProvinceConfigFilePath = "D:\\Microsoft Visual Studio 2010\\Project\\WeatherForecast\\Debug\\province.ini";
	string ProvinceConfigFilePath = "province.ini";
	ifstream infile(ProvinceConfigFilePath.c_str());
	//if (!infile) {
	//	return "无法找到模板："+filePath;
	//}
	string line, province, id;
	string pattern="\t";
	std::vector<std::string> result;
	//vector<vector<string >>m_VectorProvince;
	//map<string , string> vMapProvince;
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
			//vMapProvince.insert(map<string, string> :: value_type(province, id));
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
	//string CityConfigFilePath = "D:\\Microsoft Visual Studio 2010\\Project\\WeatherForecast\\Debug\\city.ini";
	string CityConfigFilePath = "city.ini";
	ifstream infile1(CityConfigFilePath.c_str());
	//if (!infile) {
	//	return "无法找到模板："+filePath;
	//}
	string city, cityid, provinceid;
	//vector<vector<string >>m_VectorProvince;
	//map<string , string> vMapProvince;
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

	getCityNum= 1340;
	m_cbProvince.SetCurSel(9);
	for (int i =0 ; i<m_VectorCity.size(); i++)
	{	
		if (m_VectorCity[i][2] == m_VectorProvince[9][0])
		{
			std::wstring stemp = s2ws( m_VectorCity[i][1] ); 
			LPCWSTR result = stemp.c_str();
			m_cbCity.InsertString(-1,result );	
		}
	}
	m_cbCity.SetCurSel(0);
		
	queryshowFun( );
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.





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
			queryshowFun( );
			flag =0;
		}

	}
	else
	{
		CDialogEx::OnPaint();
	}
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


	///*vector<string> p = ["13日星期一","晴","高温 12℃","低温 0℃"];*/
	//vector<string> p;
	//p.push_back("13日星期一");
	//p.push_back("晴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("14日星期二");
	//p.push_back("晴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("15日星期三");
	//p.push_back("多云");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("16日星期四");
	//p.push_back("晴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("17日星期五");
	//p.push_back("阴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("18日星期六");
	//p.push_back("晴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);
	//p.clear();
	//p.push_back("19日星期日");
	//p.push_back("晴");
	//p.push_back("高温 12℃");
	//p.push_back("低温 0℃");
	//date_weather.push_back(p);


	return date_weather;
}

void CWeatherForecastDlg::OnBnClickedButtonQuery()
{
	// TODO: Add your control notification handler code here

	//1、首先通过HTTP获取最新的天气预报信息 2、保存成xml格式 3、读取xml  4、展示xml
	 //获得城市名称
    //CEdit* pBoxOne;
    //pBoxOne = (CEdit*) GetDlgItem(IDC_EDIT_QUERY_CITY);
    //CString str;
    //pBoxOne-> GetWindowText(str);

	std::string URL = "http://wthrcdn.etouch.cn/WeatherApi?citykey=";
	CHttpClient httpClient;
	std::string Response;

	
	//char s[12];             //设定12位对于存储32位int值足够  
 //   itoa(getCityNum,s,10);            //itoa函数亦可以实现，但是属于C中函数，在C++中推荐用流的方法  
 //   string string_temp=s;  

	std::string urls=URL+m_VectorCity[getCityNum][0];
	httpClient.Gets(urls,Response );
	std::cout<<Response<<std::endl;
	//ofstream finalNameListOut("tmp.xml",ios::out);　　//写操作
	//finalNameListOut<<Response;

	//write_string_to_file_append( "tmp.xml",Response );  

	vector<vector<string> > date_weather;
	date_weather = ReadXML(Response );
	
	//当前天气
	SetDlgItemText(IDC_STATIC_NOWTEMP,CString( nowTemp.c_str()));
	//SetFont(&font);

	SetDlgItemText(IDC_STATIC_CURRENT_DATE,CString((date_weather[0][0]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_WEATHERTYPE,CString((date_weather[0][1]).c_str()));
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

	//SetDlgItemText(IDC_STATIC_DATE5,CString((date_weather[5][0]).c_str()));
	//SetDlgItemText(IDC_STATIC_WEATHERTYPE5,CString((date_weather[5][1]).c_str()));
	//SetDlgItemText(IDC_STATIC_HIGHTEMP5,CString((date_weather[5][2]).c_str()));
	//SetDlgItemText(IDC_STATIC_LOWTEMP5,CString((date_weather[5][3]).c_str()));

	//SetDlgItemText(IDC_STATIC_DATE6,CString((date_weather[6][0]).c_str()));
	//SetDlgItemText(IDC_STATIC_WEATHERTYPE6,CString((date_weather[6][1]).c_str()));
	//SetDlgItemText(IDC_STATIC_HIGHTEMP6,CString((date_weather[6][2]).c_str()));
	//SetDlgItemText(IDC_STATIC_LOWTEMP6,CString((date_weather[6][3]).c_str()));


	//画天气图
	//string str_srcImageFolderPath = "D:\\Microsoft Visual Studio 2010\\Project\\WeatherForecast\\Debug\\pic\\";
	string str_srcImageFolderPath = "pic\\";

	string str_srcImagePath = str_srcImageFolderPath + date_weather[0][1]+".jpg";
	Mat m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	Mat m_SrcMat=m_showFrame.clone();
	IplImage pImage = m_showFrame;
	CDC *pDC = GetDlgItem( IDC_STATIC_CURRENT_WEATHER )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	HDC hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	CRect rect;
	// 矩形类
	GetDlgItem( IDC_STATIC_CURRENT_WEATHER )->GetClientRect( &rect ); 
	CvvImage cimg;
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//1天
	str_srcImagePath = str_srcImageFolderPath + date_weather[1][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER1 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER1 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//2天
	str_srcImagePath = str_srcImageFolderPath + date_weather[2][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER2 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER2 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//3天
	str_srcImagePath = str_srcImageFolderPath + date_weather[3][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER3 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER3 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//4天
	str_srcImagePath = str_srcImageFolderPath + date_weather[4][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER4 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER4 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	////5天
	//str_srcImagePath = str_srcImageFolderPath + date_weather[5][1]+".jpg";
	//m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	//// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	//m_SrcMat=m_showFrame.clone();
	//pImage = m_showFrame;
	//pDC = GetDlgItem( IDC_STATIC_WEATHER5 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	//hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	//// 矩形类
	//GetDlgItem( IDC_STATIC_WEATHER5 )->GetClientRect( &rect ); 
	//cimg.CopyOf( &pImage, pImage.nChannels );
	//cimg.DrawToHDC( hdc, &rect );
	//ReleaseDC( pDC );

	////6天
	//str_srcImagePath = str_srcImageFolderPath + date_weather[6][1]+".jpg";
	//m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	//// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	//m_SrcMat=m_showFrame.clone();
	//pImage = m_showFrame;
	//pDC = GetDlgItem( IDC_STATIC_WEATHER6 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	//hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	//// 矩形类
	//GetDlgItem( IDC_STATIC_WEATHER6 )->GetClientRect( &rect ); 
	//cimg.CopyOf( &pImage, pImage.nChannels );
	//cimg.DrawToHDC( hdc, &rect );
	//ReleaseDC( pDC );

}


void CWeatherForecastDlg::OnCbnSelchangeComboProvince()
{
	// TODO: Add your control notification handler code here

	//nSelected=((CComboBox*)GetDlgItem(IDC_COMBO_PROVINCE))->GetCurSel();
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
	
}


void CWeatherForecastDlg::OnCbnSelchangeComboCity()
{
	// TODO: Add your control notification handler code here

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
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}



void CWeatherForecastDlg::queryshowFun( )
{
	// TODO: Add your control notification handler code here

	//1、首先通过HTTP获取最新的天气预报信息 2、保存成xml格式 3、读取xml  4、展示xml
	 //获得城市名称
	std::string URL = "http://wthrcdn.etouch.cn/WeatherApi?citykey=";
	CHttpClient httpClient;
	std::string Response;
	std::string urls=URL+m_VectorCity[getCityNum][0];
	
	httpClient.Gets(urls,Response );
	//std::cout<<Response<<std::endl;


	vector<vector<string> > date_weather;
	date_weather = ReadXML(Response );
	
	//当前天气
	SetDlgItemText(IDC_STATIC_NOWTEMP,CString( nowTemp.c_str()));
	//SetFont(&font);

	SetDlgItemText(IDC_STATIC_CURRENT_DATE,CString((date_weather[0][0]).c_str()));
	SetDlgItemText(IDC_STATIC_CURRENT_WEATHERTYPE,CString((date_weather[0][1]).c_str()));
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
	//string str_srcImageFolderPath = "D:\\Microsoft Visual Studio 2010\\Project\\WeatherForecast\\Debug\\pic\\";
	string str_srcImageFolderPath = ".\\pic\\";

	string str_srcImagePath = str_srcImageFolderPath + date_weather[0][1]+".jpg";
	Mat m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	Mat m_SrcMat=m_showFrame.clone();
	IplImage pImage = m_showFrame;
	CDC *pDC = GetDlgItem( IDC_STATIC_CURRENT_WEATHER )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	HDC hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	CRect rect;
	// 矩形类
	GetDlgItem( IDC_STATIC_CURRENT_WEATHER )->GetClientRect( &rect ); 
	CvvImage cimg;
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//1天
	str_srcImagePath = str_srcImageFolderPath + date_weather[1][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER1 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER1 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );
	Invalidate();

	//2天
	str_srcImagePath = str_srcImageFolderPath + date_weather[2][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER2 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER2 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//3天
	str_srcImagePath = str_srcImageFolderPath + date_weather[3][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER3 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER3 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	//4天
	str_srcImagePath = str_srcImageFolderPath + date_weather[4][1]+".jpg";
	m_showFrame = imread(str_srcImagePath, CV_LOAD_IMAGE_UNCHANGED );	
	// m_showFrame = imread(strPathName.GetBuffer(0), CV_LOAD_IMAGE_UNCHANGED );
	m_SrcMat=m_showFrame.clone();
	pImage = m_showFrame;
	pDC = GetDlgItem( IDC_STATIC_WEATHER4 )->GetDC();//根据ID获得窗口指针再获取与该窗口关联的上下文指针
	hdc = pDC->GetSafeHdc();                      // 获取设备上下文句柄
	// 矩形类
	GetDlgItem( IDC_STATIC_WEATHER4 )->GetClientRect( &rect ); 
	cimg.CopyOf( &pImage, pImage.nChannels );
	cimg.DrawToHDC( hdc, &rect );
	ReleaseDC( pDC );

	Invalidate();
}