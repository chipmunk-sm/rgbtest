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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QCloseEvent>
#include <QtMath>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onCustomContextMenu(const QPoint &point);
private:

    typedef enum{
        menuAction_F1_Help = 1,
        menuAction_F2_Switch_color,
        menuAction_F3_Gradient_Solid,
        menuAction_F4_Gradient_direction,
        menuAction_F5_Switch_fullscreen_normal,
        menuAction_F6_Autoswitch_color,
        menuAction_ADD_Brightness,
        menuAction_SUB_Brightness
    }menuAction;

    typedef enum{
        GradientDirection0   = 0,
        GradientDirection45  = 45,
        GradientDirection90  = 90,
        GradientDirection135 = 135,
        GradientDirection180 = 180,
        GradientDirection225 = 225,
        GradientDirection270 = 270,
        GradientDirection315 = 315
    }GradientDirection;

    typedef enum{
        BackgroundColor_white,
        BackgroundColor_red,
        BackgroundColor_green,
        BackgroundColor_blue,
        BackgroundColor_black
    }BackgroundColor;

    Ui::MainWindow *m_ui;
    GradientDirection m_direction;
    BackgroundColor m_BackgroundColor;
    bool m_gradient;
    int m_Brightness;
    bool m_AutoswitchColor;

    void AddMenuAction(QMenu *mainMenu, QVariant actionId, QString actionName);
    void RunMenuAction(menuAction menuaction);
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    void setHelpOnOff(bool helpOn);
    void setHelpActiveColor(BackgroundColor activeColor);
    void setHelpActiveGradient(bool enableGradient);
    void setHelpActiveDirection(GradientDirection gradientdirection);
    void setHelpFullScreen(bool fullScreen);
    void setHelpBrightness(int brightnessNew, int brightnessOld);
    void setHelpAutoswitchColor(bool autoswitchEnable);
    void UpdateWindowBackground();
};

#endif // MAINWINDOW_H
