/* ExwordTools - Class used to retreive model information
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

#include "Model.h"

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>

Model Unknown;

ModelDatabase* ModelDatabase::m_self = 0;

ModelDatabase::ModelDatabase()
{
    wxString modelFile = wxString::Format(wxT("%s%c%s"),
                                          wxStandardPaths::Get().GetDataDir().c_str(),
                                          wxFileName::GetPathSeparator(),
                                          wxT("models.txt"));
    wxFileInputStream file(modelFile);
    unsigned long series;
    wxString modelString;
    wxString main;
    wxString sub;
    wxString ext;
    wxString key;
    if (file.IsOk()) {
        wxTextInputStream input(file);
        while (!file.Eof()) {
            wxString line = input.ReadLine();
            wxStringTokenizer tokens(line, wxT("\t "), wxTOKEN_STRTOK);
            if (tokens.CountTokens() >= 5 && tokens.GetNextToken() == wxT("Dataplus")) {
                if (!tokens.GetNextToken().ToULong(&series))
                    continue;
                modelString = tokens.GetNextToken();
                main = tokens.GetNextToken();
                sub = tokens.GetNextToken();
                if (sub == wxT("gy999"))
                    ext = tokens.GetNextToken();
                else
                    ext = wxT("");
                key.Printf(wxT("%s:%s:%s"), main.c_str(), sub.c_str(), ext.c_str());
                models[key] = Model(series, modelString);
            }
        }
    }
}

Model& ModelDatabase::Lookup(wxString main, wxString sub, wxString ext)
{
    wxString key;
    key.Printf(wxT("%s:%s:%s"), main.c_str(), sub.c_str(), ext.c_str());
    if (models.find(key) == models.end()) {
        wxLogMessage(_("Failed to find model information for device %s:%s:%s"), main.c_str(), sub.c_str(), ext.c_str());
        return Unknown;
    }
    return models[key];
}

ModelDatabase* ModelDatabase::Get()
{
    if (!m_self) {
        ModelDatabase::m_self = new ModelDatabase();
    }
    return ModelDatabase::m_self;
}

/*
/[Dd]ataplus\s+\d[A-Z]+-[A-Z]+\d+\s+gy\d{3},ON,\d{4}\s+gy\d{3}\s+(CY\d{3})?/
*/
