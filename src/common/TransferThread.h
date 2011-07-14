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

#ifndef TRANSFERTHREAD_H
#define TRANSFERTHREAD_H

#include <wx/frame.h>
#include <wx/thread.h>

enum {
        myID_START = 1,
        myID_FINISH,
};

BEGIN_DECLARE_EVENT_TYPES()
        DECLARE_LOCAL_EVENT_TYPE(myEVT_THREAD, 1)
END_DECLARE_EVENT_TYPES()

class TransferThread : public wxThread
{
    public:
        TransferThread(wxFrame *frame);
        bool Start(void *);
        virtual void *Entry();
        virtual void OnExit();
        virtual void *Action() = 0;
        void FireEvent(wxString text, int id);
    protected:
        void *m_data;
        wxFrame *m_frame;
};

#endif
