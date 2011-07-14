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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <wx/utils.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/stattext.h>

class ProgressDialog : public wxDialog
{
    public:
        ProgressDialog(wxWindow *parent, wxString message);
        virtual ~ProgressDialog();
        virtual void Update(int value, const wxString& newmsg = wxEmptyString);
        virtual void Pulse();
        virtual void Update() { wxDialog::Update(); };
        virtual bool Show(bool show = true);
        void SetPulseMode(bool pulse = true) { m_pulse = pulse; }
        void OnClose(wxCloseEvent& event);

    private:
        bool m_pulse;
        wxGauge *m_gauge;
        wxStaticText *m_message;
        wxWindowDisabler *m_winDisabler;

    DECLARE_EVENT_TABLE();
};

#endif
