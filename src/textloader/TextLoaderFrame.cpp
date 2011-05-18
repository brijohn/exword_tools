/* ExwordTextLoader - Main TextLoader frame
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

#include <wx/dnd.h>

#include "TextLoaderFrame.h"
#include "UploadThread.h"

class ExwordFileDropTarget : public wxFileDropTarget
{
    public:
        ExwordFileDropTarget(TextLoaderFrame *frame) { m_frame = frame; };
        virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
            m_frame->UploadFiles(filenames);
            return true;
        }
    private:
        TextLoaderFrame *m_frame;
};

ProgressDialog::ProgressDialog(wxWindow *parent, wxString message) : wxProgressDialog(_("Transfer progress"), message, 100, parent, wxPD_APP_MODAL)
{
    SetSize(wxDefaultCoord, wxDefaultCoord, 350, wxDefaultCoord, wxSIZE_AUTO_HEIGHT);
    Centre();
}

void ProgressDialog::OnClose(wxCloseEvent &event)
{
    if (event.CanVeto()) {
        event.Veto();
        return;
    }
    event.Skip();
}

BEGIN_EVENT_TABLE(ProgressDialog, wxProgressDialog)
    EVT_CLOSE(ProgressDialog::OnClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(TextLoaderFrame, TextLoaderGUI)
    EVT_BUTTON(XRCID("m_connect"), TextLoaderFrame::OnConnect)
    EVT_BUTTON(XRCID("m_delete"), TextLoaderFrame::OnDelete)
    EVT_RADIOBUTTON(XRCID("m_internal"), TextLoaderFrame::OnInternal)
    EVT_RADIOBUTTON(XRCID("m_sd"), TextLoaderFrame::OnSDCard)
    EVT_COMMAND(myID_UPDATE, myEVT_UPDATE_PROGRESS, TextLoaderFrame::OnThreadUpdate)
    EVT_COMMAND(myID_START, myEVT_UPDATE_PROGRESS, TextLoaderFrame::OnThreadStart)
    EVT_COMMAND(myID_FINISH, myEVT_UPDATE_PROGRESS, TextLoaderFrame::OnThreadFinish)
END_EVENT_TABLE()

TextLoaderFrame::TextLoaderFrame()
{
    m_progress = NULL;
    m_sd->Enable(false);
    m_filelist->InsertColumn(0, _("Filename"));
    m_filelist->SetColumnWidth(0, 485);
    m_filelist->SetFont(wxFont(8, 76, 90, 90, false, wxEmptyString));
    m_filelist->SetDropTarget(new ExwordFileDropTarget(this));
}

TextLoaderFrame::~TextLoaderFrame()
{
    m_exword.Disconnect();
}

ExwordRegion TextLoaderFrame::GetRegionFromString(wxString name)
{
    if (name == wxT("Japanese"))
        return JAPANESE;
    else if(name == wxT("Chinese"))
        return CHINESE;
    else if(name == wxT("Korean"))
        return KOREAN;
    else if(name == wxT("German"))
        return GERMAN;
    else if(name == wxT("Spanish"))
        return SPANISH;
    else if(name == wxT("French"))
        return FRENCH;
    else if(name == wxT("Russian"))
        return RUSSIAN;
    return JAPANESE;
}

void TextLoaderFrame::UpdateStatusbar()
{
    wxString text = wxT("");
    if (m_exword.IsConnected()) {
        Capacity cap = m_exword.GetCapacity();
        unsigned long percent = ((float)cap.GetFree() / (float)cap.GetTotal()) * 100;
        text.Printf(_("Capacity: %lu / %lu (%lu%%)"), cap.GetFree(), cap.GetTotal(), percent);
    }
    m_status->SetStatusText(text);
}

void TextLoaderFrame::UpdateFilelist()
{
    wxArrayString files = m_exword.List(wxT(""), wxT("*.TXT"));
    m_filelist->DeleteAllItems();
    for (unsigned int i = 0; i < files.GetCount(); ++i) {
        m_filelist->InsertItem(i, files[i]);
    }
}

void TextLoaderFrame::UploadFiles(const wxArrayString& filenames)
{
    if (m_exword.IsConnected()) {
        UploadThread *thread = new UploadThread(this, &m_exword);
        if (!thread->Start(new wxArrayString(filenames)))
            wxMessageBox(_("Failed to start UploadThread"), _("Error"));
    }
}

void TextLoaderFrame::OnConnect(wxCommandEvent& event)
{
    if (!m_exword.IsConnected()) {
        wxString region = m_region->GetString(m_region->GetCurrentSelection());
        if (!m_exword.Connect(TEXT, GetRegionFromString(region))) {
            wxMessageBox(_("Device not found"), _("Error"), wxOK, this);
            return;
        }
        m_region->Enable(false);
        if (m_exword.IsSdInserted())
            m_sd->Enable(true);
        m_connect->SetLabel(_("Disconnect"));
        UpdateFilelist();
    } else {
        m_region->Enable(true);
        m_sd->Enable(false);
        m_filelist->DeleteAllItems();
        m_internal->SetValue(true);
        m_connect->SetLabel(_("Connect"));
        m_exword.Disconnect();
    }
    UpdateStatusbar();
}

void TextLoaderFrame::OnDelete(wxCommandEvent& event)
{
    long item;
    wxString filename;
    item = m_filelist->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        filename = m_filelist->GetItemText(item);
        m_exword.DeleteFile(filename);
        m_filelist->DeleteItem(item);
    }
}

void TextLoaderFrame::OnInternal(wxCommandEvent& event)
{
    m_exword.SetStorage(INTERNAL);
    UpdateFilelist();
    UpdateStatusbar();
}

void TextLoaderFrame::OnSDCard(wxCommandEvent& event)
{
    m_exword.SetStorage(SD);
    UpdateFilelist();
    UpdateStatusbar();
}

void TextLoaderFrame::OnThreadStart(wxCommandEvent& event)
{
    if (m_progress) {
        m_progress->Destroy();
        m_progress = NULL;
    }
    m_progress = new ProgressDialog(this, event.GetString());
}

void TextLoaderFrame::OnThreadUpdate(wxCommandEvent& event)
{
    unsigned long percent = event.GetInt();
    wxString text;
    text.Printf(_("Copying %s (%lu%%)"), event.GetString().c_str(), percent);
    if (m_progress)
        m_progress->Update(percent, text);
}

void TextLoaderFrame::OnThreadFinish(wxCommandEvent& event)
{
    if (m_progress) {
        m_progress->Show(false);
    }
    UpdateFilelist();
    UpdateStatusbar();
}

