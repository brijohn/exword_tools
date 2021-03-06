/* ExwordTools - Base class used for threading
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


#include "ThreadBase.h"

DEFINE_LOCAL_EVENT_TYPE(myEVT_THREAD)

ThreadBase::ThreadBase(wxEvtHandler *evthandler)
        : wxThread(wxTHREAD_DETACHED)
{
    m_evthandler = evthandler;
    m_data = NULL;
}

bool ThreadBase::Start(void *data)
{
    m_data = data;
    if (this->Create() != wxTHREAD_NO_ERROR)
        return false;
    return (this->Run() == wxTHREAD_NO_ERROR);
}

void ThreadBase::OnExit()
{
}

void ThreadBase::FireEvent(wxString text, int id)
{
    wxCommandEvent event(myEVT_THREAD, id);
    event.SetString(text);
    wxPostEvent(m_evthandler, event);
}

void *ThreadBase::Entry()
{
    return Action();
}

