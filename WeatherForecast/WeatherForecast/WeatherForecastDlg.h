
// WeatherForecastDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "string"
#include <vector>
using namespace std;


// CWeatherForecastDlg dialog
class CWeatherForecastDlg : public CDialogEx
{
// Construction
public:
	CWeatherForecastDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WEATHERFORECAST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonQuery();
	CComboBox m_cbProvince;
	CComboBox m_cbCity;
	afx_msg void OnCbnSelchangeComboProvince();
	BOOL ShowJpgGif(CDC* pDC,CString strPath, int x, int y);

	UINT nProvinceSelected;
	UINT nCitySelected;

	vector<vector<string> > ReadXML(string xmladdr );
	vector <vector<string >> m_VectorProvince;
	vector <vector<string >> m_VectorCity;
	//vector <string > environmentInfo;
	vector <vector <string > > zhishuInfo;
	int getCityNum;
	string nowTemp;
	void queryshowFun();
	CFont  font;
	
	afx_msg void OnCbnSelchangeComboCity();
	afx_msg void OnEnChangeEditQueryCity();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnStnClickedStaticWeather1();
	afx_msg void OnStnClickedStaticWeather2();
	afx_msg void OnStnClickedStaticCurrentWeather();
	afx_msg void OnStnClickedStaticWeather3();
	afx_msg void OnStnClickedStaticWeather4();
};
