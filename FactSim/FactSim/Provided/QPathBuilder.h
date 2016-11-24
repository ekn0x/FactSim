#ifndef Q_PATH_BUILDER_H
#define Q_PATH_BUILDER_H


#include <QList>
#include <QPair>

#include <QPointF>
#include <QRectF>
#include <QPolygonF>

#include <QPixmap>
#include <QPainter>

#include <QtMath>

/// \brief QPathBuilder est une classe utilitaire facilitant la création de trajets.
///
/// \details Cette classe donne accès à plusieurs outils et mécanismes permettant la construiction d'un trajet.
/// De plus, elle donne plusieurs informations pertinentes sur le trajet créé telle que la longueur totale, les points, les vecteurs, ...
///
/// Le trajet construit correspond à une courbe linéaire par parties. C'est à dire qu'un 
/// trajet est une succession de segments linéaires déterminés par une liste de points.
/// Chaque trajet possède deux informations supplémentaires concernant les orientations
/// d'entrée et de sortie. Dans tous les cas, le trajet débute à l'origine (0, 0) du référentiel local.
/// L'image suivante montre un trajet simple constitué de 3 points.
/// <table border="0" align="center">
/// <tr><td align="center" valign="bottom" valign="bottom" width="300px"> \image html Path1.png "Trajet simple" 
///		<td width="100px">
///		<td align="center" valign="bottom" valign="bottom" width="300px"> \image html Legend.png "Légende"
/// </table>
/// 
/// Les orientations d'entrée et de sortie ne sont pas nécessairement alignées avec les 2 premiers points et les 2 derniers points tel 
/// qu'illustré sur la figure précédente. Même s'il est plus fréquent d'avoir cette situation, il est possible de définir 
/// des orientations différentes comme le montre la figure suivante.
/// \image html Path2.png "Exemple de trajet où les orientations d'entrée et de sortie ne sont pas alignées"
///
/// Les longueurs utilisées sont sans dimensions et dépendent de l'interprétation de leur usage. 
/// L'affichage direct d'un trajet implique que les longueurs sont utilisées directement en pixels.
/// Les orientations sont toutes définies positivement dans le sens horaire. Ainsi, une orientation définie négativement 
/// applique un changement d'orientation dans le sens anti-horaire. La majorité des fonctions existent en version radians ou degrés 
/// pour la convenance de l'utilisateur.
/// 
/// QPathBuilder s'utilise simplement en construisant le trajet en ajoutant des sections les unes à la suite des autres. 
/// Il existe 4 familles de fonctions aidant à la construction du trajet :
///  - réinitialisation du trajet : reset
///  - changement d'orientation : setOrientation et rotate
///  - ajout de section(s) : toutes les fonctions débutant par add*
///  - il est aussi possible d'utiliser un langage de script pour créer les trajets plus rapidement ..to finish..
///
/// Le langage de script est tout simplement une version compacte textuel des appels de fonctions existantes.
///
/// Au fur et à mesure que le trajet se construit, plusieurs informations sont accessibles :
///  - est-ce que le trajet est valide: isValid()
///  - le nombre de points créés : count()
///  - la longueur totale : length()
///  - la liste des points et des vecteurs : points() et vectors()
///  - le point et l'oritation d'entrée : entryPoint() et entryOrientation()
///  - le point et l'oritation de sortie : exitPoint() et exitOrientation()
///  - la boîte capable : boundingBox()
///  - ***est-ce que le trajet se croise : isOverlapping*** to do
///  - ***est-ce que le trajet est convexe : isConvex*** to do
///
/// Il existe aussi quelques fonctions utilitaires :
///  - dessin du trajet : draw() et drawFromVectors()
///  - création d'un pixmap : toPixmap()
///  - création d'une figure géométrique correspondant au trajet avec une épaisseur (création d'un polygon) : shape()
//  - ***dessin cosmétique du trajet***todo (dot or line with % start offset)
///
class QPathBuilder
{
public:
	// Class default function
	QPathBuilder();
	~QPathBuilder() = default;
	QPathBuilder(QPathBuilder const & builder) = default;
	QPathBuilder(QPathBuilder && builder) = default;
	QPathBuilder& operator=(QPathBuilder const & builder) = default;
	QPathBuilder& operator=(QPathBuilder && builder) = default;

	/// \brief Retourne vrai si le trajet est valide.
	/// \details Un trajet est considéré valide s'il possède 2 points ou plus.
	bool isValid() const { return mPoints.size() >= 2; }

	/// \brief Retourne le nombre du points du trajet.
	int count() const { return mPoints.size(); }

	/// \brief Retourne la longueur totale du trajet.
	/// \details La longueur d'un trajet est la somme de la longueur de tous les segments linéaires existants.
	/// Si le trajet est invalide, la longueur est 0.
	qreal length() const { return mLength; }

	/// \brief Retourne l'orientation d'entrée (en radians).
	qreal entryOrientation() const { return qRadiansToDegrees(entryOrientationRad()); }
	/// \brief Retourne l'orientation d'entrée (en degrés).
	qreal entryOrientationRad() const { return mEntryOrientation; }
	/// \brief Retourne le point d'entrée (premier point du trajet).
	QPointF entryPoint() const { return mPoints.first(); }

	/// \brief Retourne l'orientation de sortie (en degrés).
	qreal exitOrientation() const { return qRadiansToDegrees(exitOrientationRad()); }
	/// \brief Retourne l'orientation de sortie (en radians).
	qreal exitOrientationRad() const { return mExitOrientation; }
	/// \brief Retourne le point de sortie (dernier point du trajet).
	QPointF exitPoint() const { return mPoints.last(); }
	
	/// \brief Retourne la liste des points définissant le trajet.
	QList<QPointF> const & points() const { return mPoints; }
	/// \brief Retourne la liste de vecteurs définissant le trajet.
	/// \details La liste des vecteurs est définie par inchaque vecteur par un QPair<qreal, qreal> qui représente le déplacement selon le point précédent. 
	/// Les constituants du QPair sont la longueur du segment (first) et l'angle du segement en radians (second).
	/// Il est important de noter que la longueur de la liste de vectors a une longueur de count() - 1.
	/// Finalement, n'oublions pas qu'un trajet débute toujours à l'origine (0, 0).
	QList<QPair<qreal, qreal>> const & vectors() const { return mVectors; }
	
	/// \brief Retourne la boîte capable du trajet.
	QRectF const & boundingBox() const { return mBoundingBox; }

	/// \brief Retourne un figure géométrique correspondant au trajet ayant une épaisseur.
	/// \details Cette fonction crée un polygone correspondant au trajet selon une épaisseur donnée. Le polygon créé peut être invalide si il est concave. Aussi, un polygone ayant un chevauchement donne un résultat suprenant s'il est dessiné tel quel (voir la définition de void QPainter::drawPolygon et plus spécifiquement du paramètre Qt::FillRule fillRule).
	/// <table border="0" align="center">
	/// <tr><td align="left"> 
	///		pathBuilder.addLinear(200);<br>
	///		pathBuilder.addLShape(100.0, 300.0, 75.0, 7, true);<br>
	///		pathBuilder.addUShape(150, 100, 250, 12, true);<br>
	///		pathBuilder.addLinearOffsetAngle(100, -45);<br>
	///		pathBuilder.addCircular(100, -235, 17);<br>
	///		QPolygonF shape = pathBuilder.shape(20.0);<br>
	/// <tr><td align="center"> 
	///		\image html shape.png "Exemple de polygone créé" <br>
	/// </table>
	QPolygonF shape(qreal width) const;

	// ... to do ...
	// QPolygonF polygonShape(qreal width) const; 
	// BoundingBox
	// 
	//  - est-ce que le trajet se croise : isOverlap  ***
	//  - est-ce que le trajet est convexe : isConvex  ***


	/// \brief Dessine le trajet avec le painter donné.
	void draw(	QPainter & painter, 
				QPen const & pathPen = QPen(QColor(132, 164, 217), 2.0),
				QBrush const & pointBrush = QBrush(QColor(67, 114, 196)),
				qreal pointRadius = 2.5, 
				QPen const & startVectorPen = QPen(QColor(198, 17, 198), 3.0), 
				QPen const & endVectorPen = QPen(QColor(208, 109, 42), 3.0), 
				qreal vectorLength = 35.0,
				QPen const & boundingBoxPen = QColor(196, 196, 196));
	/// \brief Dessine le trajet avec le painter donné. Cette fonction est utilitaire et sert principalement au développement.
	void drawFromVectors(QPainter & painter, 
				QPen const & pathPen = QPen(QColor(132, 164, 217), 2.0),
				QBrush const & pointBrush = QBrush(QColor(67, 114, 196)),
				qreal pointRadius = 2.5, 
				QPen const & startVectorPen = QPen(QColor(198, 17, 198), 3.0), 
				QPen const & endVectorPen = QPen(QColor(208, 109, 42), 3.0), 
				qreal vectorLength = 35.0,
				QPen const & boundingBoxPen = QColor(196, 196, 196));

	/// \brief Dessine le trajet avec le painter donné.
	QPixmap toPixmap(int margin = 35, 
				QColor const & fillColor = Qt::transparent,
				QPen const & pathPen = QPen(QColor(132, 164, 217), 2.0),
				QBrush const & pointBrush = QBrush(QColor(67, 114, 196)),
				qreal pointRadius = 2.5, 
				QPen const & startVectorPen = QPen(QColor(198, 17, 198), 3.0), 
				QPen const & endVectorPen = QPen(QColor(208, 109, 42), 3.0), 
				qreal vectorLength = 35.0,
				QPen const & boundingBoxPen = QColor(196, 196, 196));


	// Buildiing operations - - - - - - - - - - - - - - - - - - - - - 
	/// \brief Réinitialise le trajet.
	void reset();


	/// \brief Détermine l'orientation courante en degrée.
	/// \details Ceci est une fonction surchargée de setOrientationRad().
	/// \param[in] orientationDeg L'orientation en radians : _orientationDeg_ &isin; ]-360, 360[
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool setOrientation(qreal orientationDeg) { return setOrientationRad(qDegreesToRadians(orientationDeg)); }

	/// \brief Détermine l'orientation courante en radians.
	/// \details Cette fonction détermine l'orientation de construction et par le fait même l'orientation de sortie. 
	/// Si le trajet est toujours à son état initial (sans aucune section ajoutée), l'orientation d'entrée est modifiée à l'orentation spécifiée.
	/// L'orientation est spécifiée par rapport au zéro et ne tient pas compte de l'orientation courante.
	/// \image html setOrientation.png
	/// \param[in] orientationRad L'orientation en radians : _orientationRad_ &isin; ]-2&pi;, 2&pi;[
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool setOrientationRad(qreal orientationRad);

	/// \brief Applique une rotation de l'orientation courante en degrée.
	/// \details Ceci est une fonction surchargée de rotateRad().
	/// \param[in] angleDeg La rotation en radians : _angleDeg_ &isin; [-180, 180]
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool rotate(qreal angleDeg) { return rotateRad(qDegreesToRadians(angleDeg)); }

	/// \brief Applique une rotation de l'orientation courante en radians.
	/// \details Cette fonction applique une rotation de l'orientation de construction et l'orientation de sortie. 
	/// Si le trajet est toujours à son état initial (sans aucune section ajoutée), l'orientation d'entrée est aussi modifiée.
	/// \image html rotate.png
	/// \param[in] angleRad Rotation en radians : _angleRad_ &isin; [-&pi;, &pi;]
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool rotateRad(qreal angleRad);


	/// \brief Ajoute une section linéaire.
	/// \details Cette fonction ajoute au trajet une section de longueur _length_ suivant l'orientation courante. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addLinearSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addLinear.png "addLinear(250.0)"
	/// </table>
	/// \param[in] length Longueur du segment : _length_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLinear(qreal length);

	/// \brief Ajoute une section linéaire avec une rotation.
	/// \details Ceci est une fonction surchargée de addLinearOffsetAngleRad().
	/// \param[in] length Longueur du segment : _length_ &isin; ]0, &infin;
	/// \param[in] angleDeg Rotation en degrés : _angleDeg_ &isin; [-180, 180]
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLinearOffsetAngle(qreal length, qreal angleDeg) { return addLinearOffsetAngleRad(length, qDegreesToRadians(angleDeg)); }

	/// \brief Ajoute une section linéaire avec une rotation.
	/// \details Cette fonction ajoute au trajet une section de longueur _length_ suivant une rotation de _angleRad_ selon l'orientation courante. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addLinearOffsetAngleRadSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addLinearOffsetAngleRad.png "addLinearOffsetAngleRad(250.0, qDegreesToRadians(15.0))"
	/// </table>
	/// \param[in] length Longueur du segment : _length_ &isin; ]0, &infin;
	/// \param[in] angleRad Rotation en radians : _angleRad_ &isin; [-&pi;, &pi;]
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLinearOffsetAngleRad(qreal length, qreal angleRad);

	/// \brief Ajoute une section linéaire considérant un décalage.
	/// \details Cette fonction ajoute au trajet une section de longueur déterminé par un décalage parallèle (_deltaParallel_) et perpendiculaire (_deltaPerpendicular_) suivant l'orientation courante. 
	/// L'orientation courante est modifiée pour représenter le suivi du trajet. La longueur du vecteur formé par _deltaParallel_ et _deltaPerpendicular_ doit être supérieur à zéro.
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addLinearOffsetDeltaSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addLinearOffsetDelta.png "addLinearOffsetDelta(250.0, 45.0)"
	/// </table>
	/// \param[in] deltaParallel Longueur décalage parallèle : _deltaParallel_ &isin; ]0, &infin;
	/// \param[in] deltaPerpendicular Longueur décalage perpendiculaire : _deltaPerpendicular_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLinearOffsetDelta(qreal deltaParallel, qreal deltaPerpendicular);


	/// \brief Ajoute une section circulaire.
	/// \details Ceci est une fonction surchargée de addCircularRad().
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleDeg Angle définissant la longueur d'arc en degrés : _angleDeg_ &isin; ]-360, 360;[
	/// \param[in] nPoints Nombre de points à insérer : _nPoints_ &isin; [2, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addCircular(qreal radius, qreal angleDeg, int nPoints) { return addCircularRad(radius, qDegreesToRadians(angleDeg), nPoints); }

	/// \brief Ajoute une section circulaire.
	/// \details Cette fonction ajoute au trajet une succession de segments passant par une section circulaire défini par le rayon, l'angle de l'arc et le nombre de segments désirés. 
	/// Si l'angle de l'arc est positif, la rotation est dans le sens horaire et anti-horaire sinon.
	/// L'orientation finale ne représente pas l'orientation du dernier segment mais plutôt celui déterminé par la fin du parcours d'arc.
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addCircularRadSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addCircularRad.png "addCircularRad(100.0, qDegreesToRadians(135.0), 8)"
	/// </table>
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleRad Angle définissant la longueur d'arc en radians : _angleRad_ &isin; ]-2&pi;, 2&pi;[
	/// \param[in] nPoints Nombre de points à insérer : _nPoints_ &isin; [2, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addCircularRad(qreal radius, qreal angleRad, int nPoints);

	/// \brief Ajoute une section circulaire.
	/// \details Ceci est une fonction surchargée de addCircularRad().
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleDeg Angle définissant la longueur d'arc en degrés : _angleDeg_ &isin; ]-360, 360;[
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addCircular(qreal radius, qreal angleDeg, qreal minArcLength) { return addCircularRad(radius, qDegreesToRadians(angleDeg), minArcLength); }

	/// \brief Ajoute une section circulaire.
	/// \details Ceci est une fonction surchargée de addCircularRad().
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleRad Angle définissant la longueur d'arc en radians : _angleRad_ &isin; ]-2&pi;, 2&pi;[
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addCircularRad(qreal radius, qreal angleRad, qreal minArcLength);


	/// \brief Ajoute trois sections consécutives : linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addExtendedCircularRad().
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleDeg Angle définissant la longueur d'arc en degrés : _angleDeg_ &isin; ]-360, 360;[
	/// \param[in] nPoints Nombre de points à insérer : _nPoints_ &isin; [2, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addExtendedCircular(qreal length1, qreal radius, qreal angleDeg, int nPoints, qreal length2) { return addExtendedCircularRad(length1, radius, qDegreesToRadians(angleDeg), nPoints, length2); }

	/// \brief Ajoute trois sections consécutives : linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addExtendedCircularRad().
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleDeg Angle définissant la longueur d'arc en degrés : _angleDeg_ &isin; ]-360, 360;[
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addExtendedCircular(qreal length1, qreal radius, qreal angleDeg, qreal minArcLength, qreal length2) { return addExtendedCircularRad(length1, radius, qDegreesToRadians(angleDeg), minArcLength, length2); }

	/// \brief Ajoute trois sections consécutives : linéaire + circulaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 3 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addExtendedCircularRadSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addExtendedCircularRad.png "addExtendedCircularRad(100.0, 75.0, qDegreesToRadians(225.0), 9, 150.0)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleRad Angle définissant la longueur d'arc en radians : _angleRad_ &isin; ]-2&pi;, 2&pi;[
	/// \param[in] nPoints Nombre de points à insérer : nPoints &isin; [2, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, int nPoints, qreal length2);

	/// \brief Ajoute trois sections consécutives : linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addExtendedCircularRad().
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] angleRad Angle définissant la longueur d'arc en radians : _angleRad_ &isin; ]-2&pi;, 2&pi;[
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, qreal minArcLength, qreal length2);


	/// \brief Ajoute deux sections linéaires consécutives créant un coude à 90° : linéaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 2 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addLShapeSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addLShape.png "addLShape(150.0, 75.0, true)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLShape(qreal length1, qreal length2, bool turnRight = true);

	/// \brief Ajoute deux sections linéaires entrecoupées par une section circulaire créant un coude à 90° : linéaire + circulaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 3 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addLShapeRoundSchema.png "Schéma" 
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addLShapeRound.png "addLShape(250.0, 100.0, 75.0, 8, true)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ < _length1_ &cap; _radius_ < _length2_
	/// \param[in] nPoints Nombre de points à insérer pour la section circulaire : nPoints &isin; [2, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLShape(qreal length1, qreal length2, qreal radius, int nPoints, bool turnRight = true);

	/// \brief Ajoute deux sections linéaires entrecoupées par une section circulaire créant un coude à 90° : linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addLShape().
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] radius Rayon du cercle : _radius_ &isin; ]0, &infin;
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addLShape(qreal length1, qreal length2, qreal radius, qreal minArcLength, bool turnRight = true);


	/// \brief Ajoute trois sections linéaires consécutives créant un retour à 180° : linéaire + linéaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 3 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addUShapeSchema.png "Schéma"
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addUShape.png "addUShape(250.0, 150.0, 125.0, true)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] height Hauteur du retour : _height_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addUShape(qreal length1, qreal height, qreal length2, bool turnRight = true);

	/// \brief Ajoute deux sections linéaires entrecoupées par une section circulaire créant un retour à 180° : linéaire + circulaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 3 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addUShapeRoundSchema.png "Schéma"
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addUShapeRound.png "addUShape(250.0, 200.0, 150.0, 12, false)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] height Hauteur du retour : _height_ / 2 < _length1_ &cap; _height_ / 2 < _length2_
	/// \param[in] nPoints Nombre de points à insérer pour la section circulaire : nPoints &isin; [2, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addUShape(qreal length1, qreal height, qreal length2, int nPoints, bool turnRight = true);

	/// \brief Ajoute deux sections linéaires entrecoupées par une section circulaire créant un retour à 180° : linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addUShape(). 
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] height Hauteur du retour : _height_ / 2 < _length1_ &cap; _height_ / 2 < _length2_
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addUShape(qreal length1, qreal height, qreal length2, qreal minArcLength, bool turnRight = true);


	/// \brief Ajoute cinq sections linéaires consécutives créant une forme de S : linéaire + linéaire + linéaire + linéaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 5 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addSShapeSchema.png "Schéma"
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addSShape.png "addSShape(250.0, 100.0, 200.0, 250.0, true)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin;
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin;
	/// \param[in] length3 Longueur du dernier segment : _length3_ &isin; ]0, &infin;
	/// \param[in] height Hauteur : _height_ &isin; ]0, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addSShape(qreal length1, qreal length2, qreal length3, qreal height, bool turnRight = true);

	/// \brief Ajoute trois sections linéaires entrecoupées par deux sections circulaires créant une forme de S : linéaire + circulaire + linéaire + circulaire + linéaire.
	/// \details Cette fonction représente une combinaison successive des 5 sections de base. 
	/// <table border="0" align="center">
	/// <tr><td align="center" valign="bottom" width="300px"> \image html addSShapeRoundSchema.png "Schéma"
	///		<td width="100px">
	///		<td align="center" valign="bottom" width="300px"> \image html addSShapeRound.png "addSShape(150.0, 300.0, 150.0, 250.0, 7, false)"
	/// </table>
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin; &cap; _length1_ > _height_ / 4
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin; &cap; _length2_ > _height_ / 2
	/// \param[in] length3 Longueur du dernier segment : _length3_ &isin; ]0, &infin; &cap; _length3_ > _height_ / 4
	/// \param[in] height Hauteur : _height_ &isin; ]0, &infin;
	/// \param[in] nPoints Nombre de points à insérer pour chacune des sections circulaires : nPoints &isin; [2, &infin;
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addSShape(qreal length1, qreal length2, qreal length3, qreal height, int nPoints, bool turnRight = true);

	/// \brief Ajoute trois sections linéaires entrecoupées par deux sections circulaires créant une forme de S : linéaire + circulaire + linéaire + circulaire + linéaire.
	/// \details Ceci est une fonction surchargée de addSShape(). 
	/// \param[in] length1 Longueur du premier segment : _length1_ &isin; ]0, &infin; &cap; _length1_ > _height_ / 4
	/// \param[in] length2 Longueur du dernier segment : _length2_ &isin; ]0, &infin; &cap; _length2_ > _height_ / 2
	/// \param[in] length3 Longueur du dernier segment : _length3_ &isin; ]0, &infin; &cap; _length3_ > _height_ / 4
	/// \param[in] height Hauteur : _height_ &isin; ]0, &infin;
	/// \param[in] minArcLength Détermine la longueur cible (et minimum) de chaque segment à créer : _minArcLength_ &isin; ]0, &infin;  &cap;  _minArcLength_ > | angleRad | x radius
	/// \param[in] turnRight Applique une rotation à droite si vrai sinon vers la gauche
	/// \return Retourne vrai si les paramètres d'entrées sont valides.
	bool addSShape(qreal length1, qreal length2, qreal length3, qreal height, qreal minArcLength, bool turnRight = true);


	//bool addOffset
	//  ----------\
	//             \
	//              \----------


private:
	qreal mLength{ 0.0 };

	qreal mEntryOrientation{ 0.0 };
	qreal mExitOrientation{ 0.0 };

	QList<QPointF> mPoints;
	QList<QPair<qreal, qreal>> mVectors; // length | angle (radians)

	QRectF mBoundingBox;
	void updateBoundingBox(QPointF const & point);


	bool validateSetOrientationRad(qreal orientationRad) { return orientationRad > -k2Pi && orientationRad < k2Pi; }
	bool validateRotateRad(qreal angleRad) { return angleRad >= -kPi && angleRad <= kPi; }
	bool validateLinear(qreal length) { return length > eps; }
	bool validateAddLinearOffsetAngleRad(qreal length, qreal angleRad) { return length > eps && angleRad >= -kPi && angleRad <= kPi; }
	bool validateAddLinearOffsetDelta(qreal deltaParallel, qreal deltaPerpendicular) { return abs(deltaParallel) > eps || abs(deltaPerpendicular) > eps; }
	bool validateAddCircularRad(qreal radius, qreal angleRad, int nPoints) { return radius > eps && angleRad > -k2Pi && angleRad < k2Pi && nPoints >= 2; }
	bool validateAddCircularRad(qreal radius, qreal angleRad, qreal minArcLength) { return radius > eps && angleRad > -k2Pi && angleRad < k2Pi && minArcLength > eps && minArcLength < abs(angleRad) * radius / 2.0; }
	bool validateAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, int nPoints, qreal length2) { return validateLinear(length1) && validateAddCircularRad(radius, angleRad, nPoints) && validateLinear(length2); }
	bool validateAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, qreal minArcLength, qreal length2) { return validateLinear(length1) && validateAddCircularRad(radius, angleRad, minArcLength) && validateLinear(length2); }
	bool validateAddLShape(qreal length1, qreal length2) { return length1 > eps && length2 > eps; }
	bool validateAddLShape(qreal length1, qreal length2, qreal radius, int nPoints) { return length1 > radius && length2 > radius && radius > eps && nPoints >= 2; }
	bool validateAddLShape(qreal length1, qreal length2, qreal radius, qreal minArcLength) { return length1 > radius && length2 > radius && radius > eps && minArcLength > eps && minArcLength < kPi_2 * radius / 2.0; }
	bool validateAddUShape(qreal length1, qreal height, qreal length2) { return length1 > eps && height > eps && length2 > eps; }
	bool validateAddUShape(qreal length1, qreal height, qreal length2, int nPoints) { return length1 > height / 2.0 && height > eps && length2 > height / 2.0 && nPoints >= 2; }
	bool validateAddUShape(qreal length1, qreal height, qreal length2, qreal minArcLength) { return length1 > height / 2.0 && height > eps && length2 > height / 2.0 && minArcLength > eps && minArcLength < kPi * height / 4.0; }
	bool validateAddSShape(qreal length1, qreal length2, qreal length3, qreal height) { return length1 > eps && length2 > eps && length3 > eps && height > eps; }
	bool validateAddSShape(qreal length1, qreal length2, qreal length3, qreal height, int nPoints) { return length1 > height / 4.0 && length2 > height / 4.0 && length3 > height / 4.0 && height > eps && nPoints >= 2; }
	bool validateAddSShape(qreal length1, qreal length2, qreal length3, qreal height, qreal minArcLength) { return length1 > height / 4.0 && length2 > height / 4.0 && length3 > height / 4.0 && height > eps && minArcLength > eps && minArcLength < kPi * height / 4.0; }

	void doSetOrientationRad(qreal orientationRad);
	void doRotateRad(qreal angleRad);
	void doAddLinear(qreal length);
	void doAddLinearOffsetAngleRad(qreal length, qreal angleRad);
	void doAddLinearOffsetDelta(qreal deltaParallel, qreal deltaPerpendicular);
	void doAddCircularRad(qreal radius, qreal angleRad, int nPoints);
	void doAddCircularRad(qreal radius, qreal angleRad, qreal minLength);
	void doAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, int nPoints, qreal length2);
	void doAddExtendedCircularRad(qreal length1, qreal radius, qreal angleRad, qreal minArcLength, qreal length2);
	void doAddLShape(qreal length1, qreal length2, bool turnRight);
	void doAddLShape(qreal length1, qreal length2, qreal radius, int nPoints, bool turnRight);
	void doAddLShape(qreal length1, qreal length2, qreal radius, qreal minArcLength, bool turnRight);
	void doAddUShape(qreal length1, qreal height, qreal length2, bool turnRight);
	void doAddUShape(qreal length1, qreal height, qreal length2, int nPoints, bool turnRight);
	void doAddUShape(qreal length1, qreal height, qreal length2, qreal minArcLength, bool turnRight);
	void doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, bool turnRight);
	void doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, int nPoints, bool turnRight);
	void doAddSShape(qreal length1, qreal length2, qreal length3, qreal height, qreal minArcLength, bool turnRight);



	// Utilities
	// constants
	static const qreal eps;
	static const qreal kPi;
	static const qreal kPi_2;
	static const qreal k2Pi;
	static const qreal kDeg2Rad;
	static const qreal kRad2Deg;
	// general math
	static qreal min(qreal v1, qreal v2) { return v1 <= v2 ? v1 : v2; }
	static qreal max(qreal v1, qreal v2) { return v1 >= v2 ? v1 : v2; }
	static qreal abs(qreal v) { return v < 0.0 ? -v : v; }
	static qreal sign(qreal v) { return v < 0.0 ? -1.0 : (v > 0.0 ? 1.0 : 0.0); }
	// vector & geometry
	static qreal deg2Rad(qreal deg) { return deg * kDeg2Rad; }
	static qreal rad2Deg(qreal rad) { return rad * kRad2Deg; }
	static qreal angleDisparity(qreal angle1Rad, qreal angle2Rad);
	static qreal midAngle(qreal angle1Rad, qreal angle2Rad);
	static void midAngle(qreal angle1Rad, qreal angle2Rad, qreal & midAngleDisparityRad, qreal & midAngleRad);
	static QPointF pointFromVector(QPointF const & start, qreal length, qreal angleRad) { return start + length * QPointF(qCos(angleRad), qSin(angleRad)); }
	static QPair<qreal, qreal> vector(qreal length, qreal angle) { return QPair<qreal, qreal>(length, angle); }
	static QPair<qreal, qreal> vector(QPointF const & vector) { return QPair<qreal, qreal>(vector.x(), vector.y()); }
	static qreal length2(qreal x, qreal y) { return x * x + y * y; }
	static qreal length2(QPointF const & p) { return length2(p.x(), - p.x()); }
	static qreal length2(QPointF const & p1, QPointF const & p2) { return length2(p2 - p1); }
	static qreal length(qreal x, qreal y) { return qSqrt(length2(x, y)); }
	static qreal length(QPointF const & p) { return length(p.x(), p.y()); }
	static qreal length(QPointF const & p1, QPointF const & p2) { return length(p2 - p1); }
	static qreal angle(qreal x, qreal y) { return qAtan2(y, x); }
	static qreal angle(QPointF const & p) { return angle(p.x(), p.y()); }
	static qreal angle(QPointF const & p1, QPointF const & p2) { return angle(p2 - p1); }
	static QPair<QPointF, QPointF> lateralPoints(QPointF const & center, qreal length, qreal angleRad);
	// drawing
	static void drawSimpleArrow(QPainter & painter, qreal length, QPointF const & translate, qreal rotation);
};

#endif // Q_PATH_BUILDER_H
