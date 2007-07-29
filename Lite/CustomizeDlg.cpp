// CustomizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "CustomizeDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDlg dialog

CCustomizeDlg::CCustomizeDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CCustomizeDlg::IDD, pParent), pfirst(NULL), plast(NULL)
{
	//{{AFX_DATA_INIT(CCustomizeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CCustomizeDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomizeDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelChanged)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEYDONE, OnHotkeyDone)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDlg message handlers


char* CCustomizeDlg::UIRead(CTreeCtrl& tree, char* buf, HTREEITEM parent)
{
	static CString separator = LoadString(IDS_SEPARATOR);
	BYTE type = *buf;
	//	*buf=TYPE;
	buf++;
	WORD count = *(WORD*)buf;	//取得sub item count
	buf += 6 + *(WORD*)(buf + 4);	//到第一個sub item

	while (count)
	{
		LPSTR text;
		WORD len;
		//	type=*buf;	id=*(WORD*)(buf+1);
		if (!*buf)	//如果是separator
		{
			tree.InsertItem(separator, parent);
			buf++;
		}
		else
		{
			text = buf + 7;
			len = *(WORD*)(buf + 5);
			HTREEITEM hitem = tree.InsertItem(text, parent);
			HotkeyData* data = NewHotkeyData();	//new HotkeyData
			tree.SetItemData(hitem, DWORD(data));

			data->type = *buf;
			data->id = *(WORD*)(buf + 1);
			data->state = *(WORD*)(buf + 3);
			data->key = data->fVirt = 0;
			data->hitem = hitem;

			//將HotkeyData中的id對照accels table,順便建立每個使用同樣id的其他熱鍵
			if (!(data->type & CT_HAS_SUB))
			{
				WORD id = data->id;
				WORD state = data->state;
				HTREEITEM hitem = data->hitem;
				BYTE type = data->type;
				for (WORD i = 0; i < accel_count ; i++)
				{
					if (id == accels[i].cmd)
					{
						if (!data)
						{
							data = NewHotkeyData();
							data->id = id;
							data->state = state;
							data->type = type;
							data->hitem = hitem;
						}
						data->fVirt = accels[i].fVirt & ~(FNOINVERT | FVIRTKEY);
						data->key = accels[i].key;
						data = NULL;
					}
				}
			}


			if (*buf & CT_HAS_SUB)
				buf = UIRead(tree, buf, hitem);
			else
				buf += 7 + len;	//Next item
		}
		count--;
	}
	tree.Expand(parent, TVE_EXPAND);
	return buf;
}

BOOL CCustomizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	tree.Attach(::GetDlgItem(m_hWnd, IDC_TREE));
	assign.Attach(::GetDlgItem(m_hWnd, IDC_ASSIGN));
	list.Attach(::GetDlgItem(m_hWnd, IDC_LIST));
	hkedit.SubclassDlgItem(IDC_HOTKEY, this);
	hkedit2.Attach(::GetDlgItem(m_hWnd, IDC_EDIT));
	hkedit2.SetLimitText(1);
	::SetWindowLong(hkedit2.m_hWnd, GWL_USERDATA, (LONG)this);
	old_hkedit2_proc = (WNDPROC)::SetWindowLong(hkedit2.m_hWnd, GWL_WNDPROC, LONG(HotkeyEdit2Proc));

	CFile ui;
	if (ui.Open(ConfigPath + UI_FILENAME, CFile::modeRead))
	{
		DWORD l = ui.GetLength();
		char* ui_buf = new char[l];
		ui.Read(ui_buf, l);
		ui.Close();
//		讀取Accelerator
		accel_count = *(WORD*)ui_buf;
		accels = (ACCEL*)(ui_buf + sizeof(WORD));
//		讀取Accelerator結束,開始讀取 UI,建立HotkeyData的list
		char* menu = ((char*)accels) + accel_count * sizeof(ACCEL);
		menuitem_count = *(WORD*)(menu + 1);
		UIRead(tree, menu, TVI_ROOT);

		delete []ui_buf;
	}
	tree.SelectSetFirstVisible(tree.GetChildItem(TVI_ROOT));
	tree.ShowWindow(SW_SHOW);
	return TRUE;
}


void CCustomizeDlg::OnOK()
{
	CFile ui;
	if (!ui.Open(ConfigPath + UI_FILENAME, CFile::modeWrite | CFile::modeCreate))
		return;

	HotkeyData* ptmp = pfirst;
	accel_count = 0;
	while (ptmp)
		accel_count++, ptmp = ptmp->pnext;

	accels = new ACCEL[accel_count];
	accel_count = 0;
	UIWriteAccels();

	ui.Write(&accel_count, 2);
	ui.Write(accels, accel_count*sizeof(ACCEL));
	delete []accels;
	char data[] = {CT_MENU | CT_HAS_SUB, 0, 0, 0, 0, 1, 0, 0};
	*(WORD*)(data + 1) = menuitem_count;
	ui.Write(data, 8);
	UIWrite(ui, TVI_ROOT);
	ui.Close();

	UICleanup();
	CDialog::OnOK();
}

void CCustomizeDlg::OnApply()
{
	HTREEITEM hitem = tree.GetSelectedItem();
	HotkeyData* data = (HotkeyData*)tree.GetItemData(hitem);
	if (!data)
		return;

	if (!hkedit.key)	//沒設定熱鍵或是有子選單的項目都只更新文字顯示
	{
		UpdateItemDisplay(hitem);
		return;
	}

	HotkeyData* old = HotkeyDataFromKeys(hkedit.fVirt, hkedit.key);

	if (old)	//如果現在正要設定熱鍵原本已經被使用
	{
		if (old->id != data->id)	//如果本來是被別的功能使用
		{
			if (MessageBox(LoadString(IDS_REPLACE_OLD_HOTKEY_CONFIRM) , NULL, MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
				return;
		}
		else	//如果是正在設定的功能本來就有的熱鍵,先刪除再新增到第一個
		{
			//找出list中對應的項目並刪除
			int c = list.GetCount();
			for (int i = 0;i < c;i++)
				if (list.GetItemData(i) == DWORD(old))
				{
					list.DeleteString(i);
					break;
				}
		}
		DeleteHotkeyForID(old);
	}

	//新增熱鍵
	HotkeyData* hkitem = (HotkeyData*)tree.GetItemData(hitem);
	WORD id = hkitem->id;
	//加到hkitem前一個，並且更新畫面
	hkitem = HotkeyDataFromID(pfirst, id);

	while (hkitem)
	{
		HotkeyData* newkitem;
		if (hkitem->key)	//如果原本有熱鍵,建立新的,插入到他前面
		{
			newkitem = new HotkeyData;
			newkitem->type = hkitem->type;
			newkitem->id = id;
			newkitem->state = hkitem->state;
			newkitem->fVirt = hkedit.fVirt;
			newkitem->key = hkedit.key;
			newkitem->hitem = hkitem->hitem;
			//插入串列
			newkitem->pnext = hkitem;
			newkitem->pprev = hkitem->pprev;
			hkitem->pprev = newkitem;
			if (!newkitem->pprev)
				pfirst = newkitem;
			else
				newkitem->pprev->pnext = newkitem;
			//插入串列完成
		}
		else	//如果本來沒熱鍵,不用配置記憶體
		{
			newkitem = hkitem;
			hkitem->fVirt = hkedit.fVirt;
			hkitem->key = hkedit.key;
		}

		if (newkitem->hitem == hitem)		//如果是現在正選取的項目
		{
			list.InsertString(0, HotkeyToStr(newkitem->fVirt, newkitem->key));
			list.SetItemData(0, DWORD(newkitem));
		}
		tree.SetItemData(newkitem->hitem, DWORD(newkitem));
		UpdateItemDisplay(newkitem->hitem);

		while (hkitem && hkitem->id == id)	//略過相同id的其他熱鍵
			hkitem = hkitem->pnext;

		hkitem = HotkeyDataFromID(hkitem, id);
	}
	list.SetCurSel(0);
	assign.SetWindowText(tree.GetItemText(hitem));
}

void CCustomizeDlg::OnDel()
{
	int sel = list.GetCurSel();
	if (sel == -1)
		return;
	if (MessageBox(LoadString(IDS_DEL_CONFIRM) , NULL, MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;

	hkedit.key = 0;
	hkedit.fVirt = 0;
	hkedit.SetWindowText(NULL);

	HotkeyData* hkdel = (HotkeyData*)list.GetItemData(sel);
	list.DeleteString(sel);
	list.SetCurSel(0);
	DeleteHotkeyForID(hkdel);
}

void CCustomizeDlg::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	list.ResetContent();	//清除list顯示
	HTREEITEM item = tree.GetSelectedItem();
	if (!item)
		return;

	assign.SetWindowText(NULL);
	HotkeyData* data = (HotkeyData*)tree.GetItemData(item);
	if (!data)
	{
		hkedit.EnableWindow(FALSE);
		hkedit.SetWindowText(NULL);
		hkedit2.EnableWindow(FALSE);
		hkedit2.SetWindowText(NULL);
		return;
	}

	hkedit.fVirt = 0;	hkedit.key = 0;
	hkedit.SetWindowText(NULL);
	if (data->type & CT_MENU)
	{
		CString text = tree.GetItemText(item);
		int p = text.Find('&');
		char a[] = {0, 0};
		if (p != -1)	*a = text[p+1];
		hkedit2.SetWindowText(a);
		if (data->type & CT_HAS_SUB)
		{
			hkedit2.EnableWindow(TRUE);
			hkedit.EnableWindow(FALSE);
			return;
		}
		else
		{
			hkedit.EnableWindow(TRUE);
			hkedit2.EnableWindow(TRUE);
		}
	}
	else
	{
		hkedit2.SetWindowText(NULL);
		if (data->type & CT_HAS_SUB)
		{
			hkedit.EnableWindow(FALSE);
			hkedit2.EnableWindow(FALSE);
			return;
		}
		else
		{
			hkedit.EnableWindow(TRUE);
			hkedit2.EnableWindow(FALSE);
		}
	}

	WORD id = data->id;
	while (data && data->id == id)
	{
		if (data->key)
		{
			int idx = list.AddString(HotkeyToStr(data->fVirt, data->key));
			list.SetItemData(idx, DWORD(data));
			list.SetCurSel(0);
		}
		data = data->pnext;
	}
}

LRESULT CALLBACK CCustomizeDlg::HotkeyEdit2Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char a[] = {0, 0};
	if (msg == WM_CHAR)
	{
		if (isalnum(wparam))
		{
			*a = wparam;
			::SetWindowText(hwnd, a);
		}
	}
	CCustomizeDlg* dlg = (CCustomizeDlg*)::GetWindowLong(hwnd, GWL_USERDATA);
	return CallWindowProc(dlg->old_hkedit2_proc, hwnd, msg, wparam, lparam);
}

LRESULT CCustomizeDlg::OnHotkeyDone(WPARAM wparam, LPARAM lparam)
{
	HotkeyData* hkitem = HotkeyDataFromKeys(hkedit.fVirt, hkedit.key);
	if (hkitem)
		assign.SetWindowText(tree.GetItemText(hkitem->hitem));
	else
		assign.SetWindowText(LoadString(IDS_NOT_ASSIGNED));
	return 0;
}

void CCustomizeDlg::UpdateItemDisplay(HTREEITEM item)
{
	if (!item)
		return;
	CString text = tree.GetItemText(item);
	int p;
	if ((p = text.Find('\t')) != -1)
		text = text.Left(p);
	char a[] = " (&A)", _a[2];
	hkedit2.GetWindowText(_a, 2);

	HotkeyData* hkitem = (HotkeyData*)tree.GetItemData(item);
	if (hkitem->type & CT_MENU && item == tree.GetSelectedItem())
	{
		if ((p = text.Find('&')) != -1)
		{
			if (isalnum(*_a))
				text.SetAt(p + 1, *_a);
			else
				text = text.Left(p - 2);
		}
		else
		{
			if (isalnum(*_a))
			{
				a[3] = *_a;
				text += a;
			}
		}
	}

	if (hkitem && hkitem->key)
	{
		text += '\t';
		text += HotkeyToStr(hkitem->fVirt, hkitem->key);
	}
	tree.SetItemText(item, text);
}

void CCustomizeDlg::UIWrite(CFile& ui, HTREEITEM parent)
{
	HTREEITEM item = tree.GetChildItem(parent);
	while (item)
	{
		HotkeyData* hkitem = (HotkeyData*)tree.GetItemData(item);
		if (hkitem)
		{
			CString text = tree.GetItemText(item);
			char data[1+2+2+2];
			*data = hkitem->type;
			*(WORD*)(data + 1) = hkitem->id;
			WORD l = text.GetLength() + 1;
			*(WORD*)(data + 3) = hkitem->state;
			*(WORD*)(data + 5) = l;
			ui.Write(data, 7);
			ui.Write(LPCTSTR(text), l);

			if (hkitem->type & CT_HAS_SUB)
				UIWrite(ui, item);
		}
		else
		{
			BYTE type = 0;
			ui.Write(&type, 1);
		}
		item = tree.GetNextSiblingItem(item);
	}

}

void CCustomizeDlg::UIWriteAccels()
{
	HotkeyData* hkitem = pfirst;
	while (hkitem)
	{
		hkitem->fVirt |= (FNOINVERT | FVIRTKEY);	//加入fVirt的其他flags
		BOOL can_add = TRUE;
		for (int i = 0;i < accel_count;i++)	//檢查是否已經有此熱鍵
		{
			if (accels[i].fVirt == hkitem->fVirt && accels[i].key == hkitem->key)
			{
				can_add = FALSE;
				break;
			}
		}
		if (can_add)
		{
			accels[accel_count].cmd = hkitem->id;
			accels[accel_count].fVirt = hkitem->fVirt;
			accels[accel_count].key = hkitem->key;
			accel_count++;
		}
		hkitem = hkitem->pnext;
	}
}

void CCustomizeDlg::OnCancel()
{
	UICleanup();
	CDialog::OnCancel();
}

void CCustomizeDlg::UICleanup()
{
	HotkeyData* hkitem;
	while (pfirst)
	{
		hkitem = pfirst;
		pfirst = pfirst->pnext;
		delete hkitem;
	}
	pfirst = NULL;
	plast = NULL;
}


CCustomizeDlg::HotkeyData* CCustomizeDlg::HotkeyDataFromID(CCustomizeDlg::HotkeyData* start, WORD id)
{
	HotkeyData* hkitem = start;
	while (hkitem)
	{
		if (hkitem->id == id && !(hkitem->type&CT_HAS_SUB))
			return hkitem;
		hkitem = hkitem->pnext;
	}
	return NULL;
}

CCustomizeDlg::HotkeyData* CCustomizeDlg::NewHotkeyData()
{
	HotkeyData* nk = new HotkeyData;
	if (plast)
	{
		plast->pnext = nk;
		nk->pprev = plast;
		plast = nk;
	}
	else
	{
		pfirst = plast = nk;
		nk->pprev = NULL;
	}
	return nk;
}

void CCustomizeDlg::DeleteHotkeyData(CCustomizeDlg::HotkeyData *hkdel)
{
	if (hkdel->pprev)
		hkdel->pprev->pnext = hkdel->pnext;
	else
		pfirst = hkdel->pnext;

	if (hkdel->pnext)
		hkdel->pnext->pprev = hkdel->pprev;
	else
		plast = hkdel->pprev;

	delete hkdel;
}

CCustomizeDlg::HotkeyData* CCustomizeDlg::HotkeyDataFromKeys(BYTE fVirt, WORD key)
{
	HotkeyData* hkitem = pfirst;
	while (hkitem)
	{
		if (hkitem->fVirt == fVirt && hkitem->key == key && !(hkitem->type&CT_HAS_SUB))
			return hkitem;
		hkitem = hkitem->pnext;
	}
	return NULL;
}

// 從所有具有相同 ID (hkdel->id) 的選單項目中，移除和 hkdel 相同的熱鍵 (hkdel->fVirt + hkdel->key)
void CCustomizeDlg::DeleteHotkeyForID(CCustomizeDlg::HotkeyData* hkdel)
{
	WORD id = hkdel->id;
	BYTE fVirt = hkdel->fVirt;
	WORD key = hkdel->key;

	hkdel = HotkeyDataFromID(pfirst, id);
	while (hkdel)
	{
		HotkeyData* knext = HotkeyDataFromID(hkdel->pnext, id);
		if (hkdel->fVirt == fVirt && hkdel->key == key)
		{
			HTREEITEM hitem = hkdel->hitem;
			if (hkdel->pprev && hkdel->pprev->id == id)	//如果同id前一個還有熱鍵
				DeleteHotkeyData(hkdel);
			else if (hkdel->pnext && hkdel->pnext->id == id)	//如果同id前一個沒有但是後一個還有熱鍵
			{
				tree.SetItemData(hitem, DWORD(hkdel->pnext));
				DeleteHotkeyData(hkdel);
			}
			else	//已經沒有熱鍵了,不能釋放記憶體
			{
				hkdel->fVirt = 0;
				hkdel->key = 0;
			}
			UpdateItemDisplay(hitem);	//更新文字顯示
		}
		hkdel = knext;
	}
}

void CCustomizeDlg::OnDestroy()
{
	tree.Detach();
	assign.Detach();
	list.Detach();
	hkedit.UnsubclassWindow();
	hkedit2.Detach();
	CDialog::OnDestroy();
}
