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


#include <stdlib.h>
#include <time.h>

#include <algorithm>


#include <qapp.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>


#include <kapp.h>
#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <kiconloader.h>


#include "ktipwindow.moc"


TipWindow::TipWindow()
  : QDialog(0,0,TRUE)
{
  setCaption(i18n("Kandalf's useful tips"));

  QVBoxLayout *vbox = new QVBoxLayout(this, 4);

  QHBoxLayout *hbox = new QHBoxLayout(4);
  vbox->addLayout(hbox, 5);

  text = new QLabel(this);
  text->setAlignment(WordBreak | AlignTop | AlignLeft);
  text->setMargin(10);
  text->setBackgroundColor(white);

  hbox->addWidget(text,1);

  QLabel *l = new QLabel(this);
  l->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QPixmap image(kapp->getIconLoader()->loadIcon("wizard.gif"));
  l->setPixmap(image);
  l->setFixedSize(image.size());
  hbox->addWidget(l,2);

  QFrame *f = new QFrame(this);
  f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(f,1);

  hbox = new QHBoxLayout(4);
  vbox->addLayout(hbox, 1);

  startup = new QCheckBox(i18n("Run on startup"), this);
  startup->setFixedHeight(startup->height());
  hbox->addWidget(startup, 2);

  prev = new QPushButton(i18n("&Previous"), this);
  prev->setFixedHeight(prev->height());
  hbox->addWidget(prev, 1);

  next = new QPushButton(i18n("&Next"), this);
  next->setFixedHeight(next->height());
  hbox->addWidget(next, 1);

  ok = new QPushButton(i18n("&OK"), this);
  ok->setFixedHeight(ok->height());
  ok->setDefault(true);
  hbox->addWidget(ok, 1);

  connect(next, SIGNAL(clicked()), this, SLOT(nextTip()));
  connect(prev, SIGNAL(clicked()), this, SLOT(prevTip()));
  connect(ok, SIGNAL(clicked()), kapp, SLOT(quit()));
  connect(startup, SIGNAL(clicked()), this, SLOT(startupClicked()));

  resize(550, 230);

  loadTips();
  if (!tips.empty()) {
      current = rand() % tips.size();
  } else {
      warning("Failed to load tips");
  }


  KConfig config("kdewizardrc");
  config.setGroup("General");
  startup->setChecked(config.readBoolEntry("TipsOnStart", true));

  nextTip();
}

void TipWindow::closeEvent(QCloseEvent *e)
{
  QDialog::closeEvent(e);
  kapp->quit();
}

void TipWindow::startupClicked()
{
  KConfig config("kdewizardrc");
  config.setGroup("General");
  config.writeEntry("TipsOnStart", startup->isChecked());
  config.sync();
}


// if you change something here, please update the script
// preparetips, which depends on extracting exactly the same
// text as done here.
void TipWindow::loadTips()
{
  tips.clear();
  const QString dirname = kapp->kde_datadir()+"/ktip/tips/";
  QDir dir(dirname);
  dir.setFilter(QDir::Files);
  for (unsigned i=0; i<dir.count(); i++) {
    loadFile(dirname + dir[i]);
  }
  if (tips.empty()) {
      tips.push_back(i18n("Failed to load tips."));
  }
  std::random_shuffle(tips.begin(), tips.end());
}

void TipWindow::loadFile(const QString &fname)
{
  if (fname.isEmpty())
    return;

  QFile f(fname);
  if (f.open(IO_ReadOnly))
    {
      QTextStream ts(&f);

      QString line, tag, tip;
      bool inTip = false;
      while (!ts.eof())
	{
	  line = ts.readLine();
	  tag = line.stripWhiteSpace().lower();
          if (tag == "%") {
              tip = tip.stripWhiteSpace();
              if (tip.isEmpty()) {
                  tip = "";
                  continue;
              }
              tips.push_back(tip);
              tip = "";
          } else {
              tip.append(line).append("\n");
          }
	}

      f.close();
    }
}


void TipWindow::nextTip()
{
  current += 1;
  if (current >= tips.size())
    current = 0;

  text->setText(tips.at(current));
}


void TipWindow::prevTip()
{
  current -= 1;
  if (current < 0)
    current = tips.size()-1;

  text->setText(tips.at(current));
}

void TipApp::start()
{
  window = new TipWindow;
  setMainWidget(window);
  window->show();
}

TipApp::TipApp(int argc, char *argv[]) : KApplication(argc, argv)
{
  window = NULL;
}


TipApp::~TipApp()
{
  delete window;
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  bool isInit = false;
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "-init") == 0) {
      isInit = true;
      break;
    }
  }

  TipApp app(argc, argv);
  if (isInit) {
    KConfig config("kdewizardrc");
    config.setGroup("General");

    if (!(config.readBoolEntry("TipsOnStart", true))) {
      return 0;
    }
  }
  app.start();

  return 0;
}
