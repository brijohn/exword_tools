/* ExwordLibraryInstaller - Unpacks and installs ex-word add-on libraries
 *
 * Copyright (C) 2011 - Brian Johnson <brijohn@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <wx/wx.h>
#include <wx/platinfo.h>
#include <wx/filename.h>

#include "LibraryInstaller.h"
#include "InstallWizard.h"
#include "UnpackThread.h"

BEGIN_EVENT_TABLE( InstallWizard, InstallWizard_Base )
    EVT_COMMAND(myID_WRITETEXT, myEVT_THREAD, InstallWizard::OnThreadWriteText)
    EVT_COMMAND(myID_DECRYPT, myEVT_THREAD, InstallWizard::OnThreadDecrypt)
    EVT_COMMAND(myID_FINISH, myEVT_THREAD, InstallWizard::OnThreadFinished)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, InstallWizard::OnChangePage)
    EVT_WIZARD_PAGE_SHOWN(wxID_ANY, InstallWizard::OnDisplay)
END_EVENT_TABLE()

void setControlEnable(int id, bool state)
{
	wxWindow *win = wxWindow::FindWindowById(id);
	if(win) win->Enable(state);
}

void setControlShow(int id, bool show)
{
	wxWindow *win = wxWindow::FindWindowById(id);
	if(win) win->Show(show);
}

InstallWizard::InstallWizard(wxFrame* parent) : InstallWizard_Base(parent)
{
    unshield_set_log_level(0);
    m_state = 1;
    m_cab = NULL;
    m_dynlib = NULL;
    GetPageAreaSizer()->Add(m_start);
    setControlShow(wxID_CANCEL, false);
}

InstallWizard::~InstallWizard() {
    if(m_cab)
        unshield_close(m_cab);
    if (m_dynlib)
        delete m_dynlib;
}

void InstallWizard::OnThreadWriteText(wxCommandEvent& event)
{
	wxString text = event.GetString();
	m_install->AppendText(text);
}

void InstallWizard::OnThreadFinished(wxCommandEvent& event)
{
	setControlEnable(wxID_FORWARD, true);
	m_install->AppendText(_T("Installation complete!"));
}

void InstallWizard::OnThreadDecrypt(wxCommandEvent& event)
{
	wxString text = event.GetString();
	wxExecute(text, wxEXEC_SYNC | wxEXEC_NODISABLE);
	wxGetApp().m_execDone.Post();
}

int InstallWizard::CheckDecryptFunction()
{
    wxOperatingSystemId id = wxPlatformInfo::Get().GetOperatingSystemId();
    if (id & wxOS_WINDOWS) {
        m_dynlib = new wxDynamicLibrary(GetDecrypterPath(), wxDL_VERBATIM);
        if (!m_dynlib->IsLoaded())
            return DECRYPT_DLL_MISSING;
        m_decryptInit = (DecryptInit) m_dynlib->GetSymbol(wxT("?XMDSRInitDecryption2@@YAHPADK@Z"));
        m_decrypt = (Decrypt) m_dynlib->GetSymbol(wxT("?XMDSRDecryption2@@YAXPADK0@Z"));
        m_decryptFin = (DecryptFin) m_dynlib->GetSymbol(wxT("?XMDSRFini2@@YAXXZ"));
        if (m_decrypt == NULL || m_decryptFin == NULL || m_decryptInit == NULL)
            return DECRYPT_INVALID_DLL;
        return DECRYPT_OK;
    } else {
        wxString wine;
        wine.Printf(wxT("wine %s"), GetDecrypterPath().c_str());
        long ret = wxExecute(wine, wxEXEC_SYNC);
        return ret;
    }
}

int InstallWizard::InitDecrypt(char *key, unsigned long length)
{
    return m_decryptInit(key, length);
}

void InstallWizard::FinDecrypt()
{
    m_decryptFin();
}

void InstallWizard::DoDecrypt(char *src, unsigned long size, char *dest)
{
    m_decrypt(src, size, dest);
}

wxString InstallWizard::GetDecrypterPath()
{
    wxString path;
    wxOperatingSystemId id = wxPlatformInfo::Get().GetOperatingSystemId();
    if (id & wxOS_WINDOWS) {
        path = wxT("MDSR2.dll");
    } else {
        path = LibraryInstaller::GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("exword_decrypt.exe");
    }
    return path;
}

bool InstallWizard::RunWizard()
{
    return wxWizard::RunWizard(m_start);
}

bool InstallWizard::OpenCab()
{
    wxString cabFile;
    cabFile.Printf(wxT("%s%c%s"), m_cabDir->GetPath().c_str(),
                   wxFileName::GetPathSeparator(), wxT("data1.cab"));
    m_cab = unshield_open(cabFile.utf8_str().data());
    return m_cab ? true : false;
}

Unshield * InstallWizard::GetCab() const
{
    return m_cab;
}

wxArrayString InstallWizard::GetSelectedDictionaries()
{
    wxArrayString names;
    for (unsigned int i = 0; i < m_dictionaries->GetCount(); ++i) {
        if (m_dictionaries->IsChecked(i)) {
            names.Add(m_dict_names[i]);
        }
    }
    return names;
}

void InstallWizard::OnChangePage(wxWizardEvent& event)
{
    if (event.GetDirection()) {
        if (m_state == 1) {
            UnshieldFileGroup *group;
            unsigned int g_count, i;
            char dict_name[6];
            char type[3];
            m_dict_names.Clear();
            if (OpenCab()) {
                g_count = unshield_file_group_count(m_cab);
                for (i = 0; i < g_count; i++) {
                    group = unshield_file_group_get(m_cab, i);
                    int ret = sscanf(group->name, "\x8e\xab\x8f\x91User1 %5s %2s", dict_name, type);
                    if (ret == 2 && strcmp(type, "DB") == 0)
                        m_dict_names.Add(wxString::FromAscii(dict_name));
                }
                if (m_dict_names.GetCount() == 0) {
                    wxMessageBox(_("No dictionaries contained in cabinet file"), _("Error"), wxOK, this);
                    event.Veto();
                    return;
                } else {
                    m_dictionaries->Set(m_dict_names);
                }
            } else {
               wxMessageBox(_("Unable to open cab file"), _("Error"), wxOK, this);
               event.Veto();
               return;
            }
        } else if (m_state == 2) {
            bool selected = false;
            for (unsigned int i = 0; i < m_dictionaries->GetCount(); ++i) {
                if (m_dictionaries->IsChecked(i)) {
                    selected = true;
                    break;
                }
            }
            if (!selected) {
                wxMessageBox(_("You must select a least one dictionary."), _("Error"), wxOK, this);
                event.Veto();
                return;
            }
        }
        ++m_state;
    } else {
        --m_state;
    }
}

void InstallWizard::OnDisplay(wxWizardEvent& event)
{
    if (m_state == 3) {
        setControlEnable(wxID_FORWARD, false);
        setControlEnable(wxID_BACKWARD, false);
        UnpackThread *thread = new UnpackThread(this);
        thread->Create();
        thread->Run();
    }
}

