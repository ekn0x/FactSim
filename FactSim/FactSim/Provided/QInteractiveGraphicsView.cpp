#include "QInteractiveGraphicsView.h"


#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QScrollBar>

#include <QGuiApplication>

//#include "QInteractiveCursorFactory.h"

//// tools functions
static const qreal PI_2{ 6.283185307179586476925286766559 };
QPointF swap(QPointF const  & p);
qreal maxAbs(QPointF const  & p);
qreal square(qreal v);
qreal limit(qreal min, qreal value, qreal max);
qreal negFMod(qreal value, qreal mod); // mod > 0
QPointF translationFrom(QTransform const & transform);
QPointF scaleFrom(QTransform const & transform);
qreal singleScaleFrom(QTransform const & transform);
qreal rotationFrom(QTransform const & transform);
void drawCrossAngle(QPainter & painter, qreal centerX, qreal centerY, qreal angleRad, qreal lengthFromCenter, qreal offsetFromCenter = 0.0);
//// - - - - - - - - - - - -


QInteractiveGraphicsView::QInteractiveGraphicsView(QGraphicsScene * scene, QWidget * parent)
	: QGraphicsView(scene, parent)
{
	setTransformationAnchor(QGraphicsView::NoAnchor);

	connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &QInteractiveGraphicsView::viewChanged);
	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &QInteractiveGraphicsView::viewChanged);

	//mPersistentInteractionCursorTranslating = new QCursor(QInteractiveCursorFactory::getTranslationCursor(QSize(64, 64)));
	//mPersistentInteractionCursorScaling = new QCursor(QInteractiveCursorFactory::getScaleCursor(QSize(64, 64)));
	//mPersistentInteractionCursorRotating = new QCursor(QInteractiveCursorFactory::getRotationCursor(QSize(64, 64)));
}

QInteractiveGraphicsView::~QInteractiveGraphicsView()
{
	if (mPersistentInteractionCursorTranslating) delete mPersistentInteractionCursorTranslating;
	if (mPersistentInteractionCursorScaling) delete mPersistentInteractionCursorScaling;
	if (mPersistentInteractionCursorRotating) delete mPersistentInteractionCursorRotating;
}

void QInteractiveGraphicsView::fitInView()
{
	QGraphicsView::fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);

	mCurrentScale = singleScaleFrom(transform());
}

void QInteractiveGraphicsView::setSceneRect(QRectF const & rect)
{
	QGraphicsView::setSceneRect(rect);

	mCurrentScale = singleScaleFrom(transform());
}

void QInteractiveGraphicsView::applyTranslation(QPointF const & delta)
{
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
	verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
}

void QInteractiveGraphicsView::applyTransformationView(qreal interactiveRotationValue, qreal interactiveScaleValue, bool interactiveAction)
{
	resetTransform();
	mCurrentInteractiveScale = interactiveScaleValue;
	mCurrentInteractiveRotation = interactiveRotationValue;
	qreal currentInteractiveRotation = negFMod(mCurrentRotation + interactiveRotationValue, PI_2);
	qreal currentInteractiveScale = limit(mMinimumScale, mCurrentScale * interactiveScaleValue, mMaximumScale);

	if (interactiveAction) {
		rotate(qRadiansToDegrees(currentInteractiveRotation));
		scale(currentInteractiveScale, currentInteractiveScale);
	} else {
		mCurrentRotation = currentInteractiveRotation;
		mCurrentScale = currentInteractiveScale;
		rotate(qRadiansToDegrees(mCurrentRotation));
		scale(mCurrentScale, mCurrentScale);
	}

	alignSceneToView(mInteractiveRefScenePos, mInteractiveRefPos);
}
void QInteractiveGraphicsView::alignSceneToView(QPointF scenePos, QPoint viewPos)
{
	const QPointF translation = mapFromScene(scenePos) - viewPos;
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + translation.x());
	verticalScrollBar()->setValue(verticalScrollBar()->value() + translation.y());
}
qreal QInteractiveGraphicsView::getInteractiveScaling(qreal delta, qreal scaleSensitivity)
{
	return qPow(2.0, delta / scaleSensitivity);
}
qreal QInteractiveGraphicsView::getInteractiveRotation(QPointF const & delta)
{
	return qAtan2(delta.y(), delta.x());
}


void QInteractiveGraphicsView::keyPressEvent(QKeyEvent * event)
{
	int key{ event->key() };

	if (mPersistentInteractionMode != PersistentInteractionMode::None && key == Qt::Key_Escape) {
		switch (mPersistentInteractionMode) {
			case PersistentInteractionMode::MouseTranslating:
				alignSceneToView(mInteractiveRefScenePos, mInteractiveRefPos);
				break;
			case PersistentInteractionMode::MouseScaling:
				applyTransformationView(0.0, 1.0, false);
				break;
			case PersistentInteractionMode::MouseRotating:
				applyTransformationView(0.0, 1.0, false);
				break;
		}
		mPersistentInteractionMode = PersistentInteractionMode::None;
		QGuiApplication::restoreOverrideCursor();
	} else if (key >= Qt::Key_Left && key <= Qt::Key_Down) {
		switch (event->modifiers()) {
			case Qt::NoModifier: 
				break;
			case Qt::ShiftModifier:
				if (mTranslationInteractionEnabled) {
					switch (key) {
						case Qt::Key_Left:
							applyTranslation(QPointF(mKeyTranslationSensitivity / mCurrentScale, 0.0));
							break;
						case Qt::Key_Up:
							applyTranslation(QPointF(0.0, mKeyTranslationSensitivity / mCurrentScale));
							break;
						case Qt::Key_Right:
							applyTranslation(QPointF(-mKeyTranslationSensitivity / mCurrentScale, 0.0));
							break;
						case Qt::Key_Down:
							applyTranslation(QPointF(0.0, -mKeyTranslationSensitivity / mCurrentScale));
							break;
					}
					emit(viewChanged());
				}
				break;
			case Qt::ControlModifier:
				if (mScaleInteractionEnabled) {
					setInteractiveRefPos(viewport()->rect().center());
					switch (key) {
						case Qt::Key_Left:
						case Qt::Key_Down:
							applyTransformationView(0.0, 1.0 / mKeyScaleSensitivity, false);
							break;
						case Qt::Key_Right:
						case Qt::Key_Up:
							applyTransformationView(0.0, mKeyScaleSensitivity, false);
							break;
					}
					emit(viewChanged());
				}
				break;
			case Qt::AltModifier:
				if (mRotationInteractionEnabled) {
					setInteractiveRefPos(viewport()->rect().center());
					switch (key) {
						case Qt::Key_Left:
						case Qt::Key_Down:
							applyTransformationView(-mKeyRotationSensitivity, 1.0, false);
							break;
						case Qt::Key_Right:
						case Qt::Key_Up:
							applyTransformationView(mKeyRotationSensitivity, 1.0, false);
							break;
					}
					emit(viewChanged());
				}
				break;
		}
	} else {
		QGraphicsView::keyPressEvent(event);
	}
}

void QInteractiveGraphicsView::keyReleaseEvent(QKeyEvent * event)
{
	QGraphicsView::keyReleaseEvent(event);
}

void QInteractiveGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
	switch (event->buttons()) {
		case Qt::LeftButton:
			switch (event->modifiers()) {
				case Qt::NoModifier: 
					break;
				case Qt::ShiftModifier:
					if (mTranslationInteractionEnabled) {
						centerOn(mapToScene(event->pos()));
						emit(viewChanged());
					}
					break;
				case Qt::ControlModifier:
					if (mScaleInteractionEnabled) {
						setInteractiveRefPos(event->pos());
						mCurrentScale = 1.0;
						applyTransformationView(0.0, 1.0, false);
						emit(viewChanged());
					}
					break;
				case Qt::AltModifier:
					if (mRotationInteractionEnabled) {
						setInteractiveRefPos(event->pos());
						mCurrentRotation = 0.0;
						applyTransformationView(0.0, 1.0, false);
						emit(viewChanged());
					}
					break;
				case static_cast<int>(Qt::ShiftModifier) | static_cast<int>(Qt::ControlModifier):
					if (mTranslationInteractionEnabled && mScaleInteractionEnabled && mRotationInteractionEnabled) {
						mCurrentScale = 1.0;
						mCurrentRotation = 0.0;
						applyTransformationView(0.0, 1.0, false);
						centerOn(scene()->sceneRect().center());
						emit(viewChanged());
					}
					break;
				case static_cast<int>(Qt::ShiftModifier) | static_cast<int>(Qt::AltModifier) | static_cast<int>(Qt::ControlModifier):
					if (mTranslationInteractionEnabled && mScaleInteractionEnabled && mRotationInteractionEnabled) {
						fitInView();
						emit(viewChanged());
					}
					break;
			}
			break;
	}

	QGraphicsView::mouseDoubleClickEvent(event);
}

void QInteractiveGraphicsView::updateMousePosition(QMouseEvent * event)
{
	mPreviousMousePos = mCurrentMousePos;
	mCurrentMousePos = event->pos();
}

void QInteractiveGraphicsView::mouseMoveEvent(QMouseEvent * event)
{
	updateMousePosition(event);

	switch (mPersistentInteractionMode) {
		case PersistentInteractionMode::MouseTranslating:
			applyTranslation(mCurrentMousePos - mPreviousMousePos);
			emit(viewInteracted());
			break;
		case PersistentInteractionMode::MouseScaling:
			applyTransformationView(0.0, getInteractiveScaling((mCurrentMousePos - mInteractiveRefPos).x(), mMouseMoveScaleSensitivity), true);
			emit(viewInteracted());
			break;
		case PersistentInteractionMode::MouseRotating:
			applyTransformationView(getInteractiveRotation(mCurrentMousePos - mInteractiveRefPos), 1.0, true);
			emit(viewInteracted());
			break;
	}

	QGraphicsView::mouseMoveEvent(event);
}

void QInteractiveGraphicsView::setInteractiveRefPos(QPoint const & pos)
{
	mInteractiveRefPos = pos;
	mInteractiveRefScenePos = mapToScene(mInteractiveRefPos);
}

void QInteractiveGraphicsView::mousePressEvent(QMouseEvent * event)
{
	updateMousePosition(event);
	setInteractiveRefPos(event->pos());

	mPersistentInteractionMode = PersistentInteractionMode::None;
	//if (scene()->itemAt(mCurrentMousePos, transform()) == nullptr) {
		switch (event->buttons()) {
			case Qt::LeftButton:
				switch (event->modifiers()) {
					case Qt::NoModifier: 
						break;
					case Qt::ShiftModifier:
						if (mTranslationInteractionEnabled) {
							mPersistentInteractionMode = PersistentInteractionMode::MouseTranslating;
							if (mPersistentInteractionCursorTranslating) QGuiApplication::setOverrideCursor(*mPersistentInteractionCursorTranslating);
						}
						break;
					case Qt::ControlModifier:
						if (mScaleInteractionEnabled) {
							mPersistentInteractionMode = PersistentInteractionMode::MouseScaling;
							if (mPersistentInteractionCursorScaling) QGuiApplication::setOverrideCursor(*mPersistentInteractionCursorScaling);
						}
						break;
					case Qt::AltModifier:
						if (mRotationInteractionEnabled) {
							mPersistentInteractionMode = PersistentInteractionMode::MouseRotating;
							if (mPersistentInteractionCursorRotating) QGuiApplication::setOverrideCursor(*mPersistentInteractionCursorRotating);
						}
						break;
				}
				break;
		}
	//}

	QGraphicsView::mousePressEvent(event);
}

void QInteractiveGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
	if (mPersistentInteractionMode != PersistentInteractionMode::None) {
		switch (mPersistentInteractionMode) {
			case PersistentInteractionMode::MouseTranslating:
				emit(viewChanged());
				break;
			case PersistentInteractionMode::MouseScaling:
				applyTransformationView(0.0, getInteractiveScaling((mCurrentMousePos - mInteractiveRefPos).x(), mMouseMoveScaleSensitivity), false);
				emit(viewChanged());
				break;
			case PersistentInteractionMode::MouseRotating:
				applyTransformationView(getInteractiveRotation(mCurrentMousePos - mInteractiveRefPos), 1.0, false);
				emit(viewChanged());
				break;
		}
		mPersistentInteractionMode = PersistentInteractionMode::None;
		QGuiApplication::restoreOverrideCursor();
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void QInteractiveGraphicsView::wheelEvent(QWheelEvent * event)
{
	setInteractiveRefPos(event->pos());

	switch (event->modifiers()) {
		case Qt::NoModifier: 
			if (mTranslationInteractionEnabled) {
				applyTranslation(QPointF(event->angleDelta()) / 8.0 * mMouseWheelTranslationSensitivity);
				emit(viewChanged());
			}
			break;
		case Qt::ShiftModifier:
			if (mTranslationInteractionEnabled) {
				applyTranslation(swap(QPointF(event->angleDelta()) / 8.0 * mMouseWheelTranslationSensitivity));
				emit(viewChanged());
			}
			break;
		case Qt::ControlModifier:
			if (mScaleInteractionEnabled) {
				applyTransformationView(0.0, getInteractiveScaling(maxAbs(event->angleDelta()) / 8.0, mMouseWheelScaleSensitivity), false);
				emit(viewChanged());
			}
			break;
		case Qt::AltModifier: // swap the x/y wheel value
			if (mRotationInteractionEnabled) {
				applyTransformationView(qDegreesToRadians(maxAbs(event->angleDelta()) / 8.0 * mMouseWheelRotationSensitivity), 1.0, false);
				emit(viewChanged());
			}
			break;
	}
	
	//No call to parent function - behavior change completely // QGraphicsView::wheelEvent(event);
}

void QInteractiveGraphicsView::paintEvent(QPaintEvent *event)
{
	QGraphicsView::paintEvent(event);

	if (mPersistentInteractionMode != PersistentInteractionMode::None && mPersistentInteractionVisualCue) {
		QPainter painter(viewport());
		painter.setPen(mPersistentInteractionVisualCuePen);
		switch (mPersistentInteractionMode) {
			case PersistentInteractionMode::MouseTranslating:
				break;
			case PersistentInteractionMode::MouseScaling:
				painter.drawLine(mInteractiveRefPos.x(), 0, mInteractiveRefPos.x(), height() - 1);
				break;
			case PersistentInteractionMode::MouseRotating:
				drawCrossAngle(painter, mInteractiveRefPos.x(), mInteractiveRefPos.y(), mCurrentInteractiveRotation, qSqrt(square(width()) + square(height())), 5.0);
				break;
		}
	}
}






// tools function
QPointF swap(QPointF const  & p) {
	return QPointF(p.y(), p.x());
}
qreal maxAbs(QPointF const  & p) {
	return qAbs(p.x()) > qAbs(p.y()) ? p.x() : p.y();
}
qreal square(qreal v) {
	return v * v;
}
qreal limit(qreal min, qreal value, qreal max) {
	if (value <= min) return min;
	if (value >= max) return max;
	return value;
}
qreal negFMod(qreal value, qreal mod) // mod > 0
{
	if (value >= 0 && value < mod) {
		return value;
	} else {
		return fmod(value - qFloor(value / mod) * mod, mod);
	}
}
QPointF translationFrom(QTransform const & transform)
{
	return QPointF(transform.m31(), transform.m32());
}
QPointF scaleFrom(QTransform const & transform)
{
	return QPointF(qSqrt(square(transform.m11()) + square(transform.m21())), qSqrt(square(transform.m12()) + square(transform.m22())));
}
qreal singleScaleFrom(QTransform const & transform)
{
	return qSqrt(square(transform.m11()) + square(transform.m21()));
}
qreal rotationFrom(QTransform const & transform)
{
	if (qAbs(transform.m22()) > 0.00001) {
		return qAtan(transform.m12() / transform.m22());
	}
	else if (qAbs(transform.m11()) > 0.00001) {
		return qAtan(transform.m21() / transform.m11());
	}

	Q_ASSERT_X(true, __func__, "Should not append!");
	return 0.0;
}
void drawCrossAngle(QPainter & painter, qreal centerX, qreal centerY, qreal angleRad, qreal lengthFromCenter, qreal offsetFromCenter)
{
	qreal cosOffset{ offsetFromCenter * qCos(angleRad) };
	qreal sinOffset{ offsetFromCenter * qSin(angleRad) };
	qreal cosLength{ lengthFromCenter * qCos(angleRad) };
	qreal sinLength{ lengthFromCenter * qSin(angleRad) };

	painter.drawLine(centerX - cosOffset, centerY - sinOffset, centerX - cosLength, centerY - sinLength);
	painter.drawLine(centerX + cosOffset, centerY + sinOffset, centerX + cosLength, centerY + sinLength);
	painter.drawLine(centerX + sinOffset, centerY - cosOffset, centerX + sinLength, centerY - cosLength);
	painter.drawLine(centerX - sinOffset, centerY + cosOffset, centerX - sinLength, centerY + cosLength);
}