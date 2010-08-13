#include "stdafx.h"
#include "OptionBasicPage_Win.h"
#include "../../Utils/Strings.h"
#include "../../mplayerc.h"
#include "../../svplib/svplib.h"

BOOL OptionBasicPage::OnInitDialog(HWND hwnd, LPARAM lParam)
{
  // background image picker
  m_userbkgnd_edit.SubclassWindow(GetDlgItem(IDC_EDIT_BKGNDFILE));
  m_userbkgnd_edit.Init(ID_BKGND_PICKER);
  // upgrade strategy combo
  m_upgradestrategy_combo.Attach(GetDlgItem(IDC_COMBO_AUTOUPDATEVER));
  WTL::CString text;
  text.LoadString(IDS_UPGRADESTRATEGY);
  std::vector<std::wstring> text_ar;
  Strings::Split(text, L"|", text_ar);
  for (std::vector<std::wstring>::iterator it = text_ar.begin();
    it != text_ar.end(); it++)
    m_upgradestrategy_combo.AddString(it->c_str());

  m_autoscalecheckbox.Attach(GetDlgItem(IDC_CHECK_BKGNDAUTOSCALE));
  m_aeroglasscheckbox.Attach(GetDlgItem(IDC_CHECK_AERO));

  AppSettings& s = AfxGetAppSettings();

  // retrieve variables from preference
  m_bkgnd = s.logoext?1:0;  // logoext is "use external logo"
  m_autoscalecheckbox.EnableWindow(s.logoext);

  // feed variables onto screen
  m_userbkgnd_edit.SetWindowText(s.logofn);

  m_aeroglasscheckbox.EnableWindow(s.bAeroGlassAvalibility);

  // s.logostretch 
  // 1 keep aspect ratio
  // 2 stretch to full screen
  m_autoscalebkgnd = !!(s.logostretch & 1);
  m_useaero = s.bAeroGlass;
  m_repeat = s.fLoopForever;
  m_mintotray = s.fTrayIcon;
  m_autoresume =  s.autoResumePlay;
  m_autofullscreen = s.launchfullscreen;
  m_autoupgrade = (s.tLastCheckUpdater < 2000000000);

  //This might need revise
  CSVPToolBox svpTool;
  CPath updPath( svpTool.GetPlayerPath(_T("UPD")));
  updPath.AddBackslash();
  CString szUpdfilesPath(updPath);
  CString szBranch = svpTool.fileGetContent(szUpdfilesPath + _T("branch") );
  if (szBranch == L"stable")
    m_upgradestrategy_combo.SetCurSel(1);
  else if (szBranch == L"beta")
    m_upgradestrategy_combo.SetCurSel(2);
  else
    m_upgradestrategy_combo.SetCurSel(0);

  DoDataExchange();
  return TRUE;
}

void OptionBasicPage::OnDestroy()
{
  m_upgradestrategy_combo.Detach();
  m_autoscalecheckbox.Detach();
  m_aeroglasscheckbox.Detach();
}

// event when setting is updated
void OptionBasicPage::OnBkgndUpdated(UINT uNotifyCode, int nID, CWindow wndCtl)
{
  // update scale checkbox and exit box
  m_autoscalecheckbox.EnableWindow(nID == IDC_RADIO_USERBKGND);
}

void OptionBasicPage::OnBkgndPickerSetFocused(UINT uNotifyCode, int nID, CWindow wndCtl)
{
  m_bkgnd = 1;
  DoDataExchange();
}

void OptionBasicPage::OnBkgndPicker(UINT uNotifyCode, int nID, CWindow wndCtl)
{
  // call ::OpenFileDialog to prompt choosing a file
  WTL::CFileDialog fd(TRUE, NULL, NULL, OFN_EXPLORER, 
    L"*.bmp;*.jpg;*.gif;*.png\0*.bmp;*.jpg;*.gif;*.png\0\0", m_hWnd);

  if(fd.DoModal(m_hWnd) != IDOK)
    return;

  // set window text of edit control only
  m_userbkgnd_edit.SetWindowText(fd.m_szFileName);

  // we are using external bkng pic now
  m_bkgnd = 1;
  DoDataExchange();
}

int OptionBasicPage::OnSetActive()
{
  return 0;
}

int OptionBasicPage::OnApply()
{
  // retrieve variables from screen
  DoDataExchange(TRUE);
  AppSettings& s = AfxGetAppSettings();
  // feed variables into preference
  s.logoext = m_bkgnd==1?true:false;
  m_userbkgnd_edit.GetWindowText(s.logofn);

  s.logostretch = m_autoscalebkgnd?3:0;
  s.bAeroGlass = m_useaero;
  s.fLoopForever = m_repeat?true:false;
  s.fTrayIcon = m_mintotray?true:false;
  s.autoResumePlay = m_autoresume;
  s.launchfullscreen = m_autofullscreen?true:false;

  if (m_autoupgrade)
    s.tLastCheckUpdater = (UINT)time(NULL) - 100000;
  else
    s.tLastCheckUpdater = 2000000000;

  //This might need revise
  CSVPToolBox svpTool;
  CPath updPath( svpTool.GetPlayerPath(_T("UPD")));
  updPath.AddBackslash();
  CString szUpdfilesPath(updPath);
  switch (m_upgradestrategy_combo.GetCurSel())
  {
    case 1:
      svpTool.filePutContent( szUpdfilesPath + _T("branch") , L"stable");
      break;
    case 2:
      svpTool.filePutContent( szUpdfilesPath + _T("branch") , L"beta");
      break;
    default:
      ::DeleteFile(szUpdfilesPath + _T("branch"));
      break;
  }

  return PSNRET_NOERROR;
}
