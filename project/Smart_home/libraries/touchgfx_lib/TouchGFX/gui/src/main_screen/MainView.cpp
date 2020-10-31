#include <gui/main_screen/MainView.hpp>
#include <math.h>
#include <stdlib.h>

MainView::MainView()
{
    tickCounter = 0;
}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();


}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::handleTickEvent()
{
    tickCounter++;


}

void MainView::sliderValueChanged(int value)
{

}
