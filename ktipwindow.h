/*****************************************************************

Copyright (c) 2000, Matthias Hoelzer-Kluepfel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __ktipwindow_h__
#define __ktipwindow_h__



#include <qdialog.h>
#include <kapp.h>
#include <vector>


class QCheckBox;
class QPushButton;


class TipWindow : public QDialog
{
  Q_OBJECT

public:

  TipWindow(const QString &swallowCaption);

public slots:

  void nextTip();
  void prevTip();
  void startupClicked();

protected:
  void closeEvent(QCloseEvent *);

private:

  void loadTips();
  void loadFile(const QString &filename);

  QLabel *text;
  QCheckBox    *startup;
  QPushButton  *next, *prev, *ok;

  std::vector<QString>  tips;

  int current;

};


class TipApp : public KApplication
{
  Q_OBJECT

public:

  TipApp(int argc, char *argv[]);
  ~TipApp();

  void start(const QString &swallowCaption);


private:

  TipWindow *window;

};


#endif
