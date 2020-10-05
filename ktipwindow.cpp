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


#include <qapp.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtextbrowser.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>


#include <kapp.h>
#include <kuniqueapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kwin.h>


#include "ktipwindow.moc"


TipWindow::TipWindow()
  : KDialog(0,0,false)
{
    
  KWin::setState( winId(), NET::StaysOnTop );
  setCaption(i18n("Kandalf's useful tips"));

  QVBoxLayout *vbox = new QVBoxLayout(this, 4);

  QHBoxLayout *hbox = new QHBoxLayout(vbox, 4);

  text = new QTextBrowser(this);
  text->mimeSourceFactory()->addFilePath(KGlobal::dirs()->findResourceDir("data", "kdewizard/pics")+"kdewizard/pics/");
  QStringList icons = KGlobal::dirs()->resourceDirs("icon");
  QStringList::Iterator it;
  for (it = icons.begin(); it != icons.end(); ++it)
    text->mimeSourceFactory()->addFilePath(*it);
  hbox->addWidget(text);

  QLabel *l = new QLabel(this);
  l->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  l->setPixmap(locate("data", "kdewizard/pics/wizard_small.png"));
  l->setBackgroundColor(Qt::white);
  hbox->addWidget(l);

  QFrame *f = new QFrame(this);
  f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(f);

  hbox = new QHBoxLayout(vbox, 4);

  startup = new QCheckBox(i18n("Run on startup"), this);
  hbox->addWidget(startup, 1);

  prev = new QPushButton(i18n("&Previous"), this);
  hbox->addWidget(prev);

  next = new QPushButton(i18n("&Next"), this);
  hbox->addWidget(next);

  ok = new QPushButton(i18n("&OK"), this);
  ok->setDefault(true);
  hbox->addWidget(ok);

  connect(next, SIGNAL(clicked()), this, SLOT(nextTip()));
  connect(prev, SIGNAL(clicked()), this, SLOT(prevTip()));
  connect(ok, SIGNAL(clicked()), kapp, SLOT(quit()));
  connect(startup, SIGNAL(clicked()), this, SLOT(startupClicked()));

  resize(550, 230);

  loadTips();

  current = kapp->random() % tips.count();

  KConfig *config = new KConfig("kdewizardrc", true);
  config->setGroup("General");
  startup->setChecked(config->readBoolEntry("TipsOnStart", true));

  nextTip();
}


void TipWindow::startupClicked()
{
  KConfig *config = new KConfig("kdewizardrc");
  config->setGroup("General");
  config->writeEntry("TipsOnStart", startup->isChecked());
  config->sync();
}


// if you change something here, please update the script
// preparetips, which depends on extracting exactly the same
// text as done here.
void TipWindow::loadTips()
{
  QString fname;

  fname = locate("data", QString("kdewizard/tips"));

  if (fname.isEmpty())
    return;

  tips.clear();

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
	
	  if (tag == "<html>")
	    {
	      inTip = true;
	      tip = QString::null;
	      continue;
	    }
	
	  if (inTip)
	    {
	      if (tag == "</html>")
		{
		  tips.append(tip);
		  inTip = false;
		}
	      else
		tip.append(line).append("\n");
	    }

	}

      f.close();
    }
}


void TipWindow::nextTip()
{
  current += 1;
  if (current >= tips.count())
    current = 0;
  text->setText(QString("<html>%1</html>").arg(i18n(tips[current].latin1())));
}


void TipWindow::prevTip()
{
  current -= 1;
  if (current < 0)
    current = tips.count()-1;
  text->setText(QString("<html>%1</html>").arg(i18n(tips[current].latin1())));
}


TipApp::TipApp() : KUniqueApplication()
{
  window = new TipWindow;
  window->show();

  setMainWidget(window);

  connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));
}


TipApp::~TipApp()
{
  delete window;
}


static const char *description = I18N_NOOP("Kandalf's tips");


int main(int argc, char *argv[])
{
  KAboutData aboutData("ktip", I18N_NOOP("KTip"),
    "0.2", description, KAboutData::License_GPL,
    "(c) 1998-2000, KDE Developers");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KUniqueApplication::addCmdLineOptions();

  if (!KUniqueApplication::start())
    exit(-1);

  TipApp app;

  return app.exec();
}
