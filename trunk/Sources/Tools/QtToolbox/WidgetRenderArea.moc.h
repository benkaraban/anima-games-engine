/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef WIDGET_RENDER_AREA_H_
#define WIDGET_RENDER_AREA_H_

#include <QWidget>

namespace QtToolbox
{

/**
* Definit une QWidget permettant d'afficher un rendue 3D.
* La classe gere les evenements de QT, mais ne se charge pas de rendu.
* L'etape du rendu est délégee a la classe fille par l'intermediaire de la fonction paintRenderArea.
*/
class LM_API_QTB WidgetRenderArea : public QWidget
{
    Q_OBJECT

public:

    explicit WidgetRenderArea(QWidget* parent=0, Qt::WindowFlags f=0);
    virtual ~WidgetRenderArea();


    /**
    */
    QPaintEngine * paintEngine () const;

    /**
    */
    void setMouseTracking(bool enable);

public Q_SLOTS:
    /**MAJ de la widget en appelant 3DDraw()
    */
    virtual void updateRenderArea();

   /* 
    virtual void startAnimate()=0;

    
    virtual void stopAnimate()=0;

   
    virtual void animate() = 0;*/

protected:
    bool event(QEvent *);
    /*!
    This virtual function is called once before the first call to
    paintGL() or resizeGL(), and then once whenever the widget has
    been assigned a new QGLContext. Reimplement it in a subclass.

    This function should set up any required OpenGL context rendering
    flags, defining display lists, etc.

    There is no need to call makeCurrent() because this has already
    been done when this function is called.
*/
    


    /**
    * Cette fonction est appelé lorsque la widget a ete redimentionnee.
    * Elle est appele par l'evenement resizeEvent
    * La widget dispose de sa nouvelle taille disponible par les methode width() et height().
    * La fonction doit être redefinie par les classe fille.
    * L'evenement paint est appele automatiquement apres levenement resize il est donc 
    * inutile de peindre la widget dans la fonction resize.
    */
    virtual void resizeRenderArea() = 0;

    /*!
    This virtual function is called whenever the widget needs to be
    painted. Reimplement it in a subclass.

    There is no need to call makeCurrent() because this has already
    been done when this function is called.
*/
    virtual void paintRenderArea() = 0;
protected:
    //virtual void initializeOverlayGL();
    //virtual void resizeOverlayGL(int w, int h);
    //virtual void paintOverlayGL();

    //void setAutoBufferSwap(bool on);
    //bool autoBufferSwap() const;

    /*!
    \fn void QGLWidget::paintEvent(QPaintEvent *event)

    Handles paint events passed in the \a event parameter. Will cause
    the virtual paintGL() function to be called.

    The widget's rendering context will become the current context and
    initializeGL() will be called if it hasn't already been called.
*/
    /**
    */
    void paintEvent(QPaintEvent*);

    /**
    */
    void resizeEvent(QResizeEvent*);

    /**
    */
    virtual void initializeRenderArea();

    /**
    * execute la fonction virtuel paintRenderArea()
    * appel initialize si il n'a pas ete appele avant
    */
    virtual void drawRenderArea();


private:
    Q_DISABLE_COPY(WidgetRenderArea)
};

} //namespace QtToolbox

#endif //WIDGET_RENDER_AREA_H_
