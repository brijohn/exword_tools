/* ExwordTools - Class for a simple progress dialog
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

#include <wx/sizer.h>

#include "ProgressDialog.h"

ProgressDialog::ProgressDialog(wxWindow *parent, wxString message) : wxDialog(parent, wxID_ANY, _("Transfer progress"))
{
    wxSize sizeDlg;
    wxSize sizeLabel;
    wxSize sizeGauge;
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    m_message = new wxStaticText(this, wxID_ANY, message);
    sizer->Add(m_message, 0, wxLEFT | wxTOP, 8);
    m_gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    m_gauge->SetValue(0);
    sizer->Add(m_gauge, 0, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 8);
    sizeGauge = m_gauge->GetSize();
    sizeLabel = m_message->GetSize();
    sizeDlg.y = 24 + sizeLabel.y + sizeGauge.y;
    sizeDlg.x = 350;
    SetSizerAndFit(sizer);
    SetClientSize(sizeDlg);
    Centre();

    m_winDisabler = new wxWindowDisabler(this);

    Show();
    Enable();

    Update();
}

void ProgressDialog::OnClose(wxCloseEvent &event)
{
    event.Veto();
}

void ProgressDialog::Update(int value, const wxString& newmsg)
{
    m_gauge->SetValue(value);

    if (!newmsg.empty() && newmsg != m_message->GetLabel())
        m_message->SetLabel(newmsg);

    wxYieldIfNeeded();
    Update();
}

void ProgressDialog::Pulse()
{
    m_gauge->Pulse();

    wxYieldIfNeeded();
    Update();
}

bool ProgressDialog::Show(bool show)
{
    if (!show) {
        delete m_winDisabler;
        m_winDisabler = (wxWindowDisabler *)NULL;
    }
    return wxDialog::Show(show);
}

ProgressDialog::~ProgressDialog()
{
    delete m_winDisabler;
    m_winDisabler = (wxWindowDisabler *)NULL;
}

BEGIN_EVENT_TABLE(ProgressDialog, wxDialog)
    EVT_CLOSE(ProgressDialog::OnClose)
END_EVENT_TABLE()

