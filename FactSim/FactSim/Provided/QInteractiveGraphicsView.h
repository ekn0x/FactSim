#ifndef Q_INTERACTIVE_GRAPHICS_VIEW_H
#define Q_INTERACTIVE_GRAPHICS_VIEW_H


#include <QGraphicsView>
#include <QtMath>
#include <QCursor>
#include <QScrollBar>

//
//																			INTERACTION BEHAVIOR TABLE
//
// +----------------------------++----------------------------------------------------------------------------------------------------------------------------------------------+
// |							||																	Modifier*																	|
// | User interaction			++----------------------------------+-----------------------------------+-----------------------------------+-----------------------------------+
// |							||	No modifiers					|	SHIFT modifier					|	CTRL modifier					|	ALT modifier					|
// +============================++==================================+===================================+===================================+===================================+  --+   *				  --+   *
// |							||									| Recenter 							| Reset								| Reset								|    |   CTRL + SHIFT		|   CTRL + ALT + SHIFT
// | Mouse left double click	||									| the view							| scale								| rotation							|    +-- modifiers will		+-- modifiers will
// |							||									|									| 1.0								| 0.0								|    |   reset the view		|   fit all scene object
// +----------------------------++----------------------------------+-----------------------------------+-----------------------------------+-----------------------------------+  --+					  --+   in center view
// |							||									| Translation						| Scaling							| Rotation							|
// | Mouse left click and move	||									| according to						| according to						| according to current view			|
// |							||									| current view						| mouseMoveScaleSensitivity			| defined by relative position		|
// |							||									|									| and relative x position			|									|
// +----------------------------++----------------------------------+-----------------------------------+-----------------------------------+-----------------------------------+
// |							|| Translation						| Inverted wheel					| Scaling							| Rotation							|
// | Mouse wheel**				|| according to						| translation						| according to						| according to						|
// | 							|| mouseWheelTranslationSensitivity	| according to						| mouseWheelScaleSensitivity		| mouseWheelRotationSensitivity		|
// |							|| [primary and secondary wheel]	| mouseWheelTranslationSensitivity	| [primary or secondary wheel]		| [primary or secondary wheel]		|
// |							||									| [primary and secondary wheel]		|									|									|
// +----------------------------++----------------------------------+-----------------------------------+-----------------------------------+-----------------------------------+
// |							||									| Translation						| Scaling							| Rotation							|
// | Arrow keys***				||									| according to						| according to						| according to						|
// | >  V  ^  <					||									| keyTranslationSensitivity			| keyScaleSensitivity				| keyRotationSensitivity			|
// +----------------------------++----------------------------------+-----------------------------------+-----------------------------------+-----------------------------------+
// | Escape key					||									|																											|
// | while mouse move			||									|											Cancel interaction												|
// | interaction is active		||									|																											|
// +----------------------------++----------------------------------+-----------------------------------------------------------------------------------------------------------+
//
// * All modifiers are mutualy exclusive. Except for left double click with CTRL + SHIFT  &  CTRL + ALT + SHIFT
// ** The standard QGraphicsView mouse wheel behavior is redefined.
// *** The standard QGraphicsView key arrow behavior is redefined.
//
//
// Les curseurs assignés à la classe deviennent la propriété de la classe. Ils doivent être alloué dynamiquement.
//
//

class QInteractiveGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	QInteractiveGraphicsView(QGraphicsScene * scene, QWidget * parent = nullptr);
	~QInteractiveGraphicsView();

	void alignSceneToView(QPointF scenePos, QPoint viewPos);

	QPointF currentTranslation() const { return QPointF(horizontalScrollBar()->value(), verticalScrollBar()->value()); }
	qreal currentScale() const { return mCurrentScale; }
	qreal currentRotation() const { return mCurrentRotation; }

	QPointF currentInteractiveTranslation() const { return QPointF(horizontalScrollBar()->value(), verticalScrollBar()->value()); }
	qreal currentInteractiveScale() const { return mCurrentInteractiveScale; }
	qreal currentInteractiveRotation() const { return mCurrentInteractiveRotation; }


	// Redefinition (not override)
	void fitInView();
	void setSceneRect(QRectF const & rect);

	// Getter / Setter
	bool translationInteractionEnabled() const { return mTranslationInteractionEnabled; }
	bool scaleInteractionEnabled() const { return mScaleInteractionEnabled; }
	bool rotationInteractionEnabled() const { return mRotationInteractionEnabled; }
	void setTranslationInteractionEnabled(bool enabled) { mTranslationInteractionEnabled = enabled; }
	void setScaleInteractionEnabled(bool enabled) { mScaleInteractionEnabled = enabled; }
	void setRotationInteractionEnabled(bool enabled) { mRotationInteractionEnabled = enabled; }
	void setInteractionEnabled(bool enabled) { mTranslationInteractionEnabled = mScaleInteractionEnabled = mRotationInteractionEnabled = enabled; }

	// in zoom factor (x time)
	qreal minimumScale() const { return mMinimumScale; }
	void setMinimumScale(qreal minimum) { mMinimumScale = minimum; }
	// in zoom factor (x time)
	qreal maximumScale() const { return mMaximumScale; }
	void setMaximumScale(qreal maximum) { mMaximumScale = maximum; }

	// in : pixels / 2x zoom factor (when zoom is 1x)
	qreal mouseMoveScaleSensitivity() const { return mMouseMoveScaleSensitivity; }
	void setMouseMoveScaleSensitivity(qreal sensitivity) { mMouseMoveScaleSensitivity = sensitivity; }

	// in : pixels / degree (when zoom is 1x)
	qreal mouseWheelTranslationSensitivity() const { return mMouseWheelTranslationSensitivity; }
	void setMouseWheelTranslationSensitivity(qreal sensitivity) { mMouseWheelTranslationSensitivity = sensitivity; }
	// in : mouse wheel degrees / 2x zoom factor
	qreal mouseWheelScaleSensitivity() const { return mMouseWheelScaleSensitivity; }
	void setMouseWheelScaleSensitivity(qreal sensitivity) { mMouseWheelScaleSensitivity = sensitivity; }//qPow(2.0, 1.0 / sensitivity); }
	// in : mouse wheel degree / view degree
	qreal mouseWheelRotationSensitivity() const { return mMouseWheelRotationSensitivity; }
	void setMouseWheelRotationSensitivity(qreal sensitivity) { mMouseWheelRotationSensitivity = sensitivity; }

	// in : pixels / hit
	qreal keyTranslationSensitivity() const { return mKeyTranslationSensitivity; }
	void setKeyTranslationSensitivity(qreal sensibility) { mKeyTranslationSensitivity = sensibility; }
	// in : x zoom factor / hit
	qreal keyScaleSensitivity() const { return mKeyScaleSensitivity; }
	void setKeyScaleSensitivity(qreal sensibility) { mKeyScaleSensitivity = sensibility; }
	// in : degree / hit
	qreal keyRotationSensitivity() const { return qRadiansToDegrees(mKeyRotationSensitivity); }
	void setKeyRotationSensitivity(qreal sensibility) { mKeyRotationSensitivity = qDegreesToRadians(sensibility); }

	bool persistentInteractionVisualCue() const { return mPersistentInteractionVisualCue; }
	QPen persistentInteractionVisualCuePen() const { return mPersistentInteractionVisualCuePen; }
	void setPersistentInteractionVisualCue(bool visualCue) { mPersistentInteractionVisualCue = visualCue; }
	void setPersistentInteractionVisualCuePen(QPen const & pen) { mPersistentInteractionVisualCuePen = pen; }

	QCursor* persistentInteractionCursorTranslating() const { return mPersistentInteractionCursorTranslating; }
	QCursor* persistentInteractionCursorScaling() const { return mPersistentInteractionCursorScaling; }
	QCursor* persistentInteractionCursorRotating() const { return mPersistentInteractionCursorRotating; }
	void setPersistentInteractionCursorTranslating(QCursor * cursor) { if (mPersistentInteractionCursorTranslating) delete mPersistentInteractionCursorTranslating; mPersistentInteractionCursorTranslating = cursor; }
	void setPersistentInteractionCursorScaling(QCursor * cursor) { if (mPersistentInteractionCursorTranslating) delete mPersistentInteractionCursorTranslating; mPersistentInteractionCursorScaling = cursor; }
	void setPersistentInteractionCursorRotating(QCursor * cursor) { if (mPersistentInteractionCursorTranslating) delete mPersistentInteractionCursorTranslating; mPersistentInteractionCursorRotating = cursor; }

signals:
	void viewChanged();
	void viewInteracted();

private:
	using QGraphicsView::fitInView;
	using QGraphicsView::setSceneRect;

protected:
	bool mTranslationInteractionEnabled{ true };
	bool mScaleInteractionEnabled{ true };
	bool mRotationInteractionEnabled{ true };

	enum class PersistentInteractionMode { None, MouseTranslating, MouseScaling, MouseRotating };
	PersistentInteractionMode mPersistentInteractionMode{ PersistentInteractionMode::None };
	bool mPersistentInteractionVisualCue{ false };
	QPen mPersistentInteractionVisualCuePen{ QPen(Qt::black, 1.0) };
	QCursor * mPersistentInteractionCursorTranslating{ nullptr };
	QCursor * mPersistentInteractionCursorScaling{ nullptr };
	QCursor * mPersistentInteractionCursorRotating{ nullptr };

	QPoint mPreviousMousePos, mCurrentMousePos;
	QPoint mInteractiveRefPos;
	QPointF mInteractiveRefScenePos;
	qreal mCurrentScale{ 1.0 };
	qreal mCurrentRotation{ 0.0 };
	qreal mCurrentInteractiveScale{ mCurrentScale };
	qreal mCurrentInteractiveRotation{ mCurrentRotation };
	void applyTranslation(QPointF const & delta);
	void applyTransformationView(qreal interactiveRotationValue, qreal interactiveScaleValue, bool interactiveAction);
	qreal getInteractiveScaling(qreal delta, qreal scaleSensitivity);
	qreal getInteractiveRotation(QPointF const & delta);
	void setInteractiveRefPos(QPoint const & pos);
	void updateMousePosition(QMouseEvent * event);

	qreal mMinimumScale{ 0.01 }; 
	qreal mMaximumScale{ 100.0 };
	qreal mMouseMoveScaleSensitivity{ 250.0 };
	qreal mMouseWheelTranslationSensitivity{ 4.0 };
	qreal mMouseWheelScaleSensitivity{ 360.0 };// { qPow(2.0 / 1.0, 1 / (360.0 / 1.0)) };
	qreal mMouseWheelRotationSensitivity{ 1.0 };
	qreal mKeyTranslationSensitivity{ 20.0 };
	qreal mKeyScaleSensitivity{ 1.05 };
	qreal mKeyRotationSensitivity{ qDegreesToRadians(15.0) };

	virtual void keyPressEvent(QKeyEvent * event) override;
	virtual void keyReleaseEvent(QKeyEvent * event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent * event) override;
	virtual void mouseMoveEvent(QMouseEvent * event) override;
	virtual void mousePressEvent(QMouseEvent * event) override;
	virtual void mouseReleaseEvent(QMouseEvent * event) override;
	virtual void wheelEvent(QWheelEvent * event) override;
	virtual void paintEvent(QPaintEvent *event) override;
};


#endif // Q_INTERACTIVE_GRAPHICS_VIEW_H
