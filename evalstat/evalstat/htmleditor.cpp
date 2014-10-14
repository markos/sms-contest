/***************************************************************************
                          htmleditor.cpp  -  description
                             -------------------
    begin                : ??? ???? 18 2003
    copyright            : (C) 2003 by Alex Davelos
    email                : dav@bullet.gr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "htmleditor.h"

HTMLEditor::HTMLEditor(const string & fname)
{
        html_title = "Title";
        filename = fname;
        openFile ( );
}
HTMLEditor::~HTMLEditor()
{
  out.close();      
}

void HTMLEditor::writeHeader()
{
    out << "<HEAD>" << endl;
    out << "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-1\">" << endl;
    out << "<META NAME=\"Description\" CONTENT=\"" + getTitle() + "\">" << endl;
    out << "<TITLE>" + getTitle() + "</TITLE>" << endl;
    out << "</HEAD>" << endl;
    out << "<H2 align=center>Vodafone SMS Competition Results</H2>" << endl;
    out << "<BODY TEXT=\"#000000\" BGCOLOR=red COLOR=white>\n" <<endl;
    out << "<TABLE border=1 align=center cellpadding=5>\n" <<endl;
}

void HTMLEditor::writeFooter()
{
    out << "</TABLE>\n";
    out << "</BODY>\n";
    out << "</HTML>\n";
}


/** No descriptions */
void HTMLEditor::openFile(){
  out.open(filename.c_str(),ios::out);
}
/** No descriptions */
ofstream& HTMLEditor::getOutput(){
  return out;
}
/** No descriptions */
void HTMLEditor::writeRow(vector<string> rowData){

     out<< "<TR>";
     vector<string>::iterator i;
     for ( i = rowData.begin(); i !=rowData.end(); i++)
     {
            out<< "<TD>" << *i << "</TD>";
     }
     out << "</TR>\n";
}
/** No descriptions */
void HTMLEditor::writeTableTitle(vector<string> titleData){

     out<< "<TR>";
     vector<string>::iterator i;
     for ( i = titleData.begin(); i !=titleData.end(); i++)
     {
            out<< "<TH>" << *i << "</TH>";
     }
     out << "</TR>";
     
}
/** No descriptions */
 HTMLEditor::HTMLEditor(const HTMLEditor& edit){
   
}
