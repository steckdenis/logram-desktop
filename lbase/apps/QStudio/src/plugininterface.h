#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtGui>
/*QScintilla*/
#include <Qsci/qsciscintilla.h>
#include<Qsci/qscilexercpp.h>


class PluginInterface : public QObject
{
    public:
        virtual ~PluginInterface() {}

        virtual QString pluginName() const=0; //on retourne le nom du plugin (pour le menu)

        virtual void initialize(QsciScintilla *parent,QMainWindow *mainwindow) = 0; //initialisation du plugin, qui permet d'avoir accès au textEdit et à la fenêtre principale ;)

    public slots:
        virtual void Action() = 0; //ce que fait le plugin
};

Q_DECLARE_INTERFACE(PluginInterface,"org.logram-project.qstudio.PluginInterface/0.9")

#endif // PLUGININTERFACE_H

