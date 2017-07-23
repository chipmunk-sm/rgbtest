/* This file is part of rgbtest - LCD Dead Pixel Test
 * Copyright (C) 2017 chipmunk-sm <dannico@linuxmail.org>
 * https://github.com/chipmunk-sm/rgbtest
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_direction(GradientDirection0),
    m_BackgroundColor(BackgroundColor_white),
    m_gradient(false),
    m_Brightness(255),
    m_AutoswitchColor(false)
{

    m_ui->setupUi(this);

    setWindowTitle("rgbtest");
    setWindowIcon(QPixmap(":/data/rgbtest.svg"));

    m_ui->helpLabel->setText(
    tr(
    "<html>"
    "<head/>"
    "<body>"
    //"Copyright (C) 2017 chipmunk-sm <a href='mailto:dannico@linuxmail.org'>dannico@linuxmail.org</a><br>"
    "<a href='https://github.com/chipmunk-sm/rgbtest'>https://github.com/chipmunk-sm/rgbtest</a><br>"
    "<br>"
    "<b>F1</b> - Help <b>On</b>/<n>Off</n><br>"
    "<b>F2</b> – Switch color (<b>white</b>-<n>red</n>-<n>green</n>-<n>blue</n>-<n>black</n>)<br>"
    "<b>F3</b> – <n>Gradient</n> ↔ <b>Solid</b><br>"
    "<b>F4</b> – Gradient direction (<b>0</b>-<n>45</n>-<n>90</n>-<n>135</n>-<n>180</n>-<n>225</n>-<n>270</n>-<n>315</n>)<br>"
    "<b>F5</b> – <n>Fullscreen</n> ↔ <b>Normal</b><br>"
    //"<b>F6</b> – <n>Autoswitch color</n><br>"
    "<b>'+'</b> or <b>'-'</b> – <n>Brightnes</n> <b>255</b> (0-255)"
    "</body>"
    "</html>"
    ));

    m_ui->helpLabel->adjustSize();

    installEventFilter(this);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    restoreGeometry(settings.value("baseWindow/geometry").toByteArray());

    UpdateWindowBackground();

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::AddMenuAction(QMenu *mainMenu, QVariant actionId, QString actionName)
{
    auto qaction = new QAction(actionName, mainMenu);
    qaction->setData(actionId);
    mainMenu->addAction(qaction);
}

void MainWindow::RunMenuAction(menuAction menuaction)
{
    switch(menuaction)
    {
        case menuAction_F1_Help:
        {
            setHelpOnOff(m_ui->helpLabel->isVisible());
        }
        break;
        case menuAction_F2_Switch_color:
        {
            switch (m_BackgroundColor)
            {
                case BackgroundColor_white: m_BackgroundColor = BackgroundColor_red; break;
                case BackgroundColor_red:   m_BackgroundColor = BackgroundColor_green; break;
                case BackgroundColor_green: m_BackgroundColor = BackgroundColor_blue; break;
                case BackgroundColor_blue:  m_BackgroundColor = BackgroundColor_black; break;
                case BackgroundColor_black: m_BackgroundColor = BackgroundColor_white; break;
            }
            setHelpActiveColor(m_BackgroundColor);
            UpdateWindowBackground();
        }
        break;
        case menuAction_F3_Gradient_Solid:
        {
            m_gradient = !m_gradient;
            setHelpActiveGradient(m_gradient);
            UpdateWindowBackground();
        }
        break;
        case menuAction_F4_Gradient_direction:
        {
            switch (m_direction)
            {
                case GradientDirection0:   m_direction = GradientDirection45; break;
                case GradientDirection45:  m_direction = GradientDirection90; break;
                case GradientDirection90:  m_direction = GradientDirection135; break;
                case GradientDirection135: m_direction = GradientDirection180; break;
                case GradientDirection180: m_direction = GradientDirection225; break;
                case GradientDirection225: m_direction = GradientDirection270; break;
                case GradientDirection270: m_direction = GradientDirection315; break;
                case GradientDirection315: m_direction = GradientDirection0; break;
            }
            setHelpActiveDirection(m_direction);
            UpdateWindowBackground();
        }
        break;
        case menuAction_F5_Switch_fullscreen_normal:
        {
            setWindowState(windowState() == Qt::WindowFullScreen ? Qt::WindowNoState : Qt::WindowFullScreen);
            setHelpFullScreen(windowState() == Qt::WindowFullScreen);
            UpdateWindowBackground();
        }
        break;
        case menuAction_F6_Autoswitch_color:
        {
            m_AutoswitchColor = !m_AutoswitchColor;
            setHelpAutoswitchColor(m_AutoswitchColor);
        }
        break;
        case menuAction_ADD_Brightness:
        {
            auto tmpBrightness = m_Brightness;
            if(m_Brightness > 254)
                m_Brightness = 0;
            else
                m_Brightness++;

            setHelpBrightness(m_Brightness,tmpBrightness);
            UpdateWindowBackground();
        }
        break;
        case menuAction_SUB_Brightness:
        {

            auto tmpBrightness = m_Brightness;
            if(m_Brightness < 1)
                m_Brightness = 255;
            else
                m_Brightness--;

            setHelpBrightness(m_Brightness,tmpBrightness);
            UpdateWindowBackground();
        }
        break;
    }
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{

    Q_UNUSED(point);

    QMenu menu(this);

    AddMenuAction(&menu, menuAction_F1_Help,
                  QObject::tr("F1 - Help On/Off"));

    AddMenuAction(&menu, menuAction_F2_Switch_color,
                  QObject::tr("F2 – Switch color"));

    AddMenuAction(&menu, menuAction_F3_Gradient_Solid,
                  QObject::tr("F3 – Gradient ↔ Solid"));

    AddMenuAction(&menu, menuAction_F4_Gradient_direction,
                  QObject::tr("F4 – Gradient direction"));

    AddMenuAction(&menu, menuAction_F5_Switch_fullscreen_normal,
                  QObject::tr("F5 – Fullscreen ↔ Normal"));

    //    AddMenuAction(&menu, menuAction_F6_Autoswitch_color,
    //                  QObject::tr("F6 – Autoswitch color"));

    AddMenuAction(&menu, menuAction_ADD_Brightness,
                  QObject::tr("+ Brightness"));

    AddMenuAction(&menu, menuAction_SUB_Brightness,
                  QObject::tr("- Brightness"));

    auto ret = menu.exec(QCursor::pos());
    if(ret == nullptr)
        return;

    RunMenuAction((menuAction)ret->data().toInt());

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("baseWindow/geometry", saveGeometry());
    event->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch(keyEvent->key())
        {
            case Qt::Key_F1: RunMenuAction(menuAction_F1_Help); break;
            case Qt::Key_F2: RunMenuAction(menuAction_F2_Switch_color); break;
            case Qt::Key_F3: RunMenuAction(menuAction_F3_Gradient_Solid); break;
            case Qt::Key_F4: RunMenuAction(menuAction_F4_Gradient_direction); break;
            case Qt::Key_F5: RunMenuAction(menuAction_F5_Switch_fullscreen_normal); break;
            case Qt::Key_F6: RunMenuAction(menuAction_F6_Autoswitch_color); break;
            case Qt::Key_Plus:  RunMenuAction(menuAction_ADD_Brightness); break;
            case Qt::Key_Minus: RunMenuAction(menuAction_SUB_Brightness); break;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::setHelpOnOff(bool helpOn)
{
    //F1 - Help On/Off
    auto textVal = m_ui->helpLabel->text();

    textVal.replace(tr("<b>On</b>"), tr("<n>On</n>"), Qt::CaseInsensitive);
    textVal.replace(tr("<b>Off</b>"), tr("<n>Off</n>"), Qt::CaseInsensitive);

    if(helpOn)
        textVal.replace(tr("<n>On</n>"), tr("<b>On</b>"), Qt::CaseInsensitive);
    else
        textVal.replace(tr("<n>Off</n>"), tr("<b>Off</b>"), Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);

    m_ui->helpLabel->setVisible(!helpOn);

}

void MainWindow::setHelpActiveColor(BackgroundColor activeColor)
{
    //F2 – Switch color (white-red-green-blue-black)
    QString colorName;

    if(activeColor == BackgroundColor_white)
        colorName = tr("white");
    else if(activeColor == BackgroundColor_red)
        colorName = tr("red");
    else if(activeColor == BackgroundColor_green)
        colorName = tr("green");
    else if(activeColor == BackgroundColor_blue)
        colorName = tr("blue");
    else
        colorName = tr("black");

    auto textVal = m_ui->helpLabel->text();
    textVal.replace(tr("<b>white</b>"), tr("<n>white</n>"), Qt::CaseInsensitive);
    textVal.replace(tr("<b>red</b>"),   tr("<n>red</n>"),   Qt::CaseInsensitive);
    textVal.replace(tr("<b>green</b>"), tr("<n>green</n>"), Qt::CaseInsensitive);
    textVal.replace(tr("<b>blue</b>"),  tr("<n>blue</n>"),  Qt::CaseInsensitive);
    textVal.replace(tr("<b>black</b>"), tr("<n>black</n>"), Qt::CaseInsensitive);

    textVal.replace(
                "<n>" + colorName + "</n>",
                "<b>" + colorName + "</b>",
                Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::setHelpActiveGradient(bool enableGradient)
{
    //F3 – Gradient↔Solid
    auto textVal = m_ui->helpLabel->text();

    textVal.replace(tr("<b>Gradient</b>"), tr("<n>Gradient</n>"), Qt::CaseInsensitive);
    textVal.replace(tr("<b>Solid</b>"), tr("<n>Solid</n>"), Qt::CaseInsensitive);

    if(enableGradient)
        textVal.replace(tr("<n>Gradient</n>"), tr("<b>Gradient</b>"), Qt::CaseInsensitive);
    else
        textVal.replace(tr("<n>Solid</n>"), tr("<b>Solid</b>"), Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::setHelpActiveDirection(GradientDirection gradientdirection)
{
    //F4 – Gradient direction (0-45-90-135-180-225-270-315)
    QString directionName = QString::number((int)gradientdirection);
    auto textVal = m_ui->helpLabel->text();

    textVal.replace("<b>0</b>",   "<n>0</n>",   Qt::CaseInsensitive);
    textVal.replace("<b>45</b>",  "<n>45</n>",  Qt::CaseInsensitive);
    textVal.replace("<b>90</b>",  "<n>90</n>",  Qt::CaseInsensitive);
    textVal.replace("<b>135</b>", "<n>135</n>", Qt::CaseInsensitive);
    textVal.replace("<b>180</b>", "<n>180</n>", Qt::CaseInsensitive);
    textVal.replace("<b>225</b>", "<n>225</n>", Qt::CaseInsensitive);
    textVal.replace("<b>270</b>", "<n>270</n>", Qt::CaseInsensitive);
    textVal.replace("<b>315</b>", "<n>315</n>", Qt::CaseInsensitive);

    textVal.replace("<n>" + directionName + "</n>",
                    "<b>" + directionName + "</b>",
                    Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::setHelpFullScreen(bool fullScreen)
{
    //F5 – Fullscreen ↔ Normal
    auto textVal = m_ui->helpLabel->text();

    textVal.replace(tr("<b>Fullscreen</b>"), tr("<n>Fullscreen</n>"), Qt::CaseInsensitive);
    textVal.replace(tr("<b>Normal</b>"), tr("<n>Normal</n>"), Qt::CaseInsensitive);

    if(fullScreen)
        textVal.replace(tr("<n>Fullscreen</n>"), tr("<b>Fullscreen</b>"), Qt::CaseInsensitive);
    else
        textVal.replace(tr("<n>Normal</n>"), tr("<b>Normal</b>"), Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::setHelpBrightness(int brightnessNew, int brightnessOld)
{
    //'+' or '-' – Brightness
    auto textVal = m_ui->helpLabel->text();

    auto strBr1 = tr("<n>Brightnes</n> <b>");
    auto strBr2 = tr("</b>");

    auto strOld = strBr1 + QString::number(brightnessOld) + strBr2;
    auto strNew = strBr1 + QString::number(brightnessNew) + strBr2;

    textVal.replace(strOld, strNew, Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::setHelpAutoswitchColor(bool autoswitchEnable)
{
    //F7 – Autoswitch color
    auto textVal = m_ui->helpLabel->text();
    if(autoswitchEnable)
        textVal.replace(tr("<n>Autoswitch color</n>"), tr("<b>Autoswitch color</b>"), Qt::CaseInsensitive);
    else
        textVal.replace(tr("<b>Autoswitch color</b>"), tr("<n>Autoswitch color</n>"), Qt::CaseInsensitive);

    m_ui->helpLabel->setText(textVal);
}

void MainWindow::UpdateWindowBackground()
{

    QString styleWidowBackgroundColor;
    QString styleLabelForegroundColor = "white";

    if(m_BackgroundColor == BackgroundColor_white)
    {
        styleWidowBackgroundColor = tr("rgb(") +
                QString::number(m_Brightness) + tr(",") +
                QString::number(m_Brightness) + tr(",") +
                QString::number(m_Brightness) + tr(")");//

        if(m_Brightness > 160)
            styleLabelForegroundColor = "black";
    }
    else if(m_BackgroundColor == BackgroundColor_red)
    {
        styleWidowBackgroundColor = tr("rgb(") + QString::number(m_Brightness) + tr(",0,0)");//"red"
    }
    else if(m_BackgroundColor == BackgroundColor_green)
    {
        styleWidowBackgroundColor = tr("rgb(0,") + QString::number(m_Brightness) + tr(",0)");//"green"
    }
    else if(m_BackgroundColor == BackgroundColor_blue)
    {
        styleWidowBackgroundColor = tr("rgb(0,0,") + QString::number(m_Brightness) + tr(")");//"blue"
    }
    else if(m_BackgroundColor == BackgroundColor_black)
    {
        styleWidowBackgroundColor = "black";

    }

    if(m_gradient)
    {

        double x1 = 0.0;
        double y1 = 0.0;
        double x2 = 0.0;
        double y2 = 1.0;

        switch (m_direction)
        {
            case GradientDirection0:
            x1 = 1.0;
            y1 = 0.0;
            x2 = 0.0;
            y2 = 0.0;
            break;
            case GradientDirection45:
            x1 = 1.0;
            y1 = 1.0;
            x2 = 0.0;
            y2 = 0.0;
            break;
            case GradientDirection90:
            x1 = 0.0;
            y1 = 1.0;
            x2 = 0.0;
            y2 = 0.0;
            break;
            case GradientDirection135:
            x1 = 0.0;
            y1 = 1.0;
            x2 = 1.0;
            y2 = 0.0;
            break;
            case GradientDirection180:
            x1 = 0.0;
            y1 = 0.0;
            x2 = 1.0;
            y2 = 0.0;
            break;
            case GradientDirection225:
            x1 = 0.0;
            y1 = 0.0;
            x2 = 1.0;
            y2 = 1.0;
            break;
            case GradientDirection270:
            x1 = 0.0;
            y1 = 0.0;
            x2 = 0.0;
            y2 = 1.0;
            break;
            case GradientDirection315:
            x1 = 1.0;
            y1 = 0.0;
            x2 = 0.0;
            y2 = 1.0;
            break;
        }

        setStyleSheet("* {background: qlineargradient("
                      "x1: " + QString::number(x1) + ","
                      "y1: " + QString::number(y1) + ","
                      "x2: " + QString::number(x2) + ","
                      "y2: " + QString::number(y2) + ","
                      "stop: 0 " + styleWidowBackgroundColor + ", "
                      "stop: 1 black);}"
                      "QLabel {color: " + styleLabelForegroundColor + ";background:transparent;}"
                      "QMenu {background: Window;}");

    }
    else
    {
        setStyleSheet("* {background: " + styleWidowBackgroundColor + ";} "
                 "QLabel {color: " + styleLabelForegroundColor + ";background:transparent;}"
                  "QMenu {background: Window;}");
    }

}
