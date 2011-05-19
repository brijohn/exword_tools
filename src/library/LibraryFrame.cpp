/* ExwordLibrary - Main frame for ExwordLibrary
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

#include "LibraryFrame.h"
#include "InstallThread.h"

BEGIN_EVENT_TABLE(LibraryFrame, LibraryGUI)
    EVT_BUTTON(XRCID("m_connect"), LibraryFrame::OnConnect)
    EVT_RADIOBUTTON(XRCID("m_internal"), LibraryFrame::OnInternal)
    EVT_RADIOBUTTON(XRCID("m_sd"), LibraryFrame::OnSDCard)
    EVT_BUTTON(XRCID("m_install"), LibraryFrame::OnInstall)
    EVT_BUTTON(XRCID("m_remove"), LibraryFrame::OnRemove)
    EVT_COMMAND(myID_UPDATE, myEVT_UPDATE_PROGRESS, LibraryFrame::OnThreadUpdate)
    EVT_COMMAND(myID_START, myEVT_UPDATE_PROGRESS, LibraryFrame::OnThreadStart)
    EVT_COMMAND(myID_FINISH, myEVT_UPDATE_PROGRESS, LibraryFrame::OnThreadFinish)
    EVT_TIMER(wxID_ANY, LibraryFrame::OnPulse)
END_EVENT_TABLE()

LibraryFrame::LibraryFrame() : m_pulser(this)
{
    int fieldWidths[] = {175, -1};
    m_progress = NULL;
    m_sd->Enable(false);
    Users::iterator it;
    for (it = m_users.begin(); it != m_users.end(); ++it) {
        m_user->Append(it->first);
    }
    m_user->SetSelection(0);
    m_status->SetFieldsCount(2, fieldWidths);
    m_local->InsertColumn(0, _("Filename"), wxLIST_FORMAT_LEFT, 160);
    m_local->InsertColumn(1, _("Size"), wxLIST_FORMAT_LEFT, 60);
    m_remote->InsertColumn(0, _("Filename"));
    m_remote->SetColumnWidth(0, 210);
    m_remote->SetFont(wxFont(8, 76, 90, 90, false, wxEmptyString));
    m_local->SetFont(wxFont(8, 76, 90, 90, false, wxEmptyString));
}

LibraryFrame::~LibraryFrame()
{
    m_exword.Disconnect();
}

ExwordRegion LibraryFrame::GetRegionFromString(wxString name)
{
    if (name == _("Japanese"))
        return JAPANESE;
    else if(name == _("Chinese"))
        return CHINESE;
    else if(name == _("Korean"))
        return KOREAN;
    else if(name == _("German"))
        return GERMAN;
    else if(name == _("Spanish"))
        return SPANISH;
    else if(name == _("French"))
        return FRENCH;
    else if(name == _("Russian"))
        return RUSSIAN;
    return JAPANESE;
}

void LibraryFrame::UpdateStatusbar()
{
    wxString capTxt = wxT("");
    wxString modelTxt = wxT("");
    if (m_exword.IsConnected()) {
        Capacity cap = m_exword.GetCapacity();
        Model model = m_exword.GetModel();
        unsigned long percent = ((float)cap.GetFree() / (float)cap.GetTotal()) * 100;
        capTxt.Printf(_("Capacity: %lu / %lu (%lu%%)"), cap.GetFree(), cap.GetTotal(), percent);
        modelTxt.Printf(wxT("%s %s"), model.GetSeriesName().c_str(), model.GetModel().c_str());
    }
    m_status->SetStatusText(modelTxt, 0);
    m_status->SetStatusText(capTxt, 1);
}

void LibraryFrame::OnConnect(wxCommandEvent& event)
{
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    DictionaryListCtrl * local = dynamic_cast<DictionaryListCtrl*>(m_local);
    if (!m_exword.IsConnected()) {
        wxMemoryBuffer key;
        wxString region = m_region->GetString(m_region->GetCurrentSelection());
        if (m_user->GetValue() == wxT("")) {
            wxMessageBox(_("Must specify username"), _("Error"), wxOK, this);
            return;
        }
        if (!m_exword.Connect(LIBRARY, GetRegionFromString(region))) {
            wxMessageBox(_("Device not found"), _("Error"), wxOK, this);
            return;
        }
        if (m_users.find(m_user->GetValue()) == m_users.end() || !m_exword.Authenticate(m_user->GetValue(), m_users[m_user->GetValue()])) {
            if (wxMessageBox(_("Reset username?"), _("Confirm"), wxYES_NO, this) == wxNO) {
                m_exword.Disconnect();
                return;
            }
            key = m_exword.Reset(m_user->GetValue());
            if (m_users.find(m_user->GetValue()) == m_users.end())
                m_user->Append(m_user->GetValue());
            m_users[m_user->GetValue()] = key;
            m_exword.Authenticate(m_user->GetValue(), m_users[m_user->GetValue()]);
        }
        m_user->Enable(false);
        m_region->Enable(false);
        if (m_exword.IsSdInserted())
            m_sd->Enable(true);
        m_connect->SetLabel(_("Disconnect"));
        local->SetDictionaries(m_exword.GetLocalDictionaries());
        remote->SetDictionaries(m_exword.GetRemoteDictionaries());
    } else {
        local->ClearDictionaries();
        remote->ClearDictionaries();
        m_user->Enable(true);
        m_region->Enable(true);
        m_sd->Enable(false);
        m_internal->SetValue(true);
        m_connect->SetLabel(_("Connect"));
        m_exword.Disconnect();
    }
    UpdateStatusbar();
}

void LibraryFrame::OnInternal(wxCommandEvent& event)
{
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    m_exword.SetStorage(INTERNAL);
    remote->SetDictionaries(m_exword.GetRemoteDictionaries());
    UpdateStatusbar();
}

void LibraryFrame::OnSDCard(wxCommandEvent& event)
{
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    m_exword.SetStorage(SD);
    remote->SetDictionaries(m_exword.GetRemoteDictionaries());
    UpdateStatusbar();
}

void LibraryFrame::OnInstall(wxCommandEvent& event)
{
    DictionaryListCtrl * local = dynamic_cast<DictionaryListCtrl*>(m_local);
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    Dictionary * dict = local->GetSelectedDictionary();
    if (dict) {
        if (remote->DictionaryExists(dict->GetId())) {
            wxMessageBox(_("Dictionary already installed"), _("Error"), wxOK, this);
        } else {
            Capacity cap = m_exword.GetCapacity();
            Model model = m_exword.GetModel();
            unsigned long min_series = dict->GetMinSupportedSeries();
            if (dict->GetSize() < cap.GetFree()) {
                if (min_series <= model.GetSeries()) {
                    InstallThread *thread = new InstallThread(this, &m_exword);
                    if (!thread->Start(dict))
                        wxMessageBox(_("Failed to start InstallThread"), _("Error"), wxOK, this);
                } else {
                    wxMessageBox(wxString::Format(_("Requires a Dataplus %lu or above"), min_series), _("Error"), wxOK, this);
                }
            } else {
                wxMessageBox(_("Insufficient space"), _("Error"), wxOK, this);
            }
        }
    } else {
        wxMessageBox(_("No local dictionary selected"), _("Information"), wxOK, this);
    }
}

void LibraryFrame::OnRemove(wxCommandEvent& event)
{
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    Dictionary * dict = remote->GetSelectedDictionary();
    if (dict) {
        m_pulser.Start(100);
        RemoveThread *thread = new RemoveThread(this, &m_exword);
        if (!thread->Start(dict))
            wxMessageBox(_("Failed to start RemoveThread"), _("Error"), wxOK, this);
    } else {
        wxMessageBox(_("No remote dictionary selected"), _("Information"), wxOK, this);
    }
}


void LibraryFrame::OnThreadStart(wxCommandEvent& event)
{
    if (m_progress) {
        m_progress->Destroy();
        m_progress = NULL;
    }
    m_progress = new ProgressDialog(this, event.GetString());

}

void LibraryFrame::OnThreadUpdate(wxCommandEvent& event)
{
    unsigned long percent = event.GetInt();
    wxString text;
    text.Printf(_("Copying %s (%lu%%)"), event.GetString().c_str(), percent);
    if (m_progress)
        m_progress->Update(percent, text);
}

void LibraryFrame::OnThreadFinish(wxCommandEvent& event)
{
    DictionaryListCtrl * remote = dynamic_cast<DictionaryListCtrl*>(m_remote);
    if (m_progress)
        m_progress->Show(false);
    remote->SetDictionaries(m_exword.GetRemoteDictionaries());
    UpdateStatusbar();
    m_pulser.Stop();
}

void LibraryFrame::OnPulse(wxTimerEvent& event)
{
    if (m_progress != NULL)
        m_progress->Pulse();
}
