#include "QPathBuilder.h"



const qreal QPathBuilder::eps{ 1.0E-5 };
const qreal QPathBuilder::kPi{ 3.1415926535897932384626433832795 };
const qreal QPathBuilder::kPi_2{ 1.5707963267948966192313216916398 };
const qreal QPathBuilder::k2Pi{ 6.283185307179586476925286766559 };
const qreal QPathBuilder::kDeg2Rad{ 0.01745329251994329576923690768489 };
const qreal QPathBuilder::kRad2Deg{ 57.295779513082320876798154814105 };


void QPathBuilder::updateBoundingBox(QPointF const & point)
{
	qreal left = min(point.x(), mBoundingBox.left());
	qreal right = max(point.x(), mBoundingBox.right());
	qreal top = min(point.y(), mBoundingBox.top());
	qreal bottom = max(point.y(), mBoundingBox.bottom());

	mBoundingBox = QRectF(QPointF(left, top), QPoint(right, bottom));
}

QPair<QPointF, QPointF> QPathBuilder::lateralPoints(QPointF const & center, qreal length, qreal angleRad)
{
	return QPair<QPointF, QPointF>(	pointFromVector(center, length, angleRad - kPi_2),
									pointFromVector(center, length, angleRad + kPi_2));
}

qreal QPathBuilder::angleDisparity(qreal angle1Rad, qreal angle2Rad)
{
	qreal cos1{ qCos(angle1Rad) };
	qreal sin1{ qSin(angle1Rad) };
	qreal cos2{ qCos(angle2Rad) };
	qreal sin2{ qSin(angle2Rad) };

	return qAcos(cos1 * cos2 + sin1 * sin2);
}

qreal QPathBuilder::midAngle(qreal angle1Rad, qreal angle2Rad)
{
	qreal cos1{ qCos(angle1Rad) };
	qreal sin1{ qSin(angle1Rad) };
	qreal cos2{ qCos(angle2Rad) };
	qreal sin2{ qSin(angle2Rad) };

	qreal dot{ cos1 * cos2 + sin1 * sin2 }; // dot = dot V1 to V2
	qreal dotPrime{ -sin1 * cos2 + cos1 * sin2 }; // dot' = dot V1 rotated 90 clockwise to V2
	// Note : dot is negative if v2 point backward to v1

	return angle1Rad + sign(dotPrime) * qAcos(dot) / 2.0;
}

void QPathBuilder::midAngle(qreal angle1Rad, qreal angle2Rad, qreal & midAngleDisparityRad, qreal & midAngleRad)
{
	qreal cos1{ qCos(angle1Rad) };
	qreal sin1{ qSin(angle1Rad) };
	qreal cos2{ qCos(angle2Rad) };
	qreal sin2{ qSin(angle2Rad) };

	qreal dot{ cos1 * cos2 + sin1 * sin2 }; // dot = dot V1 to V2
	qreal dotPrime{ -sin1 * cos2 + cos1 * sin2 }; // dot' = dot V1 rotated 90 clockwise to V2
	// Note : dot is negative if v2 point backward to v1

	midAngleDisparityRad = qAcos(min(dot, 1.0)) / 2.0;
	midAngleRad = angle1Rad + sign(dotPrime) * midAngleDisparityRad;
}

QPathBuilder::QPathBuilder()
{
	reset();
}

QPolygonF QPathBuilder::shape(qreal width) const
{
	if (!isValid()) {
		return QPolygonF();
	}

	QPolygonF pathShape;
	qreal halfWidth = width / 2.0;
	qreal midAngleRad, midAngleDisparityRad;
	qreal lengthDisp;
	QVector<QPair<QPointF, QPointF>> shapePoints(mPoints.size());

	shapePoints[0] = lateralPoints(mPoints[0], halfWidth, mVectors[0].second);
	for (int i{ 1 }; i < shapePoints.size() - 1; ++i) {
		midAngle(mVectors[i].second, mVectors[i - 1].second, midAngleDisparityRad, midAngleRad);
		qreal cos{qCos(midAngleDisparityRad)};
		if (cos > eps) {
			lengthDisp = halfWidth / cos;
		} else {
			midAngleRad += kPi_2;
			lengthDisp = halfWidth;
		}
		shapePoints[i] = lateralPoints(mPoints[i], lengthDisp, midAngleRad);
	}
	int lastIndex{ shapePoints.size() - 1 };
	shapePoints[lastIndex] = lateralPoints(mPoints[lastIndex], halfWidth, mVectors[lastIndex - 1].second);

	pathShape.clear();
	for (int i{ 0 }; i < shapePoints.size(); ++i) {
		pathShape << shapePoints[i].first;
	}
	for (int i{ shapePoints.size() - 1 }; i >= 0; --i) {
		pathShape << shapePoints[i].second;
	}

	return pathShape;
}

void QPathBuilder::drawSimpleArrow(QPainter & painter, qreal length, QPointF const & translate, qreal rotation)
{
	painter.save();
	painter.translate(translate);
	painter.rotate(qRadiansToDegrees(rotation));
	painter.drawLine(0.0, 0.0, length, 0.0);
	painter.drawLine(length, 0.0, length * 0.75, length * 0.15);
	painter.drawLine(length, 0.0, length * 0.75, -length * 0.15);
	painter.restore();
}


void QPathBuilder::draw(QPainter & painter, QPen const & pathPen, QBrush const & pointBrush, qreal pointRadius, QPen const & startVectorPen, QPen const & endVectorPen, qreal vectorLength, QPen const & boundingBoxPen)
{
	if (isValid()) {
		// bounding box
		painter.setPen(boundingBoxPen);
		painter.setBrush(Qt::transparent);
		painter.drawRect(mBoundingBox);

		// path
		painter.setPen(pathPen);
		int n{ mPoints.size() };
		for (int i{ 1 }; i < n; ++i) {
			painter.drawLine(mPoints[i - 1], mPoints[i]);
		}

		// point
		if (pointRadius > eps) {
			painter.setPen(Qt::NoPen);
			painter.setBrush(pointBrush);
			for (auto const & point : mPoints) {
				painter.drawEllipse(point, pointRadius, pointRadius);
			}
		}

		// start vector
		painter.setPen(startVectorPen);
		drawSimpleArrow(painter, vectorLength, entryPoint(), mEntryOrientation);

		// end vector
		painter.setPen(endVectorPen);
		drawSimpleArrow(painter, vectorLength, mPoints.last(), mExitOrientation);
	}
}

void QPathBuilder::drawFromVectors(QPainter & painter, QPen const & pathPen, QBrush const & pointBrush, qreal pointRadius, QPen const & startVectorPen, QPen const & endVectorPen, qreal vectorLength, QPen const & boundingBoxPen)
{
	if (isValid()) {
		// bounding box
		painter.setPen(boundingBoxPen);
		painter.setBrush(Qt::transparent);
		painter.drawRect(mBoundingBox);

		QPointF curPoint1(entryPoint());
		QPointF curPoint2(entryPoint());

		int n{ mVectors.size() };
		for (int i{ 0 }; i < n; ++i) {
			curPoint1 = curPoint2;
			curPoint2 = pointFromVector(curPoint1, mVectors[i].first, mVectors[i].second);

			painter.setPen(pathPen);
			painter.drawLine(curPoint1, curPoint2);

			if (pointRadius > eps) {
				painter.setPen(Qt::NoPen);
				painter.setBrush(pointBrush);
				painter.drawEllipse(curPoint1, pointRadius, pointRadius);
			}
		}
		painter.drawEllipse(curPoint2, pointRadius, pointRadius);

		// start vector
		painter.setPen(startVectorPen);
		drawSimpleArrow(painter, vectorLength, entryPoint(), mEntryOrientation);

		// end vector
		painter.setPen(endVectorPen);
		drawSimpleArrow(painter, vectorLength, curPoint2, mExitOrientation);
	}
}

QPixmap QPathBuilder::toPixmap(int margin, QColor const & fillColor, QPen const & pathPen, QBrush const & pointBrush, qreal pointRadius, QPen const & startVectorPen, QPen const & endVectorPen, qreal vectorLength, QPen const & boundingBoxPen)
{
	if (!isValid()) {
		return QPixmap();
	}

	QRect imageRect(QPoint(0, 0), mBoundingBox.size().toSize() + QSize(margin * 2, margin * 2));

	QPixmap pixmap(imageRect.size());
	pixmap.fill(fillColor);

	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate(QPoint(margin, margin) - mBoundingBox.topLeft());

	draw(painter, pathPen, pointBrush, pointRadius, startVectorPen, endVectorPen, vectorLength, boundingBoxPen);

	return pixmap;
}

void QPathBuilder::reset()
{
	mPoints = QList<QPointF>() << QPointF(0.0, 0.0);
	mVectors = QList<QPair<qreal, qreal>>();

	mEntryOrientation = mExitOrientation = 0.0;
	mLength = 0.0;

	mBoundingBox = QRectF();
}



void QPathBuilder::doSetOrientationRad(qreal orientationRad)
{
	mExitOrientation = orientationRad;

	if (mPoints.size() == 1) {
		mEntryOrientation = mExitOrientation;
	}
}
void QPathBuilder::doRotateRad(qreal angleRad)
{
	mExitOrientation += angleRad;

	if (mPoints.size() == 1) {
		mEntryOrientation = mExitOrientation;
	}
}
void QPathBuilder::doAddLinear(qreal length)
{
	mPoints << pointFromVector(mPoints.last(), length, mExitOrientation);
	mVectors << vector(length, mExitOrientation);

	mLength += length;
	updateBoundingBox(mPoints.last());
}
void QPathBuilder::doAddLinearOffsetAngleRad(qreal length, qreal angleRad)
{
	mExitOrientation += angleRad;
//	doRotateRad(mExitOrientation += angleRad);

	doAddLinear(length);
}
void QPathBuilder::doAddLinearOffsetDelta(qreal deltaParallel, qreal deltaPerpendicular)
{
	doAddLinearOffsetAngleRad(length(deltaParallel, deltaPerpendicular), angle(deltaParallel, deltaPerpendicular));
}
void QPathBuilder::doAddCircularRad(qreal radius, qreal angleRad, int nPoints)
{
	qreal direction{ sign(angleRad) };
	QPointF center{ pointFromVector(mPoints.last(), radius, mExitOrientation + direction * kPi_2) };
	qreal deltaAngleRad{ angleRad / nPoints };
	qreal vectorLength{ length(pointFromVector(QPointF(0.0, 0.0), radius, 0.0), pointFromVector(QPointF(0.0, 0.0), radius, angleRad / nPoints)) };

	qreal curAngleRad{ angle(center, mPoints.last()) };
	for (int i{ 0 }; i < nPoints; ++i) {
		curAngleRad += deltaAngleRad;
		QPointF newPoint = pointFromVector(center, radius, curAngleRad);

		mVectors << vector(vectorLength, angle(mPoints.last(), newPoint));
		mPoints << newPoint;
		updateBoundingBox(mPoints.last());
	}

	mExitOrientation += angleRad;
	mLength += vectorLength * nPoints;
}
void QPathBuilder::doAddCircularRad(qreal radius, qreal angleRad, qreal minLength)
{
	addCircularRad(radius, angleRad, qCeil(abs(angleRad) * radius / minLength));
}
void QPathBuilder::doAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, int nPoints, qreal length2)
{
	addLinear(length1);
	addCircularRad(radius, angleRad, nPoints);
	addLinear(length2);
}
void QPathBuilder::doAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, qreal minArcLength, qreal length2)
{
	addLinear(length1);
	addCircularRad(radius, angleRad, minArcLength);
	addLinear(length2);
}
void QPathBuilder::doAddLShape(qreal length1, qreal length2, bool turnRight)
{
	addLinear(length1);
	addLinearOffsetAngleRad(length2, turnRight ? kPi_2 : -kPi_2);
}
void QPathBuilder::doAddLShape(qreal length1, qreal length2, qreal radius, int nPoints, bool turnRight)
{
	doAddExtendedCircularRad(length1 - radius, radius, turnRight ? kPi_2 : -kPi_2, nPoints, length2 - radius);
}
void QPathBuilder::doAddLShape(qreal length1, qreal length2, qreal radius, qreal minArcLength, bool turnRight)
{
	doAddExtendedCircularRad(length1 - radius, radius, turnRight ? kPi_2 : -kPi_2, minArcLength, length2 - radius);
}
void QPathBuilder::doAddUShape(qreal length1, qreal height, qreal length2, bool turnRight)
{
	qreal angleRad{ turnRight ? kPi_2 : -kPi_2 };

	addLinear(length1);
	addLinearOffsetAngleRad(height, angleRad);
	addLinearOffsetAngleRad(length2, angleRad);
}
void QPathBuilder::doAddUShape(qreal length1, qreal height, qreal length2, int nPoints, bool turnRight)
{
	qreal radius{ height / 2.0 };

	doAddExtendedCircularRad(length1 - radius, radius, turnRight ? kPi : -kPi, nPoints, length2 - radius);
}
void QPathBuilder::doAddUShape(qreal length1, qreal height, qreal length2, qreal minArcLength, bool turnRight)
{
	qreal radius{ height / 2.0 };

	doAddExtendedCircularRad(length1 - radius, radius, turnRight ? kPi : -kPi, minArcLength, length2 - radius);
}
void QPathBuilder::doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, bool turnRight)
{
	qreal angleRad{ turnRight ? kPi_2 : -kPi_2 };

	addLinear(length1);
	addLinearOffsetAngleRad(height / 2.0, angleRad);
	addLinearOffsetAngleRad(length2, angleRad);
	addLinearOffsetAngleRad(height / 2.0, -angleRad);
	addLinearOffsetAngleRad(length3, -angleRad);
}
void QPathBuilder::doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, int nPoints, bool turnRight)
{
	qreal angleRad{ turnRight ? kPi : -kPi };

	addLinear(length1 - height / 4.0);
	addCircularRad(height / 4.0, angleRad, nPoints);
	addLinear(length2 - height / 2.0);
	addCircularRad(height / 4.0, -angleRad, nPoints);
	addLinear(length3 - height / 4.0);
}
void QPathBuilder::doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, qreal minArcLength, bool turnRight)
{
	qreal angleRad{ turnRight ? kPi : -kPi };

	addLinear(length1 - height / 4.0);
	addCircularRad(height / 4.0, angleRad, minArcLength);
	addLinear(length2 - height / 2.0);
	addCircularRad(height / 4.0, -angleRad, minArcLength);
	addLinear(length3 - height / 4.0);
}




bool QPathBuilder::setOrientationRad(qreal orientationRad)
{
	if (!validateSetOrientationRad(orientationRad)) {
		return false;
	}

	doSetOrientationRad(orientationRad);
	return true;
}
bool QPathBuilder::rotateRad(qreal angleRad)
{
	if (!validateRotateRad(angleRad)) {
		return false;
	}

	doRotateRad(angleRad);
	return true;
}
bool QPathBuilder::addLinear(qreal length)
{
	if (!validateLinear(length)) {
		return false;
	}

	doAddLinear(length);
	return true;
}
bool QPathBuilder::addLinearOffsetAngleRad(qreal length, qreal angleRad)
{
	if (!validateAddLinearOffsetAngleRad(length, angleRad)) {
		return false;
	}

	doAddLinearOffsetAngleRad(length, angleRad);
	return true;
}
bool QPathBuilder::addLinearOffsetDelta(qreal deltaParallel, qreal deltaPerpendicular)
{
	if (!validateAddLinearOffsetDelta(deltaParallel, deltaPerpendicular)) {
		return false;
	}

	doAddLinearOffsetDelta(deltaParallel, deltaPerpendicular);
	return true;
}
bool QPathBuilder::addCircularRad(qreal radius, qreal angleRad, int nPoints)
{
	if (!validateAddCircularRad(radius, angleRad, nPoints)) {
		return false;
	}

	doAddCircularRad(radius, angleRad, nPoints);
	return true;
}
bool QPathBuilder::addCircularRad(qreal radius, qreal angleRad, qreal minArcLength)
{
	if (!validateAddCircularRad(radius, angleRad, minArcLength)) {
		return false;
	}

	doAddCircularRad(radius, angleRad, minArcLength);
	return true;
}
bool QPathBuilder::addExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, int nPoints, qreal length2)
{
	if (!validateAddExtendedCircularRad(length1, radius, angleRad, nPoints, length2)) {
		return false;
	}

	doAddExtendedCircularRad(length1, radius, angleRad, nPoints, length2);
	return true;
}
bool QPathBuilder::addExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, qreal minArcLength, qreal length2)
{
	if (!validateAddExtendedCircularRad(length1, radius, angleRad, minArcLength, length2)) {
		return false;
	}

	doAddExtendedCircularRad(length1, radius, angleRad, minArcLength, length2);
	return true;
}
bool QPathBuilder::addLShape(qreal length1, qreal length2, bool turnRight)
{
	if (!validateAddLShape(length1, length2)) {
		return false;
	}

	doAddLShape(length1, length2, turnRight);
	return true;
}
bool QPathBuilder::addLShape(qreal length1, qreal length2, qreal radius, int nPoints, bool turnRight)
{
	if (!validateAddLShape(length1, length2, radius, nPoints)) {
		return false;
	}

	doAddLShape(length1, length2, radius, nPoints, turnRight);
	return true;
}
bool QPathBuilder::addLShape(qreal length1, qreal length2, qreal radius, qreal minArcLength, bool turnRight)
{
	if (!validateAddLShape(length1, length2, radius, minArcLength)) {
		return false;
	}

	doAddLShape(length1, length2, radius, minArcLength, turnRight);
	return true;
}
bool QPathBuilder::addUShape(qreal length1, qreal height, qreal length2, bool turnRight)
{
	if (!validateAddUShape(length1, height, length2)) {
		return false;
	}

	doAddUShape(length1, height, length2, turnRight);
	return true;
}
bool QPathBuilder::addUShape(qreal length1, qreal height, qreal length2, int nPoints, bool turnRight)
{
	if (!validateAddUShape(length1, height, length2, nPoints)) {
		return false;
	}

	doAddUShape(length1, height, length2, nPoints, turnRight);
	return true;
}
bool QPathBuilder::addUShape(qreal length1, qreal height, qreal length2, qreal minArcLength, bool turnRight)
{
	if (!validateAddUShape(length1, height, length2, minArcLength)) {
		return false;
	}

	doAddUShape(length1, height, length2, minArcLength, turnRight);
	return true;
}
bool QPathBuilder::addSShape(qreal length1, qreal length2, qreal length3, qreal height, bool turnRight)
{
	if (!validateAddSShape(length1, length2, length3, height)) {
		return false;
	}

	doAddSShape(length1, length2, length3, height, turnRight);
	return true;
}
bool QPathBuilder::addSShape(qreal length1, qreal length2, qreal length3, qreal height, int nPoints, bool turnRight)
{
	if (!validateAddSShape(length1, length2, length3, height, nPoints)) {
		return false;
	}

	doAddSShape(length1, length2, length3, height, nPoints, turnRight);
	return true;
}
bool QPathBuilder::addSShape(qreal length1, qreal length2, qreal length3, qreal height, qreal minArcLength, bool turnRight)
{
	if (!validateAddSShape(length1, length2, length3, height, minArcLength)) {
		return false;
	}

	doAddSShape(length1, length2, length3, height, minArcLength, turnRight);
	return true;
}










