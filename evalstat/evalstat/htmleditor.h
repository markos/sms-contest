/***************************************************************************
                          htmleditor.h  -  description
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

#ifndef HTMLEDITOR_H
#define HTMLEDITOR_H


#include <vector>
#include <string>
#include <fstream>
using namespace std;

/**
  *@author Alex Davelos
  */

class HTMLEditor {
public:

   // Constructor
   HTMLEditor(const string& fname);
   // Destructor
   ~HTMLEditor();
   // Write Header to HTML Buffer
   void writeHeader();
   // Write Footer to HTML Buffer
   void writeFooter();
   // Set the html title string
   void setTitle(const string& title){html_title=title;}
   // Get the html title string
   const string& getTitle(){return html_title;}
  /** Return ofstream object (used from sqltable class) */
  ofstream& getOutput();
  /** No descriptions */
  void writeTableTitle(vector<string> titleData);
  /** No descriptions */
  void writeRow(vector<string> rowData);
  /** No descriptions */
   HTMLEditor(const HTMLEditor& edit);

   ofstream out;
   
protected:           
  /**This method is called only  from inside this class , or any subclass of HTMLEditor */
  void openFile();

private:     
   // Title of the page
   string html_title;
   //Pathname to the html file
   string filename;


};

#endif
