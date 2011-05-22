/* ExwordLibrary - Class used to storing and saving usernames and auth keys
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

#include <wx/filename.h>
#include <wx/file.h>

#include "ExwordDevice.h"
#include "Users.h"

Users::Users() : UserHash()
{
    wxString userFile;
    wxFile file;
    char * data;
    wxString key;
    int i = 0;
    userFile.Printf(wxT("%s%cusers.dat"), Exword::GetUserDataDir().c_str(), wxFileName::GetPathSeparator());
    if (wxFile::Exists(userFile.c_str()) &&
        wxFile::Access(userFile.c_str(), wxFile::read)) {
        if (file.Open(userFile.c_str(), wxFile::read)) {
            data = new char[file.Length()];
            file.Read(data, file.Length());
            while (i < file.Length()) {
                ++i;
                key = wxString::FromAscii(data + i);
                (*this)[key] = wxMemoryBuffer(20);
                i += strlen(data + i) + 1;
		((*this)[key]).AppendData(data + i, 20);
                i += 20;
            }
            delete [] data;
        }
    }
}

Users::~Users()
{
    wxString userFile;
    wxFile file;
    char *data;
    int str_len;
    Users::iterator it;
    userFile.Printf(wxT("%s%cusers.dat"), Exword::GetUserDataDir().c_str(), wxFileName::GetPathSeparator());
    if (wxFile::Access(userFile.c_str(), wxFile::write)) {
        if (file.Create(userFile.c_str(), wxFile::write)) {
            for(it = this->begin(); it != this->end(); ++it) {
                wxString key = it->first;
                wxMemoryBuffer value = it->second;
                str_len =  strlen(key.utf8_str().data());
                data = new char[22 + str_len];
                memset(data, 0, 22 + str_len);
                data[0] = str_len + 1;
                memcpy(data + 1, key.utf8_str().data(), str_len);
                memcpy(data + 2 + str_len, value.GetData(), 20);
                file.Write(data, 22 + str_len);
                delete [] data;
            }
        }
    }
}

